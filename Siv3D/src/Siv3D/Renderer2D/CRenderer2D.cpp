//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <algorithm>
# include <cmath>
# include "IRenderer2D.hpp"
# include "CircleArcArrow.hpp"
# include "SuperEllipseTessellation.hpp"
# include <Siv3D/Image.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/LineCap.hpp>
# include <Siv3D/Resource.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Geometry2D/BezierTessellation.hpp>
# include <Siv3D/Geometry2D/PathArrowGeometry.hpp>
# include <Siv3D/Geometry2D/DoubleHeadedArrowGeometry.hpp>

namespace s3d::detail::SuperEllipseTessellation
{
	namespace
	{
		constexpr uint32 MaxDepth = 16;

		// On a quadrant, u^n + v^n = 1 is convex or concave. Its unique
		// maximum chord deviation has gradient perpendicular to the chord:
		// u/v = (dv / -du)^(1/(n-1)). Work with ratios <= 1.
		Vec2 FurthestPoint(const Vec2 from, const Vec2 to, const double n,
			const double inverseN, const double inverseGradientExponent)
		{
			const double dx = (from.x - to.x), dy = (to.y - from.y);
			if (n == 2.0)
			{
				return Vec2{ dy, dx } / std::sqrt(dx * dx + dy * dy);
			}
			if (n == 0.5)
			{
				const double inverseSum = (1.0 / (dx + dy));
				const Vec2 root{ dx * inverseSum, dy * inverseSum };
				return (root * root);
			}
			const double ratio = (std::min(dx, dy) / std::max(dx, dy));
			const double power = std::pow(ratio, ((1.0 < n) ? inverseGradientExponent : (n * inverseGradientExponent)));
			const double smaller = ((1.0 < n) ? power : (power * ratio));
			const double sum = (1.0 + ((1.0 < n) ? (power * ratio) : power));
			const double larger = ((n == 4.0) ? (1.0 / std::sqrt(std::sqrt(sum))) : std::pow(sum, -inverseN));
			return ((dy >= dx) == (1.0 < n))
				? Vec2{ larger, smaller * larger } : Vec2{ smaller * larger, larger };
		}
	}

	void Tessellate(Quarter& output, const double a, const double b, const double n,
		const double scale, const double strokeMargin)
	{
		output.limited = false;
		if (n == 1.0)
		{
			output.points[0] = { a, 0 };
			output.points[1] = { 0, b };
			output.count = 2;
			return;
		}

		const double inverseN = (1.0 / n);
		const double inverseGradientExponent = (1.0 / std::abs(n - 1.0));
		const double diagonal = std::pow(0.5, inverseN);
		const double metricA = ((a + strokeMargin) * scale), metricB = ((b + strokeMargin) * scale);
		const double aa = (metricA * metricA), bb = (metricB * metricB);
		struct Segment
		{
			uint16 from, to;
			uint32 depth;
			Vec2 middle;
			double errorSq;
		};
		std::array<Segment, MaxOctantSegments> pending;
		size_t pendingCount = 0;
		output.points[0] = { 1, 0 };
		output.points[1] = { diagonal, diagonal };
		output.count = 2;
		const auto lessError = [](const Segment& left, const Segment& right) { return (left.errorSq < right.errorSq); };
		const auto enqueue = [&](const uint16 from, const uint16 to, const uint32 depth)
		{
			const Vec2 middle = FurthestPoint(output.points[from], output.points[to], n, inverseN, inverseGradientExponent);
			const Vec2 d = (output.points[to] - output.points[from]), m = (middle - output.points[from]);
			const double cross = (d.cross(m) * metricA * metricB);
			// Both this octant and its diagonal reflection must meet the target.
			const double lengthSq = std::min((d.x * d.x * aa + d.y * d.y * bb), (d.x * d.x * bb + d.y * d.y * aa));
			if (not ((cross * cross) > (TargetError * TargetError * lengthSq))) return;
			if (depth == MaxDepth)
			{
				output.limited = true;
				return;
			}
			pending[pendingCount++] = { from, to, depth, middle, (cross * cross) / lengthSq };
			std::push_heap(pending.begin(), pending.begin() + pendingCount, lessError);
		};
		enqueue(0, 1, 0);

		// Refine the worst remaining interval, not the next one in contour order.
		// A fixed heap bounds work and storage even when the target cannot be met.
		while (pendingCount && (output.count <= MaxOctantSegments))
		{
			std::pop_heap(pending.begin(), pending.begin() + pendingCount, lessError);
			const Segment segment = pending[--pendingCount];
			const auto middle = static_cast<uint16>(output.count);
			output.points[output.count++] = segment.middle;
			enqueue(segment.from, middle, segment.depth + 1);
			enqueue(middle, segment.to, segment.depth + 1);
		}
		output.limited |= (pendingCount != 0);
		std::sort(output.points.begin(), output.points.begin() + output.count, [](const Vec2 left, const Vec2 right)
		{
			return ((left.x > right.x) || ((left.x == right.x) && (left.y < right.y)));
		});

		// Reflect the normalized octant before applying the source axes.
		const size_t octantCount = output.count;
		for (size_t i = (octantCount - 1); i-- > 0;)
		{
			const Vec2 p = output.points[i];
			output.points[output.count++] = { p.y, p.x };
		}
		for (size_t i = 0; i < output.count; ++i)
		{
			output.points[i] *= Vec2{ a, b };
		}
	}

	void WriteContour(Array<Vec2>& destination, const Quarter& quarter, const Vec2 center)
	{
		destination.resize((quarter.count - 1) * 4);
		Vec2* out = destination.data();
		VisitContour(quarter, [&](const Vec2 point) { *out++ = (center + point); });
	}
}

namespace s3d
{
	namespace
	{
		void Tessellate(Array<Vec2>& destination, const detail::BezierTessellation::Curve& source,
			ISiv3DRenderer2D& renderer, const double targetError, const int32 maxSegments)
		{
			assert(0 < targetError);
			assert(1 <= maxSegments && maxSegments <= 4096);
			const Mat3x2 affine = renderer.getLocalTransform() * renderer.getCameraTransform();
			const auto metric = detail::BezierTessellation::TransformMetric(source, affine, renderer.getSceneScale());
			detail::BezierTessellation::Workspace workspace;
			detail::BezierTessellation::Flatten(destination, workspace, source, metric, targetError, static_cast<uint32>(maxSegments));
		}

		void TessellatePath(Array<Vec2>& destination, const std::span<const Bezier3> curves, const CloseRing closeRing,
			ISiv3DRenderer2D& renderer, const double targetError, const int32 maxSegmentsPerCurve)
		{
			const Mat3x2 affine = renderer.getLocalTransform() * renderer.getCameraTransform();
			const Vec2 sceneScale = renderer.getSceneScale();
			detail::BezierTessellation::Workspace workspace;
			detail::BezierTessellation::FlattenPath(destination, workspace, curves, static_cast<bool>(closeRing), targetError,
				static_cast<uint32>(maxSegmentsPerCurve), [&](const auto& source)
				{
					return detail::BezierTessellation::TransformMetric(source, affine, sceneScale);
				});
		}

		template <class Bezier>
		void DrawBezierArrow(ISiv3DRenderer2D& renderer, Array<Vec2>& points, const Bezier& curve,
			const LineCap startCap, const float thickness, const SizeF& headSize, const Float4& color,
			const double targetError, const int32 maxSegments)
		{
			assert(0 < targetError);
			assert(1 <= maxSegments && maxSegments <= 4096);
			const auto head = detail::FindBezierArrowHead(curve, headSize.y);
			if (not head) return;
			if (head->parameter == 0.0)
			{
				points.clear();
				points.push_back(head->base);
			}
			else
			{
				Tessellate(points, curve.split(head->parameter).first, renderer, targetError, maxSegments);
				points.back() = head->base;
			}
			const float headWidth = (thickness + static_cast<float>(headSize.x * head->sizeScale));
			if (not renderer.addLineStringArrow(startCap, points, head->tip, thickness, headWidth, color))
			{
				Tessellate(points, curve, renderer, targetError, maxSegments);
				renderer.addLineString(startCap, LineCap::Flat, points, none, thickness, false, CloseRing::No, color);
			}
		}

		template <class Head>
		bool SubmitDoubleArrow(ISiv3DRenderer2D& renderer, const Array<Vec2>& points,
			const detail::DoubleArrowHeads<Head>& heads, const float thickness, const SizeF& headSize, const Float4& color)
		{
			return renderer.addLineStringDoubleHeadedArrow(points, heads.start.tip, heads.end.tip, thickness,
				(thickness + static_cast<float>(headSize.x * heads.start.sizeScale)),
				(thickness + static_cast<float>(headSize.x * heads.end.sizeScale)), color);
		}

		template <class Bezier>
		void DrawBezierDoubleHeadedArrow(ISiv3DRenderer2D& renderer, Array<Vec2>& points, const Bezier& curve,
			const float thickness, const SizeF& headSize, const Float4& color, const double targetError, const int32 maxSegments)
		{
			assert(0 < targetError);
			assert(1 <= maxSegments && maxSegments <= 4096);
			const auto heads = detail::FindBezierDoubleArrowHeads(curve, headSize.y);
			if (not heads) return;
			Tessellate(points, curve.subcurve(heads->start.parameter, heads->end.parameter), renderer, targetError, maxSegments);
			points.front() = heads->start.base;
			points.back() = heads->end.base;
			if (not SubmitDoubleArrow(renderer, points, *heads, thickness, headSize, color))
			{
				Tessellate(points, curve, renderer, targetError, maxSegments);
				renderer.addLineString(LineCap::Flat, LineCap::Flat, points, none, thickness, false, CloseRing::No, color);
			}
		}

		template <class Fill>
		void DrawFrame(ISiv3DRenderer2D& renderer, Array<Vec2>& points, const SuperEllipse& shape,
			const float thickness, const Fill& fill)
		{
			detail::SuperEllipseTessellation::Quarter quarter;
			detail::SuperEllipseTessellation::Tessellate(quarter, Abs(shape.a), Abs(shape.b), shape.n,
				renderer.getRasterScaling(), (thickness * 0.5));
			detail::SuperEllipseTessellation::WriteContour(points, quarter, shape.center);
			renderer.addLineString(LineCap::Square, LineCap::Square, points, none, thickness, false, CloseRing::Yes, fill);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleArcArrow
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addCircleArcArrow(const Circle& circle, const LineCap startCap, const double startAngle,
		const double angle, const float thickness, const SizeF& headSize, const Float4& color)
	{
		const auto placement = detail::CircleArcArrow::Place(circle, startAngle, angle, headSize.y);
		if (not placement) return;
		const float scale = getRasterScaling();
		detail::CircleArcArrow::WritePoints(m_curvePoints, *placement, thickness, scale);
		const float headWidth = (thickness + static_cast<float>(headSize.x * placement->sizeScale));
		if (not addLineStringArrow(startCap, m_curvePoints, placement->tip, thickness, headWidth, color))
		{
			detail::CircleArcArrow::WritePoints(m_curvePoints, *placement, thickness, scale, true);
			addLineString(startCap, LineCap::Flat, m_curvePoints, none, thickness, false, CloseRing::No, color);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCircleArcDoubleHeadedArrow
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addCircleArcDoubleHeadedArrow(const Circle& circle, const double startAngle,
		const double angle, const float thickness, const SizeF& headSize, const Float4& color)
	{
		const auto placement = detail::CircleArcArrow::PlaceDouble(circle, startAngle, angle, headSize.y);
		if (not placement) return;
		const float scale = getRasterScaling();
		detail::CircleArcArrow::WriteDoublePoints(m_curvePoints, *placement, thickness, scale);
		const float width = (thickness + static_cast<float>(headSize.x * placement->sizeScale));
		if (not addLineStringDoubleHeadedArrow(m_curvePoints, placement->arc.start, placement->arc.tip, thickness, width, width, color))
		{
			detail::CircleArcArrow::WritePoints(m_curvePoints, placement->arc, thickness, scale, true);
			addLineString(LineCap::Flat, LineCap::Flat, m_curvePoints, none, thickness, false, CloseRing::No, color);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSuperEllipseFrame
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addSuperEllipseFrame(const SuperEllipse& shape, const float thickness, const Float4& color)
	{
		DrawFrame(*this, m_curvePoints, shape, thickness, color);
	}

	void ISiv3DRenderer2D::addSuperEllipseFrame(const SuperEllipse& shape, const float thickness, const PatternParameters& pattern)
	{
		DrawFrame(*this, m_curvePoints, shape, thickness, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPolylineStyled
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addPolylineStyled(const std::span<const Vec2> points, const CloseRing closeRing,
		const float thickness, const LineStyle& style, const Float4& color)
	{
		const float width = Abs(thickness);
		if (style.type == LinePattern::Solid)
		{
			addLineString(style.cap, style.cap, points, none, width, false, closeRing, color);
			return;
		}
		if (not detail::DashedPath::Split(m_dashedPath, points, static_cast<bool>(closeRing), width, style)) return;
		for (const auto& run : m_dashedPath.runs)
		{
			addLineString(LineCap::Flat, LineCap::Flat, m_dashedPath.view(run), none, width, false,
				(run.closed ? CloseRing::Yes : CloseRing::No), color);
		}
		for (const auto& dot : m_dashedPath.dots)
		{
			addRoundDot(dot.start, dot.end, width, dot.uRange, color);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPolylineArrow
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addPolylineArrow(const std::span<const Vec2> points, const LineCap startCap,
		const float thickness, const SizeF& headSize, const Float4& color)
	{
		const auto head = detail::FindLineStringArrowHead(points, headSize.y);
		if (not head) return;
		detail::WriteLineStringArrowShaft(m_curvePoints, points, *head);
		const float headWidth = (thickness + static_cast<float>(headSize.x * head->sizeScale));
		if (not addLineStringArrow(startCap, m_curvePoints, head->tip, thickness, headWidth, color))
		{
			addLineString(startCap, LineCap::Flat, points, none, thickness, false, CloseRing::No, color);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPolylineDoubleHeadedArrow
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addPolylineDoubleHeadedArrow(const std::span<const Vec2> points, const float thickness,
		const SizeF& headSize, const Float4& color)
	{
		const auto heads = detail::FindLineStringDoubleArrowHeads(points, headSize.y);
		if (not heads) return;
		detail::WriteLineStringDoubleArrowShaft(m_curvePoints, points, *heads);
		if (not SubmitDoubleArrow(*this, m_curvePoints, *heads, thickness, headSize, color))
		{
			addLineString(LineCap::Flat, LineCap::Flat, points, none, thickness, false, CloseRing::No, color);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBezier
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addBezier(const Bezier2& curve, const LineCap startCap, const LineCap endCap,
		const float thickness, const Float4& color, const double targetError, const int32 maxSegments)
	{
		Tessellate(m_curvePoints, curve, *this, targetError, maxSegments);
		addLineString(startCap, endCap, m_curvePoints, none, Abs(thickness), false, CloseRing::No, color);
	}

	void ISiv3DRenderer2D::addBezier(const Bezier3& curve, const LineCap startCap, const LineCap endCap,
		const float thickness, const Float4& color, const double targetError, const int32 maxSegments)
	{
		Tessellate(m_curvePoints, curve, *this, targetError, maxSegments);
		addLineString(startCap, endCap, m_curvePoints, none, Abs(thickness), false, CloseRing::No, color);
	}

	void ISiv3DRenderer2D::addBezier(const Bezier2& curve, const LineCap startCap, const LineCap endCap,
		const float thickness, const Float4& startColor, const Float4& endColor, const double targetError, const int32 maxSegments)
	{
		Tessellate(m_curvePoints, curve, *this, targetError, maxSegments);
		addLineString(startCap, endCap, m_curvePoints, none, Abs(thickness), false, startColor, endColor);
	}

	void ISiv3DRenderer2D::addBezier(const Bezier3& curve, const LineCap startCap, const LineCap endCap,
		const float thickness, const Float4& startColor, const Float4& endColor, const double targetError, const int32 maxSegments)
	{
		Tessellate(m_curvePoints, curve, *this, targetError, maxSegments);
		addLineString(startCap, endCap, m_curvePoints, none, Abs(thickness), false, startColor, endColor);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBezierStyled
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addBezierStyled(const Bezier2& curve, const float thickness, const LineStyle& style,
		const Float4& color, const double targetError, const int32 maxSegments)
	{
		Tessellate(m_curvePoints, curve, *this, targetError, maxSegments);
		addPolylineStyled(m_curvePoints, CloseRing::No, thickness, style, color);
	}

	void ISiv3DRenderer2D::addBezierStyled(const Bezier3& curve, const float thickness, const LineStyle& style,
		const Float4& color, const double targetError, const int32 maxSegments)
	{
		Tessellate(m_curvePoints, curve, *this, targetError, maxSegments);
		addPolylineStyled(m_curvePoints, CloseRing::No, thickness, style, color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBezierArrow
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addBezierArrow(const Bezier2& curve, const LineCap startCap, const float thickness,
		const SizeF& headSize, const Float4& color, const double targetError, const int32 maxSegments)
	{
		DrawBezierArrow(*this, m_curvePoints, curve, startCap, thickness, headSize, color, targetError, maxSegments);
	}

	void ISiv3DRenderer2D::addBezierArrow(const Bezier3& curve, const LineCap startCap, const float thickness,
		const SizeF& headSize, const Float4& color, const double targetError, const int32 maxSegments)
	{
		DrawBezierArrow(*this, m_curvePoints, curve, startCap, thickness, headSize, color, targetError, maxSegments);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBezierDoubleHeadedArrow
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addBezierDoubleHeadedArrow(const Bezier2& curve, const float thickness,
		const SizeF& headSize, const Float4& color, const double targetError, const int32 maxSegments)
	{
		DrawBezierDoubleHeadedArrow(*this, m_curvePoints, curve, thickness, headSize, color, targetError, maxSegments);
	}

	void ISiv3DRenderer2D::addBezierDoubleHeadedArrow(const Bezier3& curve, const float thickness,
		const SizeF& headSize, const Float4& color, const double targetError, const int32 maxSegments)
	{
		DrawBezierDoubleHeadedArrow(*this, m_curvePoints, curve, thickness, headSize, color, targetError, maxSegments);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSpline
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addSpline(const std::span<const Bezier3> curves, const CloseRing closeRing,
		const float thickness, const Float4& color, const double targetError, const int32 maxSegmentsPerCurve)
	{
		TessellatePath(m_curvePoints, curves, closeRing, *this, targetError, maxSegmentsPerCurve);
		addLineString(LineCap::Square, LineCap::Square, m_curvePoints, none, Abs(thickness), false, closeRing, color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSplineStyled
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addSplineStyled(const std::span<const Bezier3> curves, const CloseRing closeRing,
		const float thickness, const LineStyle& style, const Float4& color, const double targetError, const int32 maxSegmentsPerCurve)
	{
		TessellatePath(m_curvePoints, curves, closeRing, *this, targetError, maxSegmentsPerCurve);
		addPolylineStyled(m_curvePoints, closeRing, thickness, style, color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSplineArrow
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addSplineArrow(const std::span<const Bezier3> curves, const LineCap startCap,
		const float thickness, const SizeF& headSize, const Float4& color, const double targetError, const int32 maxSegmentsPerCurve)
	{
		const auto head = detail::FindSplineArrowHead(curves, headSize.y);
		if (not head) return;
		const Mat3x2 affine = getLocalTransform() * getCameraTransform();
		const Vec2 sceneScale = getSceneScale();
		detail::BezierTessellation::Workspace workspace;
		const auto metricFor = [&](const auto& source)
		{
			return detail::BezierTessellation::TransformMetric(source, affine, sceneScale);
		};
		detail::BezierTessellation::FlattenPath(m_curvePoints, workspace, curves.first(head->segment), false,
			targetError, static_cast<uint32>(maxSegmentsPerCurve), metricFor);
		const detail::BezierTessellation::Curve partial{ curves[head->segment].split(head->parameter).first };
		detail::BezierTessellation::Append(m_curvePoints, workspace, partial, metricFor(partial), targetError,
			static_cast<uint32>(maxSegmentsPerCurve));
		m_curvePoints.back() = head->base;
		const float headWidth = (thickness + static_cast<float>(headSize.x * head->sizeScale));
		if (not addLineStringArrow(startCap, m_curvePoints, head->tip, thickness, headWidth, color))
		{
			detail::BezierTessellation::FlattenPath(m_curvePoints, workspace, curves, false, targetError,
				static_cast<uint32>(maxSegmentsPerCurve), metricFor);
			addLineString(startCap, LineCap::Flat, m_curvePoints, none, thickness, false, CloseRing::No, color);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSplineDoubleHeadedArrow
	//
	////////////////////////////////////////////////////////////////

	void ISiv3DRenderer2D::addSplineDoubleHeadedArrow(const std::span<const Bezier3> curves, const float thickness,
		const SizeF& headSize, const Float4& color, const double targetError, const int32 maxSegmentsPerCurve)
	{
		const auto heads = detail::FindSplineDoubleArrowHeads(curves, headSize.y);
		if (not heads) return;
		const Mat3x2 affine = getLocalTransform() * getCameraTransform();
		const Vec2 sceneScale = getSceneScale();
		detail::BezierTessellation::Workspace workspace;
		const auto metricFor = [&](const auto& source)
		{
			return detail::BezierTessellation::TransformMetric(source, affine, sceneScale);
		};
		m_curvePoints.clear();
		for (size_t i = heads->start.segment; i <= heads->end.segment; ++i)
		{
			const double from = ((i == heads->start.segment) ? heads->start.parameter : 0.0);
			const double to = ((i == heads->end.segment) ? heads->end.parameter : 1.0);
			const detail::BezierTessellation::Curve source{ curves[i].subcurve(from, to) };
			detail::BezierTessellation::Append(m_curvePoints, workspace, source, metricFor(source), targetError,
				static_cast<uint32>(maxSegmentsPerCurve));
		}
		m_curvePoints.front() = heads->start.base;
		m_curvePoints.back() = heads->end.base;
		if (not SubmitDoubleArrow(*this, m_curvePoints, *heads, thickness, headSize, color))
		{
			detail::BezierTessellation::FlattenPath(m_curvePoints, workspace, curves, false, targetError,
				static_cast<uint32>(maxSegmentsPerCurve), metricFor);
			addLineString(LineCap::Flat, LineCap::Flat, m_curvePoints, none, thickness, false, CloseRing::No, color);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateShadowTexture
	//
	////////////////////////////////////////////////////////////////

	std::unique_ptr<Texture> ISiv3DRenderer2D::CreateShadowTexture()
	{
		const Image boxShadowImage{ Resource(U"engine/texture/box-shadow/256.png") };

		const Array<Image> boxShadowImageMips =
		{
			Image{ Resource(U"engine/texture/box-shadow/128.png") },
			Image{ Resource(U"engine/texture/box-shadow/64.png") },
			Image{ Resource(U"engine/texture/box-shadow/32.png") },
			Image{ Resource(U"engine/texture/box-shadow/16.png") },
			Image{ Resource(U"engine/texture/box-shadow/8.png") },
		};

		auto shadowTexture = std::make_unique<Texture>(boxShadowImage, boxShadowImageMips);

		if (shadowTexture->isEmpty())
		{
			throw InternalEngineError{ "Failed to create a box-shadow texture" };
		}

		return shadowTexture;
	}

	void ISiv3DRenderer2D::setRenderTarget(const Optional<RenderTexture>& renderTarget)
	{
		if (renderTarget && renderTarget->isEmpty())
		{
			throw Error{ "ScopedRenderTarget2D requires a non-empty RenderTexture (use none for Scene)" };
		}
		if (m_renderTarget == renderTarget)
		{
			return;
		}
		flush();
		setRenderTargetState(renderTarget);
	}

	void ISiv3DRenderer2D::restoreRenderTarget(const Optional<RenderTexture>& renderTarget, const Optional<Rect>& viewport)
	{
		const ScopeExit restore{ [&]
		{
			setRenderTargetState(renderTarget);
			setViewport(viewport);
		} };
		if (m_renderTarget != renderTarget)
		{
			flush();
		}
	}

	void ISiv3DRenderer2D::setRenderTargetState(const Optional<RenderTexture>& renderTarget)
	{
		if (m_renderTarget != renderTarget)
		{
			m_renderTarget = renderTarget;
			m_lastValidatedInput = Texture::IDType::Invalid();
		}
		setSceneScale(m_sceneScale);
	}

	void ISiv3DRenderer2D::ThrowRenderTargetFeedback()
	{
		throw Error{ "A RenderTexture cannot be sampled while it is the current 2D render target" };
	}
}
