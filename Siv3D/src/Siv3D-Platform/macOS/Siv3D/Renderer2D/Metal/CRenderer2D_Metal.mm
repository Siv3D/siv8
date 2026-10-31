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

# include "CRenderer2D_Metal.hpp"
# include <Siv3D/Blob.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/Vertex2D.hpp>
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>
# include <Siv3D/Renderer2D/Renderer2DDrawSubmission.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Shader/Metal/CShader_Metal.hpp>
# include <Siv3D/Texture/Metal/CTexture_Metal.hpp>
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

	struct CommandState
	{
		Mat3x3 transform = Mat3x3::Identity();

		Mat3x2 screenMat = Mat3x2::Identity();
		
		uint32 startIndexLocation = 0;
	};

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

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

		m_engineShader.init(*SIV3D_ENGINE(EngineShader));
		
		m_vertexBufferManager.init(m_device);

		m_shadowTexture = CreateShadowTexture();
	}

	////////////////////////////////////////////////////////////////
	//
	//	addProceduralVertices
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addProceduralVertices(const uint32 vertexCount)
	{
		Renderer2DDrawSubmission::SubmitProcedural(m_commandManager, m_engineShader, m_currentCustomShader, vertexCount);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLine
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addLine(const LineCap startCap, const LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, start, end, thickness, colors, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_Metal::createBuffer, this), style, start, end, thickness, colors, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitStyledLine(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, style);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundDot
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRoundDot(const Float2& start, const Float2& end, const float thickness, const Float2& uRange, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundDot(std::bind_front(&CRenderer2D_Metal::createBuffer, this), start, end, thickness, uRange, color))
		{
			Renderer2DDrawSubmission::SubmitStyledLine(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, LineStyle::RoundDotFlat);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addArrow
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addArrow(LineCap startCap, const Float2& start, const Float2& end, float thickness, const Float2& headSize, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildArrow(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, start, end, thickness, headSize, colors, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
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
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addTriangle(const Float2(&points)[3], const Float4(&colors)[3])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, colors))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addTriangle(const Float2(&points)[3], const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
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
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addRect(const FloatRect& rect, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, colors))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addRect(const FloatRect& rect, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
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
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addRectFrame(const FloatRect& innerRect, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), innerRect, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRectDashedFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRectDashedFrame(const FloatRect& innerRect, const float offset, const float thickness, const float dashRatio, const uint32 dashCount, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectDashedFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), innerRect, offset, thickness, dashRatio, dashCount, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
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
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addQuad(const FloatQuad& quad, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_Metal::createBuffer, this), quad, colors))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addQuad(const FloatQuad& quad, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_Metal::createBuffer, this), quad, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircle
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCircle(const Float2& center, const float r, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addCircle(const Float2& center, const float r, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCircleFrame(const Float2& center, const float rInner, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, rInner, thickness, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addCircleFrame(const Float2& center, const float rInner, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, rInner, thickness, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleDashedFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCircleDashedFrame(const Float2& center, const float rInner, const float startAngle, const float thickness, const float dashRatio, const uint32 dashCount, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleDashedFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, rInner, startAngle, thickness, dashRatio, dashCount, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCirclePie
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCirclePie(const Float2& center, const float r, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCirclePie(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, startAngle, angle, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addCirclePie(const Float2& center, const float r, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCirclePie(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, startAngle, angle, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleArc
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCircleArc(const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_Metal::createBuffer, this), lineCap, center, rInner, startAngle, angle, thickness, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addCircleArc(const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_Metal::createBuffer, this), lineCap, center, rInner, startAngle, angle, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_Metal::addCircleArc(const LineCap startCap, const LineCap endCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, center, rInner, startAngle, angle, thickness, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addCircleArc(const LineCap startCap, const LineCap endCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, center, rInner, startAngle, angle, thickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleSegment
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCircleSegment(const Float2& center, const float r, const float startAngle, const float angle, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleSegment(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, startAngle, angle, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addCircleSegment(const Float2& center, const float r, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleSegment(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, startAngle, angle, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleCrescent
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCircleCrescent(const Circle& circle, const double openingAngle, const double width, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleCrescent(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			circle, openingAngle, width, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleTeardrop
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCircleTeardrop(const Circle& circle, const Vec2& tip, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleTeardrop(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			circle, tip, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipse
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addEllipse(const Float2& center, const float a, const float b, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipse(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addEllipse(const Float2& center, const float a, const float b, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipse(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipseFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addEllipseFrame(const Float2& center, const float a, const float b, const float innerThickness, const float outerThickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, innerThickness, outerThickness, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addEllipseFrame(const Float2& center, const float a, const float b, const float innerThickness, const float outerThickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, innerThickness, outerThickness, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipseDashedFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addEllipseDashedFrame(const Float2& center, const float a, const float b, const float innerThickness, const float outerThickness, const float offset, const float dashRatio, const uint32 dashCount, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseDashedFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, innerThickness, outerThickness, offset, dashRatio, dashCount, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipseRotated
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addEllipseRotated(const Float2& center, const float a, const float b, const float rotation, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseRotated(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, rotation, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addEllipseRotated(const Float2& center, const float a, const float b, const float rotation, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseRotated(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, rotation, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipseFrameRotated
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addEllipseFrameRotated(const Float2& center, const float a, const float b, const float rotation, const float innerThickness, const float outerThickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrameRotated(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, rotation, innerThickness, outerThickness, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addEllipseFrameRotated(const Float2& center, const float a, const float b, const float rotation, const float innerThickness, const float outerThickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrameRotated(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, rotation, innerThickness, outerThickness, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipsePie
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addEllipsePie(const Float2& center, const float a, const float b, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipsePie(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, startAngle, angle, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addEllipsePie(const Float2& center, const float a, const float b, const float startAngle, const float angle, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipsePie(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, startAngle, angle, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addEllipseArc
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addEllipseArc(const LineCap startCap, const LineCap endCap, const Float2& center, const float a, const float b, const float startAngle, const float angle, const float innerThickness, const float outerThickness, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseArc(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, center, a, b, startAngle, angle, innerThickness, outerThickness, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addEllipseArc(const LineCap startCap, const LineCap endCap, const Float2& center, const float a, const float b, const float startAngle, const float angle, const float innerThickness, const float outerThickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseArc(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, center, a, b, startAngle, angle, innerThickness, outerThickness, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addArcStyled
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addArcStyled(const LineStyle& style, const Float2& center, const float a, const float b,
		const float startAngle, const float angle, const float innerThickness, const float outerThickness, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildArcStyled(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			style, center, a, b, startAngle, angle, innerThickness, outerThickness, color, m_commandManager.getCurrentRasterScaling()))
		{
			if (style.type == LinePattern::RoundDot)
				Renderer2DDrawSubmission::SubmitStyledLine(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, LineStyle::RoundDotFlat);
			else Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSuperEllipse
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addSuperEllipse(const Float2& center, const float a, const float b, const float n, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildSuperEllipse(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, n, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addSuperEllipse(const Float2& center, const float a, const float b, const float n, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildSuperEllipse(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, a, b, n, ColorFillDirection::InOut, pattern.primaryColor, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRect
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRoundRect(const FloatRect& rect, const float r, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, r, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addRoundRect(const FloatRect& rect, const float r, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, r, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addRoundRect(const FloatRect& rect, const float r, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, r, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_Metal::addRoundRect(const FloatRect& rect, const std::array<float, 4>& radii, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, radii, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
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
			innerRect, innerR, outerRect, outerR, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			innerRect, innerR, outerRect, outerR, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			innerRect, innerR, outerRect, outerR, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_Metal::addRoundRectFrame(const FloatRect& innerRect, const std::array<float, 4>& innerRadii, const FloatRect& outerRect, const std::array<float, 4>& outerRadii, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			innerRect, innerRadii, outerRect, outerRadii, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRectFrameStyled
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRoundRectFrameStyled(const LineStyle& style, const FloatRect& innerRect, const float innerR,
		const FloatRect& outerRect, const float outerR, const float thickness, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectFrameStyled(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			style, innerRect, innerR, outerRect, outerR, thickness, color, m_commandManager.getCurrentRasterScaling()))
		{
			if (style.type == LinePattern::RoundDot)
				Renderer2DDrawSubmission::SubmitStyledLine(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, LineStyle::RoundDotFlat);
			else Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRectDashedFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRoundRectDashedFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const float offset, const float dashRatio, const uint32 dashCount, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectDashedFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			innerRect, innerR, outerRect, outerR, offset, dashRatio, dashCount, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRectBalloon
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRoundRectBalloon(const detail::RoundRectBalloon::Placement& balloon, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectBalloon(std::bind_front(&CRenderer2D_Metal::createBuffer, this), balloon, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRectBalloonFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRoundRectBalloonFrame(const detail::RoundRectBalloon::Placement& balloon, const float thickness, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectBalloonFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), balloon, thickness, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
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
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addPolygon(const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, triangleIndices, offset, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_Metal::addPolygon(const std::span<const Float2> vertices, const std::span<const Vertex2D::IndexType> indices, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, indices, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addPolygon(const std::span<const Float2> vertices, const std::span<const Vertex2D::IndexType> indices, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, indices, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPolygonTransformed
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addPolygonTransformed(const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const float s, const float c, const Float2& offset, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygonTransformed(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, triangleIndices, s, c, offset, color))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addPolygonTransformed(const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const float s, const float c, const Float2& offset, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygonTransformed(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, triangleIndices, s, c, offset, pattern.primaryColor))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addShape2DFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addShape2DFrame(const std::span<const Float2> vertices, const Optional<Float2>& offset, const float thickness, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildShape2DFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, offset, thickness, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addShape2DFrame(const std::span<const Float2> vertices, const Optional<Float2>& offset, const float thickness, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildShape2DFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, offset, thickness, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	tryAddSmallConvexFrame
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer2D_Metal::tryAddSmallConvexFrame(const std::span<const Vec2> points, const JoinStyle joinStyle, const double innerThickness, const double outerThickness, const Float4& color)
	{
		const auto result = Vertex2DBuilder::TryBuildSmallConvexFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, joinStyle, innerThickness, outerThickness, color, m_commandManager.getCurrentRasterScaling());
		if (not result) return false;
		if (const auto indexCount = *result)
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
		return true;
	}

	bool CRenderer2D_Metal::tryAddSmallConvexFrame(const std::span<const Vec2> points, const JoinStyle joinStyle, const double innerThickness, const double outerThickness, const PatternParameters& pattern)
	{
		const auto result = Vertex2DBuilder::TryBuildSmallConvexFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, joinStyle, innerThickness, outerThickness, pattern.primaryColor, m_commandManager.getCurrentRasterScaling());
		if (not result) return false;
		if (const auto indexCount = *result)
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLineString
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, points, offset, thickness, inner, closeRing, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const Float4& colorStart, const Float4& colorEnd)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, points, offset, thickness, inner, colorStart, colorEnd, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const PatternParameters& pattern)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, points, offset, thickness, inner, closeRing, pattern.primaryColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitPattern(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, pattern);
		}
	}

	void CRenderer2D_Metal::addLineString(const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const std::span<const ColorF> colors)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLineString(std::bind_front(&CRenderer2D_Metal::createBuffer, this), startCap, endCap, points, offset, thickness, inner, closeRing, colors, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLineStringArrow
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer2D_Metal::addLineStringArrow(const LineCap startCap, const std::span<const Vec2> points,
		const Float2& tip, const float thickness, const float headWidth, const Float4& color)
	{
		const auto count = Vertex2DBuilder::BuildLineStringArrow(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			startCap, points, tip, thickness, headWidth, color, m_commandManager.getCurrentRasterScaling());
		if (not count) return false;
		if (*count) Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, *count);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLineStringDoubleHeadedArrow
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer2D_Metal::addLineStringDoubleHeadedArrow(const std::span<const Vec2> points, const Float2& startTip,
		const Float2& endTip, const float thickness, const float startHeadWidth, const float endHeadWidth, const Float4& color)
	{
		const auto count = Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(std::bind_front(&CRenderer2D_Metal::createBuffer, this),
			points, startTip, endTip, thickness, startHeadWidth, endHeadWidth, color, m_commandManager.getCurrentRasterScaling());
		if (not count) return false;
		if (*count) Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, *count);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTexturedQuad
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addTexturedQuad(const Texture& texture, const FloatQuad& quad, const FloatRect& uv, const Float4& color)
	{
		checkRenderTargetFeedback(texture);
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedQuad(std::bind_front(&CRenderer2D_Metal::createBuffer, this), quad, uv, color))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	void CRenderer2D_Metal::addTexturedQuad(const Texture& texture, const FloatQuad& quad, const FloatRect& uv, const Float4(&colors)[4])
	{
		checkRenderTargetFeedback(texture);
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedQuad(std::bind_front(&CRenderer2D_Metal::createBuffer, this), quad, uv, colors))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTexturedCircle
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addTexturedCircle(const Texture& texture, const Circle& circle, const FloatRect& uv, const Float4& color)
	{
		checkRenderTargetFeedback(texture);
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedCircle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), circle, uv, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	void CRenderer2D_Metal::addTexturedCircle(const Texture& texture, const Circle& circle, const FloatRect& uv, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		checkRenderTargetFeedback(texture);
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedCircle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), circle, uv, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTexturedCirclePie
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addTexturedCirclePie(const Texture& texture, const Float2& center, const float r, const float startAngle, const float angle, const FloatRect& uv, const Float4& innerColor, const Float4& outerColor)
	{
		checkRenderTargetFeedback(texture);
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedCirclePie(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, startAngle, angle, uv, innerColor, outerColor, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTexturedRoundRect
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addTexturedRoundRect(const Texture& texture, const FloatRect& rect, const float w, const float h, const float r, const FloatRect& uvRect, const Float4& color)
	{
		checkRenderTargetFeedback(texture);
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedRoundRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, w, h, r, uvRect, color, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	void CRenderer2D_Metal::addTexturedRoundRect(const Texture& texture, const FloatRect& rect, const float w, const float h, const float r, const FloatRect& uvRect, const Float4& color0, const Float4& color1, const ColorFillDirection colorType)
	{
		checkRenderTargetFeedback(texture);
		if (const auto indexCount = Vertex2DBuilder::BuildTexturedRoundRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, w, h, r, uvRect, colorType, color0, color1, m_commandManager.getCurrentRasterScaling()))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRectShadow
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRectShadow(const FloatRect& rect, const float blur, const Float4& color, const bool fill)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectShadow(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, blur, color, fill))
		{
			Renderer2DDrawSubmission::SubmitTextureFrom(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, [this]() -> const Texture& { return getShadowTexture(); });
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleShadow
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCircleShadow(const Circle& circle, const float blur, const Float4& color, const bool fill)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleShadow(std::bind_front(&CRenderer2D_Metal::createBuffer, this), circle, blur, color, m_commandManager.getCurrentRasterScaling(), fill))
		{
			Renderer2DDrawSubmission::SubmitTextureFrom(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, [this]() -> const Texture& { return getShadowTexture(); });
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundRectShadow
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRoundRectShadow(const RoundRect& roundRect, const float blur, const Float4& color, const bool fill)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRectShadow(std::bind_front(&CRenderer2D_Metal::createBuffer, this), roundRect, blur, color, m_commandManager.getCurrentRasterScaling(), fill))
		{
			Renderer2DDrawSubmission::SubmitTextureFrom(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, [this]() -> const Texture& { return getShadowTexture(); });
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addMesh2D
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addMesh2D(const std::span<const Vertex2D> vertices, const std::span<const TriangleIndex> indices, const Optional<Float2>& offset)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildMesh2D(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, indices, offset))
		{
			Renderer2DDrawSubmission::SubmitShape(m_commandManager, m_engineShader, m_currentCustomShader, indexCount);
		}
	}

	void CRenderer2D_Metal::addMesh2D(const Texture& texture, const std::span<const Vertex2D> vertices, const std::span<const TriangleIndex> indices, const Optional<Float2>& offset)
	{
		checkRenderTargetFeedback(texture);
		if (const auto indexCount = Vertex2DBuilder::BuildMesh2D(std::bind_front(&CRenderer2D_Metal::createBuffer, this), vertices, indices, offset))
		{
			Renderer2DDrawSubmission::SubmitTexture(m_commandManager, m_engineShader, m_currentCustomShader, indexCount, texture);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setConstantBuffer
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::setConstantBuffer(const ShaderStage stage, const uint32 slot, const void* data, const size_t size)
	{
		m_commandManager.pushConstantBuffer(stage, slot, data, size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginConstantBufferScope, endConstantBufferScope
	//
	////////////////////////////////////////////////////////////////

	uint32 CRenderer2D_Metal::beginConstantBufferScope(const ShaderStage stage, const uint32 slot, const void* data, const size_t size)
	{
		return m_commandManager.beginConstantBufferScope(stage, slot, data, size);
	}

	void CRenderer2D_Metal::endConstantBufferScope(const ShaderStage stage, const uint32 slot, const uint32 previous)
	{
		m_commandManager.endConstantBufferScope(stage, slot, previous);
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::flush()
	{
		flush(false);
	}

	void CRenderer2D_Metal::flush(const bool finalScenePass)
	{
		// A scope may span System::Update(). Present still needs a completed Scene.
		if (finalScenePass && m_renderTarget)
		{
			flush(false);
			const auto target = m_renderTarget;
			const ScopeExit restore{ [&] { setRenderTargetState(target); } };
			setRenderTargetState(none);
			flush(true);
			return;
		}
		ScopeExit cleanUp = [this]()
		{
			// 成否にかかわらず、この実行のコマンドと対応するインデックス区間を消費する。
			m_indexReadPos = m_vertexBufferManager.indexCount();
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

		// 描画がなくても、そのシーンの最初のパスでは背景をクリアする。
		if ((m_renderTarget || m_scenePassStarted) && not m_commandManager.hasDraws())
		{
			return;
		}
		
		const Size currentRenderTargetSize = m_renderTarget ? m_renderTarget->size() : m_pRenderer->getSceneBufferSize();
		const Float2 sceneScale = m_commandManager.getSceneScale();

		// Draw2D
		NS::SharedPtr<MTL::RenderPassDescriptor> offscreenRenderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
		MTL::RenderPassColorAttachmentDescriptor* cd = offscreenRenderPassDescriptor->colorAttachments()->object(0);
		
		if (m_renderTarget)
		{
			cd->setTexture(m_pTexture->getTexture(m_renderTarget->id()));
		}
		else if (m_pRenderer->getSceneSampleCount() == 1)
		{
			cd->setTexture(m_pRenderer->getSceneTextureNonMSAA().getTexture());
		}
		else
		{
			cd->setTexture(m_pRenderer->getSceneTextureMSAA().getTexture());
			cd->setResolveTexture(m_pRenderer->getSceneTextureNonMSAA().getTexture());
		}

		cd->setLoadAction((m_renderTarget || m_scenePassStarted) ? MTL::LoadActionLoad : MTL::LoadActionClear);
		const ColorF& backgroundColor = m_pRenderer->getSceneStyle().getFrameBackgroundColor();
		cd->setClearColor(MTL::ClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1));
		cd->setStoreAction((m_renderTarget || (m_pRenderer->getSceneSampleCount() == 1)) ? MTL::StoreActionStore
			: (finalScenePass ? MTL::StoreActionMultisampleResolve : MTL::StoreActionStoreAndMultisampleResolve));
		
		LOG_COMMAND("----");

		@autoreleasepool
		{
			MTL::CommandBuffer* commandBuffer = m_pRenderer->getFrameContext().getCommandBuffer();
			MTL::RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(offscreenRenderPassDescriptor.get());
			if (not renderCommandEncoder)
			{
				throw InternalEngineError{ "MTL::CommandBuffer::renderCommandEncoder() failed" };
			}
			if (not m_renderTarget) { m_scenePassStarted = true; }
			// パイプライン生成などで例外が発生した場合もエンコードを終了する。
			const ScopeExit endEncoding{ [renderCommandEncoder]() noexcept
			{
				renderCommandEncoder->endEncoding();
			} };
			
			PipelineStateDesc pipelineStateDesc
			{
				.vs = m_engineShader.vsShape,
				.ps = m_engineShader.psShape,
				.pixelFormat = static_cast<uint16>(cd->texture()->pixelFormat()),
				.sampleCount = static_cast<uint16>(cd->texture()->sampleCount()),
				.blendState = BlendState::Default2D,
			};
			
			// pixelFormat と sampleCount はこの encoder 内で固定。最初の Draw では必ず設定する。
			bool pipelineDirty = true;

			m_pRenderer->getSamplerState().resetStates();
			
			CommandState commandState;
			commandState.screenMat = Mat3x2::Screen(currentRenderTargetSize);
			commandState.startIndexLocation = m_indexReadPos;
			
			renderCommandEncoder->setVertexBuffer(m_vertexBufferManager.getVertexBuffer(), 0, 0);

			// 通常 Draw に関数呼び出しを追加せず、両描画の状態準備を共通化する。
			const auto prepareDraw = [&] SIV3D_ALWAYS_INLINE
			{
				// 変換・色・テクスチャ・頂点区間だけの変更では、同じパイプラインを再利用する。
				if (pipelineDirty)
				{
					const auto pipeline = m_pRenderer->getRenderPipelineState().get(pipelineStateDesc);
					renderCommandEncoder->setRenderPipelineState(pipeline);
					pipelineDirty = false;
				}

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

				if (m_psEffectConstants.isDirty())
				{
					m_psEffectConstants._update_if_dirty();
					renderCommandEncoder->setFragmentBytes(m_psEffectConstants.data(), m_psEffectConstants.size(), 1);
				}
			};
			
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
						prepareDraw();

						const MetalDrawCommand& draw = m_commandManager.getDraw(command.index);
						const uint32 indexCount = draw.indexCount;
						
						LOG_COMMAND(fmt::format("Draw[{}] indexCount = {}, startIndexLocation = {}, baseVertex = {}", command.index, indexCount, commandState.startIndexLocation, draw.baseVertex));
						
						// インデックスバッファ内のバイト位置と、頂点区間の基準位置を別々に指定する
						renderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt16, m_vertexBufferManager.getIndexBuffer(),
							(sizeof(Vertex2D::IndexType) * commandState.startIndexLocation), 1, draw.baseVertex, 0);
						commandState.startIndexLocation += indexCount;
						
						++stat.drawCalls;
						stat.triangleCount += (indexCount / 3);

						break;
					}
				case MetalRenderer2DCommandType::DrawProcedural:
					{
						prepareDraw();
						const uint32 vertexCount = m_commandManager.getProceduralDraw(command.index);
						renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger{ 0 }, vertexCount);

						++stat.drawCalls;
						stat.triangleCount += (vertexCount / 3);
						LOG_COMMAND(fmt::format("DrawProcedural[{}] vertexCount = {}", command.index, vertexCount));
						break;
					}
				case MetalRenderer2DCommandType::SetConstantBuffer:
					{
						const auto& buffers = m_commandManager.getConstantBuffers();
						const auto& cb = buffers.get(command.index);
						bindCustomConstantBuffer(renderCommandEncoder, cb, buffers.data(cb));
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
						m_psEffectConstants->setPattern(patternParameter);
						LOG_COMMAND(fmt::format("PatternParameters[{}]", command.index));
						break;
					}
				case MetalRenderer2DCommandType::BlendState:
					{
						pipelineStateDesc.blendState = m_commandManager.getBlendState(command.index);
						pipelineDirty = true;
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
						m_vsConstants->setTransform(commandState.transform * Mat3x3{ commandState.screenMat });
						
						LOG_COMMAND(fmt::format("Viewport[{}] ({}, {}, {}, {})", command.index, vp.originX, vp.originY, vp.width, vp.height));
						break;
					}
				case MetalRenderer2DCommandType::SDFParams:
					{
						const auto& sdfParameter = m_commandManager.getSDFParameters(command.index);
						m_psConstants->setSDFParameters(sdfParameter);
						LOG_COMMAND(fmt::format("SDFParams[{}]", command.index));
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
							pipelineDirty = true;
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
							pipelineDirty = true;
							LOG_COMMAND(fmt::format("SetPS[{}]: {}", command.index, psID.value()));
						}

						break;
					}
				case MetalRenderer2DCommandType::Transform:
					{
						commandState.transform = m_commandManager.getCombinedTransform(command.index);
						// Resolve scene scaling with the target used by this execution, including pending draws.
						if (sceneScale != Float2{ 1.0f, 1.0f })
						{
							commandState.transform = (commandState.transform * Mat3x3{ Mat3x2::Scale(sceneScale) });
						}
						m_vsConstants->setTransform(commandState.transform * Mat3x3{ commandState.screenMat });

						LOG_COMMAND(U"Transform[{}] {}"_fmt(command.index, commandState.transform));
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
		}

		SIV3D_ENGINE(Profiler)->reportStat(ProfilerStat::Renderer2D_DrawCalls, stat.drawCalls);
		SIV3D_ENGINE(Profiler)->reportStat(ProfilerStat::Renderer2D_TriangleCount, stat.triangleCount);
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
	//	setSDFParameters
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::setSDFParameters(const std::array<Float4, 3>& params)
	{
		m_commandManager.pushSDFParameters(params);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCustomVS, setCustomVS
	//
	////////////////////////////////////////////////////////////////

	Optional<VertexShader> CRenderer2D_Metal::getCustomVS() const
	{
		return m_currentCustomShader.vs;
	}

	void CRenderer2D_Metal::setCustomVS(const Optional<VertexShader>& vs)
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

	Optional<PixelShader> CRenderer2D_Metal::getCustomPS() const
	{
		return m_currentCustomShader.ps;
	}

	void CRenderer2D_Metal::setCustomPS(const Optional<PixelShader>& ps)
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
	//	getQuadWarpTransform, setQuadWarpTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x3& CRenderer2D_Metal::getQuadWarpTransform() const
	{
		return m_commandManager.getCurrentQuadWarpTransform();
	}

	void CRenderer2D_Metal::setQuadWarpTransform(const Mat3x3& matrix)
	{
		m_commandManager.pushQuadWarpTransform(matrix);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getRMSScaling
	//
	////////////////////////////////////////////////////////////////

	float CRenderer2D_Metal::getRMSScaling() const noexcept
	{
		return m_commandManager.getCurrentRMSScaling();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getShadowTexture
	//
	////////////////////////////////////////////////////////////////

	const Texture& CRenderer2D_Metal::getShadowTexture() const noexcept
	{
		return *m_shadowTexture;	
	}

	////////////////////////////////////////////////////////////////
	//
	//	prepareFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::prepareFrame(const size_t frameIndex)
	{
		m_vertexBufferManager.prepareFrame(frameIndex);
		m_constantBufferFrameIndex = frameIndex;
		m_constantBufferFrames[frameIndex].pageIndex = 0;
		m_constantBufferFrames[frameIndex].offset = 0;
		m_indexReadPos = 0;
		m_scenePassStarted = false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	createBuffer
	//
	////////////////////////////////////////////////////////////////

	Vertex2DBufferPointer CRenderer2D_Metal::createBuffer(const uint16 vertexCount, const uint32 indexCount)
	{
		const auto buffer = m_vertexBufferManager.requestBuffer(vertexCount, indexCount);
		if (buffer.pVertex)
		{
			m_commandManager.pushBaseVertex(m_vertexBufferManager.getBaseVertex());
		}
		return buffer;
	}

	////////////////////////////////////////////////////////////////
	//
	//	bindCustomConstantBuffer
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::bindCustomConstantBuffer(MTL::RenderCommandEncoder* encoder, const ConstantBuffer2DCommand& command, const void* data)
	{
		if (command.size == 0)
		{
			if (command.stage == ShaderStage::Vertex)
			{
				encoder->setVertexBuffer(nullptr, 0, command.slot);
			}
			else
			{
				encoder->setFragmentBuffer(nullptr, 0, command.slot);
			}
			return;
		}

		if (command.size <= 4096)
		{
			if (command.stage == ShaderStage::Vertex)
			{
				encoder->setVertexBytes(data, command.size, command.slot);
			}
			else
			{
				encoder->setFragmentBytes(data, command.size, command.slot);
			}
			return;
		}

		constexpr size_t PageSize = (64 * 1024);
		// Intel Mac を含む constant アドレス空間のオフセット要件を満たす。
		constexpr size_t OffsetAlignment = 256;
		auto& frame = m_constantBufferFrames[m_constantBufferFrameIndex];
		if (PageSize < (frame.offset + command.size))
		{
			++frame.pageIndex;
			frame.offset = 0;
		}

		if (frame.pageIndex == frame.pages.size())
		{
			auto page = NS::TransferPtr(m_device->newBuffer(PageSize,
				(MTL::ResourceStorageModeShared | MTL::ResourceCPUCacheModeWriteCombined)));
			if (not page)
			{
				throw InternalEngineError{ "Failed to allocate a custom 2D constant buffer upload page" };
			}
			frame.pages.push_back(std::move(page));
		}

		MTL::Buffer* buffer = frame.pages[frame.pageIndex].get();
		std::memcpy((static_cast<std::byte*>(buffer->contents()) + frame.offset), data, command.size);
		if (command.stage == ShaderStage::Vertex)
		{
			encoder->setVertexBuffer(buffer, frame.offset, command.slot);
		}
		else
		{
			encoder->setFragmentBuffer(buffer, frame.offset, command.slot);
		}
		// flush() では巻き戻さない。GPU 完了後の prepareFrame() でのみ再利用する。
		frame.offset += ((command.size + OffsetAlignment - 1) & ~(OffsetAlignment - 1));
	}
}
