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
# include "String.hpp"
# include "PointVector.hpp"
# include "GlyphIndex.hpp"

namespace s3d
{
	class Image;

	////////////////////////////////////////////////////////////////
	//
	//	Emoji
	//
	////////////////////////////////////////////////////////////////

	/// @brief 絵文字を表すオブジェクト
	///
	/// Unicode の絵文字シーケンスをコードポイント文字列として保持します。
	/// また、絵文字グリフの有無の確認、グリフインデックスの取得、
	/// 絵文字画像の生成を行うための静的関数を提供します。
	struct Emoji
	{
		/// @brief 絵文字画像生成時のデフォルトの大きさ（ピクセル）
		static constexpr int32 DefaultSize = 128;

		/// @brief デフォルトの絵文字画像サイズ（ピクセル）
		static constexpr Size ImageSize{ DefaultSize, DefaultSize };

		/// @brief 絵文字を表す Unicode コードポイント列
		String codePoints;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の Emoji オブジェクトを構築します。
		[[nodiscard]]
		Emoji() = default;

		/// @brief 文字列リテラルから Emoji オブジェクトを構築します。
		/// @param emoji 絵文字を表す UTF-32 文字列
		[[nodiscard]]
		explicit Emoji(const char32* emoji);

		/// @brief 文字列ビューから Emoji オブジェクトを構築します。
		/// @param emoji 絵文字を表す文字列ビュー
		[[nodiscard]]
		explicit Emoji(StringView emoji);

		/// @brief 文字列から Emoji オブジェクトを構築します。
		/// @param emoji 絵文字を表す文字列
		[[nodiscard]]
		explicit Emoji(String emoji);

		////////////////////////////////////////////////////////////////
		//
		//	HasGlyph
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した絵文字に対応するグリフが利用可能かを返します。
		/// @param emoji 絵文字を表す文字列
		/// @return 利用可能なグリフが存在する場合は true、存在しない場合は false
		[[nodiscard]]
		static bool HasGlyph(StringView emoji);

		////////////////////////////////////////////////////////////////
		//
		//	GetGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した絵文字に対応するグリフインデックスを返します。
		/// @param emoji 絵文字を表す文字列
		/// @return グリフインデックス。対応するグリフが存在しない場合は GlyphIndexNotdef
		[[nodiscard]]
		static GlyphIndex GetGlyphIndex(StringView emoji);

		////////////////////////////////////////////////////////////////
		//
		//	CreateImage
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した絵文字からカラー画像を生成します。
		/// @param emoji 絵文字を表す文字列
		/// @param size 生成する画像の一辺の大きさ（ピクセル）
		/// @return 生成された絵文字画像。生成に失敗した場合は空の Image
		[[nodiscard]]
		static Image CreateImage(StringView emoji, int32 size = DefaultSize);

		////////////////////////////////////////////////////////////////
		//
		//	CreateImageByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief グリフインデックスからカラー画像を生成します。
		/// @param glyphIndex 絵文字のグリフインデックス
		/// @param size 生成する画像の一辺の大きさ（ピクセル）
		/// @return 生成された絵文字画像。生成に失敗した場合は空の Image
		[[nodiscard]]
		static Image CreateImageByGlyphIndex(GlyphIndex glyphIndex, int32 size = DefaultSize);

		////////////////////////////////////////////////////////////////
		//
		//	CreateSilhouetteImage
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した絵文字からシルエット画像を生成します。
		/// @param emoji 絵文字を表す文字列
		/// @param size 生成する画像の一辺の大きさ（ピクセル）
		/// @return 生成された絵文字のシルエット画像。生成に失敗した場合は空の Image
		[[nodiscard]]
		static Image CreateSilhouetteImage(StringView emoji, int32 size = DefaultSize);

		////////////////////////////////////////////////////////////////
		//
		//	CreateSilhouetteImageByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief グリフインデックスからシルエット画像を生成します。
		/// @param glyphIndex 絵文字のグリフインデックス
		/// @param size 生成する画像の一辺の大きさ（ピクセル）
		/// @return 生成された絵文字のシルエット画像。生成に失敗した場合は空の Image
		[[nodiscard]]
		static Image CreateSilhouetteImageByGlyphIndex(GlyphIndex glyphIndex, int32 size = DefaultSize);
	};

	inline namespace Literals
	{
		inline namespace EmojiLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_emoji
			//
			////////////////////////////////////////////////////////////////

			/// @brief 絵文字を表す文字列リテラルから Emoji オブジェクトを作成します。
			/// @param s 絵文字を表す UTF-32 文字列
			/// @param length 文字列の長さ
			/// @return 作成された Emoji オブジェクト
			[[nodiscard]]
			Emoji operator ""_emoji(const char32 * s, size_t length);
		}
	}
}
