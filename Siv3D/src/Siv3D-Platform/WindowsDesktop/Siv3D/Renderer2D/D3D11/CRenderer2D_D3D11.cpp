//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CRenderer2D_D3D11.hpp"
# include <Siv3D/Blob.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/Mat3x3.hpp>
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>
# include <Siv3D/Renderer2D/Renderer2DDrawSubmission.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/ConstantBuffer/D3D11/ConstantBuffer_D3D11.hpp>
# include <Siv3D/Texture/D3D11/CTexture_D3D11.hpp>
# include <Siv3D/Profiler/IProfiler.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/FmtOptional.hpp>
# include <Siv3D/EngineLog.hpp>

/*
#	define LOG_COMMAND(...) LOG_TRACE(__VA_ARGS__)
/*/
#	define LOG_COMMAND(...) ((void)0)
//*/

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr D3D11_VIEWPORT MakeViewport(const Point pos, const Size size) noexcept
		{
			return{
				.TopLeftX	= static_cast<float>(pos.x),
				.TopLeftY	= static_cast<float>(pos.y),
				.Width		= static_cast<float>(size.x),
				.Height		= static_cast<float>(size.y),
				.MinDepth	= D3D11_MIN_DEPTH,
				.MaxDepth	= D3D11_MAX_DEPTH,
			};
		}
	}

	struct CommandState
	{
		BatchInfo2D batchInfo;
		
		RasterizerState rasterizerState = RasterizerState::Default2D;
		
		Optional<Rect> scissorRect;

		Mat3x3 transform = Mat3x3::Identity();

		Mat3x2 screenMat = Mat3x2::Identity();
	};

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CRenderer2D_D3D11::~CRenderer2D_D3D11()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_D3D11::~CRenderer2D_D3D11()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::init()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_D3D11::init()");

		// 各種ポインタを保存
		{
			m_pRenderer	= static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
			m_pShader	= static_cast<CShader_D3D11*>(SIV3D_ENGINE(Shader));
			m_pTexture	= static_cast<CTexture_D3D11*>(SIV3D_ENGINE(Texture));
			m_device	= m_pRenderer->getDevice().getDevice();
			m_context	= m_pRenderer->getDevice().getContext();
		}
		
		// 標準 InputLayout を作成
		{
			const D3D11_INPUT_ELEMENT_DESC layout[3] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			{
				const Blob& bytecode = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::Shape2D).getBytecode();

				if (FAILED(m_device->CreateInputLayout(layout, 3, bytecode.data(), bytecode.size(), &m_inputLayout)))
				{
					throw InternalEngineError{ "ID3D11Device::CreateInputLayout() failed" };
				}
			}

			if (not m_vertexBufferManager2D.init(m_device, m_context))
			{
				throw InternalEngineError{ "D3D11VertexBufferManager2D::init() failed" };
			}
		}

		m_engineShader.init(*SIV3D_ENGINE(EngineShader));

		m_shadowTexture = CreateShadowTexture();
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLine
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addLine(const LineCap startCap, const LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, start, end, thickness, colors, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), style, start, end, thickness, colors, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitStyledLine(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, style);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addArrow
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer2D_D3D11::addLineStringArrow(const LineCap startCap, const std::span<const Vec2> points,
		const Float2& tip, const float thickness, const float headWidth, const Float4& color)
	{
		const auto count = Vertex2DBuilder::BuildLineStringArrow(std::bind_front(&CRenderer2D_D3D11::createBuffer, this),
			startCap, points, tip, thickness, headWidth, color, m_commandManager.getCurrentRasterScaling());
		if (not count) return false;
		if (*count) Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, *count);
		return true;
	}

	void CRenderer2D_D3D11::addArrow(LineCap startCap, const Float2& start, const Float2& end, float thickness, const Float2& headSize, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildArrow(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, start, end, thickness, headSize, colors, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTriangle
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), points, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const Float4(&colors)[3])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), points, colors))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), points, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRect
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addRect(const FloatRect& rect, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addRect(const FloatRect& rect, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, colors))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addRect(const FloatRect& rect, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRectFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addRectFrame(const FloatRect& innerRect, const float thickness, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), innerRect, thickness, colorType, color0, color1))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addRectFrame(const FloatRect& innerRect, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), innerRect, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRectDashedFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addRectDashedFrame(const FloatRect& innerRect, const float offset, const float thickness, const float dashRatio, const uint32 dashCount, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectDashedFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), innerRect, offset, thickness, dashRatio, dashCount, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircle
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircle(const Float2& center, const float r, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircle(const Float2& center, const float r, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircleFrame(const Float2& center, const float rInner, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, rInner, thickness, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircleFrame(const Float2& center, const float rInner, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, rInner, thickness, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleDashedFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircleDashedFrame(const Float2& center, const float rInner, const float startAngle, const float thickness, const float dashRatio, const uint32 dashCount, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleDashedFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, rInner, startAngle, thickness, dashRatio, dashCount, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCirclePie
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCirclePie(const Float2& center, const float r, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCirclePie(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, startAngle, angle, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addCirclePie(const Float2& center, const float r, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCirclePie(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, startAngle, angle, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleArc
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircleArc(const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), lineCap, center, rInner, startAngle, angle, thickness, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircleArc(const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), lineCap, center, rInner, startAngle, angle, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_D3D11::addCircleArc(const LineCap startCap, const LineCap endCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, center, rInner, startAngle, angle, thickness, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircleArc(const LineCap startCap, const LineCap endCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, center, rInner, startAngle, angle, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleSegment
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircleSegment(const Float2& center, const float r, const float startAngle, const float angle, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleSegment(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, startAngle, angle, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircleSegment(const Float2& center, const float r, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleSegment(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, startAngle, angle, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipse
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addEllipse(const Float2& center, const float a, const float b, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipse(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addEllipse(const Float2& center, const float a, const float b, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipse(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipseFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addEllipseFrame(const Float2& center, const float a, const float b, const float innerThickness, const float outerThickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, innerThickness, outerThickness, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addEllipseFrame(const Float2& center, const float a, const float b, const float innerThickness, const float outerThickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, innerThickness, outerThickness, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_D3D11::addEllipseRotated(const Float2& center, const float a, const float b, const float rotation, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseRotated(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, rotation, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addEllipseRotated(const Float2& center, const float a, const float b, const float rotation, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseRotated(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, rotation, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_D3D11::addEllipseFrameRotated(const Float2& center, const float a, const float b, const float rotation, const float innerThickness, const float outerThickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrameRotated(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, rotation, innerThickness, outerThickness, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addEllipseFrameRotated(const Float2& center, const float a, const float b, const float rotation, const float innerThickness, const float outerThickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrameRotated(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, rotation, innerThickness, outerThickness, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipseDashedFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addEllipseDashedFrame(const Float2& center, const float a, const float b, const float innerThickness, const float outerThickness, const float offset, const float dashRatio, const uint32 dashCount, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseDashedFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, innerThickness, outerThickness, offset, dashRatio, dashCount, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipseArc
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addEllipseArc(const LineCap startCap, const LineCap endCap, const Float2& center, const float a, const float b, const float startAngle, const float angle, const float innerThickness, const float outerThickness, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseArc(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, center, a, b, startAngle, angle, innerThickness, outerThickness, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addEllipseArc(const LineCap startCap, const LineCap endCap, const Float2& center, const float a, const float b, const float startAngle, const float angle, const float innerThickness, const float outerThickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseArc(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, center, a, b, startAngle, angle, innerThickness, outerThickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipsePie
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addEllipsePie(const Float2& center, const float a, const float b, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipsePie(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, startAngle, angle, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addEllipsePie(const Float2& center, const float a, const float b, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipsePie(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, startAngle, angle, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSuperEllipse
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addSuperEllipse(const Float2& center, const float a, const float b, const float n, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildSuperEllipse(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, n, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addSuperEllipse(const Float2& center, const float a, const float b, const float n, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildSuperEllipse(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, n, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addQuad
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addQuad(const FloatQuad& quad, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), quad, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addQuad(const FloatQuad& quad, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), quad, colors))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addQuad(const FloatQuad& quad, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), quad, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRect
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addRoundRect(const FloatRect& rect, const float r, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, r, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addRoundRect(const FloatRect& rect, const std::array<float, 4>& radii, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, radii, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addRoundRect(const FloatRect& rect, const float r, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, r, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addRoundRect(const FloatRect& rect, const float r, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, r, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRectFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this),
			innerRect, innerR, outerRect, outerR, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addRoundRectFrame(const FloatRect& innerRect, const std::array<float, 4>& innerRadii, const FloatRect& outerRect, const std::array<float, 4>& outerRadii, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this),
			innerRect, innerRadii, outerRect, outerRadii, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this),
			innerRect, innerR, outerRect, outerR, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this),
			innerRect, innerR, outerRect, outerR, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRectDashedFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addRoundRectDashedFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const float offset, const float dashRatio, const uint32 dashCount, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectDashedFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this),
			innerRect, innerR, outerRect, outerR, offset, dashRatio, dashCount, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPolygon
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addPolygon(const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, triangleIndices, offset, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addPolygon(const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, triangleIndices, offset, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_D3D11::addPolygon(const std::span<const Float2> vertices, const std::span<const Vertex2D::IndexType> indices, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, indices, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addPolygon(const std::span<const Float2> vertices, const std::span<const Vertex2D::IndexType> indices, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, indices, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPolygonTransformed
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addPolygonTransformed(const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const float s, const float c, const Float2& offset, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygonTransformed(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, triangleIndices, s, c, offset, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addPolygonTransformed(const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const float s, const float c, const Float2& offset, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygonTransformed(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, triangleIndices, s, c, offset, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addShape2DFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addShape2DFrame(const std::span<const Float2> vertices, const Optional<Float2>& offset, const float thickness, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildShape2DFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, offset, thickness, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addShape2DFrame(const std::span<const Float2> vertices, const Optional<Float2>& offset, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildShape2DFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, offset, thickness, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLineString
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer2D_D3D11::tryAddSmallConvexFrame(const std::span<const Vec2> points, const JoinStyle joinStyle, const double innerThickness, const double outerThickness, const Float4& color)
	{
		const auto result = Vertex2DBuilder::TryBuildSmallConvexFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), points, joinStyle, innerThickness, outerThickness, color, m_commandManager.getCurrentRasterScaling());
		if (not result) return false;
		if (const auto indexCount = *result)
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
		return true;
	}

	bool CRenderer2D_D3D11::tryAddSmallConvexFrame(const std::span<const Vec2> points, const JoinStyle joinStyle, const double innerThickness, const double outerThickness, const PatternParameters& pattern)
	{
		const auto result = Vertex2DBuilder::TryBuildSmallConvexFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), points, joinStyle, innerThickness, outerThickness, pattern.primaryColor, m_commandManager.getCurrentRasterScaling());
		if (not result) return false;
		if (const auto indexCount = *result)
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
		return true;
	}

	void CRenderer2D_D3D11::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, points, offset, thickness, inner, closeRing, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const Float4& colorStart, const Float4& colorEnd)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, points, offset, thickness, inner, colorStart, colorEnd, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, points, offset, thickness, inner, closeRing, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_D3D11::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const std::span<const ColorF> colors)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, points, offset, thickness, inner, closeRing, colors, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTexturedCircle
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addTexturedCircle(const Texture& texture, const Circle& circle, const FloatRect& uv, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedCircle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), circle, uv, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	void CRenderer2D_D3D11::addTexturedCircle(const Texture& texture, const Circle& circle, const FloatRect& uv, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedCircle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), circle, uv, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTexturedCirclePie
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addTexturedCirclePie(const Texture& texture, const Float2& center, const float r, const float startAngle, const float angle, const FloatRect& uv, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedCirclePie(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, startAngle, angle, uv, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTexturedQuad
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addTexturedQuad(const Texture& texture, const FloatQuad& quad, const FloatRect& uv, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedQuad(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), quad, uv, color))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	void CRenderer2D_D3D11::addTexturedQuad(const Texture& texture, const FloatQuad& quad, const FloatRect& uv, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedQuad(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), quad, uv, colors))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTexturedRoundRect
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addTexturedRoundRect(const Texture& texture, const FloatRect& rect, const float w, const float h, const float r, const FloatRect& uvRect, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedRoundRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, w, h, r, uvRect, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	void CRenderer2D_D3D11::addTexturedRoundRect(const Texture& texture, const FloatRect& rect, const float w, const float h, const float r, const FloatRect& uvRect, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedRoundRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, w, h, r, uvRect, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleShadow
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircleShadow(const Circle& circle, const float blur, const Float4& color, const bool fill)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleShadow(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), circle, blur, color, m_commandManager.getCurrentRasterScaling(), fill))
		{
			Renderer2DDrawSubmission::SubmitTextureFrom(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, [this]() -> const Texture& { return getShadowTexture(); });
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRectShadow
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addRectShadow(const FloatRect& rect, const float blur, const Float4& color, const bool fill)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectShadow(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, blur, color, fill))
		{
			Renderer2DDrawSubmission::SubmitTextureFrom(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, [this]() -> const Texture& { return getShadowTexture(); });
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRectShadow
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addRoundRectShadow(const RoundRect& roundRect, const float blur, const Float4& color, const bool fill)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectShadow(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), roundRect, blur, color, m_commandManager.getCurrentRasterScaling(), fill))
		{
			Renderer2DDrawSubmission::SubmitTextureFrom(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, [this]() -> const Texture& { return getShadowTexture(); });
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addMesh2D
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addMesh2D(const std::span<const Vertex2D> vertices, const std::span<const TriangleIndex> indices, const Optional<Float2>& offset)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildMesh2D(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, indices, offset))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_D3D11::addMesh2D(const Texture& texture, const std::span<const Vertex2D> vertices, const std::span<const TriangleIndex> indices, const Optional<Float2>& offset)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildMesh2D(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), vertices, indices, offset))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setConstantBuffer
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::setConstantBuffer(const ShaderStage stage, const uint32 slot, const void* data, const size_t size)
	{
		m_commandManager.pushConstantBuffer(stage, slot, data, size);
	}

	uint32 CRenderer2D_D3D11::beginConstantBufferScope(const ShaderStage stage, const uint32 slot, const void* data, const size_t size)
	{
		return m_commandManager.beginConstantBufferScope(stage, slot, data, size);
	}

	void CRenderer2D_D3D11::endConstantBufferScope(const ShaderStage stage, const uint32 slot, const uint32 previous)
	{
		m_commandManager.endConstantBufferScope(stage, slot, previous);
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::flush()
	{
		ScopeExit cleanUp = [this]()
		{
			m_vertexBufferManager2D.reset();
			m_commandManager.reset();
			// 次の実行でもカスタムシェーダを適用する。論理設定は変更しない。
			if (m_currentCustomShader.vs)
			{
				m_commandManager.pushCustomVS(*m_currentCustomShader.vs);
			}
			if (m_currentCustomShader.ps)
			{
				m_commandManager.pushCustomPS(*m_currentCustomShader.ps);
			}
		};

		struct Stat
		{
			uint32 drawCalls = 0;
			uint32 triangleCount = 0;
		} stat;

		m_commandManager.flush();
		m_context->IASetInputLayout(m_inputLayout.Get());
		// 他のレンダラーによる変更を持ち込まないよう、未設定スロットも含めて確立する。
		// 独自定数は、この後に実行するコマンド列の先頭で保存値を再適用する。
		const auto getBuffer = [](IConstantBuffer* buffer) noexcept -> ID3D11Buffer*
		{
			return (buffer ? static_cast<ConstantBuffer_D3D11*>(buffer)->getBuffer() : nullptr);
		};
		const std::array<ID3D11Buffer*, Graphics::ConstantBufferSlotCount> vsBuffers{
			getBuffer(m_vsConstants._base())
		};
		const std::array<ID3D11Buffer*, Graphics::ConstantBufferSlotCount> psBuffers{
			getBuffer(m_psConstants._base()), getBuffer(m_psEffectConstants._base())
		};
		m_context->VSSetConstantBuffers(0, Graphics::ConstantBufferSlotCount, vsBuffers.data());
		m_context->PSSetConstantBuffers(0, Graphics::ConstantBufferSlotCount, psBuffers.data());

		const Size currentRenderTargetSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();
		const Float2 sceneScale = m_commandManager.getSceneScale();
		{
			const D3D11_VIEWPORT viewport = MakeViewport(Point{ 0, 0 }, currentRenderTargetSize);
			m_context->RSSetViewports(1, &viewport);
		}

		m_pRenderer->getBackBuffer().bindSceneTextureAsRenderTarget();
		m_pRenderer->getDepthStencilState().set(DepthStencilState::Default2D);

		LOG_COMMAND("----");

		CommandState commandState;
		commandState.screenMat = Mat3x2::Screen(currentRenderTargetSize);

		for (const auto& command : m_commandManager.getCommands())
		{
			switch (command.type)
			{
			case D3D11Renderer2DCommandType::Null:
				{
					LOG_COMMAND("Null");
					break;
				}
			case D3D11Renderer2DCommandType::SetBuffers:
				{
					m_vertexBufferManager2D.setBuffers();
					LOG_COMMAND(fmt::format("SetBuffers[{}]", command.index));
					break;
				}
			case D3D11Renderer2DCommandType::UpdateBuffers:
				{
					commandState.batchInfo = m_vertexBufferManager2D.commitBuffers(command.index);
					LOG_COMMAND(fmt::format("UpdateBuffers[{}] BatchInfo(indexCount = {}, startIndexLocation = {}, baseVertexLocation = {})",
						command.index, commandState.batchInfo.indexCount, commandState.batchInfo.startIndexLocation, commandState.batchInfo.baseVertexLocation));
					break;
				}
			case D3D11Renderer2DCommandType::Draw:
				{
					m_vsConstants._update_if_dirty();
					m_psConstants._update_if_dirty();
					m_psEffectConstants._update_if_dirty();

					const D3D11DrawCommand& draw = m_commandManager.getDraw(command.index);
					const uint32 indexCount = draw.indexCount;
					const uint32 startIndexLocation = (commandState.batchInfo.startIndexLocation + draw.startIndex);
					const uint32 baseVertexLocation = commandState.batchInfo.baseVertexLocation;

					m_context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
					
					++stat.drawCalls;
					stat.triangleCount += (indexCount / 3);
					LOG_COMMAND(fmt::format("Draw[{}] indexCount = {}, startIndexLocation = {}", command.index, indexCount, startIndexLocation));
					break;
				}
			case D3D11Renderer2DCommandType::SetConstantBuffer:
				{
					const auto& buffers = m_commandManager.getConstantBuffers();
					const auto& cb = buffers.get(command.index);
					if (cb.size == 0)
					{
						ID3D11Buffer* empty = nullptr;
						if (cb.stage == ShaderStage::Vertex)
						{
							m_context->VSSetConstantBuffers(cb.slot, 1, &empty);
						}
						else
						{
							m_context->PSSetConstantBuffers(cb.slot, 1, &empty);
						}
						break;
					}
					auto& destination = m_customConstantBuffers[FromEnum(cb.stage)][cb.slot];
					if (destination.capacity < cb.size)
					{
						const size_t capacity = std::bit_ceil(cb.size);
						destination.buffer = IConstantBuffer::Create(capacity);
						destination.capacity = capacity;
					}

					if (not destination.buffer->_internal_update(buffers.data(cb), cb.size))
					{
						throw InternalEngineError{ "Failed to upload a custom 2D constant buffer" };
					}

					if (cb.stage == ShaderStage::Vertex)
					{
						m_pShader->setConstantBufferVS(cb.slot, destination.buffer.get());
					}
					else
					{
						m_pShader->setConstantBufferPS(cb.slot, destination.buffer.get());
					}
					break;
				}
			case D3D11Renderer2DCommandType::ColorMul:
				{
					const Float4 colorMul = m_commandManager.getColorMul(command.index);
					m_vsConstants->colorMul = colorMul;
					m_psConstants->patternBackgroundColorMul = colorMul;
					LOG_COMMAND(fmt::format("ColorMul[{}] {}", command.index, colorMul));
					break;
				}
			case D3D11Renderer2DCommandType::ColorAdd:
				{
					const Float3 colorAdd = m_commandManager.getColorAdd(command.index);
					m_psConstants->colorAdd.set(colorAdd, 0.0f);
					LOG_COMMAND(fmt::format("ColorAdd[{}] {}", command.index, colorAdd));
					break;
				}
			case D3D11Renderer2DCommandType::PatternParameters:
				{
					const auto& patternParameter = m_commandManager.getPatternParameter(command.index);
					m_psEffectConstants->setPattern(patternParameter);
					LOG_COMMAND(fmt::format("PatternParameters[{}]", command.index));
					break;
				}
			case D3D11Renderer2DCommandType::BlendState:
				{
					const auto& blendState = m_commandManager.getBlendState(command.index);
					m_pRenderer->getBlendState().set(blendState);
					LOG_COMMAND(fmt::format("BlendState[{}]", command.index));
					break;
				}
			case D3D11Renderer2DCommandType::RasterizerState:
				{
					const auto& rasterizerState = m_commandManager.getRasterizerState(command.index);
					commandState.rasterizerState = rasterizerState;

					if (commandState.scissorRect)
					{
						m_pRenderer->getRasterizerState().set(rasterizerState, true);
					}
					else
					{
						m_pRenderer->getRasterizerState().set(rasterizerState, false);
					}

					LOG_COMMAND(fmt::format("RasterizerState[{}]", command.index));
					break;
				}
			case D3D11Renderer2DCommandType::VSSamplerState0:
			case D3D11Renderer2DCommandType::VSSamplerState1:
			case D3D11Renderer2DCommandType::VSSamplerState2:
			case D3D11Renderer2DCommandType::VSSamplerState3:
			case D3D11Renderer2DCommandType::VSSamplerState4:
			case D3D11Renderer2DCommandType::VSSamplerState5:
			case D3D11Renderer2DCommandType::VSSamplerState6:
			case D3D11Renderer2DCommandType::VSSamplerState7:
				{
					const uint32 slot = FromEnum(command.type) - FromEnum(D3D11Renderer2DCommandType::VSSamplerState0);
					const auto& samplerState = m_commandManager.getVSSamplerState(slot, command.index);
					m_pRenderer->getSamplerState().setVS(slot, samplerState);
					LOG_COMMAND(fmt::format("VSSamplerState{}[{}] ", slot, command.index));
					break;
				}
			case D3D11Renderer2DCommandType::PSSamplerState0:
			case D3D11Renderer2DCommandType::PSSamplerState1:
			case D3D11Renderer2DCommandType::PSSamplerState2:
			case D3D11Renderer2DCommandType::PSSamplerState3:
			case D3D11Renderer2DCommandType::PSSamplerState4:
			case D3D11Renderer2DCommandType::PSSamplerState5:
			case D3D11Renderer2DCommandType::PSSamplerState6:
			case D3D11Renderer2DCommandType::PSSamplerState7:
				{
					const uint32 slot = FromEnum(command.type) - FromEnum(D3D11Renderer2DCommandType::PSSamplerState0);
					const auto& samplerState = m_commandManager.getPSSamplerState(slot, command.index);
					m_pRenderer->getSamplerState().setPS(slot, samplerState);
					LOG_COMMAND(fmt::format("PSSamplerState{}[{}] ", slot, command.index));
					break;
				}
			case D3D11Renderer2DCommandType::ScissorRect:
				{
					const auto& scissorRect = m_commandManager.getScissorRect(command.index);
					commandState.scissorRect = scissorRect;

					if (scissorRect)
					{
						m_pRenderer->getRasterizerState().set(commandState.rasterizerState, true);
						m_pRenderer->getRasterizerState().setScissorRect(*scissorRect);
					}
					else
					{
						m_pRenderer->getRasterizerState().set(commandState.rasterizerState, false);
					}

					LOG_COMMAND(fmt::format("ScissorRect[{}] {}", command.index, scissorRect));
					break;
				}
			case D3D11Renderer2DCommandType::Viewport:
				{
					const auto& viewport = m_commandManager.getViewport(command.index);
					
					const D3D11_VIEWPORT vp = (viewport ? MakeViewport(viewport->pos, viewport->size) : MakeViewport(Point{ 0, 0 }, currentRenderTargetSize));
					m_context->RSSetViewports(1, &vp);

					commandState.screenMat = Mat3x2::Screen(vp.Width, vp.Height);
					m_vsConstants->setTransform(commandState.transform * Mat3x3{ commandState.screenMat });

					LOG_COMMAND(fmt::format("Viewport[{}] ({}, {}, {}, {})", command.index, vp.TopLeftX, vp.TopLeftY, vp.Width, vp.Height));
					break;
				}
			case D3D11Renderer2DCommandType::SDFParams:
				{
					const auto& sdfParameter = m_commandManager.getSDFParameters(command.index);
					m_psConstants->setSDFParameters(sdfParameter);
					LOG_COMMAND(fmt::format("SDFParams[{}]", command.index));
					break;
				}
			case D3D11Renderer2DCommandType::SetVS:
				{
					const auto vsID = m_commandManager.getVS(command.index);

					if (vsID == VertexShader::IDType::Invalid())
					{
						m_pShader->setVSNull();
						LOG_COMMAND(fmt::format("SetVS[{}]: null", command.index));
					}
					else
					{
						m_pShader->setVS(vsID);
						LOG_COMMAND(fmt::format("SetVS[{}]: {}", command.index, vsID.value()));
					}

					break;
				}
			case D3D11Renderer2DCommandType::SetPS:
				{
					const auto psID = m_commandManager.getPS(command.index);

					if (psID == PixelShader::IDType::Invalid())
					{
						m_pShader->setPSNull();
						LOG_COMMAND(fmt::format("SetPS[{}]: null", command.index));
					}
					else
					{
						m_pShader->setPS(psID);
						LOG_COMMAND(fmt::format("SetPS[{}]: {}", command.index, psID.value()));
					}

					break;
				}
			case D3D11Renderer2DCommandType::Transform:
				{
					commandState.transform = m_commandManager.getCombinedTransform(command.index);
					// Resolve scene scaling with the target used by this execution, including pending draws.
					if (sceneScale != Float2{ 1.0f, 1.0f })
					{
						commandState.transform = (commandState.transform * Mat3x3{ Mat3x2::Scale(sceneScale) });
					}
					m_vsConstants->setTransform(commandState.transform * Mat3x3{ commandState.screenMat });

					LOG_COMMAND(fmt::format("Transform[{}] {}", command.index, commandState.transform));
					break;
				}
			case D3D11Renderer2DCommandType::VSTexture0:
			case D3D11Renderer2DCommandType::VSTexture1:
			case D3D11Renderer2DCommandType::VSTexture2:
			case D3D11Renderer2DCommandType::VSTexture3:
			case D3D11Renderer2DCommandType::VSTexture4:
			case D3D11Renderer2DCommandType::VSTexture5:
			case D3D11Renderer2DCommandType::VSTexture6:
			case D3D11Renderer2DCommandType::VSTexture7:
				{
					const uint32 slot = (FromEnum(command.type) - FromEnum(D3D11Renderer2DCommandType::VSTexture0));
					const auto& textureID = m_commandManager.getVSTexture(slot, command.index);

					if (textureID.isInvalid())
					{
						ID3D11ShaderResourceView* nullAttach[1] = { nullptr };
						m_context->VSSetShaderResources(slot, 1, nullAttach);
						LOG_COMMAND(fmt::format("VSTexture{}[{}]: null", slot, command.index));
					}
					else
					{
						m_context->VSSetShaderResources(slot, 1, m_pTexture->getSRVPtr(textureID));
						LOG_COMMAND(fmt::format("VSTexture{}[{}]: {}", slot, command.index, textureID.value()));
					}
					
					break;
				}
			case D3D11Renderer2DCommandType::PSTexture0:
			case D3D11Renderer2DCommandType::PSTexture1:
			case D3D11Renderer2DCommandType::PSTexture2:
			case D3D11Renderer2DCommandType::PSTexture3:
			case D3D11Renderer2DCommandType::PSTexture4:
			case D3D11Renderer2DCommandType::PSTexture5:
			case D3D11Renderer2DCommandType::PSTexture6:
			case D3D11Renderer2DCommandType::PSTexture7:
				{
					const uint32 slot = (FromEnum(command.type) - FromEnum(D3D11Renderer2DCommandType::PSTexture0));
					const auto& textureID = m_commandManager.getPSTexture(slot, command.index);

					if (textureID.isInvalid())
					{
						ID3D11ShaderResourceView* nullAttach[1] = { nullptr };
						m_context->PSSetShaderResources(slot, 1, nullAttach);
						LOG_COMMAND(fmt::format("PSTexture{}[{}]: null", slot, command.index));
					}
					else
					{
						m_context->PSSetShaderResources(slot, 1, m_pTexture->getSRVPtr(textureID));
						LOG_COMMAND(fmt::format("PSTexture{}[{}]: {}", slot, command.index, textureID.value()));
					}
					
					break;
				}
			}
		}

		SIV3D_ENGINE(Profiler)->reportStat(ProfilerStat::Renderer2D_DrawCalls, stat.drawCalls);
		SIV3D_ENGINE(Profiler)->reportStat(ProfilerStat::Renderer2D_TriangleCount, stat.triangleCount);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorMul, setColorMul
	//
	////////////////////////////////////////////////////////////////

	Float4 CRenderer2D_D3D11::getColorMul() const
	{
		return m_commandManager.getCurrentColorMul();
	}

	void CRenderer2D_D3D11::setColorMul(const Float4& color)
	{
		m_commandManager.pushColorMul(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorAdd, setColorAdd
	//
	////////////////////////////////////////////////////////////////

	Float3 CRenderer2D_D3D11::getColorAdd() const
	{
		return m_commandManager.getCurrentColorAdd();
	}

	void CRenderer2D_D3D11::setColorAdd(const Float3& color)
	{
		m_commandManager.pushColorAdd(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBlendState, setBlendState
	//
	////////////////////////////////////////////////////////////////

	BlendState CRenderer2D_D3D11::getBlendState() const
	{
		return m_commandManager.getCurrentBlendState();
	}

	void CRenderer2D_D3D11::setBlendState(const BlendState& state)
	{
		m_commandManager.pushBlendState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getRasterizerState, setRasterizerState
	//
	////////////////////////////////////////////////////////////////

	RasterizerState CRenderer2D_D3D11::getRasterizerState() const
	{
		return m_commandManager.getCurrentRasterizerState();
	}

	void CRenderer2D_D3D11::setRasterizerState(const RasterizerState& state)
	{
		m_commandManager.pushRasterizerState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getVSSamplerState, setVSSamplerState
	//
	////////////////////////////////////////////////////////////////

	SamplerState CRenderer2D_D3D11::getVSSamplerState(const uint32 slot) const
	{
		return m_commandManager.getCurrentVSSamplerState(slot);
	}

	void CRenderer2D_D3D11::setVSSamplerState(const uint32 slot, const SamplerState& state)
	{
		m_commandManager.pushVSSamplerState(state, slot);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPSSamplerState, setPSSamplerState
	//
	////////////////////////////////////////////////////////////////

	SamplerState CRenderer2D_D3D11::getPSSamplerState(const uint32 slot) const
	{
		return m_commandManager.getCurrentPSSamplerState(slot);
	}

	void CRenderer2D_D3D11::setPSSamplerState(const uint32 slot, const SamplerState& state)
	{
		m_commandManager.pushPSSamplerState(state, slot);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getScissorRect, setScissorRect
	//
	////////////////////////////////////////////////////////////////

	Optional<Rect> CRenderer2D_D3D11::getScissorRect() const
	{
		return m_commandManager.getCurrentScissorRect();
	}

	void CRenderer2D_D3D11::setScissorRect(const Optional<Rect>& rect)
	{
		m_commandManager.pushScissorRect(rect);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getViewport, setViewport
	//
	////////////////////////////////////////////////////////////////

	Optional<Rect> CRenderer2D_D3D11::getViewport() const
	{
		return m_commandManager.getCurrentViewport();
	}

	void CRenderer2D_D3D11::setViewport(const Optional<Rect>& viewport)
	{
		m_commandManager.pushViewport(viewport);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSDFParameters
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::setSDFParameters(const std::array<Float4, 3>& params)
	{
		m_commandManager.pushSDFParameters(params);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCustomVS, setCustomVS
	//
	////////////////////////////////////////////////////////////////

	Optional<VertexShader> CRenderer2D_D3D11::getCustomVS() const
	{
		return m_currentCustomShader.vs;
	}

	void CRenderer2D_D3D11::setCustomVS(const Optional<VertexShader>& vs)
	{
		if (vs && (not vs->isEmpty()))
		{
			m_currentCustomShader.vs = *vs;
			m_commandManager.pushCustomVS(*vs);
		}
		else
		{
			m_currentCustomShader.vs.reset();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCustomPS, setCustomPS
	//
	////////////////////////////////////////////////////////////////

	Optional<PixelShader> CRenderer2D_D3D11::getCustomPS() const
	{
		return m_currentCustomShader.ps;
	}

	void CRenderer2D_D3D11::setCustomPS(const Optional<PixelShader>& ps)
	{
		if (ps && (not ps->isEmpty()))
		{
			m_currentCustomShader.ps = *ps;
			m_commandManager.pushCustomPS(*ps);
		}
		else
		{
			m_currentCustomShader.ps.reset();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLocalTransform, setLocalTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CRenderer2D_D3D11::getLocalTransform() const
	{
		return m_commandManager.getCurrentLocalTransform();
	}

	void CRenderer2D_D3D11::setLocalTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushLocalTransform(matrix);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCameraTransform, setCameraTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CRenderer2D_D3D11::getCameraTransform() const
	{
		return m_commandManager.getCurrentCameraTransform();
	}

	void CRenderer2D_D3D11::setCameraTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushCameraTransform(matrix);
	}

	const Mat3x3& CRenderer2D_D3D11::getQuadWarpTransform() const
	{
		return m_commandManager.getCurrentQuadWarpTransform();
	}

	void CRenderer2D_D3D11::setQuadWarpTransform(const Mat3x3& matrix)
	{
		m_commandManager.pushQuadWarpTransform(matrix);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getRMSScaling
	//
	////////////////////////////////////////////////////////////////

	float CRenderer2D_D3D11::getRMSScaling() const noexcept
	{
		return m_commandManager.getCurrentRMSScaling();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getShadowTexture
	//
	////////////////////////////////////////////////////////////////

	const Texture& CRenderer2D_D3D11::getShadowTexture() const noexcept
	{
		return *m_shadowTexture;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	Vertex2DBufferPointer CRenderer2D_D3D11::createBuffer(const uint16 vertexCount, const uint32 indexCount)
	{
		return m_vertexBufferManager2D.requestBuffer(vertexCount, indexCount, m_commandManager);
	}
}
