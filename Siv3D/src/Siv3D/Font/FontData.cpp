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

	GlyphIndex FontData::getGlyphIndex(const StringView ch)
	{
		return m_face->getGlyphIndex(ch);
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
	//	getXAdvanceFromGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	double FontData::getXAdvanceFromGlyphIndex(const GlyphIndex glyphIndex)
	{
		return m_face->getXAdvanceFromGlyphIndex(glyphIndex, getInfo().hinting).value_or(0.0);
	}
}
