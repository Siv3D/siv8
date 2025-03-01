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

	/// @brief 絵文字
	struct Emoji
	{
		/// @brief 絵文字のデフォルトの大きさ（ピクセル）
		static constexpr int32 DefaultSize = 128;

		/// @brief 絵文字のデフォルト幅と高さ（ピクセル）
		static constexpr Size ImageSize{ DefaultSize, DefaultSize };

		/// @brief 絵文字のコードポイント
		String codePoints;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Emoji() = default;

		[[nodiscard]]
		explicit Emoji(const char32* emoji);

		[[nodiscard]]
		explicit Emoji(StringView emoji);

		[[nodiscard]]
		explicit Emoji(String emoji);

		////////////////////////////////////////////////////////////////
		//
		//	HassGlyph
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static bool HasGlyph(StringView emoji);

		////////////////////////////////////////////////////////////////
		//
		//	GetGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static GlyphIndex GetGlyphIndex(StringView emoji);

		////////////////////////////////////////////////////////////////
		//
		//	CreateImage
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Image CreateImage(StringView emoji, int32 size = DefaultSize);

		////////////////////////////////////////////////////////////////
		//
		//	CreateImageByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Image CreateImageByGlyphIndex(GlyphIndex glyphIndex, int32 size = DefaultSize);

		////////////////////////////////////////////////////////////////
		//
		//	CreateFilledImage
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Image CreateFilledImage(StringView emoji, int32 size = DefaultSize);

		////////////////////////////////////////////////////////////////
		//
		//	CreateFilledImageByGlyphIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Image CreateFilledImageByGlyphIndex(GlyphIndex glyphIndex, int32 size = DefaultSize);
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

			/// @brief 絵文字（コードポイント）を記述した文字列から Emoji オブジェクトを作成します。
			/// @param s 絵文字（コードポイント）を記述した文字列
			/// @param length 文字列の長さ
			/// @return Emoji オブジェクト
			[[nodiscard]]
			Emoji operator ""_emoji(const char32* s, size_t length);
		}
	}
}
