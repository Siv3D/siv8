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
# include "Array.hpp"
# include "AssetHandle.hpp"
# include "FontMethod.hpp"
# include "FontStyle.hpp"
# include "ReadingDirection.hpp"
# include "GlyphIndex.hpp"
# include "GlyphInfo.hpp"
# include "OutlineGlyph.hpp"
# include "BitmapGlyph.hpp"
# include "ResolvedGlyph.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	struct FontFaceProperties;
	struct DrawableText;

	////////////////////////////////////////////////////////////////
	//
	//	Font
	//
	////////////////////////////////////////////////////////////////

	/// @brief 頂点シェーダ
	class Font : public AssetHandle<Font>
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Font();

		[[nodiscard]]
		Font(int32 baseSize, FilePathView path, FontStyle style = FontStyle::Normal);

		[[nodiscard]]
		Font(int32 baseSize, FilePathView path, size_t faceIndex, FontStyle style = FontStyle::Normal);

		[[nodiscard]]
		Font(int32 baseSize, FilePathView path, StringView styleName, FontStyle style = FontStyle::Normal);

		[[nodiscard]]
		Font(int32 baseSize, FilePathView path, size_t faceIndex, StringView styleName, FontStyle style = FontStyle::Normal);

		[[nodiscard]]
		Font(FontMethod fontMethod, int32 baseSize, FilePathView path);

		[[nodiscard]]
		Font(FontMethod fontMethod, int32 baseSize, FilePathView path, StringView styleName, FontStyle style = FontStyle::Normal);

		[[nodiscard]]
		Font(FontMethod fontMethod, int32 baseSize, FilePathView path, size_t faceIndex, StringView styleName, FontStyle style = FontStyle::Normal);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		virtual ~Font();

		////////////////////////////////////////////////////////////////
		//
		//	addFallback
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォールバックフォントを追加します。
		/// @param font フォールバックとして追加するフォント
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addFallback(const Font& font) const;

		////////////////////////////////////////////////////////////////
		//
		//	baseSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォトの基本サイズを返します。
		/// @return フォントの基本サイズ
		[[nodiscard]]
		int32 baseSize() const;

		////////////////////////////////////////////////////////////////
		//
		//	method
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントのレンダリング方式を返します。
		/// @return フォントのレンダリング方式
		[[nodiscard]]
		FontMethod method() const;

		////////////////////////////////////////////////////////////////
		//
		//	style
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief フォントのスタイルを返します。
		/// @return フォントのスタイル
		[[nodiscard]]
		FontStyle style() const;

		////////////////////////////////////////////////////////////////
		//
		//	properties
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const FontFaceProperties& properties() const;

		////////////////////////////////////////////////////////////////
		//
		//	familyName
		//
		////////////////////////////////////////////////////////////////

		const String& familyName() const;

		////////////////////////////////////////////////////////////////
		//
		//	styleName
		//
		////////////////////////////////////////////////////////////////

		const String& styleName() const;

		////////////////////////////////////////////////////////////////
		//
		//	postscriptName
		//
		////////////////////////////////////////////////////////////////

		const String& postscriptName() const;

		////////////////////////////////////////////////////////////////
		//
		//	version
		//
		////////////////////////////////////////////////////////////////

		const String& version() const;

		////////////////////////////////////////////////////////////////
		//
		//	hasColor
		//
		////////////////////////////////////////////////////////////////

		bool hasColor() const;

		////////////////////////////////////////////////////////////////
		//
		//	hasGlyphNames
		//
		////////////////////////////////////////////////////////////////

		bool hasGlyphNames() const;

		////////////////////////////////////////////////////////////////
		//
		//	ascender
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントのアセンダーの高さ（ピクセル）を返します。
		/// @return フォントのアセンダーの高さ（ピクセル）
		[[nodiscard]]
		double ascender() const;

		////////////////////////////////////////////////////////////////
		//
		//	descender
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントのディセンダーの高さ（ピクセル）を返します。
		/// @return フォントのディセンダーの高さ（ピクセル）
		[[nodiscard]]
		double descender() const;

		////////////////////////////////////////////////////////////////
		//
		//	height
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントの高さ（ピクセル）を返します。
		/// @remark `ascender() + descender()` と一致します。
		/// @return フォントの高さ（ピクセル）
		[[nodiscard]]
		double height() const;

		////////////////////////////////////////////////////////////////
		//
		//	spaceXAdvance
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double spaceXAdvance() const;

		////////////////////////////////////////////////////////////////
		//
		//	spaceYAdvance
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double spaceYAdvance() const;

		////////////////////////////////////////////////////////////////
		//
		//	tabSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief インデントに含まれるスペースの数を返します。
		/// @return インデントに含まれるスペースの数
		[[nodiscard]]
		int32 tabSize() const;

		////////////////////////////////////////////////////////////////
		//
		//	setTabSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief インデントに含まれるスペースの数を設定します。
		/// @param tabSize インデントに含まれるスペースの数
		/// @return *this
		const Font& setTabSize(int32 tabSize) const;

		////////////////////////////////////////////////////////////////
		//
		//	getBufferThickness
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//int32 getBufferThickness() const;

		////////////////////////////////////////////////////////////////
		//
		//	setBufferThickness
		//
		////////////////////////////////////////////////////////////////

		//const Font& setBufferThickness(int32 thickness) const;

		////////////////////////////////////////////////////////////////
		//
		//	num_glyphs
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントが持つグリフの数を返します。
		/// @return フォントが持つグリフの数。
		[[nodiscard]]
		uint32 num_glyphs() const;

		////////////////////////////////////////////////////////////////
		//
		//	hasGlyph
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字のグリフを持つかを返します。
		/// @param codePoint 文字
		/// @param readingDirection テキストの方向
		/// @return グリフを持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasGlyph(char32 codePoint, ReadingDirection readingDirection) const;

		/// @brief 指定した文字のグリフを持つかを返します。
		/// @param ch 文字
		/// @param readingDirection テキストの方向
		/// @remark char32 型の要素 1 つでは表現できない文字のための関数です。
		/// @return グリフを持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasGlyph(StringView ch, ReadingDirection readingDirection) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字の、このフォント内でのグリフインデックスを返します。
		/// @param codePoint 文字
		/// @param readingDirection テキストの方向
		/// @return このフォント内でのグリフインデックス
		[[nodiscard]]
		GlyphIndex getGlyphIndex(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		/// @brief 指定した文字の、このフォント内でのグリフインデックスを返します。
		/// @param ch 文字
		/// @param readingDirection テキストの方向
		/// @remark char32 型の要素 1 つでは表現できない文字のための関数です。
		/// @return このフォント内でのグリフインデックス
		[[nodiscard]]
		GlyphIndex getGlyphIndex(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlyphName
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		String getGlyphName(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		[[nodiscard]]
		String getGlyphName(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlyphNameByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		String getGlyphNameByGlyphIndex(GlyphIndex glyphIndex) const;

		////////////////////////////////////////////////////////////////
		//
		//	getXAdvance
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double getXAdvance(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		[[nodiscard]]
		double getXAdvance(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	getXAdvanceByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double getXAdvanceByGlyphIndex(GlyphIndex glyphIndex) const;

		////////////////////////////////////////////////////////////////
		//
		//	getYAdvance
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double getYAdvance(char32 codePoint) const;

		[[nodiscard]]
		double getYAdvance(StringView ch) const;

		////////////////////////////////////////////////////////////////
		//
		//	getYAdvanceByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double getYAdvanceByGlyphIndex(GlyphIndex glyphIndex) const;

		////////////////////////////////////////////////////////////////
		//
		//	getResolvedGlyphs
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<ResolvedGlyph> getResolvedGlyphs(StringView s, ReadingDirection readingDirection = ReadingDirection::LeftToRight, EnableFallback enableFallback = EnableFallback::Yes, EnableLigatures enableLigatures = EnableLigatures::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlyphInfo
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		GlyphInfo getGlyphInfo(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		[[nodiscard]]
		GlyphInfo getGlyphInfo(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlyphInfoByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		GlyphInfo getGlyphInfoByGlyphIndex(GlyphIndex glyphIndex, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	renderOutline
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		OutlineGlyph renderOutline(char32 codePoint, CloseRing closeRing = CloseRing::No, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		[[nodiscard]]
		OutlineGlyph renderOutline(StringView ch, CloseRing closeRing = CloseRing::No, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	renderOutlineByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		OutlineGlyph renderOutlineByGlyphIndex(GlyphIndex glyphIndex, CloseRing closeRing = CloseRing::No, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	renderOutlines
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Array<OutlineGlyph> renderOutlines(StringView s, CloseRing closeRing = CloseRing::No, EnableLigatures enableLigatures = EnableLigatures::Yes, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	renderBitmap
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BitmapGlyph renderBitmap(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		[[nodiscard]]
		BitmapGlyph renderBitmap(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	renderBitmapByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BitmapGlyph renderBitmapByGlyphIndex(GlyphIndex glyphIndex, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;




		////////////////////////////////////////////////////////////////
		//
		//	getTexture
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントの内部でキャッシュされているテクスチャを返します。
		/// @return フォントの内部でキャッシュされているテクスチャ
		[[nodiscard]]
		const Texture& getTexture() const;



		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の Font と内容を交換します。
		/// @param Font 別の Font
		void swap(Font& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントを描画するために必要な DrawableText を、文字列から構築します。
		/// @param text 文字列
		/// @return DrawableText
		[[nodiscard]]
		DrawableText operator ()(const String& text) const;

		/// @brief フォントを描画するために必要な DrawableText を、文字列から構築します。
		/// @param text 文字列
		/// @return DrawableText
		[[nodiscard]]
		DrawableText operator ()(String&& text) const;

		/// @brief フォントを描画するために必要な DrawableText を、一連の引数を文字列に変換することで構築します。
		/// @param ...args 文字列に変換する値
		/// @return DrawableText
		template <Concept::Formattable... Args>
		[[nodiscard]]
		DrawableText operator ()(const Args& ... args) const;

		/// @brief Format できない値が Format() に渡されたときに発生するエラーです
		template <class... Args>
		DrawableText operator ()(const Args&...) = delete;




		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Font を入れ替えます。
		/// @param lhs 一方の Font
		/// @param rhs もう一方の Font
		friend void swap(Font& lhs, Font& rhs) noexcept
		{
			lhs.swap(rhs);
		}


		////////////////////////////////////////////////////////////////
		//
		//	GetFaces
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Array<FontFaceProperties> GetFaces(FilePathView path);
	};
}

