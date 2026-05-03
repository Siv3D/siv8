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

# include "CEmoji.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4244)
# include <ThirdParty/skia/include/core/SkCanvas.h>
# include <ThirdParty/skia/include/core/SkTextBlob.h>
# include <ThirdParty/skia/include/core/SkFontMetrics.h>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static GlyphIndex GetSingleGlyphIndex(hb_font_t* hbFont, hb_buffer_t* hbBuffer, const StringView emoji)
		{
			::hb_buffer_reset(hbBuffer);
			::hb_buffer_add_utf32(hbBuffer,
				reinterpret_cast<const uint32*>(emoji.data()),
				static_cast<int32>(emoji.size()), 0, static_cast<int32>(emoji.size()));
			::hb_buffer_guess_segment_properties(hbBuffer);
			::hb_shape(hbFont, hbBuffer, nullptr, 0);

			uint32 glyphCount = 0;
			const hb_glyph_info_t* glyphInfo = ::hb_buffer_get_glyph_infos(hbBuffer, &glyphCount);

			if ((glyphCount != 1) || (glyphInfo == nullptr))
			{
				return GlyphIndexNotdef;
			}

			return static_cast<GlyphIndex>(glyphInfo[0].codepoint);
		}

		[[nodiscard]]
		static Image RenderEmoji(const GlyphIndex glyphIndex, const int32 size, SkFont& font)
		{
			if (glyphIndex == GlyphIndexNotdef)
			{
				return{};
			}

			const SkGlyphID glyphID = static_cast<SkGlyphID>(glyphIndex);

			Image image{ size, Color{ 0, 0, 0, 0 } };
			{
				constexpr float ScalingFactor = (436.90667f / 512);
				font.setSize(ScalingFactor * size);

				auto canvas = SkCanvas::MakeRasterDirectN32(size, size, (uint32*)image.data(), static_cast<int32>(image.bytesPerRow()));

				SkFontMetrics metrics;
				font.getMetrics(&metrics);
				const SkScalar textWidth = font.measureText(&glyphID, sizeof(glyphID), SkTextEncoding::kGlyphID);
				const auto blob = SkTextBlob::MakeFromText(&glyphID, sizeof(glyphID), font, SkTextEncoding::kGlyphID);
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

		if (not SIV3D_ENGINE(Font)->newFace(emojiFilePath, 0, m_face))
		{
			LOG_FAIL(U"Failed to load the emoji font file: {}"_fmt(emojiFilePath));
			return;
		}

		m_hbFont = ::hb_ft_font_create_referenced(m_face);

		if (not m_hbFont)
		{
			LOG_FAIL("hb_ft_font_create_referenced (): failed");
			return;
		}

		m_hbBuffer = ::hb_buffer_create();

		std::unique_ptr<SkStreamAsset> fileStream = SkFILEStream::Make(Unicode::ToUTF8(emojiFilePath).c_str());
		m_typeface = SkTypeface_FreeType::MakeFromStream(std::move(fileStream), SkFontArguments{});
		m_font.setTypeface(m_typeface);

		m_available = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasGlyph
	//
	////////////////////////////////////////////////////////////////

	bool CEmoji::hasGlyph(StringView emoji) const
	{
		if (not m_available)
		{
			return false;
		}

		return (GetSingleGlyphIndex(m_hbFont, m_hbBuffer, emoji) != GlyphIndexNotdef);
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
			return GlyphIndexNotdef;
		}

		return GetSingleGlyphIndex(m_hbFont, m_hbBuffer, emoji);
	}

	////////////////////////////////////////////////////////////////
	//
	//	render
	//
	////////////////////////////////////////////////////////////////

	Image CEmoji::render(const GlyphIndex glyphIndex, const int32 size)
	{
		if (not m_available)
		{
			return{};
		}

		Image image = RenderEmoji(glyphIndex, size, m_font);
		{
		#if SK_PMCOLOR_BYTE_ORDER(B,G,R,A)

			image.bgraToRGBA();

		# endif
		}

		return image;
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderSilhouette
	//
	////////////////////////////////////////////////////////////////

	Image CEmoji::renderSilhouette(const GlyphIndex glyphIndex, const int32 size)
	{
		if (not m_available)
		{
			return{};
		}

		Image image = RenderEmoji(glyphIndex, size, m_font);
		{
			for (auto& pixel : image)
			{
				pixel.r = pixel.g = pixel.b = pixel.a;
			}
		}

		return image;
	}
}
