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

# include <array>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Utility.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/TextureRegion.hpp>
# include <Siv3D/TexturedRoundRect.hpp>
# include <Siv3D/ImageDraw.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr double GetRadius(const double w, const double h, const double r) noexcept
		{
			return Min((w * 0.5), (h * 0.5), Abs(r));
		}

		[[nodiscard]]
		static double WrapLength(double length, const double perimeter) noexcept
		{
			if (length < 0.0)
			{
				length = (perimeter + std::fmod(length, perimeter));
			}
			else if (perimeter <= length)
			{
				length = std::fmod(length, perimeter);
			}

			return length;
		}

		[[nodiscard]]
		static bool IsEmpty(const RectF& rect, const double innerThickness, const double outerThickness) noexcept
		{
			return ((rect.w <= 0.0) || (rect.h <= 0.0)
				|| (innerThickness < 0.0) || (outerThickness < 0.0)
				|| ((innerThickness == 0.0) && (outerThickness == 0.0)));
		}

		[[nodiscard]]
		static Array<Vec2> GetOuterVertices(const RoundRect& rect, const double offset, const PointsPerCircle& pointsPerCircle)
		{
			if ((rect.w == 0.0) || (rect.h == 0.0))
			{
				return{};
			}

			if (rect.r == 0.0)
			{
				return{ rect.tl(), rect.tr(), rect.br(), rect.bl() };
			}

			const double rr = (GetRadius(rect.w, rect.h, rect.r) + offset);
			const uint32 quality = ((pointsPerCircle.value() + 3) / 4 + 1);
			const double radDelta = (Math::HalfPi / (quality - 1));

			Array<Vec2> fanPositions(quality);

			for (uint32 i = 0; i < quality; ++i)
			{
				fanPositions[i] = Circular{ rr, (radDelta * i) }.fastToVec2();
			}

			const bool uniteV = (rect.h * 0.5 == rr);
			const bool uniteH = (rect.w * 0.5 == rr);
			const std::array<Vec2, 4> centers =
			{ {
				{ (rect.x + rect.w - rr + offset), (rect.y + rr - offset) },
				{ (rect.x + rect.w - rr + offset), (rect.y + rect.h - rr + offset) },
				{ (rect.x + rr - offset), (rect.y + rect.h - rr + offset) },
				{ (rect.x + rr - offset), (rect.y + rr - offset) },
			} };

			const uint32 vertexSize = ((quality - uniteV + quality - uniteH) * 2);

			Array<Vec2> vertices(vertexSize);
			{
				Vec2* pVertex = vertices.data();

				for (uint32 i = 0; i < (quality - uniteV); ++i)
				{
					*pVertex++ = (centers[0] + fanPositions[i]);
				}

				for (uint32 i = 0; i < (quality - uniteH); ++i)
				{
					*pVertex++ = (centers[1] + Float2{ fanPositions[quality - i - 1].x, -fanPositions[quality - i - 1].y });
				}

				for (uint32 i = 0; i < (quality - uniteV); ++i)
				{
					*pVertex++ = (centers[2] - fanPositions[i]);
				}

				for (uint32 i = 0; i < (quality - uniteH); ++i)
				{
					*pVertex++ = (centers[3] + Float2{ -fanPositions[quality - i - 1].x, fanPositions[quality - i - 1].y });
				}
			}

			return vertices;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtLength
	//
	////////////////////////////////////////////////////////////////

	Vec2 RoundRect::pointAtLength(double length) const noexcept
	{
		length = WrapLength(length, perimeter());

		const double fanLength = (r * Math::HalfPi);
		const double xLineLength = (rect.w - 2 * r);
		const double yLineLength = (rect.h - 2 * r);

		if (length <= xLineLength) // 上辺
		{
			return{ (x + r + length), y };
		}
		else if (length <= (xLineLength + fanLength)) // 右上の角
		{
			const double t = (length - xLineLength);
			return trCircle().pointAtLength(t);
		}
		else if (length <= (xLineLength + yLineLength + fanLength)) // 右辺
		{
			const double t = (length - (xLineLength + fanLength));
			return{ (x + w), (y + r + t) };
		}
		else if (length <= (xLineLength + yLineLength + 2 * fanLength)) // 右下の角
		{
			const double t = (length - (xLineLength + fanLength + yLineLength));
			return brCircle().pointAtLength(fanLength + t);
		}
		else if (length <= (2 * xLineLength + yLineLength + 2 * fanLength)) // 下辺
		{
			const double t = (length - (xLineLength + 2 * fanLength + yLineLength));
			return{ (x + w - r - t), (y + h) };
		}
		else if (length <= (2 * xLineLength + yLineLength + 3 * fanLength)) // 左下の角
		{
			const double t = (length - (2 * xLineLength + 2 * fanLength + yLineLength));
			return blCircle().pointAtLength(fanLength * 2 + t);
		}
		else if (length <= (2 * xLineLength + 2 * yLineLength + 3 * fanLength)) // 左辺
		{
			const double t = (length - (2 * xLineLength + 3 * fanLength + yLineLength));
			return{ x, (y + h - r - t) };
		}
		else // 左上の角
		{
			const double t = (length - (2 * xLineLength + 3 * fanLength + 2 * yLineLength));
			return tlCircle().pointAtLength(fanLength * 3 + t);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	interpolatedPointAt
	//
	////////////////////////////////////////////////////////////////

	Vec2 RoundRect::interpolatedPointAt(double t) const noexcept
	{
		return pointAtLength(t * perimeter());
	}


	////////////////////////////////////////////////////////////////
	//
	//	outline
	//
	////////////////////////////////////////////////////////////////

	///// @brief 角丸長方形の輪郭を LineString として返します。
	///// @param closeRing 頂点配列の終点を始点と重ねるか
	///// @param pointsPerCircle 円周の分割数
	///// @return 角丸長方形の輪郭の LineString
	//[[nodiscard]]
	//LineString outline(CloseRing closeRing, const PointsPerCircle& pointsPerCircle) const;

	///// @brief 角丸長方形の輪郭を LineString として返します。
	///// @param closeRing 頂点配列の終点を始点と重ねるか
	///// @param qualityFactor 品質係数。大きいほど分割数が増えます。
	///// @return 角丸長方形の輪郭の LineString
	//[[nodiscard]]
	//LineString outline(CloseRing closeRing = CloseRing::No, const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;

	///// @brief 角丸長方形の輪郭の一部を LineString として返します。
	///// @param distanceFromOrigin 開始地点の距離（左上の角丸の終わりから時計回りでの距離）
	///// @param length 長さ
	///// @param pointsPerCircle 円周の分割数
	///// @return 角丸長方形の輪郭の一部の LineString
	//[[nodiscard]]
	//LineString outline(double distanceFromOrigin, double length, const PointsPerCircle& pointsPerCircle) const;

	///// @brief 角丸長方形の輪郭の一部を LineString として返します。
	///// @param distanceFromOrigin 開始地点の距離（左上の角丸の終わりから時計回りでの距離）
	///// @param length 長さ
	///// @param qualityFactor 品質係数。大きいほど分割数が増えます。
	///// @return 角丸長方形の輪郭の一部の LineString
	//[[nodiscard]]
	//LineString outline(double distanceFromOrigin, double length, const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;

	LineString RoundRect::outline(const CloseRing closeRing, const PointsPerCircle& pointsPerCircle) const
	{
		Array<Vec2> vertices = outer(pointsPerCircle);
		
		if (closeRing == CloseRing::Yes)
		{
			vertices.push_back(vertices.front());
		}
		
		return vertices;
	}

	LineString RoundRect::outline(const CloseRing closeRing, const QualityFactor& qualityFactor) const
	{
		return outline(closeRing, qualityFactor.toPointsPerCircle(GetRadius(w, h, r)));
	}

	LineString RoundRect::outline(double distanceFromOrigin, double length, const PointsPerCircle& pointsPerCircle) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		const double radius = GetRadius(w, h, r);

		if (radius == 0.0)
		{
			return rect.outline(distanceFromOrigin, length);
		}

		const RoundRect roundRect{ rect, radius };
		const double perim = roundRect.perimeter();

		if (length < 0.0)
		{
			distanceFromOrigin += length;
			length = -length;
		}

		distanceFromOrigin = WrapLength(distanceFromOrigin, perim);
		length = Min(length, perim);

		uint32 quality = ((pointsPerCircle.value() + 3) / 4 + 1);

		if (quality < 2)
		{
			quality = 2;
		}

		const double fanLength = (radius * Math::HalfPi);
		const double xLineLength = (rect.w - 2.0 * radius);
		const double yLineLength = (rect.h - 2.0 * radius);
		const double fanLengthDelta = (fanLength / (quality - 1));

		LineString points{ Arg::reserve = static_cast<size_t>(quality * 4 + 2) };

		const auto AppendPoint = [&points](const Vec2& point)
		{
			if (points.empty() || (points.back() != point))
			{
				points << point;
			}
		};

		AppendPoint(roundRect.pointAtLength(distanceFromOrigin));

		if (length == 0.0)
		{
			return points;
		}

		const double distanceToTarget = (distanceFromOrigin + length);

		Array<double> keyDistances{ Arg::reserve = static_cast<size_t>(quality * 4 + 1) };
		keyDistances << 0.0;

		double d = xLineLength;
		keyDistances << d;

		for (uint32 i = 1; i < quality; ++i)
		{
			keyDistances << (d + fanLengthDelta * i);
		}

		d += (fanLength + yLineLength);
		keyDistances << d;

		for (uint32 i = 1; i < quality; ++i)
		{
			keyDistances << (d + fanLengthDelta * i);
		}

		d += (fanLength + xLineLength);
		keyDistances << d;

		for (uint32 i = 1; i < quality; ++i)
		{
			keyDistances << (d + fanLengthDelta * i);
		}

		d += (fanLength + yLineLength);
		keyDistances << d;

		for (uint32 i = 1; i < quality; ++i)
		{
			keyDistances << (d + fanLengthDelta * i);
		}

		for (double base = 0.0; base <= distanceToTarget; base += perim)
		{
			for (const double keyDistance : keyDistances)
			{
				const double currentDistance = (base + keyDistance);

				if ((distanceFromOrigin < currentDistance) && (currentDistance < distanceToTarget))
				{
					AppendPoint(roundRect.pointAtLength(currentDistance));
				}
			}
		}

		AppendPoint(roundRect.pointAtLength(distanceToTarget));

		return points;
	}

	LineString RoundRect::outline(double distanceFromOrigin, double length, const QualityFactor& qualityFactor) const
	{
		return outline(distanceFromOrigin, length, qualityFactor.toPointsPerCircle(GetRadius(w, h, r)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	Array<Vec2> RoundRect::outer(const PointsPerCircle& pointsPerCircle) const
	{
		return GetOuterVertices(*this, 0.0, pointsPerCircle);
	}

	Array<Vec2> RoundRect::outer(const QualityFactor& qualityFactor) const
	{
		return outer(qualityFactor.toPointsPerCircle(GetRadius(w, h, r)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon RoundRect::asPolygon(const PointsPerCircle& pointsPerCircle) const
	{
		const Array<Vec2> vertices = outer(pointsPerCircle);

		Array<TriangleIndex> indices(vertices.size() - 2);
		{
			TriangleIndex* pDst = indices.data();

			for (Vertex2D::IndexType i = 0; i < indices.size(); ++i)
			{
				pDst->i0 = 0;
				pDst->i1 = (i + 1);
				pDst->i2 = (i + 2);
				++pDst;
			}
		}

		return Polygon{ vertices, indices, rect, SkipValidation::Yes };
	}

	Polygon RoundRect::asPolygon(const QualityFactor& qualityFactor) const
	{
		return asPolygon(qualityFactor.toPointsPerCircle(GetRadius(w, h, r)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool RoundRect::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool RoundRect::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool RoundRect::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool RoundRect::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool RoundRect::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool RoundRect::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool RoundRect::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	paint
	//
	////////////////////////////////////////////////////////////////

	const RoundRect& RoundRect::paint(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::Fill(dst, *this, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwrite
	//
	////////////////////////////////////////////////////////////////

	const RoundRect& RoundRect::overwrite(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::Fill(dst, *this, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	paintFrame
	//
	////////////////////////////////////////////////////////////////

	const RoundRect& RoundRect::paintFrame(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paintFrame(dst, (thickness * 0.5), (thickness * 0.5), color, enableAntialiasing);
	}

	const RoundRect& RoundRect::paintFrame(Image& dst, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::RoundRectFrame(dst, *this, innerThickness, outerThickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwriteFrame
	//
	////////////////////////////////////////////////////////////////

	const RoundRect& RoundRect::overwriteFrame(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwriteFrame(dst, (thickness * 0.5), (thickness * 0.5), color, enableAntialiasing);
	}

	const RoundRect& RoundRect::overwriteFrame(Image& dst, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::RoundRectFrame(dst, *this, innerThickness, outerThickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const RoundRect& RoundRect::draw(const ColorF& color) const
	{
		if (r == 0.0)
		{
			rect.draw(color);
		}
		else
		{
			const double radius = GetRadius(rect.w, rect.h, r);

			SIV3D_ENGINE(Renderer2D)->addRoundRect(
				FloatRect{ x, y, (x + w), (y + h) },
				static_cast<float>(radius),
				color.toFloat4()
			);
		}

		return *this;
	}

	const RoundRect& RoundRect::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		if (r == 0.0)
		{
			rect.draw(topColor, bottomColor);
		}
		else
		{
			const double radius = Min(Abs(rect.w * 0.5), Abs(rect.h * 0.5), Abs(r));
			
			SIV3D_ENGINE(Renderer2D)->addRoundRect(
				FloatRect{ x, y, (x + w), (y + h) },
				static_cast<float>(radius),
				topColor->toFloat4(),
				bottomColor->toFloat4(),
				ColorFillDirection::TopBottom
			);
		}

		return *this;
	}

	const RoundRect& RoundRect::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		if (r == 0.0)
		{
			rect.draw(leftColor, rightColor);
		}
		else
		{
			const double radius = Min(Abs(rect.w * 0.5), Abs(rect.h * 0.5), Abs(r));

			SIV3D_ENGINE(Renderer2D)->addRoundRect(
				FloatRect{ x, y, (x + w), (y + h) },
				static_cast<float>(radius),
				leftColor->toFloat4(),
				rightColor->toFloat4(),
				ColorFillDirection::LeftRight
			);
		}

		return *this;
	}

	const RoundRect& RoundRect::draw(const PatternParameters& pattern) const
	{
		if (r == 0.0)
		{
			rect.draw(pattern);
		}
		else
		{
			const double radius = Min(Abs(rect.w * 0.5), Abs(rect.h * 0.5), Abs(r));

			SIV3D_ENGINE(Renderer2D)->addRoundRect(
				FloatRect{ x, y, (x + w), (y + h) },
				static_cast<float>(radius),
				pattern
			);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	const RoundRect& RoundRect::drawFrame(const double thickness, const ColorF& color) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), color);
	}

	const RoundRect& RoundRect::drawFrame(const double thickness, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), topColor, bottomColor);
	}

	const RoundRect& RoundRect::drawFrame(const double thickness, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), leftColor, rightColor);
	}

	const RoundRect& RoundRect::drawFrame(const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		if (IsEmpty(rect, innerThickness, outerThickness))
		{
			return *this;
		}

		const double radius = Abs(r);
		const RectF outerRect = rect.stretched(outerThickness);
		const RoundRect outerRoundRect{ outerRect, Min((radius + outerThickness), (Min(outerRect.w, outerRect.h) * 0.5)) };
		const RectF innerRect = rect.stretched(-innerThickness);

		if ((innerRect.w <= 0.0) || (innerRect.h <= 0.0))
		{
			outerRoundRect.draw(color);
			return *this;
		}

		const RoundRect innerRoundRect{ innerRect, Clamp((radius - innerThickness), 0.0, (Min(innerRect.w, innerRect.h) * 0.5)) };

		SIV3D_ENGINE(Renderer2D)->addRoundRectFrame(
			FloatRect{ innerRoundRect.rect },
			static_cast<float>(innerRoundRect.r),
			FloatRect{ outerRoundRect.rect },
			static_cast<float>(outerRoundRect.r),
			color.toFloat4()
		);

		return *this;
	}

	const RoundRect& RoundRect::drawFrame(const double innerThickness, const double outerThickness, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		if (IsEmpty(rect, innerThickness, outerThickness))
		{
			return *this;
		}

		const double radius = Abs(r);
		const RectF outerRect = rect.stretched(outerThickness);
		const RoundRect outerRoundRect{ outerRect, Min((radius + outerThickness), (Min(outerRect.w, outerRect.h) * 0.5)) };
		const RectF innerRect = rect.stretched(-innerThickness);

		if ((innerRect.w <= 0.0) || (innerRect.h <= 0.0))
		{
			outerRoundRect.draw(topColor, bottomColor);
			return *this;
		}

		const RoundRect innerRoundRect{ innerRect, Clamp((radius - innerThickness), 0.0, (Min(innerRect.w, innerRect.h) * 0.5)) };

		SIV3D_ENGINE(Renderer2D)->addRoundRectFrame(
			FloatRect{ innerRoundRect.rect },
			static_cast<float>(innerRoundRect.r),
			FloatRect{ outerRoundRect.rect },
			static_cast<float>(outerRoundRect.r),
			topColor->toFloat4(),
			bottomColor->toFloat4(),
			ColorFillDirection::TopBottom
		);

		return *this;
	}

	const RoundRect& RoundRect::drawFrame(const double innerThickness, const double outerThickness, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		if (IsEmpty(rect, innerThickness, outerThickness))
		{
			return *this;
		}

		const double radius = Abs(r);
		const RectF outerRect = rect.stretched(outerThickness);
		const RoundRect outerRoundRect{ outerRect, Min((radius + outerThickness), (Min(outerRect.w, outerRect.h) * 0.5)) };
		const RectF innerRect = rect.stretched(-innerThickness);

		if ((innerRect.w <= 0.0) || (innerRect.h <= 0.0))
		{
			outerRoundRect.draw(leftColor, rightColor);
			return *this;
		}

		const RoundRect innerRoundRect{ innerRect, Clamp((radius - innerThickness), 0.0, (Min(innerRect.w, innerRect.h) * 0.5)) };

		SIV3D_ENGINE(Renderer2D)->addRoundRectFrame(
			FloatRect{ innerRoundRect.rect },
			static_cast<float>(innerRoundRect.r),
			FloatRect{ outerRoundRect.rect },
			static_cast<float>(outerRoundRect.r),
			leftColor->toFloat4(),
			rightColor->toFloat4(),
			ColorFillDirection::LeftRight
		);

		return *this;
	}

	const RoundRect& RoundRect::drawFrame(const double thickness, const PatternParameters& pattern) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), pattern);
	}

	const RoundRect& RoundRect::drawFrame(const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		if (IsEmpty(rect, innerThickness, outerThickness))
		{
			return *this;
		}

		const double radius = Abs(r);
		const RectF outerRect = rect.stretched(outerThickness);
		const RoundRect outerRoundRect{ outerRect, Min((radius + outerThickness), (Min(outerRect.w, outerRect.h) * 0.5)) };
		const RectF innerRect = rect.stretched(-innerThickness);

		if ((innerRect.w <= 0.0) || (innerRect.h <= 0.0))
		{
			outerRoundRect.draw(pattern);
			return *this;
		}

		const RoundRect innerRoundRect{ innerRect, Clamp((radius - innerThickness), 0.0, (Min(innerRect.w, innerRect.h) * 0.5)) };

		SIV3D_ENGINE(Renderer2D)->addRoundRectFrame(
			FloatRect{ innerRoundRect.rect },
			static_cast<float>(innerRoundRect.r),
			FloatRect{ outerRoundRect.rect },
			static_cast<float>(outerRoundRect.r),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawShadow
	//
	////////////////////////////////////////////////////////////////

	const RoundRect& RoundRect::drawShadow(const Vec2& offset, const double blur, const double spread, const ColorF& color, const bool fill) const
	{
		// 角丸でなければ長方形へ
		if (r == 0.0)
		{
			rect.drawShadow(offset, blur, spread, color, fill);
			return *this;
		}

		RoundRect baseRoundRect = movedBy(offset).stretched(spread);

		if (blur <= 0.0)
		{
			if (fill)
			{
				baseRoundRect.draw(color);
			}

			return *this;
		}

		baseRoundRect.r += spread;
		baseRoundRect.r = Min(baseRoundRect.r, (baseRoundRect.rect.size.minComponent() * 0.5));
		const double blurClamped = Min(baseRoundRect.w, baseRoundRect.h, blur);

		SIV3D_ENGINE(Renderer2D)->addRoundRectShadow(baseRoundRect, static_cast<float>(blurClamped), color.toFloat4(), fill);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ()
	//
	////////////////////////////////////////////////////////////////

	TexturedRoundRect RoundRect::operator ()(const Texture& texture) const
	{
		return{
			texture,
			0.0f, 0.0f, 1.0f, 1.0f,
			*this
		};
	}

	TexturedRoundRect RoundRect::operator ()(const TextureRegion& textureRegion) const
	{
		return{
			textureRegion.texture,
			textureRegion.uvRect,
			*this
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const RoundRect& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.w);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.h);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.r);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::RoundRect>::format(const s3d::RoundRect& value, fmt::format_context& ctx) const
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {}, {}, {})", value.x, value.y, value.w, value.h, value.r);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.x, value.y, value.w, value.h, value.r));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::RoundRect, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::RoundRect, s3d::char32>::format(const s3d::RoundRect& value, s3d::BufferContext& ctx) const
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {}, {}, {})", value.x, value.y, value.w, value.h, value.r);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.x, value.y, value.w, value.h, value.r);
	}
}
