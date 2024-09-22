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
# include <Siv3D/Utility.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool IsEmpty(const RectF& rect, const double innerThickness, const double outerThickness) noexcept
		{
			return ((rect.w <= 0.0) || (rect.h <= 0.0)
				|| (innerThickness < 0.0) || (outerThickness < 0.0)
				|| ((innerThickness == 0.0) && (outerThickness == 0.0)));
		}
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
			const double radius = Min(Abs(rect.w * 0.5), Abs(rect.h * 0.5), Abs(r));

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
}
