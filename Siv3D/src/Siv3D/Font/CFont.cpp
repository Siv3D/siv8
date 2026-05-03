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

# include "CFont.hpp"
# include <Siv3D/TextStyle.hpp>
# include <Siv3D/TextLayoutResult.hpp>
# include <Siv3D/Graphics2D.hpp>
# include <Siv3D/ScopedCustomShader2D.hpp>
# include <Siv3D/TextEffect/NullTextEffect.hpp>
# include <Siv3D/MemoryMappedFileView.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include "FontUtility.hpp"
# include "TypefaceUtility.hpp"
# include "GlyphRenderer/OutlineGlyphRenderer.hpp"
# include "GlyphRenderer/PolygonGlyphRenderer.hpp"
# include "GlyphCache/IGlyphCache.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Array<FontFaceProperties> GetFontFaces(const ::FT_Face face)
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

				SIV3D_ENGINE(Font)->done_MM_Var(mmVar);
			}
			else
			{
				faces << GetFontFaceProperties(face, nullptr);
			}

			return faces;
		}

		[[nodiscard]]
		constexpr bool UseCustomShader(const TextStyle& textStyle, const bool isColorFont)
		{
			if (isColorFont)
			{
				return (textStyle.type == TextStyle::Type::CustomColorFontShader);
			}
			else
			{
				return (textStyle.type == TextStyle::Type::CustomTextFontShader);
			}
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
			throw InternalEngineError{ "FT_Init_FreeType() failed" };
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

		// フォント用シェーダ
		{
			m_shader = std::make_unique<FontShader>();
			m_shader->bitmapFont			= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::Texture2D);
			m_shader->msdfFont				= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::FontMSDF);
			m_shader->msdfFontOutline		= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::FontMSDF_Outline);
			m_shader->msdfFontShadow		= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::FontMSDF_Shadow);
			m_shader->msdfFontOutlineShadow	= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::FontMSDF_OutlineShadow);
			m_shader->msdfGlow				= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::FontMSDF_Glow);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFontFaces
	//
	////////////////////////////////////////////////////////////////

	Array<FontFaceProperties> CFont::getFontFaces(const FilePathView path)
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

			if (not newFace(view.data, view.size, 0, face0))
			{
				return{};
			}

			const ::FT_Long numFaces = face0->num_faces;

			faces.append(GetFontFaces(face0));

			for (::FT_Long index = 1; index < numFaces; ++index)
			{
				::FT_Face face = nullptr;

				if (not newFace(view.data, view.size, index, face))
				{
					faces.emplace_back();
					continue;
				}

				faces.append(GetFontFaces(face));

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

	Font::IDType CFont::create(const FontMethod fontMethod, const int32 baseSize, const Typeface typeface, const FontOptions& options)
	{
		const TypefaceInfo typefaceInfo = GetTypefaceInfo(typeface, fontMethod);

		if (not typefaceInfo.path)
		{
			return Font::IDType::Null();
		}

		return create(fontMethod, baseSize, typefaceInfo.path, options);
	}

	Font::IDType CFont::create(const FontMethod fontMethod, const int32 baseSize, const FilePathView path, const FontOptions& options)
	{
		if (not path)
		{
			return Font::IDType::Null();
		}

		std::unique_ptr<FontData> font = std::make_unique<FontData>(fontMethod, baseSize, path, options);

		if (not font->isInitialized())
		{
			return Font::IDType::Null();
		}

		const String info = font->toString();
		return m_fonts.add(std::move(font), info);
	}

	Font::IDType CFont::create(const FontMethod fontMethod, const int32 baseSize, std::unique_ptr<IReader> reader, const FontOptions& options)
	{
		if (not reader)
		{
			return Font::IDType::Null();
		}

		std::unique_ptr<FontData> font = std::make_unique<FontData>(fontMethod, baseSize, std::move(reader), options);

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
	//	addFallback
	//
	////////////////////////////////////////////////////////////////

	void CFont::addFallback(const Font::IDType handleID, const Font& font)
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
	//	setBufferThickness
	//
	////////////////////////////////////////////////////////////////

	void CFont::setBufferThickness(const Font::IDType handleID, const int32 thickness)
	{
		m_fonts[handleID]->getGlyphCache().setBufferThickness(thickness);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBufferThickness
	//
	////////////////////////////////////////////////////////////////

	int32 CFont::getBufferThickness(const Font::IDType handleID)
	{
		return m_fonts[handleID]->getGlyphCache().getBufferThickness();
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
	//	generateOutlineGlyphByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	OutlineGlyph CFont::generateOutlineGlyphByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex, const CloseRing closeRing, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		return RenderOutlineGlyph(font->getFace(), glyphIndex, closeRing, font->getInfo(), readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	generateOutlineGlyphs
	//
	////////////////////////////////////////////////////////////////

	Array<OutlineGlyph> CFont::generateOutlineGlyphs(const Font::IDType handleID, const StringView s, const CloseRing closeRing, const EnableLigatures enableLigatures, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		const FT_Face face = font->getFace();
		const FontFaceInfo& fontInfo = font->getInfo();
		const Array<ResolvedGlyph> resolvedGlyphs = font->getResolvedGlyphs(s, readingDirection, EnableFallback::Yes, enableLigatures);
		
		Array<OutlineGlyph> outlineGlyphs(Arg::reserve = resolvedGlyphs.size());
		{
			for (const auto& resolvedGlyph : resolvedGlyphs)
			{
				outlineGlyphs.emplace_back(RenderOutlineGlyph(face, resolvedGlyph.glyphIndex, closeRing, fontInfo, readingDirection));
			}
		}

		return outlineGlyphs;
	}

	////////////////////////////////////////////////////////////////
	//
	//	generatePolygonGlyphByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	PolygonGlyph CFont::generatePolygonGlyphByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		return RenderPolygonGlyph(font->getFace(), glyphIndex, font->getInfo(), readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderPolygons
	//
	////////////////////////////////////////////////////////////////

	Array<PolygonGlyph> CFont::generatePolygonGlyphs(const Font::IDType handleID, const StringView s, const EnableLigatures enableLigatures, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		const FT_Face face = font->getFace();
		const FontFaceInfo& fontInfo = font->getInfo();
		const Array<ResolvedGlyph> resolvedGlyphs = font->getResolvedGlyphs(s, readingDirection, EnableFallback::Yes, enableLigatures);
		
		Array<PolygonGlyph> polygonGlyphs(Arg::reserve = resolvedGlyphs.size());
		{
			for (const auto& resolvedGlyph : resolvedGlyphs)
			{
				polygonGlyphs.emplace_back(RenderPolygonGlyph(face, resolvedGlyph.glyphIndex, fontInfo, readingDirection));
			}
		}

		return polygonGlyphs;
	}

	////////////////////////////////////////////////////////////////
	//
	//	generateBitmapGlyphByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	BitmapGlyph CFont::generateBitmapGlyphByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex, const ReadingDirection readingDirection)
	{
		return m_fonts[handleID]->generateBitmapGlyphByGlyphIndex(glyphIndex, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	generateMSDFGlyphByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	MSDFGlyph CFont::generateMSDFGlyphByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex, const ReadingDirection readingDirection)
	{
		return m_fonts[handleID]->generateMSDFGlyphByGlyphIndex(glyphIndex, getBufferThickness(handleID), readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	preload
	//
	////////////////////////////////////////////////////////////////

	bool CFont::preload(const Font::IDType handleID, const Array<ResolvedGlyph>& resolvedGlyphs, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		{
			return font->getGlyphCache().preload(*font, resolvedGlyphs, readingDirection);
		}
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
	//	getGlyphByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	Glyph CFont::getGlyphByGlyphIndex(const Font::IDType handleID, const GlyphIndex glyphIndex, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		auto& cache = font->getGlyphCache();
		cache.preload(*font, Array<ResolvedGlyph>{ ResolvedGlyph{ glyphIndex, 0 } }, readingDirection);
		const auto& [glyphInfo, textureRegion] = cache.getGlyph(glyphIndex, readingDirection);
		Glyph glyph{ glyphInfo };
		glyph.codePoint = 0; // 逆引きは不可能（1 つのグリフが複数のコードポイントを持つ場合があるため）
		glyph.texture = textureRegion;
		return glyph;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphs
	//
	////////////////////////////////////////////////////////////////

	Array<Glyph> CFont::getGlyphs(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		auto& cache = font->getGlyphCache();
		cache.preload(*font, resolvedGlyphs, readingDirection);

		Array<Glyph> glyphs(Arg::reserve = resolvedGlyphs.size());
		{
			for (const auto& resolvedGlyph : resolvedGlyphs)
			{
				const auto& [glyphInfo, textureRegion] = cache.getGlyph(resolvedGlyph.glyphIndex, readingDirection);
				Glyph glyph{ glyphInfo };
				glyph.codePoint = s[resolvedGlyph.pos];
				glyph.texture = textureRegion;
				glyphs.push_back(std::move(glyph));
			}
		}

		return glyphs;
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
		const auto& fontInfo = font->getInfo();
		const bool isColorFont = fontInfo.properties.hasColor;

		if ((textStyle.type != TextStyle::Type::Default) && (not isColorFont))
		{
			if (fontInfo.renderingMethod == FontMethod::MSDF)
			{
				Graphics2D::SetMSDFParameters(textStyle, fontInfo.baseSize);
			}
		}

		if (UseCustomShader(textStyle, isColorFont))
		{
			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return font->getGlyphCache().processVertical(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
			else
			{
				return font->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
		}
		else
		{
			const ScopedCustomShader2D ps{ m_shader->getFontShader(fontInfo.renderingMethod, textStyle.type) };

			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return font->getGlyphCache().processVertical(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
			else
			{
				return font->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, false, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
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
			return font->getGlyphCache().processVertical(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, false, readingDirection);
		}
		else
		{
			return font->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, false, readingDirection);
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
		const auto& fontInfo = font->getInfo();
		const bool isColorFont = fontInfo.properties.hasColor;

		if ((textStyle.type != TextStyle::Type::Default) && (not isColorFont))
		{
			if (fontInfo.renderingMethod == FontMethod::MSDF)
			{
				Graphics2D::SetMSDFParameters(textStyle, fontInfo.baseSize);
			}
		}

		if (UseCustomShader(textStyle, isColorFont))
		{
			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return font->getGlyphCache().processVertical(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
			else
			{
				return font->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
		}
		else
		{
			const ScopedCustomShader2D ps{ m_shader->getFontShader(fontInfo.renderingMethod, textStyle.type) };

			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return font->getGlyphCache().processVertical(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
			else
			{
				return font->getGlyphCache().processHorizontal(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, true, pos, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			}
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
			return font->getGlyphCache().processVerticalFallback(IGlyphCache::TextOperation::Region, *font, resolvedGlyph, true, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, -1, -1, false, readingDirection);
		}
		else
		{
			return font->getGlyphCache().processHorizontalFallback(IGlyphCache::TextOperation::Region, *font, resolvedGlyph, true, pos, fontSize, textStyle, TextEffect::NullTextEffect{}, -1, -1, false, readingDirection);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawBaseFallback
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, double> CFont::drawBaseFallback(const Font::IDType handleID, const ResolvedGlyph& resolvedGlyph, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const int32 index, const int32 totalGlyphCount, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		const auto& fontInfo = font->getInfo();
		const bool isColorFont = fontInfo.properties.hasColor;

		if ((textStyle.type != TextStyle::Type::Default) && (not isColorFont))
		{
			if (fontInfo.renderingMethod == FontMethod::MSDF)
			{
				Graphics2D::SetMSDFParameters(textStyle, fontInfo.baseSize);
			}
		}

		if (UseCustomShader(textStyle, isColorFont))
		{
			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return font->getGlyphCache().processVerticalFallback(IGlyphCache::TextOperation::Draw, *font, resolvedGlyph, true, pos, fontSize, textStyle, textEffect, index, totalGlyphCount, isColorFont, readingDirection);
			}
			else
			{
				return font->getGlyphCache().processHorizontalFallback(IGlyphCache::TextOperation::Draw, *font, resolvedGlyph, true, pos, fontSize, textStyle, textEffect, index, totalGlyphCount, isColorFont, readingDirection);
			}
		}
		else
		{
			const ScopedCustomShader2D ps{ m_shader->getFontShader(fontInfo.renderingMethod, textStyle.type) };

			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return font->getGlyphCache().processVerticalFallback(IGlyphCache::TextOperation::Draw, *font, resolvedGlyph, true, pos, fontSize, textStyle, textEffect, index, totalGlyphCount, isColorFont, readingDirection);
			}
			else
			{
				return font->getGlyphCache().processHorizontalFallback(IGlyphCache::TextOperation::Draw, *font, resolvedGlyph, true, pos, fontSize, textStyle, textEffect, index, totalGlyphCount, isColorFont, readingDirection);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	fitsRect
	//
	////////////////////////////////////////////////////////////////

	TextLayoutResult CFont::fitsRect(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& rect, const double fontSize, const TextStyle& textStyle, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];

		//if (readingDirection == ReadingDirection::TopToBottom)
		//{
		//	return font->getGlyphCache().processVerticalRect(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, rect, fontSize, textStyle, ColorF{}, readingDirection);
		//}
		//else
		//{
			return font->getGlyphCache().processHorizontalRect(IGlyphCache::TextOperation::Region, *font, s, resolvedGlyphs, rect, fontSize, textStyle, TextEffect::NullTextEffect{}, false, readingDirection);
		//}
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawRect
	//
	////////////////////////////////////////////////////////////////

	TextLayoutResult CFont::drawRect(const Font::IDType handleID, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& rect, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const ReadingDirection readingDirection)
	{
		const auto& font = m_fonts[handleID];
		const auto& fontInfo = font->getInfo();
		const bool isColorFont = fontInfo.properties.hasColor;

		if ((textStyle.type != TextStyle::Type::Default) && (not isColorFont))
		{
			if (fontInfo.renderingMethod == FontMethod::MSDF)
			{
				Graphics2D::SetMSDFParameters(textStyle, fontInfo.baseSize);
			}
		}

		if (UseCustomShader(textStyle, isColorFont))
		{
			//if (readingDirection == ReadingDirection::TopToBottom)
			//{
			//	return font->getGlyphCache().processVerticalRect(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, rect, fontSize, textStyle, drawColor, readingDirection);
			//}
			//else
			//{
				return font->getGlyphCache().processHorizontalRect(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, rect, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			//}
		}
		else
		{
			const ScopedCustomShader2D ps{ m_shader->getFontShader(fontInfo.renderingMethod, textStyle.type) };

			//if (readingDirection == ReadingDirection::TopToBottom)
			//{
			//	return font->getGlyphCache().processVertical(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, rect, fontSize, textStyle, drawColor, readingDirection);
			//}
			//else
			//{
				return font->getGlyphCache().processHorizontalRect(IGlyphCache::TextOperation::Draw, *font, s, resolvedGlyphs, rect, fontSize, textStyle, textEffect, isColorFont, readingDirection);
			//}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFontShader
	//
	////////////////////////////////////////////////////////////////

	const PixelShader& CFont::getFontShader(const FontMethod method, const TextStyle::Type type) const
	{
		return m_shader->getFontShader(method, type);
	}

	////////////////////////////////////////////////////////////////
	//
	//	newFace
	//
	////////////////////////////////////////////////////////////////

	bool CFont::newFace(const FilePathView path, const uint32 faceIndex, FT_Face& face)
	{
		const std::string pathUTF8 = Unicode::ToUTF8(path);

		std::lock_guard lock{ m_freeTypeMutex };

		if (const ::FT_Error error = ::FT_New_Face(m_freeType, pathUTF8.c_str(), static_cast<::FT_Long>(faceIndex), &face))
		{
			if (error == ::FT_Err_Unknown_File_Format)
			{
				LOG_FAIL("FT_New_Face(): FT_Err_Unknown_File_Format");
			}
			else
			{
				LOG_FAIL("FT_New_Face(): failed");
			}

			return false;
		}

		return true;
	}

	bool CFont::newFace(const void* data, const size_t size_bytes, const uint32 faceIndex, FT_Face& face)
	{
		std::lock_guard lock{ m_freeTypeMutex };

		if (const ::FT_Error error = ::FT_New_Memory_Face(m_freeType, static_cast<const ::FT_Byte*>(data), static_cast<::FT_Long>(size_bytes), static_cast<::FT_Long>(faceIndex), &face))
		{
			if (error == ::FT_Err_Unknown_File_Format)
			{
				LOG_FAIL("FT_New_Memory_Face(): FT_Err_Unknown_File_Format");
			}
			else
			{
				LOG_FAIL("FT_New_Memory_Face(): failed");
			}

			return false;
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	done_MM_Var
	//
	////////////////////////////////////////////////////////////////

	void CFont::done_MM_Var(FT_MM_Var_* amaster)
	{
		std::lock_guard lock{ m_freeTypeMutex };

		::FT_Done_MM_Var(m_freeType, amaster);
	}
}
