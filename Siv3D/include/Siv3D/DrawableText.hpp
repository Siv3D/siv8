//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "Font.hpp"
# include "Format.hpp"
# include "ReadingDirection.hpp"

namespace s3d
{
	struct TextStyle;

	struct DrawableText
	{
		Font font;

		String text;

		Array<ResolvedGlyph> resolvedGlyphs;

		ReadingDirection readingDirection = ReadingDirection::LeftToRight;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		DrawableText() = default;

		[[nodiscard]]
		DrawableText(const Font& _font, const String& _text, ReadingDirection _readingDirection);

		[[nodiscard]]
		DrawableText(const Font& _font, String&& _text, ReadingDirection _readingDirection);

		////////////////////////////////////////////////////////////////
		//
		//	getXAdvances
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<double> getXAdvances() const;

		[[nodiscard]]
		Array<double> getXAdvances(double fontSize) const;

		////////////////////////////////////////////////////////////////
		//
		//	region
		//
		////////////////////////////////////////////////////////////////

		/// @brief テキストが描画される領域を返します。
		/// @param x 描画する左上の X 座標
		/// @param y 描画する左上の Y 座標
		/// @return テキストが描画される領域
		[[nodiscard]]
		RectF region(double x, double y) const;

		/// @brief テキストが描画される領域を返します。
		/// @param pos 描画する左上の座標
		/// @return テキストが描画される領域
		[[nodiscard]]
		RectF region(Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF region(Arg::topLeft_<Vec2> topLeft) const;

		[[nodiscard]]
		RectF region(Arg::topCenter_<Vec2> topCenter) const;

		[[nodiscard]]
		RectF region(Arg::topRight_<Vec2> topRight) const;

		[[nodiscard]]
		RectF region(Arg::rightCenter_<Vec2> rightCenter) const;

		[[nodiscard]]
		RectF region(Arg::bottomRight_<Vec2> bottomRight) const;

		[[nodiscard]]
		RectF region(Arg::bottomCenter_<Vec2> bottomCenter) const;

		[[nodiscard]]
		RectF region(Arg::bottomLeft_<Vec2> bottomLeft) const;

		[[nodiscard]]
		RectF region(Arg::leftCenter_<Vec2> leftCenter) const;

		[[nodiscard]]
		RectF region(Arg::center_<Vec2> center) const;

		[[nodiscard]]
		RectF region(double size, double x, double y) const;

		[[nodiscard]]
		RectF region(double size, Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF region(double size, Arg::topLeft_<Vec2> topLeft) const;

		[[nodiscard]]
		RectF region(double size, Arg::topCenter_<Vec2> topCenter) const;

		[[nodiscard]]
		RectF region(double size, Arg::topRight_<Vec2> topRight) const;

		[[nodiscard]]
		RectF region(double size, Arg::rightCenter_<Vec2> rightCenter) const;

		[[nodiscard]]
		RectF region(double size, Arg::bottomRight_<Vec2> bottomRight) const;

		[[nodiscard]]
		RectF region(double size, Arg::bottomCenter_<Vec2> bottomCenter) const;

		[[nodiscard]]
		RectF region(double size, Arg::bottomLeft_<Vec2> bottomLeft) const;

		[[nodiscard]]
		RectF region(double size, Arg::leftCenter_<Vec2> leftCenter) const;

		[[nodiscard]]
		RectF region(double size, Arg::center_<Vec2> center) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, double x, double y) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Arg::topLeft_<Vec2> topLeft) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Arg::topCenter_<Vec2> topCenter) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Arg::topRight_<Vec2> topRight) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Arg::rightCenter_<Vec2> rightCenter) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Arg::bottomRight_<Vec2> bottomRight) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Arg::bottomCenter_<Vec2> bottomCenter) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Arg::bottomLeft_<Vec2> bottomLeft) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Arg::leftCenter_<Vec2> leftCenter) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Arg::center_<Vec2> center) const;

		////////////////////////////////////////////////////////////////
		//
		//	regionBase
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		RectF regionBase(double x, double y) const;

		[[nodiscard]]
		RectF regionBase(Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF regionBase(Arg::left_<Vec2> left) const;

		[[nodiscard]]
		RectF regionBase(Arg::center_<Vec2> center) const;

		[[nodiscard]]
		RectF regionBase(Arg::right_<Vec2> right) const;

		[[nodiscard]]
		RectF regionBase(double size, double x, double y) const;

		[[nodiscard]]
		RectF regionBase(double size, Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF regionBase(double size, Arg::left_<Vec2> left) const;

		[[nodiscard]]
		RectF regionBase(double size, Arg::center_<Vec2> center) const;

		[[nodiscard]]
		RectF regionBase(double size, Arg::right_<Vec2> right) const;

		[[nodiscard]]
		RectF regionBase(const TextStyle& textStyle, double size, double x, double y) const; 

		[[nodiscard]]
		RectF regionBase(const TextStyle& textStyle, double size, Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF regionBase(const TextStyle& textStyle, double size, Arg::left_<Vec2> left) const;

		[[nodiscard]]
		RectF regionBase(const TextStyle& textStyle, double size, Arg::center_<Vec2> center) const;

		[[nodiscard]]
		RectF regionBase(const TextStyle& textStyle, double size, Arg::right_<Vec2> right) const;

		////////////////////////////////////////////////////////////////
		//
		//	regionAt
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		RectF regionAt(double x, double y) const;

		[[nodiscard]]
		RectF regionAt(Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF regionAt(double size, double x, double y) const;

		[[nodiscard]]
		RectF regionAt(double size, Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF regionAt(const TextStyle& textStyle, double size, double x, double y) const;

		[[nodiscard]]
		RectF regionAt(const TextStyle& textStyle, double size, Vec2 pos = Vec2{ 0, 0 }) const;

		////////////////////////////////////////////////////////////////
		//
		//	regionBaseAt
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		RectF regionBaseAt(double x, double y) const;
		
		[[nodiscard]]
		RectF regionBaseAt(Vec2 pos = Vec2{ 0, 0 }) const;
		
		[[nodiscard]]
		RectF regionBaseAt(double size, double x, double y) const;
		
		[[nodiscard]]
		RectF regionBaseAt(double size, Vec2 pos = Vec2{ 0, 0 }) const;
		
		[[nodiscard]]
		RectF regionBaseAt(const TextStyle& textStyle, double size, double x, double y) const;
		
		[[nodiscard]]
		RectF regionBaseAt(const TextStyle& textStyle, double size, Vec2 pos = Vec2{ 0, 0 }) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		RectF draw(double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF draw(Arg::topLeft_<Vec2> topLeft, const ColorF& color = Palette::White) const;

		RectF draw(Arg::topCenter_<Vec2> topCenter, const ColorF& color = Palette::White) const;

		RectF draw(Arg::topRight_<Vec2> topRight, const ColorF& color = Palette::White) const;

		RectF draw(Arg::rightCenter_<Vec2> rightCenter, const ColorF& color = Palette::White) const;

		RectF draw(Arg::bottomRight_<Vec2> bottomRight, const ColorF& color = Palette::White) const;

		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color = Palette::White) const;

		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color = Palette::White) const;

		RectF draw(Arg::leftCenter_<Vec2> leftCenter, const ColorF& color = Palette::White) const;

		RectF draw(Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		//bool draw(const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(double size, const Vec2& pos, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::topLeft_<Vec2> topLeft, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::topCenter_<Vec2> topCenter, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::topRight_<Vec2> topRight, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::rightCenter_<Vec2> rightCenter, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::bottomRight_<Vec2> bottomRight, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::leftCenter_<Vec2> leftCenter, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		//bool draw(double size, const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::topLeft_<Vec2> topLeft, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::topCenter_<Vec2> topCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::topRight_<Vec2> topRight, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::rightCenter_<Vec2> rightCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::bottomRight_<Vec2> bottomRight, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::leftCenter_<Vec2> leftCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		//bool draw(const TextStyle& textStyle, const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, const Vec2& pos, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::topLeft_<Vec2> topLeft, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::topCenter_<Vec2> topCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::topRight_<Vec2> topRight, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::rightCenter_<Vec2> rightCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::bottomRight_<Vec2> bottomRight, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::leftCenter_<Vec2> leftCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		//bool draw(const TextStyle& textStyle, double size, const RectF& area, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawBase
		//
		////////////////////////////////////////////////////////////////

		RectF drawBase(double x, double y, const ColorF& color = Palette::White) const;

		RectF drawBase(const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawBase(Arg::left_<Vec2> left, const ColorF& color = Palette::White) const;

		RectF drawBase(Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		RectF drawBase(Arg::right_<Vec2> right, const ColorF& color = Palette::White) const;

		RectF drawBase(double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF drawBase(double size, const Vec2& pos, const ColorF& color = Palette::White) const;

		RectF drawBase(double size, Arg::left_<Vec2> left, const ColorF& color = Palette::White) const;

		RectF drawBase(double size, Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		RectF drawBase(double size, Arg::right_<Vec2> right, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, double x, double y, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, Arg::left_<Vec2> left, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, Arg::right_<Vec2> right, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, double size, const Vec2& pos, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, double size, Arg::left_<Vec2> left, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, double size, Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		RectF drawBase(const TextStyle& textStyle, double size, Arg::right_<Vec2> right, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawAt
		//
		////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////
		//
		//	drawBaseAt
		//
		////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////
		//
		//	fits
		//
		////////////////////////////////////////////////////////////////

		bool fits(const RectF& area) const;

		bool fits(double size, const RectF& area) const;

	};
}

# include "detail/DrawableText.ipp"
