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

# pragma once
# include "Common.hpp"
# include "Anchor.hpp"
# include "String.hpp"
# include "Font.hpp"
# include "Format.hpp"
# include "ReadingDirection.hpp"
# include "TextLayoutResult.hpp"

namespace s3d
{
	struct TextStyle;
	struct ITextEffect;
	
	////////////////////////////////////////////////////////////////
	//
	//	DrawableText
	//
	////////////////////////////////////////////////////////////////

	/// @brief 描画可能なテキスト
	struct DrawableText
	{
		/// @brief フォント
		Font font;

		/// @brief テキスト
		String text;

		/// @brief グリフの解決結果
		Array<ResolvedGlyph> resolvedGlyphs;

		/// @brief 読み方向
		ReadingDirection readingDirection = ReadingDirection::LeftToRight;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		DrawableText() = default;

		[[nodiscard]]
		DrawableText(const Font& _font, const String& _text, EnableLigatures enableLigatures = EnableLigatures::Yes);

		[[nodiscard]]
		DrawableText(const Font& _font, String&& _text, EnableLigatures enableLigatures = EnableLigatures::Yes);

		[[nodiscard]]
		DrawableText(const Font& _font, const String& _text, ReadingDirection _readingDirection, EnableLigatures enableLigatures = EnableLigatures::Yes);

		[[nodiscard]]
		DrawableText(const Font& _font, String&& _text, ReadingDirection _readingDirection, EnableLigatures enableLigatures = EnableLigatures::Yes);

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
		RectF region(Anchor anchor, double x, double y) const;

		[[nodiscard]]
		RectF region(Anchor anchor, Vec2 pos) const;

		[[nodiscard]]
		RectF region(double size, double x, double y) const;

		[[nodiscard]]
		RectF region(double size, Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF region(double size, Anchor anchor, double x, double y) const;

		[[nodiscard]]
		RectF region(double size, Anchor anchor, Vec2 pos) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, double x, double y) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Vec2 pos = Vec2{ 0, 0 }) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Anchor anchor, double x, double y) const;

		[[nodiscard]]
		RectF region(const TextStyle& textStyle, double size, Anchor anchor, Vec2 pos) const;

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

		RectF draw(Anchor anchor, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(Anchor anchor, const Vec2& pos, const ColorF& color = Palette::White) const;

		TextLayoutResult draw(const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(double size, const Vec2& pos, const ColorF& color = Palette::White) const;

		RectF draw(double size, Anchor anchor, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(double size, Anchor anchor, const Vec2& pos, const ColorF& color = Palette::White) const;

		TextLayoutResult draw(double size, const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Anchor anchor, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Anchor anchor, const Vec2& pos, const ColorF& color = Palette::White) const;

		TextLayoutResult draw(const TextStyle& textStyle, const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, const Vec2& pos, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Anchor anchor, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Anchor anchor, const Vec2& pos, const ColorF& color = Palette::White) const;

		TextLayoutResult draw(const TextStyle& textStyle, double size, const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(double x, double y, const ITextEffect& textEffect) const;

		RectF draw(const Vec2& pos, const ITextEffect& textEffect) const;

		RectF draw(Anchor anchor, double x, double y, const ITextEffect& textEffect) const;

		RectF draw(Anchor anchor, const Vec2& pos, const ITextEffect& textEffect) const;

		TextLayoutResult draw(const RectF& area, const ITextEffect& textEffect) const;

		RectF draw(double size, double x, double y, const ITextEffect& textEffect) const;

		RectF draw(double size, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF draw(double size, Anchor anchor, double x, double y, const ITextEffect& textEffect) const;

		RectF draw(double size, Anchor anchor, const Vec2& pos, const ITextEffect& textEffect) const;

		TextLayoutResult draw(double size, const RectF& area, const ITextEffect& textEffect) const;

		RectF draw(const TextStyle& textStyle, double x, double y, const ITextEffect& textEffect) const;

		RectF draw(const TextStyle& textStyle, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF draw(const TextStyle& textStyle, Anchor anchor, double x, double y, const ITextEffect& textEffect) const;

		RectF draw(const TextStyle& textStyle, Anchor anchor, const Vec2& pos, const ITextEffect& textEffect) const;

		TextLayoutResult draw(const TextStyle& textStyle, const RectF& area, const ITextEffect& textEffect) const;

		RectF draw(const TextStyle& textStyle, double size, double x, double y, const ITextEffect& textEffect) const;

		RectF draw(const TextStyle& textStyle, double size, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF draw(const TextStyle& textStyle, double size, Anchor anchor, double x, double y, const ITextEffect& textEffect) const;

		RectF draw(const TextStyle& textStyle, double size, Anchor anchor, const Vec2& pos, const ITextEffect& textEffect) const;

		TextLayoutResult draw(const TextStyle& textStyle, double size, const RectF& area, const ITextEffect& textEffect) const;

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

		RectF drawBase(double x, double y, const ITextEffect& textEffect) const;

		RectF drawBase(const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawBase(Arg::left_<Vec2> left, const ITextEffect& textEffect) const;

		RectF drawBase(Arg::center_<Vec2> center, const ITextEffect& textEffect) const;

		RectF drawBase(Arg::right_<Vec2> right, const ITextEffect& textEffect) const;

		RectF drawBase(double size, double x, double y, const ITextEffect& textEffect) const;

		RectF drawBase(double size, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawBase(double size, Arg::left_<Vec2> left, const ITextEffect& textEffect) const;

		RectF drawBase(double size, Arg::center_<Vec2> center, const ITextEffect& textEffect) const;

		RectF drawBase(double size, Arg::right_<Vec2> right, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, double x, double y, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, Arg::left_<Vec2> left, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, Arg::center_<Vec2> center, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, Arg::right_<Vec2> right, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, double size, double x, double y, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, double size, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, double size, Arg::left_<Vec2> left, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, double size, Arg::center_<Vec2> center, const ITextEffect& textEffect) const;

		RectF drawBase(const TextStyle& textStyle, double size, Arg::right_<Vec2> right, const ITextEffect& textEffect) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawAt
		//
		////////////////////////////////////////////////////////////////

		RectF drawAt(double x, double y, const ColorF& color = Palette::White) const;

		RectF drawAt(const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawAt(double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF drawAt(double size, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawAt(const TextStyle& textStyle, double x, double y, const ColorF& color = Palette::White) const;

		RectF drawAt(const TextStyle& textStyle, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawAt(const TextStyle& textStyle, double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF drawAt(const TextStyle& textStyle, double size, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawAt(double x, double y, const ITextEffect& textEffect) const;

		RectF drawAt(const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawAt(double size, double x, double y, const ITextEffect& textEffect) const;

		RectF drawAt(double size, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawAt(const TextStyle& textStyle, double x, double y, const ITextEffect& textEffect) const;

		RectF drawAt(const TextStyle& textStyle, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawAt(const TextStyle& textStyle, double size, double x, double y, const ITextEffect& textEffect) const;

		RectF drawAt(const TextStyle& textStyle, double size, const Vec2& pos, const ITextEffect& textEffect) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawBaseAt
		//
		////////////////////////////////////////////////////////////////

		RectF drawBaseAt(double x, double y, const ColorF& color = Palette::White) const;

		RectF drawBaseAt(const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawBaseAt(double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF drawBaseAt(double size, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawBaseAt(const TextStyle& textStyle, double x, double y, const ColorF& color = Palette::White) const;

		RectF drawBaseAt(const TextStyle& textStyle, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawBaseAt(const TextStyle& textStyle, double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF drawBaseAt(const TextStyle& textStyle, double size, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF drawBaseAt(double x, double y, const ITextEffect& textEffect) const;

		RectF drawBaseAt(const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawBaseAt(double size, double x, double y, const ITextEffect& textEffect) const;

		RectF drawBaseAt(double size, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawBaseAt(const TextStyle& textStyle, double x, double y, const ITextEffect& textEffect) const;

		RectF drawBaseAt(const TextStyle& textStyle, const Vec2& pos, const ITextEffect& textEffect) const;

		RectF drawBaseAt(const TextStyle& textStyle, double size, double x, double y, const ITextEffect& textEffect) const;

		RectF drawBaseAt(const TextStyle& textStyle, double size, const Vec2& pos, const ITextEffect& textEffect) const;

		////////////////////////////////////////////////////////////////
		//
		//	fits
		//
		////////////////////////////////////////////////////////////////

		TextLayoutResult fits(const RectF& area) const;

		TextLayoutResult fits(double size, const RectF& area) const;

		TextLayoutResult fits(const TextStyle& textStyle, const RectF& area) const;

		TextLayoutResult fits(const TextStyle& textStyle, double size, const RectF& area) const;

		////////////////////////////////////////////////////////////////
		//
		//	renderOutlines
		//
		////////////////////////////////////////////////////////////////

		Array<Array<LineString>> renderOutlines(const Vec2& pos = Vec2{ 0, 0 }, CloseRing closeRing = CloseRing::No) const;

		Array<Array<LineString>> renderOutlines(double size, const Vec2& pos = Vec2{ 0, 0 }, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	renderOutlinesAt
		//
		////////////////////////////////////////////////////////////////

		Array<Array<LineString>> renderOutlinesAt(const Vec2& pos = Vec2{ 0, 0 }, CloseRing closeRing = CloseRing::No) const;

		Array<Array<LineString>> renderOutlinesAt(double size, const Vec2& pos = Vec2{ 0, 0 }, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	renderPolygons
		//
		////////////////////////////////////////////////////////////////

		Array<MultiPolygon> renderPolygons(const Vec2& pos = Vec2{ 0, 0 }) const;

		Array<MultiPolygon> renderPolygons(double size, const Vec2& pos = Vec2{ 0, 0 }) const;

		////////////////////////////////////////////////////////////////
		//
		//	renderPolygonsAt
		//
		////////////////////////////////////////////////////////////////

		Array<MultiPolygon> renderPolygonsAt(const Vec2& pos = Vec2{ 0, 0 }) const;

		Array<MultiPolygon> renderPolygonsAt(double size, const Vec2& pos = Vec2{ 0, 0 }) const;
	};
}

# include "detail/DrawableText.ipp"
