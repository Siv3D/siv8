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

# include "CFont.hpp"
# include <Siv3D/TextStyle.hpp>
# include <Siv3D/TextEffect/NullTextEffect.hpp>
# include <Siv3D/MemoryMappedFileView.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include "FontUtility.hpp"
# include "TypefaceUtility.hpp"
# include "GlyphRenderer/OutlineGlyphRenderer.hpp"
# include "GlyphRenderer/BitmapGlyphRenderer.hpp"
# include "GlyphCache/IGlyphCache.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Array<FontFaceProperties> GetFontFaces(const ::FT_Library library, const ::FT_Face face)
		{
			Array<FontFaceProperties> faces;

			::FT_MM_Var* mmVar = nullptr;

			if (::FT_Get_MM_Var(face, &mmVar) == 0)
			{
				for (::FT_UInt styleIndex = 0; styleIndex < mmVar->num_namedstyles; ++styleIndex)
				{
					::FT_Set_Named_Instance(face, (1 + styleIndex));
					
					faces << GetFontFaceProperties(face, mmVar->namedstyle[styleIndex].coords);
				}

				::FT_Done_MM_Var(library, mmVar);
			}
			else
			{
				faces << GetFontFaceProperties(face, nullptr);
			}

			return faces;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CFont::CFont() = default;

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CFont::~CFont()
	{
		LOG_SCOPED_DEBUG("CFont::~CFont()");

		m_fallbackFonts.clear();

		m_fonts.destroy();

		m_freeType = nullptr;
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CFont::init()
	{
		LOG_SCOPED_DEBUG("CFont::init()");

		if (const FT_Error error = ::FT_Init_FreeType(&m_freeType))
		{
			throw InternalEngineError{ U"FT_Init_FreeType() failed" };
		}

		// null Font を管理に登録
		{
			// null Font を作成
			auto nullFont = std::make_unique<FontData>(FontData::Null{});

			if (not nullFont->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null font" };
			}

			// 管理に登録
			m_fonts.setNullData(std::move(nullFont));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFontFileInfo
	//
	////////////////////////////////////////////////////////////////

	Array<FontFaceProperties> CFont::getFontFaces(const FilePathView path) const
	{
		MemoryMappedFileView file{ path };

		const MappedMemoryView view = file.mapAll();

		if (not view)
		{
			return{};
		}

		Array<FontFaceProperties> faces;
		{
			::FT_Face face0 = nullptr;

			if (const ::FT_Error error = ::FT_New_Memory_Face(m_freeType, static_cast<const ::FT_Byte*>(view.data), static_cast<::FT_Long>(view.size), 0, &face0))
			{
				return{};
			}

			const ::FT_Long numFaces = face0->num_faces;

			faces.append(GetFontFaces(m_freeType, face0));

			for (::FT_Long index = 1; index < numFaces; ++index)
			{
				::FT_Face face = nullptr;

				if (const FT_Error error = ::FT_New_Memory_Face(m_freeType, static_cast<const ::FT_Byte*>(view.data), static_cast<::FT_Long>(view.size), index, &face))
				{
					faces.emplace_back();
					continue;
				}

				faces.append(GetFontFaces(m_freeType, face));

				::FT_Done_Face(face);
			}

			::FT_Done_Face(face0);
		}

		return faces;
	}

	////////////////////////////////////////////////////////////////
	//
	//	create
	//
	////////////////////////////////////////////////////////////////

	Font::IDType CFont::create(const Typeface typeface, const FontMethod fontMethod, const int32 baseSize, const FontStyle style)
	{
		const TypefaceInfo typefaceInfo = GetTypefaceInfo(typeface, fontMethod);

		if (not typefaceInfo.path)
		{
			return Font::IDType::Null();
		}

		return create(typefaceInfo.path, typefaceInfo.faceIndex, typefaceInfo.styleName, fontMethod, baseSize, style);
	}

	Font::IDType CFont::create(const FilePathView path, const size_t faceIndex, const StringView styleName, const FontMethod fontMethod, const int32 baseSize, const FontStyle style)
	{
		if (not path)
		{
			return Font::IDType::Null();
		}

		std::unique_ptr<FontData> font = std::make_unique<FontData>(m_freeType, path, faceIndex, styleName, fontMethod, baseSize, style);

		if (not font->isInitialized())
		{
			return Font::IDType::Null();
		}

		const String info = font->toString();
		return m_fonts.add(std::move(font), info);
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	void CFont::release(const Font::IDType handleID)
	{
		for (const auto& fallbackFontID : m_fonts[handleID]->getFallbackFontIDs())
		{
			if (auto& fallbackFontEntry = m_fallbackFonts[fallbackFontID]; 
				fallbackFontEntry)
			{
				fallbackFontEntry.pop_back();
			}
		}

		m_fonts.erase(handleID);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addFallbackFont
	//
	////////////////////////////////////////////////////////////////

	void CFont::addFallbackFont(const Font::IDType handleID, const Font& font)
	{
		const Font::IDType fallbackFontID = font.id();
		
		m_fallbackFonts[fallbackFontID].push_back(font);
		
		m_fonts[handleID]->addFallbackFont(fallbackFontID);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getInfo
	//
	////////////////////////////////////////////////////////////////

	const FontFaceInfo& CFont::getInfo(const Font::IDType handleID)
	{
		return m_fonts[handleID]->getInfo();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setTabSize
	//
	////////////////////////////////////////////////////////////////

	void CFont::setTabSize(const Font::IDType handleID, const int32 tabSize)
	{
		m_fonts[handleID]->setTabSize(tabSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphIndex CFont::getGlyphIndex(const Font::IDType handleID, const char32 codePoint, const ReadingDirection readingDirection)
	{
		if (readingDirection == ReadingDirection::LeftToRight)
		{
			return m_fonts[handleID]->getGlyphIndex(codePoint);
		}
		else
		{
			return m_fonts[handleID]->getGlyphIndex(StringView{ &codePoint, 1 }, readingDirection);
		}
	}

	GlyphIndex CFont::getGlyphIndex(const Font::IDType handleID, const StringView ch, const ReadingDirection readingDirection)
	{
		return m_fonts[handleID]->getGlyphIndex(ch, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphNameByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	String CFont::getGlyphNameByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex)
	{
		return m_fonts[handleID]->getGlyphNameByGlyphIndex(glyphIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvanceByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	double CFont::getXAdvanceByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex)
	{
		return m_fonts[handleID]->getXAdvanceByGlyphIndex(glyphIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getYAdvanceByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	double CFont::getYAdvanceByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex)
	{
		return m_fonts[handleID]->getYAdvanceByGlyphIndex(glyphIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getYAdvance
	//
	////////////////////////////////////////////////////////////////

	double CFont::getYAdvance(const Font::IDType handleID, const StringView ch)
	{
		return m_fonts[handleID]->getYAdvance(ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getResolvedGlyphs
	//
	////////////////////////////////////////////////////////////////

	Array<ResolvedGlyph> CFont::getResolvedGlyphs(const Font::IDType handleID, const StringView s, const ReadingDirection readingDirection, const EnableFallback enableFallback, const EnableLigatures enableLigatures)
	{
		return m_fonts[handleID]->getResolvedGlyphs(s, readingDirection, enableFallback, enableLigatures);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphInfoByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphInfo CFont::getGlyphInfoByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex, const ReadingDirection readingDirection)
	{
		return m_fonts[handleID]->getGlyphInfoByGlyphIndex(glyphIndex, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderOutlineByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	OutlineGlyph CFont::renderOutlineByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex, const CloseRing closeRing, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		return RenderOutlineGlyph(font->getFace(), glyphIndex, closeRing, font->getInfo(), readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderBitmapByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	BitmapGlyph CFont::renderBitmapByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex, const ReadingDirection readingDirection)
	{
		return m_fonts[handleID]->renderBitmapByGlyphIndex(glyphIndex, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getTexture
	//
	////////////////////////////////////////////////////////////////

	const Texture& CFont::getTexture(const Font::IDType handleID)
	{
		return m_fonts[handleID]->getGlyphCache().getTexture();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvances
	//
	////////////////////////////////////////////////////////////////

	Array<double> CFont::getXAdvances(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const double fontSize)
	{
		const auto& font = m_fonts[handleID];
		{
			return font->getGlyphCache().getXAdvances(*font, s, resolvedGlyphs, fontSize);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	xAdvanceFallback
	//
	////////////////////////////////////////////////////////////////

	double CFont::xAdvanceFallback(const Font::IDType handleID, const ResolvedGlyph& resolvedGlyph, const double fontSize)
	{
		const auto& font = m_fonts[handleID];
		{
			return font->getGlyphCache().xAdvanceFallback(*font, resolvedGlyph, fontSize);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	region
	//
	////////////////////////////////////////////////////////////////

	RectF CFont::region(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];

		if (readingDirection == ReadingDirection::TopToBottom)
		{
			return m_fonts[handleID]->getGlyphCache().processVertical(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, false, readingDirection);
		}
		else
		{
			return m_fonts[handleID]->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, false, readingDirection);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	RectF CFont::draw(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		const bool isColorFont = font->getInfo().properties.hasColor;

		if ((textStyle.type != TextStyle::Type::Default) && (not isColorFont))
		{
			//if (font->getMethod() == FontMethod::SDF)
			//{
			//	Graphics2D::SetSDFParameters(textStyle);
			//}
			//else
			//{
			//	Graphics2D::SetMSDFParameters(textStyle);
			//}
		}

		if (textStyle.type == TextStyle::Type::CustomShader)
		{
			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return m_fonts[handleID]->getGlyphCache().processVertical(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
			else
			{
				return m_fonts[handleID]->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
		}
		else
		{
			//ScopedCustomShader2D ps{ m_shader->getFontShader(font->getMethod(), textStyle.type, hasColor) };

			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return m_fonts[handleID]->getGlyphCache().processVertical(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
			else
			{
				return m_fonts[handleID]->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	regionBase
	//
	////////////////////////////////////////////////////////////////

	RectF CFont::regionBase(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];

		if (readingDirection == ReadingDirection::TopToBottom)
		{
			return m_fonts[handleID]->getGlyphCache().processVertical(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, false, readingDirection);
		}
		else
		{
			return m_fonts[handleID]->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, false, readingDirection);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawBase
	//
	////////////////////////////////////////////////////////////////

	RectF CFont::drawBase(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		const bool isColorFont = font->getInfo().properties.hasColor;

		if (readingDirection == ReadingDirection::TopToBottom)
		{
			return m_fonts[handleID]->getGlyphCache().processVertical(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
		}
		else
		{
			return m_fonts[handleID]->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	regionBaseFallback
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, double> CFont::regionBaseFallback(const Font::IDType handleID, const ResolvedGlyph& resolvedGlyph, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];

		if (readingDirection == ReadingDirection::TopToBottom)
		{
			return m_fonts[handleID]->getGlyphCache().processVerticalFallback(IGlyphCache::TextOperation::Region, *font, resolvedGlyph, true, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, -1, false, readingDirection);
		}
		else
		{
			return m_fonts[handleID]->getGlyphCache().processHorizontalFallback(IGlyphCache::TextOperation::Region, *font, resolvedGlyph, true, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, -1, false, readingDirection);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawBaseFallback
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, double> CFont::drawBaseFallback(const Font::IDType handleID, const ResolvedGlyph& resolvedGlyph, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const int32 index, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		const bool isColorGlyph = font->getInfo().properties.hasColor;

		if ((textStyle.type != TextStyle::Type::Default) && (not isColorGlyph))
		{
			//if (font->getMethod() == FontMethod::SDF)
			//{
			//	Graphics2D::SetSDFParameters(textStyle);
			//}
			//else
			//{
			//	Graphics2D::SetMSDFParameters(textStyle);
			//}
		}

		if (textStyle.type == TextStyle::Type::CustomShader)
		{
			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return m_fonts[handleID]->getGlyphCache().processVerticalFallback(IGlyphCache::TextOperation::Draw, *font, resolvedGlyph, true, pos, fontSize, textStyle, textEffect, index, isColorGlyph, readingDirection);
			}
			else
			{
				return m_fonts[handleID]->getGlyphCache().processHorizontalFallback(IGlyphCache::TextOperation::Draw, *font, resolvedGlyph, true, pos, fontSize, textStyle, textEffect, index, isColorGlyph, readingDirection);
			}
		}
		else
		{
			//ScopedCustomShader2D ps{ m_shader->getFontShader(font->getMethod(), textStyle.type, hasColor) };

			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return m_fonts[handleID]->getGlyphCache().processVerticalFallback(IGlyphCache::TextOperation::Draw, *font, resolvedGlyph, true, pos, fontSize, textStyle, textEffect, index, isColorGlyph, readingDirection);
			}
			else
			{
				return m_fonts[handleID]->getGlyphCache().processHorizontalFallback(IGlyphCache::TextOperation::Draw, *font, resolvedGlyph, true, pos, fontSize, textStyle, textEffect, index, isColorGlyph, readingDirection);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	fitsRect
	//
	////////////////////////////////////////////////////////////////

	bool CFont::fitsRect(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& rect, const double fontSize, const TextStyle& textStyle, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];

		//if (readingDirection == ReadingDirection::TopToBottom)
		//{
		//	return m_fonts[handleID]->getGlyphCache().processVerticalRect(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, rect, fontSize, textStyle, ColorF{}, readingDirection);
		//}
		//else
		//{
			return m_fonts[handleID]->getGlyphCache().processHorizontalRect(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, rect, fontSize, textStyle, TextEffect::NullTextEffect{}, false, readingDirection);
		//}
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawRect
	//
	////////////////////////////////////////////////////////////////

	bool CFont::drawRect(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& rect, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		const bool isColorFont = font->getInfo().properties.hasColor;

		if ((textStyle.type != TextStyle::Type::Default) && (not isColorFont))
		{
			//if (font->getMethod() == FontMethod::SDF)
			//{
			//	Graphics2D::SetSDFParameters(textStyle);
			//}
			//else
			//{
			//	Graphics2D::SetMSDFParameters(textStyle);
			//}
		}

		if (textStyle.type == TextStyle::Type::CustomShader)
		{
			//if (readingDirection == ReadingDirection::TopToBottom)
			//{
			//	return m_fonts[handleID]->getGlyphCache().processVerticalRect(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, rect, fontSize, textStyle, drawColor, readingDirection);
			//}
			//else
			//{
				return m_fonts[handleID]->getGlyphCache().processHorizontalRect(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, rect, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			//}
		}
		else
		{
			//ScopedCustomShader2D ps{ m_shader->getFontShader(font->getMethod(), textStyle.type, hasColor) };

			//if (readingDirection == ReadingDirection::TopToBottom)
			//{
			//	return m_fonts[handleID]->getGlyphCache().processVertical(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, rect, fontSize, textStyle, drawColor, readingDirection);
			//}
			//else
			//{
				return m_fonts[handleID]->getGlyphCache().processHorizontalRect(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, rect, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			//}
		}
	}
}
