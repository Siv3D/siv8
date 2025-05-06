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

# include "FontFace.hpp"
# include "FontUtility.hpp"

namespace s3d
{
	FontFace::~FontFace()
	{
		if (m_face != nullptr)
		{
			::FT_Done_Face(m_face);
			m_face = nullptr;
		}
	}

	bool FontFace::init(const ::FT_Library library, ::FT_Face face, const StringView styleName, const int32 baseSize, const FontStyle style)
	{
		::FT_MM_Var* mmVar = nullptr;

		if (::FT_Get_MM_Var(face, &mmVar) == 0) // Variable font
		{
			if (styleName)
			{
				bool found = false;

				for (::FT_UInt styleIndex = 0; styleIndex < mmVar->num_namedstyles; ++styleIndex)
				{
					::FT_Set_Named_Instance(face, (1 + styleIndex));

					const FontFaceProperties properties = GetFontFaceProperties(face, mmVar->namedstyle[styleIndex].coords);

					if (properties.styleName == styleName)
					{
						m_properties = properties;
						found = true;
						break;
					}
				}

				if (not found)
				{
					::FT_UInt defaultStyleIndex = 0;
					::FT_Get_Default_Named_Instance(face, &defaultStyleIndex);

					::FT_Set_Named_Instance(face, defaultStyleIndex);
					m_properties = GetFontFaceProperties(face, nullptr);
				}
			}
			else
			{
				::FT_UInt defaultStyleIndex = 0;
				::FT_Get_Default_Named_Instance(face, &defaultStyleIndex);

				::FT_Set_Named_Instance(face, defaultStyleIndex);
				m_properties = GetFontFaceProperties(face, nullptr);
			}

			::FT_Done_MM_Var(library, mmVar);
		}
		else
		{
			m_properties = GetFontFaceProperties(face, nullptr);
		}

		m_face		= face;
		m_baseSize	= baseSize;
		m_style		= style;

		return true;
	}

	const FontFaceProperties& FontFace::getProperties() const noexcept
	{
		return m_properties;
	}

	int32 FontFace::getBaseSize() const noexcept
	{
		return m_baseSize;
	}

	FontStyle FontFace::getStyle() const noexcept
	{
		return m_style;
	}
}
