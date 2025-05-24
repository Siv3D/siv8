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
# include <Siv3D/PredefinedYesNo.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FontStyle.hpp>
# include <Siv3D/ReadingDirection.hpp>
# include <Siv3D/FontMethod.hpp>
# include <Siv3D/GlyphIndex.hpp>
# include <Siv3D/FontFaceProperties.hpp>
# include <Siv3D/MappedMemoryView.hpp>
# include "FontUtility.hpp"
# include "FontCommon.hpp"

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4244)
# include <ThirdParty/skia/include/core/SkFont.h>
# include <ThirdParty/skia/src/ports/SkTypeface_FreeType.h>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	struct HarfBuzzGlyphInfo
	{
		const hb_glyph_info_t* info = nullptr;

		size_t count = 0;
	};

	class FontFace
	{
	public:

		[[nodiscard]]
		FontFace() = default;

		~FontFace();

		[[nodiscard]]
		bool init(::FT_Library library, const MappedMemoryView& memoryView, ::FT_Face face, StringView styleName, FontMethod fontMethod, int32 baseSize, FontStyle style);

		[[nodiscard]]
		const FontFaceInfo& getInfo() const noexcept;

		void setTabSize(int32 tabSize) noexcept;

		[[nodiscard]]
		HarfBuzzGlyphInfo getHarfBuzzGlyphInfo(StringView s, EnableLigatures enableLigatures, ReadingDirection readingDirection) const;

		[[nodiscard]]
		Optional<float> getXAdvanceByGlyphIndex(GlyphIndex glyphIndex, EnableHinting enableHinting);

		[[nodiscard]]
		Optional<float> getYAdvanceByGlyphIndex(GlyphIndex glyphIndex);

		[[nodiscard]]
		float getYAdvance(StringView ch);

		[[nodiscard]]
		GlyphIndex getGlyphIndex(char32 codePoint);

		[[nodiscard]]
		GlyphIndex getGlyphIndex(StringView ch, const ReadingDirection readingDirection);

		[[nodiscard]]
		String getGlyphNameByGlyphIndex(GlyphIndex glyphIndex);

		[[nodiscard]]
		::FT_Face getFace() const noexcept;

		[[nodiscard]]
		SkFont* getSkFont() const noexcept;

	private:

		struct HarfBuzzObjects
		{
			::hb_font_t* hbFont = nullptr;

			::hb_buffer_t* hbBuffer = nullptr;

			~HarfBuzzObjects();

			bool init(::FT_Face face);
		};

		struct COLRv1
		{
			sk_sp<SkTypeface> skTypeface;

			SkFont skFont;
		};

		::FT_Face m_face = nullptr;

		std::unique_ptr<HarfBuzzObjects> m_hbObjects;

		std::unique_ptr<COLRv1> m_colrv1;

		FontFaceInfo m_info;
	};
}
