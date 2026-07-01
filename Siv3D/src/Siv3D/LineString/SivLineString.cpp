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
# include <Siv3D/ImageDraw.hpp>
# include <Siv3D/Polygon/LineStringBuffer.hpp>
# include <Siv3D/Geometry2D/BoundingRect.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static LineString CatmullRom(const LineString& lines, const int32 subdivisionsPerSegment, const CloseRing closeRing)
		{
			const size_t n = lines.size();

			if ((n < 2) || (subdivisionsPerSegment < 2))
			{
				return lines;
			}

			const Vec2* src = lines.data();
			const size_t segments = (closeRing ? n : (n - 1));

			LineString out;
			out.reserve(segments * static_cast<size_t>(subdivisionsPerSegment) + 1);

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

			const double inv = (1.0 / static_cast<double>(subdivisionsPerSegment));

			for (size_t i = 0; i < segments; ++i)
			{
				const std::ptrdiff_t ii = static_cast<std::ptrdiff_t>(i);

				const Vec2& p0 = At(ii - 1);
				const Vec2& p1 = At(ii);
				const Vec2& p2 = At(ii + 1);
				const Vec2& p3 = At(ii + 2);

				const bool lastSeg = (i + 1 == segments);
				const int32 count = (subdivisionsPerSegment + (lastSeg ? 1 : 0));

				for (int32 t = 0; t < count; ++t)
				{
					const double u = (t * inv);
					out.push_back(Spline::CatmullRom(p0, p1, p2, p3, u));
				}
			}

			return out;
		}

		[[nodiscard]]
		static Vec2 PointAtSegmentDistance(const Vec2& from, const Vec2& to, const double distance, const double segmentLength) noexcept
		{
			if (segmentLength <= 0.0)
			{
				return from;
			}

			return (from + ((to - from) * (distance / segmentLength)));
		}

		[[nodiscard]]
		static double WrapDistance(const double distance, const double length)
		{
			double result = std::fmod(distance, length);

			if (result < 0.0)
			{
				result += length;
			}

			return result;
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
	//	tangentAtVertex
	//
	////////////////////////////////////////////////////////////////

	Vec2 LineString::tangentAtVertex(const size_t index, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return{ Math::NaN, Math::NaN };
		}

		if (size() <= index)
		{
			throw std::out_of_range{ "LineString::tangentAtVertex() index out of range" };
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
	//	tangentAtSegment
	//
	////////////////////////////////////////////////////////////////

	Vec2 LineString::tangentAtSegment(const size_t index, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return{ Math::NaN, Math::NaN };
		}

		const bool hasCloseLine = static_cast<bool>(closeRing);

		if ((size() - 1 + hasCloseLine) <= index)
		{
			throw std::out_of_range{ "LineString::tangentAtSegment() index out of range" };
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
	//	normalAtVertex
	//
	////////////////////////////////////////////////////////////////

	Vec2 LineString::normalAtVertex(const size_t index, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return{ Math::NaN, Math::NaN };
		}

		if (size() <= index)
		{
			throw std::out_of_range{ "LineString::normalAtVertex() index out of range" };
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
	//	normalAtSegment
	//
	////////////////////////////////////////////////////////////////

	Vec2 LineString::normalAtSegment(const size_t index, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return{ Math::NaN, Math::NaN };
		}

		const bool hasCloseLine = static_cast<bool>(closeRing);

		if ((size() - 1 + hasCloseLine) <= index)
		{
			throw std::out_of_range{ "LineString::normalAtSegment() index out of range" };
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
	//	computeBoundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF LineString::computeBoundingRect() const noexcept
	{
		return Geometry2D::BoundingRect(m_vertices);
	}

	////////////////////////////////////////////////////////////////
	//
	//	catmullRom
	//
	////////////////////////////////////////////////////////////////

	LineString LineString::catmullRom(const int32 subdivisionsPerSegment) const
	{
		return CatmullRom(*this, subdivisionsPerSegment, CloseRing::No);
	}

	LineString LineString::catmullRom(const CloseRing closeRing, const int32 subdivisionsPerSegment) const
	{
		return CatmullRom(*this, subdivisionsPerSegment, closeRing);
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeLength
	//
	////////////////////////////////////////////////////////////////

	double LineString::computeLength(const CloseRing closeRing) const noexcept
	{
		const size_t n = m_vertices.size();

		if (n < 2)
		{
			return 0.0;
		}

		const Vec2* pData = m_vertices.data();

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
	//	computePointAtDistance
	//
	////////////////////////////////////////////////////////////////

	Vec2 LineString::computePointAtDistance(const double distanceFromStart, const CloseRing closeRing) const
	{
		if (isEmpty())
		{
			throw std::out_of_range{ "LineString::computePointAtDistance() line is empty" };
		}

		if (distanceFromStart <= 0.0)
		{
			return front();
		}

		const size_t n = m_vertices.size();

		if (n < 2)
		{
			return front();
		}

		const size_t segmentCount = num_segments(closeRing);
		const Vec2* pSrc = m_vertices.data();
		double accumulatedLength = 0.0;

		for (size_t i = 0; i < segmentCount; ++i)
		{
			const Vec2 from = pSrc[i];
			const Vec2 to = pSrc[((i + 1) == n) ? 0 : (i + 1)];
			const double segmentLength = from.distanceFrom(to);

			if (distanceFromStart <= (accumulatedLength + segmentLength))
			{
				return PointAtSegmentDistance(from, to, (distanceFromStart - accumulatedLength), segmentLength);
			}

			accumulatedLength += segmentLength;
		}

		return (closeRing ? front() : back());
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeVertexNormals
	//
	////////////////////////////////////////////////////////////////

	Array<Vec2> LineString::computeVertexNormals(const CloseRing closeRing) const
	{
		const size_t n = m_vertices.size();
		Array<Vec2> normals(n);

		if (n == 0)
		{
			return normals;
		}

		Vec2* pDst = normals.data();

		if (n < 2)
		{
			pDst[0] = { Math::NaN, Math::NaN };
			return normals;
		}

		const Vec2* pSrc = m_vertices.data();

		if (closeRing)
		{
			for (size_t i = 0; i < n; ++i)
			{
				const Vec2 prev = (i == 0) ? pSrc[n - 1] : pSrc[i - 1];
				const Vec2 curr = pSrc[i];
				const Vec2 next = (i == (n - 1)) ? pSrc[0] : pSrc[i + 1];
				const double a0 = (curr - prev).getAngle();
				const double a1 = (next - curr).getAngle();
				pDst[i] = Circular{ 1, (Math::LerpAngle(a0, a1, 0.5) - Math::HalfPi) };
			}
		}
		else
		{
			{
				const Vec2 forward = (pSrc[1] - pSrc[0]).normalized();
				pDst[0] = { forward.y, -forward.x };
			}

			for (size_t i = 1; i < (n - 1); ++i)
			{
				const Vec2 prev = pSrc[i - 1];
				const Vec2 curr = pSrc[i];
				const Vec2 next = pSrc[i + 1];
				const double a0 = (curr - prev).getAngle();
				const double a1 = (next - curr).getAngle();
				pDst[i] = Circular{ 1, (Math::LerpAngle(a0, a1, 0.5) - Math::HalfPi) };
			}

			{
				const Vec2 forward = (pSrc[n - 1] - pSrc[n - 2]).normalized();
				pDst[n - 1] = { forward.y, -forward.x };
			}
		}

		return normals;
	}

	////////////////////////////////////////////////////////////////
	//
	//	sliceByDistance
	//
	////////////////////////////////////////////////////////////////

	LineString LineString::sliceByDistance(const double distanceFromStart, const CloseRing closeRing) const
	{
		if (m_vertices.size() < 2)
		{
			return{};
		}

		const size_t n = m_vertices.size();
		const size_t segmentCount = num_segments(closeRing);
		const Vec2* pSrc = m_vertices.data();
		Array<double> segmentLengths(segmentCount);
		double lineStringLength = 0.0;

		for (size_t i = 0; i < segmentCount; ++i)
		{
			const double segmentLength = pSrc[i].distanceFrom(pSrc[((i + 1) == n) ? 0 : (i + 1)]);
			segmentLengths[i] = segmentLength;
			lineStringLength += segmentLength;
		}

		LineString result;
		result.reserve(segmentCount + 2);

		if (closeRing)
		{
			if (lineStringLength <= 0.0)
			{
				return{};
			}

			const double distanceBegin = WrapDistance(distanceFromStart, lineStringLength);
			const double distanceEnd = (distanceBegin + lineStringLength);
			double accumulatedLength = 0.0;

			for (size_t k = 0; k < (segmentCount * 2); ++k)
			{
				const size_t i = (k % segmentCount);
				const Vec2 from = pSrc[i];
				const Vec2 to = pSrc[((i + 1) == n) ? 0 : (i + 1)];
				const double segmentLength = segmentLengths[i];

				if (not result)
				{
					if (distanceBegin <= (accumulatedLength + segmentLength))
					{
						const Vec2 start = PointAtSegmentDistance(from, to, (distanceBegin - accumulatedLength), segmentLength);
						result << start;

						if (distanceEnd <= (accumulatedLength + segmentLength))
						{
							const Vec2 goal = PointAtSegmentDistance(from, to, (distanceEnd - accumulatedLength), segmentLength);
							result << goal;
							break;
						}

						if (result.back() != to)
						{
							result << to;
						}
					}
				}
				else
				{
					if (distanceEnd <= (accumulatedLength + segmentLength))
					{
						const Vec2 goal = PointAtSegmentDistance(from, to, (distanceEnd - accumulatedLength), segmentLength);
						result << goal;
						break;
					}

					result << to;
				}

				accumulatedLength += segmentLength;
			}
		}
		else
		{
			const double distanceBegin = Clamp(distanceFromStart, 0.0, lineStringLength);
			double accumulatedLength = 0.0;

			for (size_t i = 0; i < segmentCount; ++i)
			{
				const Vec2 from = pSrc[i];
				const Vec2 to = pSrc[i + 1];
				const double segmentLength = segmentLengths[i];

				if (not result)
				{
					if (distanceBegin <= (accumulatedLength + segmentLength))
					{
						const Vec2 start = PointAtSegmentDistance(from, to, (distanceBegin - accumulatedLength), segmentLength);
						result << start;

						if (result.back() != to)
						{
							result << to;
						}
					}
				}
				else
				{
					result << to;
				}

				accumulatedLength += segmentLength;
			}
		}

		return result;
	}

	LineString LineString::sliceByDistance(const double distanceFromStart, const double length, const CloseRing closeRing) const
	{
		if (m_vertices.size() < 2)
		{
			return{};
		}

		const size_t n = m_vertices.size();
		const size_t segmentCount = num_segments(closeRing);
		const Vec2* pSrc = m_vertices.data();
		Array<double> segmentLengths(segmentCount);
		double lineStringLength = 0.0;

		for (size_t i = 0; i < segmentCount; ++i)
		{
			const double segmentLength = pSrc[i].distanceFrom(pSrc[((i + 1) == n) ? 0 : (i + 1)]);
			segmentLengths[i] = segmentLength;
			lineStringLength += segmentLength;
		}

		double from, to;

		if (length < 0.0)
		{
			from = (distanceFromStart + length);
			to = distanceFromStart;
		}
		else
		{
			from = distanceFromStart;
			to = (distanceFromStart + length);
		}

		LineString result;
		result.reserve(segmentCount + 2);

		if (closeRing)
		{
			if (lineStringLength <= 0.0)
			{
				return{};
			}

			const double distanceBegin = WrapDistance(from, lineStringLength);
			double distanceEnd = WrapDistance(to, lineStringLength);

			if (distanceEnd < distanceBegin)
			{
				distanceEnd += lineStringLength;
			}

			double accumulatedLength = 0.0;

			for (size_t k = 0; k < (segmentCount * 2); ++k)
			{
				const size_t i = (k % segmentCount);
				const Vec2 pFrom = pSrc[i];
				const Vec2 pTo = pSrc[((i + 1) == n) ? 0 : (i + 1)];
				const double segmentLength = segmentLengths[i];

				if (not result)
				{
					if (distanceBegin <= (accumulatedLength + segmentLength))
					{
						const Vec2 start = PointAtSegmentDistance(pFrom, pTo, (distanceBegin - accumulatedLength), segmentLength);
						result << start;

						if (distanceEnd <= (accumulatedLength + segmentLength))
						{
							const Vec2 goal = PointAtSegmentDistance(pFrom, pTo, (distanceEnd - accumulatedLength), segmentLength);
							result << goal;
							break;
						}

						if (result.back() != pTo)
						{
							result << pTo;
						}
					}
				}
				else
				{
					if (distanceEnd <= (accumulatedLength + segmentLength))
					{
						const Vec2 goal = PointAtSegmentDistance(pFrom, pTo, (distanceEnd - accumulatedLength), segmentLength);
						result << goal;
						break;
					}

					result << pTo;
				}

				accumulatedLength += segmentLength;
			}
		}
		else
		{
			const double distanceBegin = Clamp(from, 0.0, lineStringLength);
			const double distanceEnd = Clamp(to, 0.0, lineStringLength);
			double accumulatedLength = 0.0;

			for (size_t i = 0; i < segmentCount; ++i)
			{
				const Vec2 pFrom = pSrc[i];
				const Vec2 pTo = pSrc[i + 1];
				const double segmentLength = segmentLengths[i];

				if (not result)
				{
					if (distanceBegin <= (accumulatedLength + segmentLength))
					{
						const Vec2 start = PointAtSegmentDistance(pFrom, pTo, (distanceBegin - accumulatedLength), segmentLength);
						result << start;

						if (distanceEnd <= (accumulatedLength + segmentLength))
						{
							const Vec2 goal = PointAtSegmentDistance(pFrom, pTo, (distanceEnd - accumulatedLength), segmentLength);
							result << goal;
							break;
						}

						if (result.back() != pTo)
						{
							result << pTo;
						}
					}
				}
				else
				{
					if (distanceEnd <= (accumulatedLength + segmentLength))
					{
						const Vec2 goal = PointAtSegmentDistance(pFrom, pTo, (distanceEnd - accumulatedLength), segmentLength);
						result << goal;
						break;
					}

					result << pTo;
				}

				accumulatedLength += segmentLength;
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paint
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::paint(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paint(dst, LineCap::Round, 1.0, color, enableAntialiasing);
	}

	const LineString& LineString::paint(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paint(dst, LineCap::Round, thickness, color, enableAntialiasing);
	}

	const LineString& LineString::paint(Image& dst, const LineCap lineCap, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::LineString(dst, *this, thickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing, lineCap);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeMiterBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon LineString::computeMiterBufferPolygon(const double distance) const
	{
		return computeMiterBufferPolygon(distance, CloseRing::No);
	}

	Polygon LineString::computeMiterBufferPolygon(const double distance, const CloseRing closeRing) const
	{
		return ComputeMiterBufferPolygon(*this, distance, closeRing);
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeRoundBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon LineString::computeRoundBufferPolygon(const double distance, const QualityFactor& qualityFactor) const
	{
		return computeRoundBufferPolygon(distance, CloseRing::No, qualityFactor);
	}

	Polygon LineString::computeRoundBufferPolygon(const double distance, const CloseRing closeRing, const QualityFactor& qualityFactor) const
	{
		return ComputeRoundBufferPolygon(*this, distance, closeRing, qualityFactor);
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwrite
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::overwrite(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwrite(dst, LineCap::Round, 1.0, color, enableAntialiasing);
	}

	const LineString& LineString::overwrite(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwrite(dst, LineCap::Round, thickness, color, enableAntialiasing);
	}

	const LineString& LineString::overwrite(Image& dst, const LineCap lineCap, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::LineString(dst, *this, thickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing, lineCap);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paintClosed
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::paintClosed(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paintClosed(dst, 1.0, color, enableAntialiasing);
	}

	const LineString& LineString::paintClosed(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::ClosedLineString(dst, *this, thickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwriteClosed
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::overwriteClosed(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwriteClosed(dst, 1.0, color, enableAntialiasing);
	}

	const LineString& LineString::overwriteClosed(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::ClosedLineString(dst, *this, thickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
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
			m_vertices, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::No,
			color.toFloat4());

		return *this;
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& colorStart, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_vertices, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			colorStart.toFloat4(),
			colorEnd.toFloat4());

		return *this;
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, const double thickness, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_vertices, s3d::none,
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
			m_vertices, s3d::none,
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
			m_vertices, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::Yes,
			color.toFloat4());

		return *this;
	}

	const LineString& LineString::drawClosed(const double thickness, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
			m_vertices, s3d::none,
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
			m_vertices, s3d::none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::Yes,
			colors);
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawVertices
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::drawVertices(const double r, const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();
		const float rF = static_cast<float>(Abs(r));

		for (const auto& vertex : m_vertices)
		{
			SIV3D_ENGINE(Renderer2D)->addCircle(
				vertex,
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
	//	drawVerticesFrame
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::drawVerticesFrame(const double r, const double thickness, const ColorF& color) const
	{
		return drawVerticesFrame(r, (thickness * 0.5), (thickness * 0.5), color);
	}

	const LineString& LineString::drawVerticesFrame(const double r, const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();
		const float rInner = static_cast<float>(Abs(r) - innerThickness);
		const float thickness = static_cast<float>(innerThickness + outerThickness);

		for (const auto& vertex : m_vertices)
		{
			SIV3D_ENGINE(Renderer2D)->addCircleFrame(
				vertex,
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
		Formatter(formatData, std::span<const Vec2>(value.m_vertices));
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
