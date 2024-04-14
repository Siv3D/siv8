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

# include "CEmoji.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>

namespace s3d
{
	CEmoji::~CEmoji()
	{
		LOG_SCOPED_DEBUG("CEmoji::~CEmoji()");

		if (m_hbBuffer)
		{
			::hb_buffer_destroy(m_hbBuffer);
			m_hbBuffer = nullptr;
		}

		if (m_hbFont)
		{
			::hb_font_destroy(m_hbFont);
			m_hbFont = nullptr;
		}

		if (m_face)
		{
			::FT_Done_Face(m_face);
			m_face = nullptr;
		}

		if (m_freeType)
		{
			::FT_Done_FreeType(m_freeType);
		}
	}

	void CEmoji::init()
	{
		LOG_SCOPED_DEBUG("CEmoji::init()");

		if (const FT_Error error = FT_Init_FreeType(&m_freeType))
		{
			throw InternalEngineError{ U"FT_Init_FreeType() failed" };
		}

		if (const FT_Error error = ::FT_New_Face(m_freeType, "Noto-COLRv1.ttf", static_cast<FT_Long>(0), &m_face))
		{
			if (error == FT_Err_Unknown_File_Format)
			{
				LOG_FAIL("FT_New_Face(): FT_Err_Unknown_File_Format");
			}
			else
			{
				LOG_FAIL("FT_New_Face(): failed");
			}

			return;
		}

		if (m_face->face_flags & FT_FACE_FLAG_COLOR)
		{
			LOG_TRACE(U"FT_FACE_FLAG_COLOR supported");
		}

		LOG_DEBUG(fmt::format("m_face->face_flags: 0x{:X}", m_face->face_flags));

		const String familyName = Unicode::FromUTF8(m_face->family_name);
		const String styleName = Unicode::FromUTF8(m_face->style_name);
		const uint32 numGlyphs = static_cast<uint32>(m_face->num_glyphs);

		LOG_DEBUG(fmt::format("family_name: {}", familyName));
		LOG_DEBUG(fmt::format("style_name: {}", styleName));
		LOG_DEBUG(fmt::format("num_glyphs: {}", numGlyphs));

		m_hbFont = ::hb_ft_font_create(m_face, nullptr);

		if (not m_hbFont)
		{
			LOG_FAIL("hb_ft_font_create(): failed");
			return;
		}

		m_hbBuffer = ::hb_buffer_create();

		renderEmoji(U"🍣", { 64, 64 });
		renderEmoji(U"🐈‍⬛", { 128, 128 });
	}

	bool CEmoji::hasEmoji(StringView emoji) const
	{
		return(false);
	}

	Image CEmoji::renderEmoji(const StringView emoji, const Size& size)
	{
		::hb_buffer_reset(m_hbBuffer);
		::hb_buffer_add_utf32(m_hbBuffer,
			reinterpret_cast<const uint32*>(emoji.data()),
			static_cast<int32>(emoji.length()), 0, static_cast<int32>(emoji.length()));
		::hb_buffer_guess_segment_properties(m_hbBuffer);
		::hb_shape(m_hbFont, m_hbBuffer, nullptr, 0);

		uint32 glyphCount;
		hb_glyph_info_t* glyphInfo = ::hb_buffer_get_glyph_infos(m_hbBuffer, &glyphCount);
		//hb_glyph_position_t* glyphPos = ::hb_buffer_get_glyph_positions(m_hbBuffer, nullptr);

		if (glyphCount != 1)
		{
			return{};
		}

		const uint32 glyphIndex = glyphInfo[0].codepoint;
		LOG_DEBUG(fmt::format("renderEmoji: glyphIndex = {}", glyphIndex));

		uint32_t flags = 0;
		flags |= FT_LOAD_BITMAP_METRICS_ONLY;  // Don't decode any bitmaps.
		flags |= FT_LOAD_NO_BITMAP; // Ignore embedded bitmaps.
		flags &= ~FT_LOAD_RENDER;  // Don't scan convert.
		flags &= ~FT_LOAD_COLOR;  // Ignore SVG.
		flags |= FT_LOAD_NO_HINTING;
		flags |= FT_LOAD_NO_AUTOHINT;
		flags |= FT_LOAD_IGNORE_TRANSFORM;

		if (::FT_Load_Glyph(m_face, glyphIndex, flags))
		{
			return{};
		}

		FT_OpaquePaint opaquePaint{ nullptr, 1 };
		if (!FT_Get_Color_Glyph_Paint(m_face, glyphIndex, FT_COLOR_INCLUDE_ROOT_TRANSFORM, &opaquePaint)) {
			return{};
		}

		LOG_DEBUG("ok");


		//FT_Palette_Data paletteData;
		//FT_Palette_Data_Get(m_face, &paletteData);
		//LOG_DEBUG(fmt::format("renderEmoji: paletteData.num_palettes = {}", paletteData.num_palettes));

		FT_Color* palette = nullptr;
		const unsigned short paletteIndex = 0;
		if (FT_Palette_Select(m_face, paletteIndex, &palette) != 0) {
			palette = nullptr;
		}

		//LOG_DEBUG(fmt::format("renderEmoji: palette->red = {}", palette->red));
		//LOG_DEBUG(fmt::format("renderEmoji: palette->green = {}", palette->green));
		//LOG_DEBUG(fmt::format("renderEmoji: palette->blue = {}", palette->blue));
		//LOG_DEBUG(fmt::format("renderEmoji: palette->alpha = {}", palette->alpha));


		FT_LayerIterator  iterator{};
		FT_UInt layer_glyph_index = 0;
		FT_UInt layer_color_index = 0;

		FT_Bool have_layers = ::FT_Get_Color_Glyph_Layer(m_face,
			glyphIndex,
			&layer_glyph_index,
			&layer_color_index,
			&iterator);

		LOG_DEBUG(fmt::format("renderEmoji: have_layers = {}", have_layers));

		if (have_layers)
		{
			do
			{
				FT_Color  layer_color;

				LOG_DEBUG(fmt::format("renderEmoji: layer_glyph_index = {}", layer_glyph_index));

				//if (layer_color_index == 0xFFFF)
				//	layer_color = text_foreground_color;
				//else
				//	layer_color = palette[layer_color_index];

				// Load and render glyph `layer_glyph_index', then
				// blend resulting pixmap (using color `layer_color')
				// with previously created pixmaps.

			} while (FT_Get_Color_Glyph_Layer(m_face,
				glyphIndex,
				&layer_glyph_index,
				&layer_color_index,
				&iterator));
		}








		//if (::FT_Load_Glyph(m_face, glyphIndex, FT_LOAD_COLOR))
		//{
		//	return{};
		//}

		//const bool isOutline = (m_face->glyph->format == FT_GLYPH_FORMAT_OUTLINE);

		//LOG_DEBUG(fmt::format("renderEmoji: isOutline = {}", isOutline));

		//const size_t bitmapWidth = m_face->glyph->bitmap.width;
		//const size_t bitmapHeight = m_face->glyph->bitmap.rows;
		//const int32 bitmapStride = m_face->glyph->bitmap.pitch;

		//LOG_DEBUG(fmt::format("renderEmoji: bitmapWidth = {}", bitmapWidth));
		//LOG_DEBUG(fmt::format("renderEmoji: bitmapHeight = {}", bitmapHeight));
		//LOG_DEBUG(fmt::format("renderEmoji: bitmapStride = {}", bitmapStride));





		return{};
	}
}
