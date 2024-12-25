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

# include "CRenderer2D_Metal.hpp"
# include <Siv3D/Blob.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/Vertex2D.hpp>
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Shader/Metal/CShader_Metal.hpp>
# include <Siv3D/Texture/Metal/CTexture_Metal.hpp>
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
		static constexpr MTL::ScissorRect MakeScissorRect(const Rect& rect) noexcept
		{
			return{
				.x		= static_cast<uint32>(rect.x),
				.y		= static_cast<uint32>(rect.y),
				.width	= static_cast<uint32>(rect.w),
				.height	= static_cast<uint32>(rect.h)
			};
		}
	
		[[nodiscard]]
		static constexpr MTL::Viewport MakeViewport(const Point pos, const Size size) noexcept
		{
			return{
				.originX	= static_cast<double>(pos.x),
				.originY	= static_cast<double>(pos.y),
				.width		= static_cast<double>(size.x),
				.height		= static_cast<double>(size.y),
				.znear		= 0.0,
				.zfar		= 1.0
			};
		}
	}

	PixelShader::IDType CRenderer2D_Metal::EngineShader::getPatternShader(const PatternType pattern) const noexcept
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
		Mat3x2 transform = Mat3x2::Identity();

		Mat3x2 screenMat = Mat3x2::Identity();
		
		uint32 startIndexLocation = 0;
	};

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CRenderer2D_Metal::~CRenderer2D_Metal()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_Metal::~CRenderer2D_Metal()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::init()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_Metal::init()");

		// 各種ポインタを保存
		{
			m_pRenderer	= static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
			m_pShader	= static_cast<CShader_Metal*>(SIV3D_ENGINE(Shader));
			m_pTexture	= static_cast<CTexture_Metal*>(SIV3D_ENGINE(Texture));
			m_device	= m_pRenderer->getDevice();
		}

		m_engineShader.vs					= SIV3D_ENGINE(EngineShader)->getVS(EngineVS::Shape2D).id();
		m_engineShader.psShape				= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::Shape2D).id();
		m_engineShader.psTexture			= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::Texture2D).id();
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
		
		m_vertexBufferManager.init(m_device);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLine
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addLine(const LineCap startCap, const LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, start, end, thickness, colors, getMaxScaling()))
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

	void CRenderer2D_Metal::addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_Metal::createBuffer, this), style, start, end, thickness, colors, getMaxScaling()))
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

	void CRenderer2D_Metal::addTriangle(const Float2(&points)[3], const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, color))
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

	void CRenderer2D_Metal::addTriangle(const Float2(&points)[3], const Float4(&colors)[3])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, colors))
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

	void CRenderer2D_Metal::addTriangle(const Float2(&points)[3], const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, pattern.primaryColor))
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

	void CRenderer2D_Metal::addRect(const FloatRect& rect, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, color))
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

	void CRenderer2D_Metal::addRect(const FloatRect& rect, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, colors))
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

	void CRenderer2D_Metal::addRect(const FloatRect& rect, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, pattern.primaryColor))
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

	void CRenderer2D_Metal::addRectFrame(const FloatRect& innerRect, const float thickness, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), innerRect, thickness, colorType, color0, color1))
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

	void CRenderer2D_Metal::addRectFrame(const FloatRect& innerRect, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), innerRect, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor))
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

	void CRenderer2D_Metal::addCircle(const Float2& center, const float r, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, colorType, color0, color1, getMaxScaling()))
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

	void CRenderer2D_Metal::addCircle(const Float2& center, const float r, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addCircleFrame(const Float2& center, const float rInner, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, rInner, thickness, innerColor, outerColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addCircleFrame(const Float2& center, const float rInner, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, rInner, thickness, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addCirclePie(const Float2& center, const float r, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCirclePie(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, startAngle, angle, innerColor, outerColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addCirclePie(const Float2& center, const float r, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCirclePie(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, startAngle, angle, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addCircleArc(const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_Metal::createBuffer, this), lineCap, center, rInner, startAngle, angle, thickness, colorType, color0, color1, getMaxScaling()))
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

	void CRenderer2D_Metal::addCircleArc(const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_Metal::createBuffer, this), lineCap, center, rInner, startAngle, angle, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addCircleSegment(const Float2& center, const float r, const float startAngle, const float angle, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleSegment(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, startAngle, angle, color, getMaxScaling()))
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

	void CRenderer2D_Metal::addCircleSegment(const Float2& center, const float r, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleSegment(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, startAngle, angle, pattern.primaryColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addEllipse(const Float2& center, const float a, const float b, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipse(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, colorType, color0, color1, getMaxScaling()))
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

	void CRenderer2D_Metal::addEllipse(const Float2& center, const float a, const float b, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipse(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
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
	//	addEllipseFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addEllipseFrame(const Float2& center, const float aInner, const float bInner, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, aInner, bInner, thickness, innerColor, outerColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addEllipseFrame(const Float2& center, const float aInner, const float bInner, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, aInner, bInner, thickness, pattern.primaryColor, pattern.primaryColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addQuad(const FloatQuad& quad, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_Metal::createBuffer, this), quad, color))
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

	void CRenderer2D_Metal::addQuad(const FloatQuad& quad, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_Metal::createBuffer, this), quad, colors))
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

	void CRenderer2D_Metal::addQuad(const FloatQuad& quad, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_Metal::createBuffer, this), quad, pattern.primaryColor))
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
	//	addRoundRect
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRoundRect(const FloatRect& rect, const float r, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, r, color, getMaxScaling()))
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

	void CRenderer2D_Metal::addRoundRect(const FloatRect& rect, const float r, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, r, colorType, color0, color1, getMaxScaling()))
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

	void CRenderer2D_Metal::addRoundRect(const FloatRect& rect, const float r, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, r, pattern.primaryColor, getMaxScaling()))
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
	//	addRoundRectFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			innerRect, innerR, outerRect, outerR, color, getMaxScaling()))
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

	void CRenderer2D_Metal::addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			innerRect, innerR, outerRect, outerR, colorType, color0, color1, getMaxScaling()))
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

	void CRenderer2D_Metal::addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			innerRect, innerR, outerRect, outerR, pattern.primaryColor, getMaxScaling()))
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
	//	addPolygon
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addPolygon(const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, triangleIndices, offset, color))
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

	void CRenderer2D_Metal::addPolygon(const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, triangleIndices, offset, pattern.primaryColor))
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

	void CRenderer2D_Metal::addPolygon(const std::span<const Float2> vertices, const std::span<const Vertex2D::IndexType> indices, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, indices, color))
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

	void CRenderer2D_Metal::addPolygon(const std::span<const Float2> vertices, const std::span<const Vertex2D::IndexType> indices, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, indices, pattern.primaryColor))
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
	//	addLineString
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, points, offset, thickness, inner, closeRing, color, getMaxScaling()))
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

	void CRenderer2D_Metal::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const Float4& colorStart, const Float4& colorEnd)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, points, offset, thickness, inner, colorStart, colorEnd, getMaxScaling()))
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

	void CRenderer2D_Metal::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, points, offset, thickness, inner, closeRing, pattern.primaryColor, getMaxScaling()))
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

	void CRenderer2D_Metal::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const std::span<const ColorF> colors)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, points, offset, thickness, inner, closeRing, colors, getMaxScaling()))
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

	////////////////////////////////////////////////////////////////
	//
	//	addTextureRegion
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addTextureRegion(const Texture& texture, const FloatRect& rect, const FloatRect& uv, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTextureRegion(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, uv, color))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psTexture);
			}

			m_commandManager.pushPSTexture(0, texture);
			m_commandManager.pushDraw(indexCount);
		}
	}


	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::flush()
	{
		ScopeExit cleanUp = [this]()
		{
			m_commandManager.reset();
			m_currentCustomShader.vs.reset();
			m_currentCustomShader.ps.reset();
		};
		
		m_commandManager.flush();
		
		const Size currentRenderTargetSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();

		// Draw2D
		NS::SharedPtr<MTL::RenderPassDescriptor> offscreenRenderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
		MTL::RenderPassColorAttachmentDescriptor* cd = offscreenRenderPassDescriptor->colorAttachments()->object(0);
		
		if (m_pRenderer->getSceneSampleCount() == 1)
		{
			cd->setTexture(m_pRenderer->getSceneTextureNonMSAA().getTexture());
		}
		else
		{
			cd->setTexture(m_pRenderer->getSceneTextureMSAA().getTexture());
			cd->setResolveTexture(m_pRenderer->getSceneTextureNonMSAA().getTexture());
		}

		cd->setLoadAction(MTL::LoadActionClear);
		const ColorF& backgroundColor = m_pRenderer->getSceneStyle().backgroundColor;
		cd->setClearColor(MTL::ClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1));
		cd->setStoreAction((m_pRenderer->getSceneSampleCount() == 1) ? MTL::StoreActionStore : MTL::StoreActionMultisampleResolve);
		
		LOG_COMMAND("----");

		@autoreleasepool
		{
			MTL::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(offscreenRenderPassDescriptor.get());
			
			PipelineStateDesc pipelineStateDesc
			{
				.vs = m_engineShader.vs,
				.ps = m_engineShader.psShape,
				.pixelFormat = static_cast<uint16>(MTL::PixelFormatRGBA8Unorm),
				.sampleCount = static_cast<uint16>(m_pRenderer->getSceneSampleCount()),
				.blendState = BlendState::Default2D,
			};
			
			m_pRenderer->getSamplerState().resetStates();
			
			CommandState commandState;
			commandState.screenMat = Mat3x2::Screen(currentRenderTargetSize);
			
			renderCommandEncoder->setVertexBuffer(m_vertexBufferManager.getVertexBuffer(), 0, 0);
			
			for (const auto& command : m_commandManager.getCommands())
			{
				switch (command.type)
				{
				case MetalRenderer2DCommandType::Null:
					{
						LOG_COMMAND("Null");
						break;
					}
				case MetalRenderer2DCommandType::Draw:
					{
						const auto pipeline = m_pRenderer->getRenderPipelineState().get(pipelineStateDesc);
						renderCommandEncoder->setRenderPipelineState(pipeline);
						
						if (m_vsConstants.isDirty())
						{
							m_vsConstants._update_if_dirty();
							renderCommandEncoder->setVertexBytes(m_vsConstants.data(), m_vsConstants.size(), 1);
						}

						if (m_psConstants.isDirty())
						{
							m_psConstants._update_if_dirty();
							renderCommandEncoder->setFragmentBytes(m_psConstants.data(), m_psConstants.size(), 0);
						}

						if (m_psPatternConstants.isDirty())
						{
							m_psPatternConstants._update_if_dirty();
							renderCommandEncoder->setFragmentBytes(m_psPatternConstants.data(), m_psPatternConstants.size(), 1);
						}

						const MetalDrawCommand& draw = m_commandManager.getDraw(command.index);
						const uint32 indexCount = draw.indexCount;
						
						LOG_COMMAND(fmt::format("Draw[{}] indexCount = {}, startIndexLocation = {}", command.index, indexCount, commandState.startIndexLocation));
						
						// indexBufferOffset, 4 の倍数でなくても大丈夫？
						renderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt16, m_vertexBufferManager.getIndexBuffer(), (sizeof(Vertex2D::IndexType) * commandState.startIndexLocation));
						commandState.startIndexLocation += indexCount;
						
						//++m_stat.drawCalls;
						//m_stat.triangleCount += (indexCount / 3);

						break;
					}
				case MetalRenderer2DCommandType::ColorMul:
					{
						const Float4 colorMul = m_commandManager.getColorMul(command.index);
						m_vsConstants->colorMul = colorMul;
						m_psConstants->patternBackgroundColorMul = colorMul;
						LOG_COMMAND(fmt::format("ColorMul[{}] {}", command.index, colorMul));
						break;
					}
				case MetalRenderer2DCommandType::ColorAdd:
					{
						const Float3 colorAdd = m_commandManager.getColorAdd(command.index);
						m_psConstants->colorAdd.set(colorAdd, 0.0f);
						LOG_COMMAND(fmt::format("ColorAdd[{}] {}", command.index, colorAdd));
						break;
					}
				case MetalRenderer2DCommandType::PatternParameters:
					{
						const auto& patternParameter = m_commandManager.getPatternParameter(command.index);
						*m_psPatternConstants = { { patternParameter[0], patternParameter[1] }, patternParameter[2] };
						LOG_COMMAND(fmt::format("PatternParameters[{}]", command.index));
						break;
					}
				case MetalRenderer2DCommandType::BlendState:
					{
						pipelineStateDesc.blendState = m_commandManager.getBlendState(command.index);
						LOG_COMMAND(fmt::format("BlendState[{}]", command.index));
						break;
					}
				case MetalRenderer2DCommandType::RasterizerState:
					{
						const auto& rasterizerState = m_commandManager.getRasterizerState(command.index);
						
						if (rasterizerState.triangleFillMode == TriangleFillMode::Solid)
						{
							renderCommandEncoder->setTriangleFillMode(MTL::TriangleFillModeFill);
						}
						else
						{
							renderCommandEncoder->setTriangleFillMode(MTL::TriangleFillModeLines);
						}
						
						if (rasterizerState.cullMode == CullMode::None)
						{
							renderCommandEncoder->setCullMode(MTL::CullModeNone);
						}
						else if (rasterizerState.cullMode == CullMode::Front)
						{
							renderCommandEncoder->setCullMode(MTL::CullModeFront);
						}
						else
						{
							renderCommandEncoder->setCullMode(MTL::CullModeBack);
						}

						LOG_COMMAND(fmt::format("RasterizerState[{}]", command.index));
						break;
					}
				case MetalRenderer2DCommandType::VSSamplerState0:
				case MetalRenderer2DCommandType::VSSamplerState1:
				case MetalRenderer2DCommandType::VSSamplerState2:
				case MetalRenderer2DCommandType::VSSamplerState3:
				case MetalRenderer2DCommandType::VSSamplerState4:
				case MetalRenderer2DCommandType::VSSamplerState5:
				case MetalRenderer2DCommandType::VSSamplerState6:
				case MetalRenderer2DCommandType::VSSamplerState7:
					{
						const uint32 slot = FromEnum(command.type) - FromEnum(MetalRenderer2DCommandType::VSSamplerState0);
						const auto& samplerState = m_commandManager.getVSSamplerState(slot, command.index);
						m_pRenderer->getSamplerState().setVS(renderCommandEncoder, slot, samplerState);
						LOG_COMMAND(fmt::format("VSSamplerState{}[{}] ", slot, command.index));
						break;
					}
				case MetalRenderer2DCommandType::PSSamplerState0:
				case MetalRenderer2DCommandType::PSSamplerState1:
				case MetalRenderer2DCommandType::PSSamplerState2:
				case MetalRenderer2DCommandType::PSSamplerState3:
				case MetalRenderer2DCommandType::PSSamplerState4:
				case MetalRenderer2DCommandType::PSSamplerState5:
				case MetalRenderer2DCommandType::PSSamplerState6:
				case MetalRenderer2DCommandType::PSSamplerState7:
					{
						const uint32 slot = FromEnum(command.type) - FromEnum(MetalRenderer2DCommandType::PSSamplerState0);
						const auto& samplerState = m_commandManager.getPSSamplerState(slot, command.index);
						m_pRenderer->getSamplerState().setPS(renderCommandEncoder, slot, samplerState);
						LOG_COMMAND(fmt::format("PSSamplerState{}[{}] ", slot, command.index));
						break;
					}
				case MetalRenderer2DCommandType::ScissorRect:
					{
						const auto& scissorRect = m_commandManager.getScissorRect(command.index);
						const Rect rect = (scissorRect ? scissorRect->clamped(Rect{ 0, 0, currentRenderTargetSize }) : Rect{ currentRenderTargetSize });
						renderCommandEncoder->setScissorRect(MakeScissorRect(rect));
						
						LOG_COMMAND(fmt::format("ScissorRect[{}] {}", command.index, rect));
						break;
					}
				case MetalRenderer2DCommandType::Viewport:
					{
						const auto& viewport = m_commandManager.getViewport(command.index);
						
						const MTL::Viewport vp = (viewport ? MakeViewport(viewport->pos, viewport->size) : MakeViewport(Point{ 0, 0 }, currentRenderTargetSize));
						renderCommandEncoder->setViewport(vp);
	
						commandState.screenMat = Mat3x2::Screen(vp.width, vp.height);
						const Mat3x2 matrix = (commandState.transform * commandState.screenMat);
						m_vsConstants->transform[0].set(matrix._11, matrix._12, matrix._31, matrix._32);
						m_vsConstants->transform[1].set(matrix._21, matrix._22, 0.0f, 1.0f);
						
						LOG_COMMAND(fmt::format("Viewport[{}] ({}, {}, {}, {})", command.index, vp.originX, vp.originY, vp.width, vp.height));
						break;
					}
				case MetalRenderer2DCommandType::SetVS:
					{
						const auto vsID = m_commandManager.getVS(command.index);

						if (vsID == VertexShader::IDType::Invalid())
						{
							//m_pShader->setVSNull();
							LOG_COMMAND(fmt::format("SetVS[{}]: null", command.index));
						}
						else
						{
							pipelineStateDesc.vs = vsID;
							LOG_COMMAND(fmt::format("SetVS[{}]: {}", command.index, vsID.value()));
						}

						break;
					}
				case MetalRenderer2DCommandType::SetPS:
					{
						const auto psID = m_commandManager.getPS(command.index);

						if (psID == PixelShader::IDType::Invalid())
						{
							//m_pShader->setPSNull();
							LOG_COMMAND(fmt::format("SetPS[{}]: null", command.index));
						}
						else
						{
							pipelineStateDesc.ps = psID;
							LOG_COMMAND(fmt::format("SetPS[{}]: {}", command.index, psID.value()));
						}

						break;
					}
				case MetalRenderer2DCommandType::Transform:
					{
						commandState.transform = m_commandManager.getCombinedTransform(command.index);
						const Mat3x2 matrix = (commandState.transform * commandState.screenMat);
						m_vsConstants->transform[0].set(matrix._11, matrix._12, matrix._31, matrix._32);
						m_vsConstants->transform[1].set(matrix._21, matrix._22, 0.0f, 1.0f);

						LOG_COMMAND(U"Transform[{}] {}"_fmt(command.index, matrix));
						break;
					}
				case MetalRenderer2DCommandType::VSTexture0:
				case MetalRenderer2DCommandType::VSTexture1:
				case MetalRenderer2DCommandType::VSTexture2:
				case MetalRenderer2DCommandType::VSTexture3:
				case MetalRenderer2DCommandType::VSTexture4:
				case MetalRenderer2DCommandType::VSTexture5:
				case MetalRenderer2DCommandType::VSTexture6:
				case MetalRenderer2DCommandType::VSTexture7:
					{
						const uint32 slot = (FromEnum(command.type) - FromEnum(MetalRenderer2DCommandType::VSTexture0));
						const auto& textureID = m_commandManager.getVSTexture(slot, command.index);

						if (textureID.isInvalid())
						{
							renderCommandEncoder->setVertexTexture(nullptr, slot);
							LOG_COMMAND(fmt::format("VSTexture{}[{}]: null", slot, command.index));
						}
						else
						{
							const MTL::Texture* texture = m_pTexture->getTexture(textureID);
							renderCommandEncoder->setVertexTexture(texture, slot);
							LOG_COMMAND(fmt::format("VSTexture{}[{}]: {}", slot, command.index, textureID.value()));
						}
						
						break;
					}
				case MetalRenderer2DCommandType::PSTexture0:
				case MetalRenderer2DCommandType::PSTexture1:
				case MetalRenderer2DCommandType::PSTexture2:
				case MetalRenderer2DCommandType::PSTexture3:
				case MetalRenderer2DCommandType::PSTexture4:
				case MetalRenderer2DCommandType::PSTexture5:
				case MetalRenderer2DCommandType::PSTexture6:
				case MetalRenderer2DCommandType::PSTexture7:
					{
						const uint32 slot = (FromEnum(command.type) - FromEnum(MetalRenderer2DCommandType::PSTexture0));
						const auto& textureID = m_commandManager.getPSTexture(slot, command.index);

						if (textureID.isInvalid())
						{
							renderCommandEncoder->setFragmentTexture(nullptr, slot);
							LOG_COMMAND(fmt::format("PSTexture{}[{}]: null", slot, command.index));
						}
						else
						{
							const MTL::Texture* texture = m_pTexture->getTexture(textureID);
							renderCommandEncoder->setFragmentTexture(texture, slot);
							LOG_COMMAND(fmt::format("PSTexture{}[{}]: {}", slot, command.index, textureID.value()));
						}
						
						break;
					}
				}
			}
						
			renderCommandEncoder->endEncoding();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorMul, setColorMul
	//
	////////////////////////////////////////////////////////////////

	Float4 CRenderer2D_Metal::getColorMul() const
	{
		return m_commandManager.getCurrentColorMul();
	}

	void CRenderer2D_Metal::setColorMul(const Float4& color)
	{
		m_commandManager.pushColorMul(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorAdd, setColorAdd
	//
	////////////////////////////////////////////////////////////////

	Float3 CRenderer2D_Metal::getColorAdd() const
	{
		return m_commandManager.getCurrentColorAdd();
	}

	void CRenderer2D_Metal::setColorAdd(const Float3& color)
	{
		m_commandManager.pushColorAdd(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBlendState, setBlendState
	//
	////////////////////////////////////////////////////////////////

	BlendState CRenderer2D_Metal::getBlendState() const
	{
		return m_commandManager.getCurrentBlendState();
	}

	void CRenderer2D_Metal::setBlendState(const BlendState& state)
	{
		m_commandManager.pushBlendState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getRasterizerState, setRasterizerState
	//
	////////////////////////////////////////////////////////////////

	RasterizerState CRenderer2D_Metal::getRasterizerState() const
	{
		return m_commandManager.getCurrentRasterizerState();
	}

	void CRenderer2D_Metal::setRasterizerState(const RasterizerState& state)
	{
		m_commandManager.pushRasterizerState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getVSSamplerState, setVSSamplerState
	//
	////////////////////////////////////////////////////////////////

	SamplerState CRenderer2D_Metal::getVSSamplerState(const uint32 slot) const
	{
		return m_commandManager.getCurrentVSSamplerState(slot);
	}

	void CRenderer2D_Metal::setVSSamplerState(const uint32 slot, const SamplerState& state)
	{
		m_commandManager.pushVSSamplerState(state, slot);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPSSamplerState, setPSSamplerState
	//
	////////////////////////////////////////////////////////////////

	SamplerState CRenderer2D_Metal::getPSSamplerState(const uint32 slot) const
	{
		return m_commandManager.getCurrentPSSamplerState(slot);
	}

	void CRenderer2D_Metal::setPSSamplerState(const uint32 slot, const SamplerState& state)
	{
		m_commandManager.pushPSSamplerState(state, slot);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getScissorRect, setScissorRect
	//
	////////////////////////////////////////////////////////////////

	Optional<Rect> CRenderer2D_Metal::getScissorRect() const
	{
		return m_commandManager.getCurrentScissorRect();
	}

	void CRenderer2D_Metal::setScissorRect(const Optional<Rect>& rect)
	{
		m_commandManager.pushScissorRect(rect);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getViewport, setViewport
	//
	////////////////////////////////////////////////////////////////

	Optional<Rect> CRenderer2D_Metal::getViewport() const
	{
		return m_commandManager.getCurrentViewport();
	}

	void CRenderer2D_Metal::setViewport(const Optional<Rect>& viewport)
	{
		m_commandManager.pushViewport(viewport);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLocalTransform, setLocalTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CRenderer2D_Metal::getLocalTransform() const
	{
		return m_commandManager.getCurrentLocalTransform();
	}

	void CRenderer2D_Metal::setLocalTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushLocalTransform(matrix);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCameraTransform, setCameraTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CRenderer2D_Metal::getCameraTransform() const
	{
		return m_commandManager.getCurrentCameraTransform();
	}

	void CRenderer2D_Metal::setCameraTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushCameraTransform(matrix);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMaxScaling
	//
	////////////////////////////////////////////////////////////////

	float CRenderer2D_Metal::getMaxScaling() const noexcept
	{
		return m_commandManager.getCurrentMaxScaling();
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::beginFrame(MTL::CommandBuffer* commandBuffer)
	{
		m_commandBuffer = commandBuffer;

		m_vertexBufferManager.updateContent();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSemaphore
	//
	////////////////////////////////////////////////////////////////

	dispatch_semaphore_t CRenderer2D_Metal::getSemaphore() const
	{
		return m_vertexBufferManager.getSemaphore();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	Vertex2DBufferPointer CRenderer2D_Metal::createBuffer(const uint16 vertexCount, const uint32 indexCount)
	{
		return m_vertexBufferManager.requestBuffer(vertexCount, indexCount);
	}
}
