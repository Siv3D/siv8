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

# include "CIcon.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>
# include <Siv3D/Font/GlyphRenderer/BitmapGlyphRenderer.hpp>
# include <Siv3D/Font/GlyphRenderer/MSDFGlyphRenderer.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static GlyphIndex GetSingleGlyphIndex(hb_font_t* hbFont, hb_buffer_t* hbBuffer, const uint32 code)
		{
			::hb_buffer_reset(hbBuffer);
			::hb_buffer_add_utf32(hbBuffer, &code, 1, 0, 1);
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
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CIcon::~CIcon()
	{
		LOG_SCOPED_DEBUG("CIcon::~CIcon()");

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

	void CIcon::init()
	{
		LOG_SCOPED_DEBUG("CIcon::init()");

		const FilePath iconFilePath = (CacheDirectory::Engine() + U"font/materialdesignicons/materialdesignicons-webfont.ttf");

		if (not FileSystem::Exists(iconFilePath))
		{
			LOG_INFO("Icon font file not found");
			return;
		}

		if (not SIV3D_ENGINE(Font)->newFace(iconFilePath, 0, m_face))
		{
			LOG_FAIL(U"Failed to load the icon font file: {}"_fmt(iconFilePath));
			return;
		}

		m_hbFont = ::hb_ft_font_create_referenced(m_face);

		if (not m_hbFont)
		{
			LOG_FAIL("hb_ft_font_create_referenced (): failed");
			return;
		}

		m_hbBuffer = ::hb_buffer_create();

		m_available = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasGlyph
	//
	////////////////////////////////////////////////////////////////

	bool CIcon::hasGlyph(const char32 icon) const
	{
		if (not m_available)
		{
			return false;
		}

		return (GetSingleGlyphIndex(m_hbFont, m_hbBuffer, icon) != GlyphIndexNotdef);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphIndex CIcon::getGlyphIndex(const char32 icon) const
	{
		if (not m_available)
		{
			return GlyphIndexNotdef;
		}

		return GetSingleGlyphIndex(m_hbFont, m_hbBuffer, icon);
	}

	////////////////////////////////////////////////////////////////
	//
	//	render
	//
	////////////////////////////////////////////////////////////////

	Image CIcon::render(const GlyphIndex glyphIndex, const int32 size)
	{
		if (not m_available)
		{
			return{};
		}

		if (::FT_Set_Pixel_Sizes(m_face, 0, size))
		{
			return{};
		}

		{
			m_faceInfo.baseSize		= static_cast<int16>(size);
			m_faceInfo.ascender		= (m_face->size->metrics.ascender / 64.0f);
			m_faceInfo.descender	= -(m_face->size->metrics.descender / 64.0f);
		}

		return RenderBitmapGlyph(m_face, glyphIndex, m_faceInfo, ReadingDirection::LeftToRight, nullptr).image;
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderMSDF
	//
	////////////////////////////////////////////////////////////////

	Image CIcon::renderMSDF(const GlyphIndex glyphIndex, const int32 size, const int32 bufferSize)
	{
		if (not m_available)
		{
			return{};
		}

		if (::FT_Set_Pixel_Sizes(m_face, 0, size))
		{
			return{};
		}

		{
			m_faceInfo.baseSize		= static_cast<int16>(size);
			m_faceInfo.ascender		= (m_face->size->metrics.ascender / 64.0f);
			m_faceInfo.descender	= -(m_face->size->metrics.descender / 64.0f);
		}

		return RenderMSDFGlyph(m_face, glyphIndex, m_faceInfo, bufferSize, ReadingDirection::LeftToRight).image;
	}
}
