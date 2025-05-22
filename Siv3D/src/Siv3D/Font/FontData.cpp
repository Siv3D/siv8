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

# include "FontData.hpp"
# include "FontUtility.hpp"
# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	FontData::FontData(Null)
	{
		m_face = std::make_unique<FontFace>();

		m_initialized = true;
	}

	FontData::FontData(const ::FT_Library library, const FilePathView path, const size_t faceIndex, const StringView styleName, const FontMethod fontMethod, const int32 baseSize, const FontStyle style)
	{
		if (not m_mappedFileView.open(path))
		{
			return;
		}

		const MappedMemoryView view = m_mappedFileView.mapAll();

		if (not view)
		{
			return;
		}

		FT_Face baseFace = nullptr;

		if (const FT_Error error = ::FT_New_Memory_Face(library, static_cast<const FT_Byte*>(view.data), static_cast<FT_Long>(view.size), static_cast<::FT_Long>(faceIndex), &baseFace))
		{
			return;
		}

		m_face = std::make_unique<FontFace>();

		if (not m_face->init(library, baseFace, styleName, fontMethod, baseSize, style))
		{
			return;
		}

		m_renderingMethod = m_face->getInfo().renderingMethod;
		
		m_initialized	= true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInitialized
	//
	////////////////////////////////////////////////////////////////

	bool FontData::isInitialized() const noexcept
	{
		return m_initialized;
	}

	////////////////////////////////////////////////////////////////
	//
	//	toString
	//
	////////////////////////////////////////////////////////////////

	String FontData::toString() const
	{
		const auto& info = m_face->getInfo();
		return U"({0}, {1}, {2}, {3})"_fmt(info.properties.postscriptName, info.renderingMethod, info.baseSize, info.style);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addFallbackFont
	//
	////////////////////////////////////////////////////////////////

	void FontData::addFallbackFont(const Font::IDType handleID)
	{
		m_fallbackFontIDs.push_back(handleID);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFallbackFontIDs
	//
	////////////////////////////////////////////////////////////////

	const Array<Font::IDType>& FontData::getFallbackFontIDs() const noexcept
	{
		return m_fallbackFontIDs;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFace
	//
	////////////////////////////////////////////////////////////////

	::FT_Face FontData::getFace() noexcept
	{
		return m_face->getFace();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getInfo
	//
	////////////////////////////////////////////////////////////////

	const FontFaceInfo& FontData::getInfo() const noexcept
	{
		return m_face->getInfo();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setTabSize
	//
	////////////////////////////////////////////////////////////////

	void FontData::setTabSize(const int32 tabSize) noexcept
	{
		m_face->setTabSize(tabSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphIndex FontData::getGlyphIndex(const char32 codePoint)
	{
		return m_face->getGlyphIndex(codePoint);
	}

	GlyphIndex FontData::getGlyphIndex(const StringView ch, const ReadingDirection readingDirection)
	{
		return m_face->getGlyphIndex(ch, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphNameByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	String FontData::getGlyphNameByGlyphIndex(const GlyphIndex glyphIndex)
	{
		return m_face->getGlyphNameByGlyphIndex(glyphIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvanceByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	double FontData::getXAdvanceByGlyphIndex(const GlyphIndex glyphIndex)
	{
		return m_face->getXAdvanceByGlyphIndex(glyphIndex, getInfo().hinting).value_or(0.0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getYAdvanceByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	double FontData::getYAdvanceByGlyphIndex(const GlyphIndex glyphIndex)
	{
		return m_face->getYAdvanceByGlyphIndex(glyphIndex, getInfo().hinting).value_or(0.0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getYAdvance
	//
	////////////////////////////////////////////////////////////////

	double FontData::getYAdvance(const StringView ch)
	{
		return m_face->getYAdvance(ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getResolvedGlyphs
	//
	////////////////////////////////////////////////////////////////

	Array<ResolvedGlyph> FontData::getResolvedGlyphs(const StringView s, const ReadingDirection readingDirection, const EnableFallback enableFallback, const EnableLigatures enableLigatures)
	{
		const HarfBuzzGlyphInfo hbGlyphInfo = m_face->getHarfBuzzGlyphInfo(s, enableLigatures, readingDirection);
		const size_t count = hbGlyphInfo.count;

		if (enableFallback && m_fallbackFontIDs)
		{
			Array<ResolvedGlyph> result;

			for (size_t i = 0; i < count;)
			{
				{
					const auto& info = hbGlyphInfo.info[i];
					const ResolvedGlyph resolvedGlyph{
						.fontIndex	= 0,
						.glyphIndex	= info.codepoint,
						.pos		= info.cluster
					};

					if (resolvedGlyph.glyphIndex != GlyphIndexNotdef)
					{
						result.push_back(resolvedGlyph);
						++i;
						continue;
					}
				}

				const uint32 pos = hbGlyphInfo.info[i].cluster;
				size_t k = 0;

				for (;;)
				{
					++k;

					if ((count <= (i + k))
						|| (hbGlyphInfo.info[(i + k)].cluster != pos))
					{
						break;
					}
				}

				const size_t fallbackStringLength = [&]() -> size_t
				{	
					if (count <= (i + k))
					{
						return (s.size() - pos);
					}
					else
					{
						return (hbGlyphInfo.info[i + k].cluster - pos);
					}
				}();

				bool fallbackDone = false;
				
				for (uint32 fallbackIndex = 1;
					const auto& fallbackFontID : m_fallbackFontIDs)
				{
					const Array<ResolvedGlyph> resolvedGlyphs = SIV3D_ENGINE(Font)->getResolvedGlyphs(fallbackFontID, s.substr(pos, fallbackStringLength), readingDirection, EnableFallback::No, enableLigatures);

					if (resolvedGlyphs.none([](const ResolvedGlyph& g) { return (g.glyphIndex == 0); }))
					{
						for (const auto& resolvedGlyph : resolvedGlyphs)
						{
							result << ResolvedGlyph{
								.fontIndex	= fallbackIndex,
								.glyphIndex	= resolvedGlyph.glyphIndex,
								.pos		= pos
							};
						}

						fallbackDone = true;
						break;
					}

					++fallbackIndex;
				}

				if (not fallbackDone)
				{
					for (size_t m = 0; m < k; ++m)
					{
						const auto& info = hbGlyphInfo.info[i + m];
						result << ResolvedGlyph{
							.fontIndex	= 0,
							.glyphIndex	= info.codepoint,
							.pos		= info.cluster
						};
					}
				}

				i += k;
			}

			return result;
		}
		else
		{
			Array<ResolvedGlyph> result(count);

			for (size_t i = 0; i < count; ++i)
			{
				const auto& info = hbGlyphInfo.info[i];
				result[i] = {
					.fontIndex	= 0,
					.glyphIndex	= info.codepoint,
					.pos		= info.cluster
				};
			}

			return result;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphInfoByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphInfo FontData::getGlyphInfoByGlyphIndex(const GlyphIndex glyphIndex, const ReadingDirection readingDirection) const
	{
		const int16 bufferThickness = 0; // [Siv3D ToDo]
		return GetGlyphInfo(m_face->getFace(), glyphIndex, m_face->getInfo(), bufferThickness, readingDirection);
	}
}
