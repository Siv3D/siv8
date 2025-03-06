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

# include "CEmoji.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4244)
# include <ThirdParty/skia/include/core/SkCanvas.h>
# include <ThirdParty/skia/include/core/SkTextBlob.h>
# include <ThirdParty/skia/include/core/SkFontMetrics.h>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

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

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CEmoji::~CEmoji()
	{
		LOG_SCOPED_DEBUG("CEmoji::~CEmoji()");

		m_typeface.reset();

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

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CEmoji::init()
	{
		LOG_SCOPED_DEBUG("CEmoji::init()");

		const FilePath emojiFilePath = (CacheDirectory::Engine() + U"font/noto-emoji/Noto-COLRv1.ttf");

		if (not FileSystem::Exists(emojiFilePath))
		{
			LOG_INFO("Emoji font file not found");
			return;
		}

		if (const FT_Error error = FT_Init_FreeType(&m_freeType))
		{
			throw InternalEngineError{ "FT_Init_FreeType() failed" };
		}

		const std::string emojiFontPathUTF8 = Unicode::ToUTF8(emojiFilePath);

		if (const FT_Error error = ::FT_New_Face(m_freeType, emojiFontPathUTF8.c_str(), static_cast<FT_Long>(0), &m_face))
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

		std::unique_ptr<SkStreamAsset> fileStream = SkFILEStream::Make(emojiFontPathUTF8.c_str());
		m_typeface = SkTypeface_FreeType::MakeFromStream(std::move(fileStream), SkFontArguments{});
		m_font.setTypeface(m_typeface);

		m_available = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasEmoji
	//
	////////////////////////////////////////////////////////////////

	bool CEmoji::hasEmoji(StringView emoji) const
	{
		if (not m_available)
		{
			return false;
		}

		::hb_buffer_reset(m_hbBuffer);
		::hb_buffer_add_utf32(m_hbBuffer,
			reinterpret_cast<const uint32*>(emoji.data()),
			static_cast<int32>(emoji.size()), 0, static_cast<int32>(emoji.size()));
		::hb_buffer_guess_segment_properties(m_hbBuffer);
		::hb_shape(m_hbFont, m_hbBuffer, nullptr, 0);

		uint32 glyphCount;
		::hb_buffer_get_glyph_infos(m_hbBuffer, &glyphCount);

		return (glyphCount == 1);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphIndex CEmoji::getGlyphIndex(const StringView emoji) const
	{
		if (not m_available)
		{
			return InvalidGlyphIndex;
		}

		::hb_buffer_reset(m_hbBuffer);
		::hb_buffer_add_utf32(m_hbBuffer,
			reinterpret_cast<const uint32*>(emoji.data()),
			static_cast<int32>(emoji.size()), 0, static_cast<int32>(emoji.size()));
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

	////////////////////////////////////////////////////////////////
	//
	//	renderEmoji
	//
	////////////////////////////////////////////////////////////////

	Image CEmoji::renderEmoji(const GlyphIndex emoji, const int32 size)
	{
		if (not m_available)
		{
			return{};
		}

		Image image = RenderEmoji(emoji, size, m_font);
		{
		#if SK_PMCOLOR_BYTE_ORDER(B,G,R,A)

			image.bgraToRGBA();

		# endif
		}

		return image;
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderFilledEmoji
	//
	////////////////////////////////////////////////////////////////

	Image CEmoji::renderFilledEmoji(const GlyphIndex emoji, const int32 size)
	{
		if (not m_available)
		{
			return{};
		}

		Image image = RenderEmoji(emoji, size, m_font);
		{
			for (auto& pixel : image)
			{
				pixel.r = pixel.g = pixel.b = pixel.a;
			}
		}

		return image;
	}
}
