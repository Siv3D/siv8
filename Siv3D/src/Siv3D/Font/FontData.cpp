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

		if (not m_face->init(library, baseFace, styleName, baseSize, style))
		{
			return;
		}

		m_faceIndex	= static_cast<uint16>(faceIndex);
		
		if (not m_face->getProperties().isScalable)
		{
			m_renderingMethod = FontMethod::Bitmap;
		}
		else
		{
			m_renderingMethod = fontMethod;
		}

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
		const auto& properties = m_face->getProperties();
		const int32 baseSize = m_face->getBaseSize();
		const FontStyle style = m_face->getStyle();

		return U"({0}, {1}, {2}, {3})"_fmt(properties.postscriptName, m_renderingMethod, baseSize, style);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getProperties
	//
	////////////////////////////////////////////////////////////////

	const FontFaceProperties& FontData::getProperties() const noexcept
	{
		return m_face->getProperties();
	}

}
