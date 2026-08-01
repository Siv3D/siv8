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

# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/RangeFormatter.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Polygon/PolygonDetail.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::movedBy(const double x, const double y) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(x, y);
		return result;
	}

	MultiPolygon MultiPolygon::movedBy(const double x, const double y) && noexcept
	{
		moveBy(x, y);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::movedBy(const Vec2 v) const&
	{
		return movedBy(v.x, v.y);
	}

	MultiPolygon MultiPolygon::movedBy(const Vec2 v) && noexcept
	{
		moveBy(v);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::moveBy(const double x, const double y) noexcept
	{
		for (auto& polygon : m_polygons)
		{
			polygon.moveBy(x, y);
		}

		return *this;
	}

	MultiPolygon& MultiPolygon::moveBy(const Vec2 v) noexcept
	{
		return moveBy(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffset
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::withOffset(const double x, const double y) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(x, y);
		return result;
	}

	MultiPolygon MultiPolygon::withOffset(const double x, const double y) && noexcept
	{
		moveBy(x, y);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::withOffset(const Vec2 v) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(v);
		return result;
	}

	MultiPolygon MultiPolygon::withOffset(const Vec2 v) && noexcept
	{
		moveBy(v);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffsetX, withOffsetY
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::withOffsetX(const double x) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(x, 0.0);
		return result;
	}

	MultiPolygon MultiPolygon::withOffsetX(const double x) && noexcept
	{
		moveBy(x, 0.0);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::withOffsetY(const double y) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(0.0, y);
		return result;
	}

	MultiPolygon MultiPolygon::withOffsetY(const double y) && noexcept
	{
		moveBy(0.0, y);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::rotated(const double angle) const&
	{
		MultiPolygon result{ *this };
		result.rotate(angle);
		return result;
	}

	MultiPolygon MultiPolygon::rotated(const double angle) &&
	{
		rotate(angle);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAt
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::rotatedAt(const Vec2 pos, const double angle) const&
	{
		MultiPolygon result{ *this };
		result.rotateAt(pos, angle);
		return result;
	}

	MultiPolygon MultiPolygon::rotatedAt(const Vec2 pos, const double angle) &&
	{
		rotateAt(pos, angle);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::rotate(const double angle)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.rotate(angle);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotateAt
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::rotateAt(const Vec2 pos, const double angle)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.rotateAt(pos, angle);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformed
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::transformed(const double s, const double c, const Vec2& pos) const&
	{
		MultiPolygon result{ *this };
		result.transform(s, c, pos);
		return result;
	}

	MultiPolygon MultiPolygon::transformed(const double s, const double c, const Vec2& pos) &&
	{
		transform(s, c, pos);
		return std::move(*this);
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	transform
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::transform(const double s, const double c, const Vec2& pos)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.transform(s, c, pos);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromOrigin
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::scaledFromOrigin(const double s) const&
	{
		MultiPolygon result{ *this };
		result.scaleFromOrigin(s);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const double s) &&
	{
		scaleFromOrigin(s);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const double sx, const double sy) const&
	{
		MultiPolygon result{ *this };
		result.scaleFromOrigin(sx, sy);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const double sx, const double sy) &&
	{
		scaleFromOrigin(sx, sy);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const Vec2 s) const&
	{
		MultiPolygon result{ *this };
		result.scaleFromOrigin(s);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const Vec2 s) &&
	{
		scaleFromOrigin(s);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFromOrigin
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::scaleFromOrigin(const double s)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFromOrigin(s);
		}
		
		return *this;
	}

	MultiPolygon& MultiPolygon::scaleFromOrigin(const double sx, const double sy)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFromOrigin(sx, sy);
		}
		
		return *this;
	}

	MultiPolygon& MultiPolygon::scaleFromOrigin(const Vec2 s)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFromOrigin(s);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFrom
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const double s) const&
	{
		MultiPolygon result{ *this };
		result.scaleFrom(pos, s);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const double s) &&
	{
		scaleFrom(pos, s);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const double sx, const double sy) const&
	{
		MultiPolygon result{ *this };
		result.scaleFrom(pos, sx, sy);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const double sx, const double sy) &&
	{
		scaleFrom(pos, sx, sy);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const Vec2 s) const&
	{
		MultiPolygon result{ *this };
		result.scaleFrom(pos, s);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const Vec2 s) &&
	{
		scaleFrom(pos, s);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFrom
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::scaleFrom(const Vec2 pos, const double s)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFrom(pos, s);
		}
		
		return *this;
	}

	MultiPolygon& MultiPolygon::scaleFrom(const Vec2 pos, const double sx, const double sy)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFrom(pos, sx, sy);
		}
		
		return *this;
	}

	MultiPolygon& MultiPolygon::scaleFrom(const Vec2 pos, const Vec2 s)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFrom(pos, s);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF MultiPolygon::boundingRect() const noexcept
	{
		RectF result = RectF::Empty();
		bool hasBoundingRect = false;

		for (const auto& polygon : m_polygons)
		{
			if (polygon.isEmpty())
			{
				continue;
			}

			const RectF& rect = polygon.boundingRect();

			if (not hasBoundingRect)
			{
				result = rect;
				hasBoundingRect = true;
				continue;
			}

			const double left = Min(result.leftX(), rect.leftX());
			const double top = Min(result.topY(), rect.topY());
			const double right = Max(result.rightX(), rect.rightX());
			const double bottom = Max(result.bottomY(), rect.bottomY());
			result.set(left, top, (right - left), (bottom - top));
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	double MultiPolygon::area() const noexcept
	{
		KahanSummation<double> totalArea;

		for (const auto& polygon : m_polygons)
		{
			totalArea += polygon.area();
		}

		return totalArea.value();
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	double MultiPolygon::perimeter() const noexcept
	{
		KahanSummation<double> totalPerimeter;

		for (const auto& polygon : m_polygons)
		{
			totalPerimeter += polygon.perimeter();
		}

		return totalPerimeter.value();
	}

	////////////////////////////////////////////////////////////////
	//
	//	centroid
	//
	////////////////////////////////////////////////////////////////

	Optional<Vec2> MultiPolygon::centroid() const noexcept
	{
		Optional<Vec2> reference;
		KahanSummation<double> totalArea;
		KahanSummation<double> weightedX;
		KahanSummation<double> weightedY;

		for (const auto& polygon : m_polygons)
		{
			const auto result = polygon._detail()->centroid();

			if (not result)
			{
				continue;
			}

			if (not reference)
			{
				reference = result->centroid;
			}

			totalArea += result->area;
			weightedX += ((result->centroid.x - reference->x) * result->area);
			weightedY += ((result->centroid.y - reference->y) * result->area);
		}

		const double area = totalArea.value();

		if ((not reference) || (area == 0.0))
		{
			return s3d::none;
		}

		return Vec2{ (reference->x + (weightedX.value() / area)), (reference->y + (weightedY.value() / area)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	simplified
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::simplified(const double maxDistance) const
	{
		MultiPolygon result;
		result.reserve(size());

		for (const auto& polygon : m_polygons)
		{
			if (polygon.isEmpty())
			{
				result.emplace_back();
			}
			else
			{
				result.push_back(polygon.simplified(maxDistance));
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool MultiPolygon::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool MultiPolygon::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool MultiPolygon::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool MultiPolygon::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool MultiPolygon::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool MultiPolygon::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool MultiPolygon::mouseOver() const noexcept
	{
		return Geometry2D::Intersects(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	chunk
	//
	////////////////////////////////////////////////////////////////

	Array<MultiPolygon> MultiPolygon::chunk(const size_type n) const
	{
		Array<MultiPolygon> result;

		if (n == 0)
		{
			return result;
		}

		const size_type s = size();
		const size_type chunkCount = (s + n - 1) / n;
		result.reserve(chunkCount);

		for (size_type i = 0; i < chunkCount; ++i)
		{
			const size_type index = (i * n);
			const size_type length = Min((s - index), n);
			result.push_back(slice(index, length));
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	in_groups
	//
	////////////////////////////////////////////////////////////////

	Array<MultiPolygon> MultiPolygon::in_groups(const size_type group) const
	{
		Array<MultiPolygon> result;

		if (group == 0)
		{
			return result;
		}

		const size_type s = size();

		if (s == 0)
		{
			return result;
		}

		const size_type g = Min(group, s);
		result.reserve(g);

		const size_type div = (s / g);
		const size_type mod = (s % g);

		size_type index = 0;

		for (size_type i = 0; i < g; ++i)
		{
			const size_type length = (div + (i < mod ? 1 : 0));
			result.push_back(slice(index, length));
			index += length;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paint
	//
	////////////////////////////////////////////////////////////////

	const MultiPolygon& MultiPolygon::paint(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.paint(dst, color, enableAntialiasing);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::paint(Image& dst, const Vec2& offset, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.paint(dst, offset, color, enableAntialiasing);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwrite
	//
	////////////////////////////////////////////////////////////////

	const MultiPolygon& MultiPolygon::overwrite(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.overwrite(dst, color, enableAntialiasing);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::overwrite(Image& dst, const Vec2& offset, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.overwrite(dst, offset, color, enableAntialiasing);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paintFrame
	//
	////////////////////////////////////////////////////////////////

	const MultiPolygon& MultiPolygon::paintFrame(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.paintFrame(dst, color, enableAntialiasing);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::paintFrame(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.paintFrame(dst, thickness, color, enableAntialiasing);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::paintFrame(Image& dst, const Vec2& offset, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.paintFrame(dst, offset, color, enableAntialiasing);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::paintFrame(Image& dst, const Vec2& offset, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.paintFrame(dst, offset, thickness, color, enableAntialiasing);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwriteFrame
	//
	////////////////////////////////////////////////////////////////

	const MultiPolygon& MultiPolygon::overwriteFrame(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.overwriteFrame(dst, color, enableAntialiasing);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::overwriteFrame(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.overwriteFrame(dst, thickness, color, enableAntialiasing);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::overwriteFrame(Image& dst, const Vec2& offset, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.overwriteFrame(dst, offset, color, enableAntialiasing);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::overwriteFrame(Image& dst, const Vec2& offset, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.overwriteFrame(dst, offset, thickness, color, enableAntialiasing);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const MultiPolygon& MultiPolygon::draw(const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.draw(color);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::draw(const Vec2& offset, const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.draw(offset, color);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::draw(const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.draw(pattern);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::draw(const Vec2& offset, const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.draw(offset, pattern);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawTransformed
	//
	////////////////////////////////////////////////////////////////

	const MultiPolygon& MultiPolygon::drawTransformed(const double angle, const Vec2& pos, const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawTransformed(angle, pos, color);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::drawTransformed(const double s, const double c, const Vec2& pos, const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawTransformed(s, c, pos, color);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::drawTransformed(const double angle, const Vec2& pos, const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawTransformed(angle, pos, pattern);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::drawTransformed(const double s, const double c, const Vec2& pos, const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawTransformed(s, c, pos, pattern);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	const MultiPolygon& MultiPolygon::drawFrame(const double thickness, const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawFrame(thickness, color);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::drawFrame(const Vec2& offset, const double thickness, const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawFrame(offset, thickness, color);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::drawFrame(const double thickness, const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawFrame(thickness, pattern);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::drawFrame(const Vec2& offset, const double thickness, const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawFrame(offset, thickness, pattern);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawWireframe
	//
	////////////////////////////////////////////////////////////////

	const MultiPolygon& MultiPolygon::drawWireframe(const double thickness, const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawWireframe(thickness, color);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::drawWireframe(const Vec2& offset, const double thickness, const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawWireframe(offset, thickness, color);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::drawWireframe(const double thickness, const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawWireframe(thickness, pattern);
		}

		return *this;
	}

	const MultiPolygon& MultiPolygon::drawWireframe(const Vec2& offset, const double thickness, const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.drawWireframe(offset, thickness, pattern);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const MultiPolygon& value)
	{
		Formatter(formatData, value.asArray());
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

s3d::ParseContext::iterator fmt::formatter<s3d::MultiPolygon, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::MultiPolygon, s3d::char32>::format(const s3d::MultiPolygon& value, s3d::BufferContext& ctx) const
{
	return s3d::FmtHelper::FormatSequence(tag, std::span(value), ctx);
}
