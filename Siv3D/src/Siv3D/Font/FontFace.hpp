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
# include <Siv3D/FontMethod.hpp>
# include <Siv3D/GlyphIndex.hpp>
# include <Siv3D/FontFaceProperties.hpp>
# include "FontCommon.hpp"

namespace s3d
{
	struct HarfBuzzGlyphInfo
	{
		const hb_glyph_info_t* info = nullptr;

		size_t count = 0;
	};

	struct FontFaceInfo
	{
		FontFaceProperties properties;

		int32 baseSize = 16;

		FontMethod renderingMethod = FontMethod::Bitmap;

		FontStyle style = FontStyle::Normal;

		int16 tabSize = 8;

		float ascender = 0.0f;

		float descender = 0.0f;

		float spaceXAdvance = 0.0f;

		float spaceYAdvance = 0.0f;
	};

	class FontFace
	{
	public:

		[[nodiscard]]
		FontFace() = default;

		~FontFace();

		[[nodiscard]]
		bool init(::FT_Library library, ::FT_Face face, StringView styleName, FontMethod fontMethod, int32 baseSize, FontStyle style);

		[[nodiscard]]
		const FontFaceInfo& getInfo() const noexcept;

		void setTabSize(int32 tabSize) noexcept;

		[[nodiscard]]
		HarfBuzzGlyphInfo getHarfBuzzGlyphInfo(StringView s, Ligature ligature) const;

		[[nodiscard]]
		Optional<float> getXAdvance(char32 codePoint);

		[[nodiscard]]
		Optional<float> getYAdvance(char32 codePoint);

		[[nodiscard]]
		GlyphIndex getGlyphIndex(char32 codePoint);

		[[nodiscard]]
		GlyphIndex getGlyphIndex(StringView ch);

		[[nodiscard]]
		String getGlyphNameByGlyphIndex(GlyphIndex glyphIndex);

	private:

		struct HarfBuzzObjects
		{
			::hb_font_t* hbFont = nullptr;

			::hb_buffer_t* hbBuffer = nullptr;

			~HarfBuzzObjects();

			bool init(::FT_Face face);
		};

		::FT_Face m_face = nullptr;

		std::unique_ptr<HarfBuzzObjects> m_hbObjects;

		FontFaceInfo m_info;
	};
}
