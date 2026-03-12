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
# include "Array.hpp"
# include "AssetHandle.hpp"
# include "FontMethod.hpp"
# include "FontOptions.hpp"
# include "ReadingDirection.hpp"
# include "GlyphIndex.hpp"
# include "GlyphInfo.hpp"
# include "Glyph.hpp"
# include "OutlineGlyph.hpp"
# include "PolygonGlyph.hpp"
# include "BitmapGlyph.hpp"
# include "MSDFGlyph.hpp"
# include "ResolvedGlyph.hpp"
# include "PredefinedYesNo.hpp"
# include "Typeface.hpp"
# include "TextStyle.hpp"

namespace s3d
{
	struct FontFaceProperties;
	struct DrawableText;
	class PixelShader;

	////////////////////////////////////////////////////////////////
	//
	//	Font
	//
	////////////////////////////////////////////////////////////////

	/// @brief フォント
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

		/// @brief フォントを作成します。
		/// @param baseSize 基本サイズ
		/// @param typeface 標準同梱フォントの種類
		/// @param options フォントのオプション
		[[nodiscard]]
		explicit Font(int32 baseSize, Typeface typeface = Typeface::Regular, const FontOptions& options = {});

		/// @brief フォントを作成します。
		/// @param baseSize 基本サイズ
		/// @param path フォントファイルのパス
		/// @param options フォントのオプション
		[[nodiscard]]
		Font(int32 baseSize, FilePathView path, const FontOptions& options = {});

		/// @brief フォントを作成します。
		/// @param fontMethod フォントのレンダリング方式
		/// @param baseSize 基本サイズ
		/// @param typeface 標準同梱フォントの種類
		/// @param options フォントのオプション
		[[nodiscard]]
		Font(FontMethod fontMethod, int32 baseSize, Typeface typeface = Typeface::Regular, const FontOptions& options = {});

		/// @brief フォントを作成します。
		/// @param fontMethod フォントのレンダリング方式
		/// @param baseSize 基本サイズ
		/// @param path フォントファイルのパス
		/// @param options フォントのオプション
		[[nodiscard]]
		Font(FontMethod fontMethod, int32 baseSize, FilePathView path, const FontOptions& options = {});

		/// @brief フォントを作成します。
		/// @param fontMethod フォントのレンダリング方式
		/// @param baseSize 基本サイズ
		/// @param bufferThickness MSDF バッファの大きさ（ピクセル）。オフセットの大きいシャドウや広い輪郭が必要な場合は大きくする
		/// @param typeface 標準同梱フォントの種類
		/// @param options フォントのオプション
		[[nodiscard]]
		Font(FontMethod fontMethod, int32 baseSize, int32 bufferThickness, Typeface typeface = Typeface::Regular, const FontOptions& options = {});

		/// @brief フォントを作成します。
		/// @param fontMethod フォントのレンダリング方式
		/// @param baseSize 基本サイズ
		/// @param bufferThickness SDF バッファの大きさ（ピクセル）。オフセットの大きいシャドウや広い輪郭が必要な場合は大きくする
		/// @param path フォントファイルのパス
		/// @param options フォントのオプション
		[[nodiscard]]
		Font(FontMethod fontMethod, int32 baseSize, int32 bufferThickness, FilePathView path, const FontOptions& options = {});

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
		//	options
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief フォントのオプションを返します。
		/// @return フォントのオプション
		[[nodiscard]]
		const FontOptions& options() const;

		////////////////////////////////////////////////////////////////
		//
		//	properties
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントのプロパティを返します。
		/// @return フォントのプロパティ
		[[nodiscard]]
		const FontFaceProperties& properties() const;

		////////////////////////////////////////////////////////////////
		//
		//	familyName
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントのファミリー名を返します。
		/// @return フォントのファミリー名
		[[nodiscard]]
		const String& familyName() const;

		////////////////////////////////////////////////////////////////
		//
		//	styleName
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントのスタイル名を返します。
		/// @return フォントのスタイル名
		[[nodiscard]]
		const String& styleName() const;

		////////////////////////////////////////////////////////////////
		//
		//	postscriptName
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントのポストスクリプト名を返します。
		/// @return フォントのポストスクリプト名
		[[nodiscard]]
		const String& postscriptName() const;

		////////////////////////////////////////////////////////////////
		//
		//	version
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォントのバージョンを返します。
		/// @return フォントのバージョン
		[[nodiscard]]
		const String& version() const;

		////////////////////////////////////////////////////////////////
		//
		//	hasColor
		//
		////////////////////////////////////////////////////////////////

		/// @brief カラーグリフであるかを返します。
		/// @return カラーグリフである場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasColor() const;

		////////////////////////////////////////////////////////////////
		//
		//	hasGlyphNames
		//
		////////////////////////////////////////////////////////////////

		/// @brief グリフ名を持つかを返します。
		/// @return グリフ名を持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasGlyphNames() const;

		////////////////////////////////////////////////////////////////
		//
		//	ascender
		//
		////////////////////////////////////////////////////////////////

		/// @brief アセンダーの高さ（ピクセル）を返します。
		/// @return アセンダーの高さ（ピクセル）
		[[nodiscard]]
		double ascender() const;

		////////////////////////////////////////////////////////////////
		//
		//	descender
		//
		////////////////////////////////////////////////////////////////

		/// @brief ディセンダーの高さ（ピクセル）を返します。
		/// @return ディセンダーの高さ（ピクセル）
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

		/// @brief 指定したフォントサイズでテキストを描画するときのフォントの高さ（ピクセル）を返します。
		/// @param size フォントサイズ
		/// @return フォントの高さ（ピクセル）
		[[nodiscard]]
		double height(double size) const;

		////////////////////////////////////////////////////////////////
		//
		//	spaceXAdvance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 水平方向のスペースの幅を返します。
		/// @return 水平方向のスペースの幅
		[[nodiscard]]
		double spaceXAdvance() const;

		////////////////////////////////////////////////////////////////
		//
		//	spaceYAdvance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 垂直方向のスペースの幅を返します。
		/// @return 垂直方向のスペースの幅
		[[nodiscard]]
		double spaceYAdvance() const;

		////////////////////////////////////////////////////////////////
		//
		//	tabSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief インデントに含まれるスペースの個数を返します。
		/// @return インデントに含まれるスペースの個数
		[[nodiscard]]
		int32 tabSize() const;

		////////////////////////////////////////////////////////////////
		//
		//	setTabSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief インデントに含まれるスペースの個数を設定します。
		/// @param tabSize インデントに含まれるスペースの個数
		/// @return *this
		const Font& setTabSize(int32 tabSize) const;

		////////////////////////////////////////////////////////////////
		//
		//	getBufferThickness
		//
		////////////////////////////////////////////////////////////////

		/// @brief SDF バッファの大きさ（ピクセル）を返します。
		/// @return SDF バッファの大きさ（ピクセル）
		[[nodiscard]]
		int32 getBufferThickness() const;

		////////////////////////////////////////////////////////////////
		//
		//	setBufferThickness
		//
		////////////////////////////////////////////////////////////////

		/// @brief SDF バッファの大きさ（ピクセル）を設定します。
		/// @param thickness SDF バッファの大きさ（ピクセル）
		/// @return *this
		const Font& setBufferThickness(int32 thickness) const;

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
		bool hasGlyph(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		/// @brief 指定した文字のグリフを持つかを返します。
		/// @param ch 文字
		/// @param readingDirection テキストの方向
		/// @remark char32 型の要素 1 つでは表現できない文字のための関数です。
		/// @return グリフを持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasGlyph(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

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

		/// @brief 指定した文字のグリフ名を返します。
		/// @param codePoint 文字
		/// @param readingDirection テキストの方向
		/// @return グリフ名
		[[nodiscard]]
		String getGlyphName(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		/// @brief 指定した文字のグリフ名を返します。
		/// @param ch 文字
		/// @param readingDirection テキストの方向
		/// @return グリフ名
		[[nodiscard]]
		String getGlyphName(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlyphNameByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したグリフのグリフ名を返します。
		/// @param glyphIndex グリフインデックス
		/// @return グリフ名
		[[nodiscard]]
		String getGlyphNameByGlyphIndex(GlyphIndex glyphIndex) const;

		////////////////////////////////////////////////////////////////
		//
		//	getXAdvance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字のグリフの水平方向のアドバンス幅を返します。
		/// @param codePoint 文字
		/// @param readingDirection テキストの方向
		/// @return 水平方向のアドバンス幅
		[[nodiscard]]
		double getXAdvance(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		/// @brief 指定した文字のグリフの水平方向のアドバンス幅を返します。
		/// @param ch 文字
		/// @param readingDirection テキストの方向
		/// @return 水平方向のアドバンス幅
		[[nodiscard]]
		double getXAdvance(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	getXAdvanceByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したグリフの水平方向のアドバンス幅を返します。
		/// @param glyphIndex グリフインデックス
		/// @return 水平方向のアドバンス幅
		[[nodiscard]]
		double getXAdvanceByGlyphIndex(GlyphIndex glyphIndex) const;

		////////////////////////////////////////////////////////////////
		//
		//	getYAdvance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字のグリフの垂直方向のアドバンス幅を返します。
		/// @param codePoint 文字
		/// @return 垂直方向のアドバンス幅
		[[nodiscard]]
		double getYAdvance(char32 codePoint) const;

		/// @brief 指定した文字のグリフの垂直方向のアドバンス幅を返します。
		/// @param ch 文字
		/// @return 垂直方向のアドバンス幅
		[[nodiscard]]
		double getYAdvance(StringView ch) const;

		////////////////////////////////////////////////////////////////
		//
		//	getYAdvanceByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したグリフの垂直方向のアドバンス幅を返します。
		/// @param glyphIndex グリフインデックス
		/// @return 垂直方向のアドバンス幅
		[[nodiscard]]
		double getYAdvanceByGlyphIndex(GlyphIndex glyphIndex) const;

		////////////////////////////////////////////////////////////////
		//
		//	getResolvedGlyphs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字列に対するグリフインデックスの配列を返します。
		/// @param s 文字列
		/// @param readingDirection テキストの方向
		/// @param enableFallback フォールバックを有効にするか
		/// @param enableLigatures 合字を有効にするか
		/// @return グリフインデックスの配列
		[[nodiscard]]
		Array<ResolvedGlyph> getResolvedGlyphs(StringView s, ReadingDirection readingDirection = ReadingDirection::LeftToRight, EnableFallback enableFallback = EnableFallback::Yes, EnableLigatures enableLigatures = EnableLigatures::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	generateOutlineGlyph
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字のアウトライングリフを返します。
		/// @param codePoint 文字
		/// @param closeRing 頂点配列の終点を始点と重ねるか
		/// @param readingDirection テキストの方向
		/// @return アウトライングリフ
		[[nodiscard]]
		OutlineGlyph generateOutlineGlyph(char32 codePoint, CloseRing closeRing = CloseRing::No, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		/// @brief 指定した文字のアウトライングリフを返します。
		/// @param ch 文字
		/// @param closeRing 頂点配列の終点を始点と重ねるか
		/// @param readingDirection テキストの方向
		/// @return アウトライングリフ
		[[nodiscard]]
		OutlineGlyph generateOutlineGlyph(StringView ch, CloseRing closeRing = CloseRing::No, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	generateOutlineGlyphByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したグリフのアウトライングリフを返します。
		/// @param glyphIndex グリフインデックス
		/// @param closeRing 頂点配列の終点を始点と重ねるか
		/// @param readingDirection テキストの方向
		/// @return アウトライングリフ
		[[nodiscard]]
		OutlineGlyph generateOutlineGlyphByGlyphIndex(GlyphIndex glyphIndex, CloseRing closeRing = CloseRing::No, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	generateOutlineGlyphs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字列のアウトライングリフの配列を返します。
		/// @param s 文字列
		/// @param closeRing 頂点配列の終点を始点と重ねるか
		/// @param enableLigatures 合字を有効にするか
		/// @param readingDirection テキストの方向
		/// @return アウトライングリフの配列
		[[nodiscard]]
		Array<OutlineGlyph> generateOutlineGlyphs(StringView s, CloseRing closeRing = CloseRing::No, EnableLigatures enableLigatures = EnableLigatures::Yes, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	generatePolygonGlyph
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字のポリゴングリフを返します。
		/// @param codePoint 文字
		/// @param readingDirection テキストの方向
		/// @return ポリゴングリフ
		[[nodiscard]]
		PolygonGlyph generatePolygonGlyph(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		/// @brief 指定した文字のポリゴングリフを返します。
		/// @param ch 文字
		/// @param readingDirection テキストの方向
		/// @return ポリゴングリフ
		[[nodiscard]]
		PolygonGlyph generatePolygonGlyph(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	generatePolygonGlyphByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したグリフのポリゴングリフを返します。
		/// @param glyphIndex グリフインデックス
		/// @param readingDirection テキストの方向
		/// @return ポリゴングリフ
		[[nodiscard]]
		PolygonGlyph generatePolygonGlyphByGlyphIndex(GlyphIndex glyphIndex, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	generatePolygonGlyphs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字列のポリゴングリフの配列を返します。
		/// @param s 文字列
		/// @param enableLigatures 合字を有効にするか
		/// @param readingDirection テキストの方向
		/// @return ポリゴングリフの配列
		[[nodiscard]]
		Array<PolygonGlyph> generatePolygonGlyphs(StringView s, EnableLigatures enableLigatures = EnableLigatures::Yes, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		//////////////////////////////////////////////////////////////////
		////
		////	generateMeshGlyph
		////
		//////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//MeshGlyph generateMeshGlyph(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		//[[nodiscard]]
		//MeshGlyph generateMeshGlyph(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		//////////////////////////////////////////////////////////////////
		////
		////	generateMeshGlyphByGlyphIndex
		////
		//////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//MeshGlyph generateMeshGlyphByGlyphIndex(GlyphIndex glyphIndex, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		//////////////////////////////////////////////////////////////////
		////
		////	generateMesheGlyphs
		////
		//////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Array<MeshGlyph> generateMesheGlyphs(StringView s, EnableLigatures enableLigatures = EnableLigatures::Yes, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	generateBitmapGlyph
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字のビットマップグリフを返します。
		/// @param codePoint 文字
		/// @param readingDirection テキストの方向
		/// @return ビットマップグリフ
		[[nodiscard]]
		BitmapGlyph generateBitmapGlyph(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		/// @brief 指定した文字のビットマップグリフを返します。
		/// @param ch 文字
		/// @param readingDirection テキストの方向
		/// @return ビットマップグリフ
		[[nodiscard]]
		BitmapGlyph generateBitmapGlyph(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	generateBitmapGlyphByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したグリフのビットマップグリフを返します。
		/// @param glyphIndex グリフインデックス
		/// @param readingDirection テキストの方向
		/// @return ビットマップグリフ
		[[nodiscard]]
		BitmapGlyph generateBitmapGlyphByGlyphIndex(GlyphIndex glyphIndex, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	generateMSDFGlyph
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字の MSDF グリフを返します。
		/// @param codePoint 文字
		/// @param readingDirection テキストの方向
		/// @return MSDF グリフ
		[[nodiscard]]
		MSDFGlyph generateMSDFGlyph(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		/// @brief 指定した文字の MSDF グリフを返します。
		/// @param ch 文字
		/// @param readingDirection テキストの方向
		/// @return MSDF グリフ
		[[nodiscard]]
		MSDFGlyph generateMSDFGlyph(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	generateMSDFGlyphByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したグリフの MSDF グリフを返します。
		/// @param glyphIndex グリフインデックス
		/// @param readingDirection テキストの方向
		/// @return MSDF グリフ
		[[nodiscard]]
		MSDFGlyph generateMSDFGlyphByGlyphIndex(GlyphIndex glyphIndex, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	preload
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字列のグリフをキャッシュします。
		/// @param chars 文字列
		/// @param enableLigatures 合字を有効にするか
		/// @param readingDirection テキストの方向
		/// @return キャッシュに成功した場合 true, それ以外の場合は false
		/// @remark 重複は自動的に除外されます。
		bool preload(StringView chars, EnableLigatures enableLigatures = EnableLigatures::Yes, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

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
		//	getGlyph
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字の描画用のグリフを返します。
		/// @param codePoint 文字
		/// @param readingDirection テキストの方向
		/// @return 描画用グリフ
		[[nodiscard]]
		Glyph getGlyph(char32 codePoint, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		/// @brief 指定した文字の描画用のグリフを返します。
		/// @param ch 文字
		/// @param readingDirection テキストの方向
		/// @return 描画用グリフ
		[[nodiscard]]
		Glyph getGlyph(StringView ch, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlyphByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字の描画用のグリフを返します。
		/// @param glyphIndex 文字のグリフインデックス
		/// @return 描画用グリフ
		[[nodiscard]]
		Glyph getGlyphByGlyphIndex(GlyphIndex glyphIndex, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlyphs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字列の描画用のグリフの配列を返します。
		/// @param s 文字列
		/// @param enableLigatures 合字を有効にするか
		/// @param readingDirection テキストの方向
		/// @return 描画用グリフの配列
		[[nodiscard]]
		Array<Glyph> getGlyphs(StringView s, EnableLigatures enableLigatures = EnableLigatures::Yes, ReadingDirection readingDirection = ReadingDirection::LeftToRight) const;

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

		/// @brief DrawableText を文字列から構築します。
		/// @param text 文字列
		/// @return DrawableText
		[[nodiscard]]
		DrawableText operator ()(const String& text) const;

		/// @brief DrawableText を文字列から構築します。
		/// @param text 文字列
		/// @return DrawableText
		[[nodiscard]]
		DrawableText operator ()(String&& text) const;

		/// @brief DrawableText を一連の値から構築します。
		/// @tparam ...Args 一連の値の型
		/// @param ...args 文字列に変換する値
		/// @return DrawableText
		template <Concept::Formattable... Args>
		[[nodiscard]]
		DrawableText operator ()(const Args&... args) const;

		/// @brief Format できない値を渡した際にエラーになります。
		/// @tparam ...Args 一連の値の型
		/// @param ...args 文字列に変換する値
		/// @return DrawableText
		template <class... Args>
		DrawableText operator ()(const Args&... args) const = delete;

		/// @brief DrawableText を文字列から構築します。
		/// @param readingDirection テキストの方向
		/// @param text 文字列
		/// @return DrawableText
		[[nodiscard]]
		DrawableText operator ()(ReadingDirection readingDirection, const String& text) const;

		/// @brief DrawableText を文字列から構築します。
		/// @param readingDirection テキストの方向
		/// @param text 文字列
		/// @return DrawableText
		[[nodiscard]]
		DrawableText operator ()(ReadingDirection readingDirection, String&& text) const;

		/// @brief DrawableText を一連の値から構築します。
		/// @tparam ...Args 一連の値の型
		/// @param readingDirection テキストの方向
		/// @param ...args 文字列に変換する値
		/// @return DrawableText
		template <Concept::Formattable... Args>
		[[nodiscard]]
		DrawableText operator ()(ReadingDirection readingDirection, const Args&... args) const;

		/// @brief Format できない値を渡した際にエラーになります。
		/// @tparam ...Args 一連の値の型
		/// @param readingDirection テキストの方向
		/// @param ...args 文字列に変換する値
		/// @return DrawableText
		template <class... Args>
		DrawableText operator ()(ReadingDirection readingDirection, const Args&... args) const = delete;

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

		/// @brief 指定したフォントファイルに含まれる書体の一覧を返します。
		/// @param path フォントファイルのパス
		/// @return 書体の一覧
		[[nodiscard]]
		static Array<FontFaceProperties> GetFaces(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	IsAvailable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した標準同梱フォントが利用可能であるかを返します。
		/// @param typeface 標準同梱フォントの種類
		/// @return 利用可能である場合 true, それ以外の場合は false
		[[nodiscard]]
		static bool IsAvailable(Typeface typeface);

		////////////////////////////////////////////////////////////////
		//
		//	GetPixelShader
		//
		////////////////////////////////////////////////////////////////

		/// @brief テキスト描画用の標準ピクセルシェーダを返します。
		/// @param method フォントのレンダリング方式
		/// @param type テキストのスタイル
		/// @param hasColor カラー情報をもつフォントか
		/// @return 指定した設定でのテキスト描画用の標準ピクセルシェーダ
		[[nodiscard]]
		static const PixelShader& GetPixelShader(FontMethod method, TextStyle::Type type = TextStyle::Type::Default);
	};
}
