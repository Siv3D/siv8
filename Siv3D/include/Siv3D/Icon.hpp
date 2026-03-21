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

	struct Icon
	{
		/// @brief アイコンのコードポイント
		char32 code = 0;

		[[nodiscard]]
		Icon() = default;

		[[nodiscard]]
		explicit constexpr Icon(char32 _code) noexcept;

		[[nodiscard]]
		static bool HasGlyph(char32 code);

		[[nodiscard]]
		static GlyphIndex GetGlyphIndex(char32 code);

		[[nodiscard]]
		static Image CreateImage(char32 code, int32 size);

		[[nodiscard]]
		static Image CreateMSDFImage(char32 code, int32 size, int32 buffer = 2);
	};

	inline namespace Literals
	{
		inline namespace IconLiterals
		{
			[[nodiscard]]
			inline constexpr Icon operator ""_icon(unsigned long long codePoint) noexcept;
		}
	}
}

# include "detail/Icon.ipp"
