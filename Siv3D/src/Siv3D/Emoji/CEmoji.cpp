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
# include <ThirdParty/skia/include/core/SkCanvas.h>
# include <ThirdParty/skia/include/core/SkTextBlob.h>
# include <ThirdParty/skia/include/core/SkFontMetrics.h>

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

		m_hbFont = ::hb_ft_font_create(m_face, nullptr);

		if (not m_hbFont)
		{
			LOG_FAIL("hb_ft_font_create(): failed");
			return;
		}

		m_hbBuffer = ::hb_buffer_create();

		m_fileStream = SkFILEStream::Make("Noto-COLRv1.ttf");
		m_typeface = SkTypeface_FreeType::MakeFromStream(std::move(m_fileStream), SkFontArguments{});
		m_font.setTypeface(m_typeface);

		renderEmoji(U"🍣", 64).save(U"emoji1.png");
		renderEmoji(U"🐈‍⬛", 128).save(U"emoji2.png");
		renderEmoji(U"🎉", 2048).save(U"emoji3.png");
		renderEmoji(U"🥭", 512).save(U"emoji4.png");
		renderEmoji(U"🍔", 512).save(U"emoji5.png");
	}

	bool CEmoji::hasEmoji(StringView emoji) const
	{
		return(false);
	}

	Image CEmoji::renderEmoji(const StringView emoji, const int32 size)
	{
		::hb_buffer_reset(m_hbBuffer);
		::hb_buffer_add_utf32(m_hbBuffer,
			reinterpret_cast<const uint32*>(emoji.data()),
			static_cast<int32>(emoji.length()), 0, static_cast<int32>(emoji.length()));
		::hb_buffer_guess_segment_properties(m_hbBuffer);
		::hb_shape(m_hbFont, m_hbBuffer, nullptr, 0);

		uint32 glyphCount;
		hb_glyph_info_t* glyphInfo = ::hb_buffer_get_glyph_infos(m_hbBuffer, &glyphCount);

		if (glyphCount != 1)
		{
			return{};
		}

		const SkGlyphID glyphIndex = static_cast<SkGlyphID>(glyphInfo[0].codepoint);

		Image image{ size_t(size), Color{ 0, 0, 0, 0 } };
		{
			const float scalingFactor = (436.90667f / 512);
			m_font.setSize(scalingFactor * size);

			auto canvas = SkCanvas::MakeRasterDirectN32(size, size, (uint32*)image.data(), static_cast<int32>(image.stride()));

			SkFontMetrics metrics;
			m_font.getMetrics(&metrics);
			const SkScalar textWidth = m_font.measureText(&glyphIndex, sizeof(glyphIndex), SkTextEncoding::kGlyphID);
			//const SkScalar textHeight = (metrics.fDescent - metrics.fAscent);

			const auto blob = SkTextBlob::MakeFromText(&glyphIndex, sizeof(glyphIndex), m_font, SkTextEncoding::kGlyphID);
			const SkScalar offsetX = ((size - textWidth) / 2);
			const SkScalar offsetY = (size - metrics.fDescent);

			canvas->drawTextBlob(blob.get(), offsetX, offsetY, SkPaint{});

			if (SkPixmap map;
				canvas->peekPixels(&map))
			{
			#if SK_PMCOLOR_BYTE_ORDER(B,G,R,A)
				
				image.swapBGRAtoRGBA();
				
			# endif
			}
		}

		return image;
	}
}
