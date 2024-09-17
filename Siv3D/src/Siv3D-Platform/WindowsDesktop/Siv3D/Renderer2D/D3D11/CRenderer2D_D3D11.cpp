//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CRenderer2D_D3D11.hpp"
# include <Siv3D/Blob.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
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

	PixelShader::IDType CRenderer2D_D3D11::EngineShader::getPatternShader(const PatternType pattern) const noexcept
	{
		switch (pattern)
		{
		case PatternType::PolkaDot:
			return psPatternPolkaDot;
		case PatternType::Stripe:
			return psPatternStripe;
		case PatternType::Checker:
			return psPatternChecker;
		case PatternType::Grid:
			return psPatternGrid;
		case PatternType::Triangle:
			return psPatternTriangle;
		case PatternType::HexGrid:
			return psPatternHexGrid;
		default:
			return psShape;
		}
	}

	struct CommandState
	{
		BatchInfo2D batchInfo;
		
		RasterizerState rasterizerState = RasterizerState::Default2D;
		
		Optional<Rect> scissorRect;

		Mat3x2 transform = Mat3x2::Identity();

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
			//m_pTexture = static_cast<CTexture_D3D11*>(SIV3D_ENGINE(Texture));
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

		m_engineShader.vs					= SIV3D_ENGINE(EngineShader)->getVS(EngineVS::Shape2D).id();
		m_engineShader.psShape				= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::Shape2D).id();
		m_engineShader.psLineDot			= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::LineDot).id();
		m_engineShader.psLineDash			= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::LineDash).id();
		m_engineShader.psLineLongDash		= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::LineLongDash).id();
		m_engineShader.psLineDashDot		= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::LineDashDot).id();
		m_engineShader.psLineRoundDot		= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::LineRoundDot).id();
		m_engineShader.psPatternPolkaDot	= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::PatternPolkaDot).id();
		m_engineShader.psPatternStripe		= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::PatternStripe).id();
		m_engineShader.psPatternGrid		= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::PatternGrid).id();
		m_engineShader.psPatternChecker		= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::PatternChecker).id();
		m_engineShader.psPatternTriangle	= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::PatternTriangle).id();
		m_engineShader.psPatternHexGrid		= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::PatternHexGrid).id();
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLine
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addLine(const LineCap startCap, const LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), startCap, endCap, start, end, thickness, colors, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), style, start, end, thickness, colors, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				switch (style.type)
				{
				case LineType::Solid:
					m_commandManager.pushEnginePS(m_engineShader.psShape);
					break;
				case LineType::Dotted:
					m_commandManager.pushEnginePS(m_engineShader.psLineDot);
					break;
				case LineType::Dashed:
					m_commandManager.pushEnginePS(m_engineShader.psLineDash);
					break;
				case LineType::LongDash:
					m_commandManager.pushEnginePS(m_engineShader.psLineLongDash);
					break;
				case LineType::DashDot:
					m_commandManager.pushEnginePS(m_engineShader.psLineDashDot);
					break;
				case LineType::RoundDot:
					m_commandManager.pushEnginePS(m_engineShader.psLineRoundDot);
					break;
				}
			}

			m_commandManager.pushDraw(indexCount);
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
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const Float4(&colors)[3])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), points, colors))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), points, pattern.primaryColor))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
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
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addRect(const FloatRect& rect, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, colors))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addRect(const FloatRect& rect, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, pattern.primaryColor))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
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
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addRectFrame(const FloatRect& innerRect, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), innerRect, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircle
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircle(const Float2& center, const float r, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, colorType, color0, color1, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircle(const Float2& center, const float r, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircleFrame(const Float2& center, const float rInner, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, rInner, thickness, innerColor, outerColor, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircleFrame(const Float2& center, const float rInner, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, rInner, thickness, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCirclePie
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCirclePie(const Float2& center, const float r, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCirclePie(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, startAngle, angle, innerColor, outerColor, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addCirclePie(const Float2& center, const float r, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCirclePie(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, startAngle, angle, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleArc
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircleArc(const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), lineCap, center, rInner, startAngle, angle, thickness, colorType, color0, color1, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircleArc(const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), lineCap, center, rInner, startAngle, angle, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleSegment
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircleSegment(const Float2& center, const float r, const float startAngle, const float angle, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleSegment(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, startAngle, angle, color, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircleSegment(const Float2& center, const float r, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleSegment(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, startAngle, angle, pattern.primaryColor, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipse
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addEllipse(const Float2& center, const float a, const float b, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipse(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, colorType, color0, color1, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addEllipse(const Float2& center, const float a, const float b, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipse(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, a, b, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}
			
			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
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
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addQuad(const FloatQuad& quad, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), quad, colors))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addQuad(const FloatQuad& quad, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), quad, pattern.primaryColor))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.getPatternShader(pattern.type));
			}

			m_commandManager.pushPatternParameter(pattern.toFloat4Array(1.0f / getMaxScaling()));

			m_commandManager.pushDraw(indexCount);
		}
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
			m_currentCustomShader.vs.reset();
			m_currentCustomShader.ps.reset();
		};

		m_commandManager.flush();
		m_context->IASetInputLayout(m_inputLayout.Get());
		m_pShader->setConstantBufferVS(0, m_vsConstants._base());
		m_pShader->setConstantBufferPS(0, m_psConstants._base());
		m_pShader->setConstantBufferPS(1, m_psPatternConstants._base());

		const Size currentRenderTargetSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();
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
					m_psPatternConstants._update_if_dirty();

					const D3D11DrawCommand& draw = m_commandManager.getDraw(command.index);
					const uint32 indexCount = draw.indexCount;
					const uint32 startIndexLocation = commandState.batchInfo.startIndexLocation;
					const uint32 baseVertexLocation = commandState.batchInfo.baseVertexLocation;

					m_context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
					commandState.batchInfo.startIndexLocation += indexCount;
					
					//++m_stat.drawCalls;
					//m_stat.triangleCount += (indexCount / 3);
					LOG_COMMAND(fmt::format("Draw[{}] indexCount = {}, startIndexLocation = {}", command.index, indexCount, startIndexLocation));
					break;
				}
			case D3D11Renderer2DCommandType::ColorMul:
				{
					const Float4 colorMul = m_commandManager.getColorMul(command.index);
					m_psConstants->colorMul = colorMul;
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
					*m_psPatternConstants = { { patternParameter[0], patternParameter[1] }, patternParameter[2] };
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
					const Mat3x2 matrix = (commandState.transform * commandState.screenMat);
					m_vsConstants->transform[0].set(matrix._11, matrix._12, matrix._31, matrix._32);
					m_vsConstants->transform[1].set(matrix._21, matrix._22, 0.0f, 1.0f);

					LOG_COMMAND(fmt::format("Viewport[{}] ({}, {}, {}, {})", command.index, vp.TopLeftX, vp.TopLeftY, vp.Width, vp.Height));
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
					const Mat3x2 matrix = (commandState.transform * commandState.screenMat);
					m_vsConstants->transform[0].set(matrix._11, matrix._12, matrix._31, matrix._32);
					m_vsConstants->transform[1].set(matrix._21, matrix._22, 0.0f, 1.0f);

					LOG_COMMAND(U"Transform[{}] {}"_fmt(command.index, matrix));
					break;
				}
			}
		}
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
		// [Siv3D ToDo]
		return SamplerState{};
	}

	void CRenderer2D_D3D11::setVSSamplerState(const uint32 slot, const SamplerState& state)
	{
		// [Siv3D ToDo]
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPSSamplerState, setPSSamplerState
	//
	////////////////////////////////////////////////////////////////

	SamplerState CRenderer2D_D3D11::getPSSamplerState(const uint32 slot) const
	{
		// [Siv3D ToDo]
		return SamplerState{};
	}

	void CRenderer2D_D3D11::setPSSamplerState(const uint32 slot, const SamplerState& state)
	{
		// [Siv3D ToDo]
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

	////////////////////////////////////////////////////////////////
	//
	//	getMaxScaling
	//
	////////////////////////////////////////////////////////////////

	float CRenderer2D_D3D11::getMaxScaling() const noexcept
	{
		return m_commandManager.getCurrentMaxScaling();
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::beginFrame()
	{

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
