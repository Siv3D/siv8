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

# include "BitmapGlyphRenderer.hpp"

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4244)
# include <ThirdParty/skia/include/core/SkBitmap.h>
# include <ThirdParty/skia/include/core/SkCanvas.h>
# include <ThirdParty/skia/include/core/SkTextBlob.h>
# include <ThirdParty/skia/include/core/SkFontMetrics.h>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	namespace
	{
		static Image RenderEmoji(const GlyphIndex emoji, const int32 size, SkFont* skFont)
		{
			if (not skFont)
			{
				return{};
			}

			if (emoji == GlyphIndexNotdef)
			{
				return{};
			}

			const SkGlyphID glyphIndex = static_cast<SkGlyphID>(emoji);

			Image image{ size, Color{ 0, 0, 0, 0 } };
			{
				constexpr float ScalingFactor = (436.90667f / 512);
				skFont->setSize(ScalingFactor * size);

				auto canvas = SkCanvas::MakeRasterDirectN32(size, size, (uint32*)image.data(), static_cast<int32>(image.bytesPerRow()));

				SkFontMetrics metrics;
				skFont->getMetrics(&metrics);
				const SkScalar textWidth = skFont->measureText(&glyphIndex, sizeof(glyphIndex), SkTextEncoding::kGlyphID);
				const auto blob = SkTextBlob::MakeFromText(&glyphIndex, sizeof(glyphIndex), *skFont, SkTextEncoding::kGlyphID);
				const SkScalar offsetX = ((size - textWidth) / 2);
				const SkScalar offsetY = (size - metrics.fDescent);

				canvas->drawTextBlob(blob.get(), offsetX, offsetY, SkPaint{});

				SkPixmap map;
				canvas->peekPixels(&map);
			}

			return image;
		}

		static BitmapGlyph RenderBitmapGlyphCOLRv1(const GlyphIndex glyphIndex, const int32 emojiSize, const FontFaceInfo& info, const ReadingDirection readingDirection, SkFont* skFont)
		{
			Image image = RenderEmoji(glyphIndex, emojiSize, skFont);
			{
			#if SK_PMCOLOR_BYTE_ORDER(B,G,R,A)

				image.bgraToRGBA();

			# endif
			}

			BitmapGlyph glyph;
			glyph.glyphIndex	= glyphIndex;
			glyph.width			= static_cast<float>(image.width());
			glyph.height		= static_cast<float>(image.height());
			glyph.ascender		= info.ascender;
			glyph.descender		= info.descender;
		
			if (readingDirection == ReadingDirection::TopToBottom)
			{
				//glyph.left		= (face->glyph->metrics.vertBearingX / 64.0f);
				//glyph.top		= (face->glyph->metrics.vertBearingY / 64.0f);
				//glyph.advance	= (face->glyph->metrics.vertAdvance / 64.0f);
			}
			else
			{
				//glyph.left		= (face->glyph->metrics.horiBearingX / 64.0f);
				//glyph.top		= (face->glyph->metrics.horiBearingY / 64.0f);
				//glyph.advance	= (face->glyph->metrics.horiAdvance / 64.0f);
			}
			glyph.left		= 0.0f;
			glyph.top		= info.ascender;
			glyph.advance	= emojiSize;
		
			glyph.image			= std::move(image);
			return glyph;
		}
	}

	BitmapGlyph RenderBitmapGlyph(const ::FT_Face face, const GlyphIndex glyphIndex, const FontFaceInfo& info, const ReadingDirection readingDirection, SkFont* skFont)
	{
		if (info.properties.hasColor)
		{
			if (info.properties.isCOLRv1)
			{
				return RenderBitmapGlyphCOLRv1(glyphIndex, info.baseSize, info, readingDirection, skFont);
			}

			if (::FT_Load_Glyph(face, glyphIndex, FT_LOAD_COLOR))
			{
				return{};
			}
		}
		else
		{
			uint32 loadFlags = FT_LOAD_NO_HINTING;

			if (not(info.style & FontStyle::Bitmap))
			{
				loadFlags |= FT_LOAD_NO_BITMAP;
			}

			if (readingDirection == ReadingDirection::TopToBottom)
			{
				loadFlags |= FT_LOAD_VERTICAL_LAYOUT;
			}

			if (::FT_Load_Glyph(face, glyphIndex, loadFlags))
			{
				return{};
			}

			if (info.style & FontStyle::Bold)
			{
				::FT_GlyphSlot_Embolden(face->glyph);
			}

			if (info.style & FontStyle::Italic)
			{
				::FT_GlyphSlot_Oblique(face->glyph);
			}
		}

		{
			if (face->glyph->format == FT_GLYPH_FORMAT_BITMAP)
			{
				// do nothing
			}
			else
			{
				const FT_Render_Mode renderMode = ((info.style & FontStyle::Bitmap) ? FT_RENDER_MODE_MONO : FT_RENDER_MODE_NORMAL);

				if (::FT_Render_Glyph(face->glyph, renderMode))
				{
					return{};
				}
			}
		}

		const uint32 bitmapWidth		= face->glyph->bitmap.width;
		const uint32 bitmapHeight		= face->glyph->bitmap.rows;
		const uint32 bitmapBytesPerRow	= face->glyph->bitmap.pitch;

		Image image{ Size{ bitmapWidth, bitmapHeight }, Color{ 255 } };
		const uint8* bitmapBuffer = face->glyph->bitmap.buffer;
		Color* pDst = image.data();

		if (face->glyph->bitmap.pixel_mode == FT_Pixel_Mode::FT_PIXEL_MODE_MONO)
		{
			const uint8* pSrcLine = bitmapBuffer;

			for (uint32 y = 0; y < bitmapHeight; ++y)
			{
				for (uint32 x = 0; x < bitmapWidth; ++x)
				{
					const uint32 offsetI = (x / 8);
					const uint32 offsetB = (7 - x % 8);
					const uint8 a = (((pSrcLine[offsetI] >> offsetB) & 0x1) * 255);
					pDst->r = pDst->g = pDst->b = pDst->a = a;
					
					++pDst;
				}

				pSrcLine += bitmapBytesPerRow;
			}
		}
		else if (face->glyph->bitmap.pixel_mode == FT_Pixel_Mode::FT_PIXEL_MODE_GRAY)
		{
			const uint8* pSrc = bitmapBuffer;
			const uint8* const pSrcEnd = (pSrc + (bitmapHeight * bitmapWidth));

			while (pSrc != pSrcEnd)
			{
				const uint8 a = *pSrc;
				pDst->r = pDst->g = pDst->b = pDst->a = a;

				++pSrc;
				++pDst;
			}
		}
		else if (face->glyph->bitmap.pixel_mode == FT_Pixel_Mode::FT_PIXEL_MODE_BGRA)
		{
			const uint8* pSrc = bitmapBuffer;
			const uint8* const pSrcEnd = (pSrc + ((bitmapHeight * bitmapWidth) * 4));

			while (pSrc != pSrcEnd)
			{
				const uint8 b = pSrc[0];
				const uint8 g = pSrc[1];
				const uint8 r = pSrc[2];
				const uint8 a = pSrc[3];
				pDst->set(r, g, b, a);

				pSrc += 4;
				++pDst;
			}
		}

		BitmapGlyph glyph;
		glyph.glyphIndex	= glyphIndex;
		glyph.width			= static_cast<float>(bitmapWidth);
		glyph.height		= static_cast<float>(bitmapHeight);
		glyph.ascender		= info.ascender;
		glyph.descender		= info.descender;
		
		if (readingDirection == ReadingDirection::TopToBottom)
		{
			glyph.left		= (face->glyph->metrics.vertBearingX / 64.0f);
			glyph.top		= (face->glyph->metrics.vertBearingY / 64.0f);
			glyph.advance	= (face->glyph->metrics.vertAdvance / 64.0f);
		}
		else
		{
			glyph.left		= (face->glyph->metrics.horiBearingX / 64.0f);
			glyph.top		= (face->glyph->metrics.horiBearingY / 64.0f);
			glyph.advance	= (face->glyph->metrics.horiAdvance / 64.0f);
		}
		
		glyph.image			= std::move(image);
		return glyph;
	}
}
