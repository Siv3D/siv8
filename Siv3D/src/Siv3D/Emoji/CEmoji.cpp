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
	namespace
	{
		static Image RenderEmoji(const GlyphIndex emoji, const int32 size, SkFont& font)
		{
			if (emoji == InvalidGlyphIndex)
			{
				return{};
			}

			const SkGlyphID glyphIndex = static_cast<SkGlyphID>(emoji);

			Image image{ size, Color{ 0, 0, 0, 0 } };
			{
				constexpr float ScalingFactor = (436.90667f / 512);
				font.setSize(ScalingFactor * size);

				auto canvas = SkCanvas::MakeRasterDirectN32(size, size, (uint32*)image.data(), static_cast<int32>(image.stride()));

				SkFontMetrics metrics;
				font.getMetrics(&metrics);
				const SkScalar textWidth = font.measureText(&glyphIndex, sizeof(glyphIndex), SkTextEncoding::kGlyphID);
				const auto blob = SkTextBlob::MakeFromText(&glyphIndex, sizeof(glyphIndex), font, SkTextEncoding::kGlyphID);
				const SkScalar offsetX = ((size - textWidth) / 2);
				const SkScalar offsetY = (size - metrics.fDescent);

				canvas->drawTextBlob(blob.get(), offsetX, offsetY, SkPaint{});

				SkPixmap map;
				canvas->peekPixels(&map);
			}

			return image;
		}
	}

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
	}

	bool CEmoji::hasEmoji(StringView emoji) const
	{
		::hb_buffer_reset(m_hbBuffer);
		::hb_buffer_add_utf32(m_hbBuffer,
			reinterpret_cast<const uint32*>(emoji.data()),
			static_cast<int32>(emoji.length()), 0, static_cast<int32>(emoji.length()));
		::hb_buffer_guess_segment_properties(m_hbBuffer);
		::hb_shape(m_hbFont, m_hbBuffer, nullptr, 0);

		uint32 glyphCount;
		::hb_buffer_get_glyph_infos(m_hbBuffer, &glyphCount);

		return (glyphCount == 1);
	}

	GlyphIndex CEmoji::getGlyphIndex(const StringView emoji) const
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
			return InvalidGlyphIndex;
		}

		return static_cast<GlyphIndex>(glyphInfo[0].codepoint);
	}

	Image CEmoji::renderEmoji(const GlyphIndex emoji, const int32 size)
	{
		Image image = RenderEmoji(emoji, size, m_font);
				
	#if SK_PMCOLOR_BYTE_ORDER(B,G,R,A)

		image.bgraToRGBA();

	# endif

		return image;
	}

	Image CEmoji::renderFilledEmoji(const GlyphIndex emoji, const int32 size)
	{
		Image image = RenderEmoji(emoji, size, m_font);

		for (auto& pixel : image)
		{
			pixel.r = pixel.g = pixel.b = pixel.a;
		}

		return image;
	}
}
