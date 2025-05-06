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

		::FT_Done_Face(baseFace);




		m_faceIndex		= static_cast<uint16>(faceIndex);
		m_fontMethod	= fontMethod;

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
		return U"()";
	}
}
