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
# include <Siv3D/ScopeExit.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static ::FT_Int SelectBestSize(const ::FT_Face face, const int32 size)
		{
			::FT_Int bestMatch = 0;
			int32 bestDiff = std::numeric_limits<int32>::max();
			
			for (::FT_Int i = 0; i < face->num_fixed_sizes; ++i)
			{
				const int32 diff = std::abs(face->available_sizes[i].height - size);
			
				if (diff < bestDiff)
				{
					bestDiff = diff;
					bestMatch = i;
				}
			}
			
			return bestMatch;
		}
	}

	FontFace::~FontFace()
	{
		m_hbObjects.reset();

		if (m_face != nullptr)
		{
			::FT_Done_Face(m_face);
			m_face = nullptr;
		}
	}

	bool FontFace::init(const ::FT_Library library, ::FT_Face face, const StringView styleName, int32 baseSize, const FontStyle style)
	{
		assert(m_face == nullptr);

		if (baseSize <= 0)
		{
			::FT_Done_Face(face);
			return false;
		}

		{
			::FT_MM_Var* mmVar = nullptr;

			const ScopeExit cleanup([&]()
			{
				if (mmVar)
				{
					::FT_Done_MM_Var(library, mmVar);
					mmVar = nullptr;
				}
			});

			if ((::FT_Get_MM_Var(face, &mmVar) == 0)
				&& (1 <= mmVar->num_namedstyles)) // Variable font
			{
				bool found = false;

				if (styleName)
				{
					for (::FT_UInt styleIndex = 0; styleIndex < mmVar->num_namedstyles; ++styleIndex)
					{
						if (::FT_Set_Named_Instance(face, (1 + styleIndex)))
						{
							continue;
						}

						const FontFaceProperties properties = GetFontFaceProperties(face, mmVar->namedstyle[styleIndex].coords);

						if (properties.styleName == styleName)
						{
							m_properties = properties;
							found = true;
							break;
						}
					}
				}
				
				if (not found)
				{
					// 1-origin
					::FT_UInt defaultStyleIndex = 0;

					if (::FT_Get_Default_Named_Instance(face, &defaultStyleIndex))
					{
						::FT_Done_Face(face);
						return false;
					}

					if (::FT_Set_Named_Instance(face, defaultStyleIndex))
					{
						::FT_Done_Face(face);
						return false;
					}

					assert(0 < defaultStyleIndex);
					m_properties = GetFontFaceProperties(face, mmVar->namedstyle[defaultStyleIndex - 1].coords);
				}
			}
			else
			{
				m_properties = GetFontFaceProperties(face, nullptr);
			}
		}

		// スケーリングできないフォント
		if (::FT_Set_Pixel_Sizes(face, 0, baseSize))
		{
			if (face->num_fixed_sizes == 0)
			{
				::FT_Done_Face(face);
				return false;
			}

			const ::FT_Int bestSizeIndex = SelectBestSize(face, baseSize);

			if (::FT_Select_Size(face, bestSizeIndex) != 0)
			{
				::FT_Done_Face(face);
				return false;
			}

			baseSize = face->available_sizes[bestSizeIndex].height;
		}

		{
			m_hbObjects = std::make_unique<HarfBuzzObjects>();

			if (not m_hbObjects->init(face))
			{
				::FT_Done_Face(face);
				return false;
			}
		}

		m_face		= face;
		m_baseSize	= baseSize;
		m_style		= style;
		m_ascender	= (m_face->size->metrics.ascender / 64.0f);
		m_descender	= -(m_face->size->metrics.descender / 64.0f);

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

	FontFace::HarfBuzzObjects::~HarfBuzzObjects()
	{
		if (hbBuffer)
		{
			::hb_buffer_destroy(hbBuffer);
			hbBuffer = nullptr;
		}

		if (hbFont)
		{
			::hb_font_destroy(hbFont);
			hbFont = nullptr;
		}
	}

	bool FontFace::HarfBuzzObjects::init(::FT_Face face)
	{
		assert(hbFont == nullptr);
		assert(hbBuffer == nullptr);

		hbFont = ::hb_ft_font_create(face, nullptr);

		if (hbFont == nullptr)
		{
			return false;
		}

		hbBuffer = ::hb_buffer_create();

		if (not ::hb_buffer_allocation_successful(hbBuffer))
		{
			::hb_font_destroy(hbFont);
			hbFont = nullptr;
			return false;
		}

		return true;
	}
}
