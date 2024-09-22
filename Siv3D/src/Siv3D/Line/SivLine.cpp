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

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr bool IsZero(const double x) noexcept
		{
			return (Abs(x) < 1e-10);
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
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF Line::boundingRect() const noexcept
	{
		const auto [x1, x2] = MinMax(start.x, end.x);
		const auto [y1, y2] = MinMax(start.y, end.y);
		return{ x1, y1, (x2 - x1), (y2 - y1) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersectsAt
	//
	////////////////////////////////////////////////////////////////

	Optional<Line::position_type> Line::intersectsAt(const Line& other) const
	{
		//
		// `Line::intersectsAt()` is based on
		// https://www.codeproject.com/Tips/862988/Find-the-Intersection-Point-of-Two-Line-Segments
		//
		// Licenced with the Code Project Open Licence (CPOL)
		// http://www.codeproject.com/info/cpol10.aspx
		//

		const Vec2 r = (end - start);
		const Vec2 s = (other.end - other.start);
		const Vec2 qp = (other.start - start);
		const double rxs = (r.x * s.y - r.y * s.x);
		const double qpxr = (qp.x * r.y - qp.y * r.x);
		const double qpxs = (qp.x * s.y - qp.y * s.x);

		if (IsZero(rxs))
		{
			if (IsZero(qpxr) && IsZero(qpxs))
			{
				const double qpr = qp.dot(r);
				const double q2pr = (other.end - start).dot(r);
				const double pqs = (start - other.start).dot(s);
				const double p2qs = (end - other.start).dot(s);

				const double rr = r.dot(r);
				const bool rrIsZero = IsZero(rr);
				const double ss = s.dot(s);
				const bool ssIsZero = IsZero(ss);

				if (rrIsZero && ssIsZero && IsZero(qp.dot(qp)))
				{
					// The two lines are both zero length and in the same position
					return start;
				}

				if ((not rrIsZero) && ((0 <= qpr && qpr <= rr) || (0 <= q2pr && q2pr <= rr)))
				{
					// Two lines are overlapping
					if (ssIsZero)
					{
						return other.start;
					}
					else
					{
						return Line::position_type{ Math::QNaN, Math::QNaN };
					}
				}

				if ((not ssIsZero) && ((0 <= pqs && pqs <= ss) || (0 <= p2qs && p2qs <= ss)))
				{
					// Two lines are overlapping
					if (rrIsZero)
					{
						return start;
					}
					else
					{
						return Line::position_type{ Math::QNaN, Math::QNaN };
					}
				}

				// Two lines are collinear but disjoint.
				return none;
			}

			// Two lines are parallel and non-intersecting.
			return none;
		}
		else
		{
			const double t = (qpxs / rxs);
			const double u = (qpxr / rxs);
			if ((0.0 <= t && t <= 1.0) && (0.0 <= u && u <= 1.0))
			{
				// An intersection was found
				return (start + r * t);
			}

			// Two line segments are not parallel but do not intersect
			return none;
		}
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

	const Line& Line::draw(const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Square, LineCap::Square, start, end, 1.0f, { colorBegin.toFloat4(), colorEnd.toFloat4() });
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

	const Line& Line::draw(const double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Square, LineCap::Square, start, end, static_cast<float>(thickness), { colorBegin.toFloat4(), colorEnd.toFloat4() });
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

	const Line& Line::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(startCap, endCap, start, end, static_cast<float>(thickness), { colorBegin.toFloat4(), colorEnd.toFloat4() });
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

	const Line& Line::draw(const LineStyle& style, const double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(style, start, end, static_cast<float>(thickness), { colorBegin.toFloat4(), colorEnd.toFloat4() });
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

	const Line& Line::drawUncapped(const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Flat, LineCap::Flat, start, end, 1.0f, { colorBegin.toFloat4(), colorEnd.toFloat4() });
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

	const Line& Line::drawUncapped(const double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Flat, LineCap::Flat, start, end, static_cast<float>(thickness), { colorBegin.toFloat4(), colorEnd.toFloat4() });
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

	const Line& Line::drawRounded(const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Round, LineCap::Round, start, end, 1.0f, { colorBegin.toFloat4(), colorEnd.toFloat4() });
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

	const Line& Line::drawRounded(const double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(LineCap::Round, LineCap::Round, start, end, static_cast<float>(thickness), { colorBegin.toFloat4(), colorEnd.toFloat4() });
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
