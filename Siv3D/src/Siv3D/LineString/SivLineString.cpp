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

# include <Siv3D/LineString.hpp>
# include <Siv3D/LineCap.hpp>
# include <Siv3D/Spline.hpp>
# include <Siv3D/RangeFormatter.hpp>
# include <Siv3D/Geometry2D/BoundingRect.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static LineString CatmullRom(const LineString& lines, const int32 interpolation, const CloseRing closeRing)
		{
			const size_t n = lines.size();

			if ((n < 2) || (interpolation < 2))
			{
				return lines;
			}

			const Vec2* src = lines.data();
			const size_t segments = (closeRing ? n : (n - 1));

			LineString out;
			out.reserve(segments * static_cast<size_t>(interpolation) + 1);

			auto At = [&](std::ptrdiff_t i) -> const Vec2&
			{
				if (closeRing)
				{
					// i は [-1 .. n+1] 程度を想定。安全に循環させる
					const ptrdiff_t m = static_cast<std::ptrdiff_t>(n);
					ptrdiff_t k = i % m;
					if (k < 0) k += m;
					return src[k];
				}
				else
				{
					// 端はクランプ（-1=>0, n=>n-1, n+1=>n-1）
					if (i <= 0) return src[0];
					if (i >= static_cast<std::ptrdiff_t>(n - 1)) return src[n - 1];
					return src[i];
				}
			};

			const double inv = (1.0 / static_cast<double>(interpolation));

			for (size_t i = 0; i < segments; ++i)
			{
				const std::ptrdiff_t ii = static_cast<std::ptrdiff_t>(i);

				const Vec2& p0 = At(ii - 1);
				const Vec2& p1 = At(ii);
				const Vec2& p2 = At(ii + 1);
				const Vec2& p3 = At(ii + 2);

				const bool lastSeg = (i + 1 == segments);
				const int32 count = (interpolation + (lastSeg ? 1 : 0));

				for (int32 t = 0; t < count; ++t)
				{
					const double u = (t * inv);
					out.push_back(Spline::CatmullRom(p0, p1, p2, p3, u));
				}
			}

			return out;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	segment
	//
	////////////////////////////////////////////////////////////////

	Line LineString::segment(const size_t index, const CloseRing closeRing) const
	{
		if (num_segments(closeRing) <= index)
		{
			throw std::out_of_range{ "LineString::segment() index out of range" };
		}

		const Vec2* pData = data();
		const size_t next = ((closeRing && ((index + 1) == size())) ? 0 : (index + 1));
		return{ pData[index], pData[next] };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getNormalAtPoint
	//
	////////////////////////////////////////////////////////////////

	Vec2 LineString::getNormalAtPoint(const size_t index, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return{ Math::NaN, Math::NaN };
		}

		if (size() <= index)
		{
			throw std::out_of_range{ "LineString::getNormalAtPoint() index out of range" };
		}

		const size_t n = size();
		const Vec2* pSrc = data();
		const Vec2 curr = pSrc[index];
		Vec2 prev, next;

		if (index == 0)
		{
			if (closeRing)
			{
				prev = pSrc[n - 1];
				next = pSrc[index + 1];
			}
			else
			{
				const Vec2 forward = (pSrc[1] - pSrc[0]).normalized();
				return{ forward.y, -forward.x };
			}
		}
		else if (index == (n - 1))
		{
			if (closeRing)
			{
				prev = pSrc[index - 1];
				next = pSrc[0];
			}
			else
			{
				const Vec2 forward = (pSrc[n - 1] - pSrc[n - 2]).normalized();
				return{ forward.y, -forward.x };
			}
		}
		else
		{
			prev = pSrc[index - 1];
			next = pSrc[index + 1];
		}

		const double a0 = (curr - prev).getAngle();
		const double a1 = (next - curr).getAngle();
		return Circular{ 1, (Math::LerpAngle(a0, a1, 0.5) - Math::HalfPi) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getNormalAtSegment
	//
	////////////////////////////////////////////////////////////////

	Vec2 LineString::getNormalAtSegment(const size_t index, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return{ Math::NaN, Math::NaN };
		}

		const bool hasCloseLine = static_cast<bool>(closeRing);

		if ((size() - 1 + hasCloseLine) <= index)
		{
			throw std::out_of_range{ "LineString::getNormalAtSegment() index out of range" };
		}

		const size_t num_lines = (size() - 1);
		const Vec2* pSrc = data();
		const Vec2 curr = pSrc[index];
		Vec2 next;

		if (closeRing)
		{
			if (index == num_lines)
			{
				next = pSrc[0];
			}
			else
			{
				next = pSrc[index + 1];
			}
		}
		else
		{
			next = pSrc[index + 1];
		}

		const Vec2 v = (next - curr).normalized();
		return{ v.y, -v.x };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getTangentAtPoint
	//
	////////////////////////////////////////////////////////////////

	Vec2 LineString::getTangentAtPoint(const size_t index, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return{ Math::NaN, Math::NaN };
		}

		if (size() <= index)
		{
			throw std::out_of_range{ "LineString::getTangentAtPoint() index out of range" };
		}

		const size_t n = size();
		const Vec2* pSrc = data();
		const Vec2 curr = pSrc[index];
		Vec2 prev, next;

		if (index == 0)
		{
			if (closeRing)
			{
				prev = pSrc[n - 1];
				next = pSrc[index + 1];
			}
			else
			{
				// 先頭は前方差分
				return (pSrc[1] - pSrc[0]).normalized();
			}
		}
		else if (index == (n - 1))
		{
			if (closeRing)
			{
				prev = pSrc[index - 1];
				next = pSrc[0];
			}
			else
			{
				// 末尾は後方差分
				return (pSrc[n - 1] - pSrc[n - 2]).normalized();
			}
		}
		else
		{
			prev = pSrc[index - 1];
			next = pSrc[index + 1];
		}

		// 法線と同じく、前後のセグメント角度の中間を接線方向にする
		const double a0 = (curr - prev).getAngle();
		const double a1 = (next - curr).getAngle();
		return Circular{ 1, Math::LerpAngle(a0, a1, 0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getTangentAtSegment
	//
	////////////////////////////////////////////////////////////////

	Vec2 LineString::getTangentAtSegment(const size_t index, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return{ Math::NaN, Math::NaN };
		}

		const bool hasCloseLine = static_cast<bool>(closeRing);

		if ((size() - 1 + hasCloseLine) <= index)
		{
			throw std::out_of_range{ "LineString::getTangentAtSegment() index out of range" };
		}

		const size_t num_lines = (size() - 1);
		const Vec2* pSrc = data();
		const Vec2 curr = pSrc[index];
		Vec2 next;

		if (closeRing)
		{
			if (index == num_lines)
			{
				next = pSrc[0];
			}
			else
			{
				next = pSrc[index + 1];
			}
		}
		else
		{
			next = pSrc[index + 1];
		}

		return (next - curr).normalized();
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeBoundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF LineString::computeBoundingRect() const noexcept
	{
		return Geometry2D::BoundingRect(m_points);
	}

	LineString LineString::catmullRom(const int32 interpolation) const
	{
		return CatmullRom(*this, interpolation, CloseRing::No);
	}

	LineString LineString::catmullRom(const CloseRing closeRing, const int32 interpolation) const
	{
		return CatmullRom(*this, interpolation, closeRing);
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeLength
	//
	////////////////////////////////////////////////////////////////

	double LineString::computeLength(const CloseRing closeRing) const noexcept
	{
		const size_t n = m_points.size();
		const Vec2* pData = m_points.data();

		double length = 0.0;
		for (size_t i = 0; i < (n - 1); ++i)
		{
			length += (pData[i].distanceFrom(pData[i + 1]));
		}

		if (closeRing)
		{
			length += (pData[n - 1]).distanceFrom(pData[0]);
		}

		return length;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::draw(const ColorF& color) const
	{
		return draw(LineCap::Square, LineCap::Square, 1.0, color);
	}

	const LineString& LineString::draw(const ColorF& colorStart, const ColorF& colorEnd) const
	{
		return draw(LineCap::Square, LineCap::Square, 1.0, colorStart, colorEnd);
	}

	const LineString& LineString::draw(const PatternParameters& pattern) const
	{
		return draw(LineCap::Square, LineCap::Square, 1.0, pattern);
	}

	const LineString& LineString::draw(const double thickness, const ColorF& color) const
	{
		return draw(LineCap::Square, LineCap::Square, thickness, color);
	}

	const LineString& LineString::draw(const double thickness, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		return draw(LineCap::Square, LineCap::Square, thickness, colorStart, colorEnd);
	}

	const LineString& LineString::draw(const double thickness, const PatternParameters& pattern) const
	{
		return draw(LineCap::Square, LineCap::Square, thickness, pattern);
	}

	const LineString& LineString::draw(const LineCap lineCap, const double thickness, const ColorF& color) const
	{
		return draw(lineCap, lineCap, thickness, color);
	}

	const LineString& LineString::draw(const LineCap lineCap, const double thickness, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		return draw(lineCap, lineCap, thickness, colorStart, colorEnd);
	}

	const LineString& LineString::draw(const LineCap lineCap, const double thickness, const PatternParameters& pattern) const
	{
		return draw(lineCap, lineCap, thickness, pattern);
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_points, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::No,
			color.toFloat4());

		return *this;
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_points, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			colorStart.toFloat4(),
			colorEnd.toFloat4());

		return *this;
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, const double thickness, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_points, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::No,
			pattern);

		return *this;
	}

	const LineString& LineString::draw(const std::span<const ColorF> colors) const
	{
		return draw(LineCap::Square, LineCap::Square, 1.0, colors);
	}

	const LineString& LineString::draw(const double thickness, const std::span<const ColorF> colors) const
	{
		return draw(LineCap::Square, LineCap::Square, thickness, colors);
	}

	const LineString& LineString::draw(const LineCap lineCap, double thickness, std::span<const ColorF> colors) const
	{
		return draw(lineCap, lineCap, thickness, colors);
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, double thickness, std::span<const ColorF> colors) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_points, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::No,
			colors);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawClosed
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::drawClosed(const ColorF& color) const
	{
		return drawClosed(1.0, color);
	}

	const LineString& LineString::drawClosed(const PatternParameters& pattern) const
	{
		return drawClosed(1.0, pattern);
	}

	const LineString& LineString::drawClosed(const double thickness, const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
			m_points, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::Yes,
			color.toFloat4());

		return *this;
	}

	const LineString& LineString::drawClosed(const double thickness, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
			m_points, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::Yes,
			pattern);

		return *this;
	}

	const LineString& LineString::drawClosed(const std::span<const ColorF> colors) const
	{
		return drawClosed(1.0, colors);
	}

	const LineString& LineString::drawClosed(const double thickness, const std::span<const ColorF> colors) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
			m_points, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::Yes,
			colors);
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawPoints
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::drawPoints(const double r, const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();
		const float rF = static_cast<float>(Abs(r));

		for (const auto& p : m_points)
		{
			SIV3D_ENGINE(Renderer2D)->addCircle(
				p,
				rF,
				color0,
				color0,
				ColorFillDirection::InOut
			);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawPointsFrame
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::drawPointsFrame(const double r, const double thickness, const ColorF& color) const
	{
		return drawPointsFrame(r, (thickness * 0.5), (thickness * 0.5), color);
	}

	const LineString& LineString::drawPointsFrame(const double r, const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();
		const float rInner = static_cast<float>(Abs(r) - innerThickness);
		const float thickness = static_cast<float>(innerThickness + outerThickness);

		for (const auto& p : m_points)
		{
			SIV3D_ENGINE(Renderer2D)->addCircleFrame(
				p,
				rInner,
				thickness,
				color0,
				color0
			);
		}

		return *this;
	}


	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const LineString& value)
	{
		Formatter(formatData, std::span<const Vec2>(value.m_points));
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

s3d::ParseContext::iterator fmt::formatter<s3d::LineString, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::LineString, s3d::char32>::format(const s3d::LineString& value, s3d::BufferContext& ctx) const
{
	return s3d::FmtHelper::FormatSequence(tag, std::span(value), ctx);
}
