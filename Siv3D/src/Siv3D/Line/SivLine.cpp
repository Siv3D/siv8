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

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/Shape2D.hpp>
# include <Siv3D/Polygon/PolygonBuffer.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		constexpr double Epsilon = 1e-12;

		[[nodiscard]]
		static constexpr bool NearZero(const double v) noexcept
		{
			return (Abs(v) <= Epsilon);
		}

		[[nodiscard]]
		static constexpr bool In01(const double t) noexcept
		{
			return ((-Epsilon <= t) && (t <= (1.0 + Epsilon)));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	closestPointTo
	//
	////////////////////////////////////////////////////////////////

	Line::position_type Line::closestPointTo(const position_type pos) const noexcept
	{
		Vec2 v = (end - start);
		
		const double d = v.length();

		if (d == 0.0)
		{
			return start;
		}

		v /= d;
		
		const double t = v.dot(pos - start);

		if (t < 0.0)
		{
			return start;
		}

		if (d < t)
		{
			return end;
		}

		return (start + v * t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	projectPoint
	//
	////////////////////////////////////////////////////////////////

	Line::position_type Line::projectPoint(const position_type pos) const noexcept
	{
		Vec2 v = (end - start);
		
		const double d = v.length();

		if (d == 0.0)
		{
			return start;
		}

		v /= d;

		const double t = v.dot(pos - start);

		return (start + v * t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFrom
	//
	////////////////////////////////////////////////////////////////

	double Line::distanceFrom(const position_type pos) const noexcept
	{
		return closestPointTo(pos).distanceFrom(pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFromSq
	//
	////////////////////////////////////////////////////////////////

	double Line::distanceFromSq(const position_type pos) const noexcept
	{
		return closestPointTo(pos).distanceFromSq(pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceTo
	//
	////////////////////////////////////////////////////////////////

	double Line::distanceTo(const position_type pos) const noexcept
	{
		return distanceFrom(pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceToSq
	//
	////////////////////////////////////////////////////////////////

	double Line::distanceToSq(const position_type pos) const noexcept
	{
		return distanceFromSq(pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withThickness
	//
	////////////////////////////////////////////////////////////////

	Quad Line::withThickness(const double thickness) const
	{
		if (thickness <= 0.0)
		{
			return{ start, end, end, start };
		}

		const Vec2 nv = ((thickness * 0.5) * perpendicularUnitVector());

		return{ (start + nv), (end + nv), (end - nv), (start - nv) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	calculateRoundBuffer
	//
	////////////////////////////////////////////////////////////////

	Polygon Line::calculateRoundBuffer(const double distance, const QualityFactor& qualityFactor) const
	{
		return CalculateLineRoundBuffer(*this, distance, qualityFactor);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersectsAt
	//
	////////////////////////////////////////////////////////////////

	Optional<Line::position_type> Line::intersectsAt(const Line& other) const
	{
		const Vec2 p = start;
		const Vec2 r = (end - start);
		const Vec2 q = other.start;
		const Vec2 s = (other.end - other.start);

		const Vec2 qp = (q - p);

		const double rxs = (r.x * s.y - r.y * s.x);
		const double qpxr = (qp.x * r.y - qp.y * r.x);

		const double rr = r.dot(r);
		const double ss = s.dot(s);

		const bool rIsPoint = NearZero(rr);
		const bool sIsPoint = NearZero(ss);

		// 両方点
		if (rIsPoint && sIsPoint)
		{
			return (NearZero((start - other.start).dot(start - other.start)) ? Optional<position_type>{ start } : none);
		}

		// 自分が点：点が other 上にあるか
		if (rIsPoint)
		{
			const Vec2 ap = (start - other.start);
			const double cross = (ap.x * s.y - ap.y * s.x);
			
			if (not NearZero(cross))
			{
				return none;
			}

			const double t = (ap.dot(s) / ss);
			return (In01(t) ? Optional<position_type>{ start } : none);
		}

		// 相手が点：点が自分の線分上にあるか
		if (sIsPoint)
		{
			const Vec2 bp = (other.start - start);
			const double cross = (bp.x * r.y - bp.y * r.x);
			
			if (not NearZero(cross))
			{
				return none;
			}

			const double t = (bp.dot(r) / rr);
			return (In01(t) ? Optional<position_type>{ other.start } : none);
		}

		// 平行
		if (NearZero(rxs))
		{
			// 同一直線でない
			if (not NearZero(qpxr))
			{
				return none;
			}

			// 同一直線：区間重なり判定（a上の t で評価）
			const double t0 = ((other.start - start).dot(r)) / rr;
			const double t1 = ((other.end - start).dot(r)) / rr;

			const auto [lo, hi] = MinMax(t0, t1);
			const double ov0 = Max(lo, 0.0);
			const double ov1 = Min(hi, 1.0);

			// 重ならない
			if (ov1 < (ov0 - Epsilon))
			{
				return none;
			}

			// 重なりが一点（端点で接する等）
			if (NearZero(ov1 - ov0))
			{
				const double t = Clamp(0.5 * (ov0 + ov1), 0.0, 1.0);
				return (start + r * t);
			}

			// 重なりが区間（交点無限）
			return position_type::NaN();
		}

		// 一般位置：一意交点
		const double qpxs = (qp.x * s.y - qp.y * s.x);
		const double t = (qpxs / rxs);
		const double u = (qpxr / rxs);

		if (In01(t) && In01(u))
		{
			const double tt = Clamp(t, 0.0, 1.0);
			return (start + r * tt);
		}

		return none;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Line& Line::draw(const ColorF& color) const
	{
		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Square, LineCap::Square, start, end, 1.0f, { colorF, colorF });
		return *this;
	}

	const Line& Line::draw(const ColorF& colorStart, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Square, LineCap::Square, start, end, 1.0f, { colorStart.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	const Line& Line::draw(const double thickness, const ColorF& color) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Square, LineCap::Square, start, end, static_cast<float>(thickness), { colorF, colorF });
		return *this;
	}

	const Line& Line::draw(const double thickness, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Square, LineCap::Square, start, end, static_cast<float>(thickness), { colorStart.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	const Line& Line::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& color) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(startCap, endCap, start, end, static_cast<float>(thickness), { colorF, colorF });
		return *this;
	}

	const Line& Line::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(startCap, endCap, start, end, static_cast<float>(thickness), { colorStart.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	const Line& Line::draw(const LineStyle& style, const double thickness, const ColorF& color) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(style, start, end, static_cast<float>(thickness), { colorF, colorF });
		return *this;
	}

	const Line& Line::draw(const LineStyle& style, const double thickness, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(style, start, end, static_cast<float>(thickness), { colorStart.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawUncapped
	//
	////////////////////////////////////////////////////////////////

	const Line& Line::drawUncapped(const ColorF& color) const
	{
		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Flat, LineCap::Flat, start, end, 1.0f, { colorF, colorF });
		return *this;
	}

	const Line& Line::drawUncapped(const ColorF& colorStart, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Flat, LineCap::Flat, start, end, 1.0f, { colorStart.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	const Line& Line::drawUncapped(const double thickness, const ColorF& color) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Flat, LineCap::Flat, start, end, static_cast<float>(thickness), { colorF, colorF });
		return *this;
	}

	const Line& Line::drawUncapped(const double thickness, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Flat, LineCap::Flat, start, end, static_cast<float>(thickness), { colorStart.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawRounded
	//
	////////////////////////////////////////////////////////////////

	const Line& Line::drawRounded(const ColorF& color) const
	{
		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Round, LineCap::Round, start, end, 1.0f, { colorF, colorF });
		return *this;
	}

	const Line& Line::drawRounded(const ColorF& colorStart, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Round, LineCap::Round, start, end, 1.0f, { colorStart.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	const Line& Line::drawRounded(const double thickness, const ColorF& color) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Round, LineCap::Round, start, end, static_cast<float>(thickness), { colorF, colorF });
		return *this;
	}

	const Line& Line::drawRounded(const double thickness, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Round, LineCap::Round, start, end, static_cast<float>(thickness), { colorStart.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawArrow
	//
	////////////////////////////////////////////////////////////////

	const Line& Line::drawArrow(const double thickness, const double headSize, const ColorF& color) const
	{
		return drawArrow(thickness, SizeF{ headSize, headSize }, color);
	}

	const Line& Line::drawArrow(const double thickness, const double headSize, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		return drawArrow(thickness, SizeF{ headSize, headSize }, colorStart, colorEnd);
	}

	const Line& Line::drawArrow(const double thickness, const SizeF& headSize, const ColorF& color) const
	{
		return drawArrow(LineCap::Flat, thickness, headSize, color);
	}

	const Line& Line::drawArrow(const double thickness, const SizeF& headSize, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		return drawArrow(LineCap::Flat, thickness, headSize, colorStart, colorEnd);
	}

	const Line& Line::drawArrow(const LineCap startCap, const double thickness, const double headSize, const ColorF& color) const
	{
		return drawArrow(startCap, thickness, SizeF{ headSize, headSize }, color);
	}

	const Line& Line::drawArrow(const LineCap startCap, const double thickness, const double headSize, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		return drawArrow(startCap, thickness, SizeF{ headSize, headSize }, colorStart, colorEnd);
	}

	const Line& Line::drawArrow(const LineCap startCap, const double thickness, const SizeF& headSize, const ColorF& color) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		if ((headSize.x <= 0.0) || (headSize.y <= 0.0))
		{
			return *this;
		}

		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addArrow(startCap, start, end, static_cast<float>(thickness), headSize, { colorF, colorF });
		return *this;
	}

	const Line& Line::drawArrow(const LineCap startCap, const double thickness, const SizeF& headSize, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		if ((headSize.x <= 0.0) || (headSize.y <= 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addArrow(startCap, start, end, static_cast<float>(thickness), headSize, { colorStart.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawDoubleHeadedArrow
	//
	////////////////////////////////////////////////////////////////

	const Line& Line::drawDoubleHeadedArrow(const double thickness, const double headSize, const ColorF& color) const
	{
		return drawDoubleHeadedArrow(thickness, SizeF{ headSize, headSize }, color);
	}

	const Line& Line::drawDoubleHeadedArrow(const double thickness, const SizeF& headSize, const ColorF& color) const
	{
		Shape2D::DoubleHeadedArrow(start, end, thickness, headSize).draw(color);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Line& value)
	{
		formatData.string.append(U"(("_sv);
		detail::AppendFloat(formatData.string, value.start.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.start.y);
		formatData.string.append(U"), ("_sv);
		detail::AppendFloat(formatData.string, value.end.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.end.y);
		formatData.string.append(U"))"_sv);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void Line::ThrowPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Line::pointAtIndex() index out of range" };
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Line>::format(const s3d::Line& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {})", value.start, value.end);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.start, value.end));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Line, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Line, s3d::char32>::format(const s3d::Line& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {})", value.start, value.end);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.start, value.end);
	}
}
