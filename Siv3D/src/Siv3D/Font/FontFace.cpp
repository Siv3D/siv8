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
# include <Siv3D/GlyphIndex.hpp>
# include <Siv3D/ScopeExit.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array<hb_feature_t, 5> NoLigatureFeatures{ {
			{ HB_TAG('l','i','g','a'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // 標準合字
			{ HB_TAG('r','l','i','g'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // 必須合字
			{ HB_TAG('d','i','g','a'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // 分音記号合字
			{ HB_TAG('c','a','l','t'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // コンテキスト依存代替字形
			{ HB_TAG('c','l','i','g'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // コンテキスト合字
		} };

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

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	FontFace::~FontFace()
	{
		m_hbObjects.reset();

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

	bool FontFace::init(const ::FT_Library library, ::FT_Face face, const StringView styleName, const FontMethod fontMethod, int32 baseSize, const FontStyle style)
	{
		assert(m_face == nullptr);

		if (not InRange(baseSize, 1, 32767))
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
							m_info.properties = properties;
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
					m_info.properties = GetFontFaceProperties(face, mmVar->namedstyle[defaultStyleIndex - 1].coords);
				}
			}
			else
			{
				m_info.properties = GetFontFaceProperties(face, nullptr);
			}
		}

		m_info.renderingMethod	= (m_info.properties.isScalable ? fontMethod : FontMethod::Bitmap);
		m_info.hinting			= ((m_info.renderingMethod == FontMethod::Bitmap) ? EnableHinting::Yes : EnableHinting::No);

		if (::FT_Set_Pixel_Sizes(face, 0, baseSize))
		{	
			//
			// スケーリングできないフォントの場合
			//

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

		m_face					= face;
		m_info.baseSize			= static_cast<int16>(baseSize);
		m_info.style			= style;
		m_info.ascender			= (m_face->size->metrics.ascender / 64.0f);
		m_info.descender		= -(m_face->size->metrics.descender / 64.0f);

		{
			const GlyphIndex glyphIndex = getGlyphIndex(U' ');
			m_info.spaceXAdvance = getXAdvanceFromGlyphIndex(glyphIndex, m_info.hinting).value_or(m_info.baseSize);
			m_info.spaceYAdvance = getYAdvanceFromGlyphIndex(glyphIndex, m_info.hinting).value_or(m_info.baseSize);
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getInfo
	//
	////////////////////////////////////////////////////////////////

	const FontFaceInfo& FontFace::getInfo() const noexcept
	{
		return m_info;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setTabSize
	//
	////////////////////////////////////////////////////////////////

	void FontFace::setTabSize(const int32 tabSize) noexcept
	{
		m_info.tabSize = static_cast<int16>(Clamp(tabSize, 0, 32767));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getHarfBuzzGlyphInfo
	//
	////////////////////////////////////////////////////////////////

	HarfBuzzGlyphInfo FontFace::getHarfBuzzGlyphInfo(const StringView s, const EnableLigatures enableLigatures) const
	{
		const auto& hbObjects = *m_hbObjects;

		::hb_buffer_clear_contents(hbObjects.hbBuffer);
		::hb_buffer_set_direction(hbObjects.hbBuffer, HB_DIRECTION_LTR);
		::hb_buffer_set_script(hbObjects.hbBuffer, HB_SCRIPT_COMMON);

		const int32 textLength = static_cast<int32>(s.length());
		::hb_buffer_add_utf32(hbObjects.hbBuffer, reinterpret_cast<const uint32_t*>(s.data()), textLength, 0, textLength);
		::hb_buffer_guess_segment_properties(hbObjects.hbBuffer);

		if (enableLigatures) // リガチャあり
		{
			::hb_shape(hbObjects.hbFont, hbObjects.hbBuffer, nullptr, 0);
		}
		else // リガチャ無し
		{
			::hb_shape(hbObjects.hbFont, hbObjects.hbBuffer, NoLigatureFeatures.data(), static_cast<uint32_t>(NoLigatureFeatures.size()));
		}

		uint32 glyphCount = 0;
		const hb_glyph_info_t* glyphInfo = ::hb_buffer_get_glyph_infos(hbObjects.hbBuffer, &glyphCount);

		return{
			.info = glyphInfo,
			.count = glyphCount
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvance
	//
	////////////////////////////////////////////////////////////////

	Optional<float> FontFace::getXAdvanceFromGlyphIndex(const GlyphIndex glyphIndex, const EnableHinting enableHinting)
	{
		::FT_Int32 loadFlag = (enableHinting ? FT_LOAD_DEFAULT : FT_LOAD_NO_HINTING);

		if (m_info.properties.hasColor)
		{
			::FT_Fixed advance = 0;

			if (::FT_Get_Advance(m_face, glyphIndex, (loadFlag | FT_LOAD_COLOR), &advance) == 0)
			{
				return (advance / 65536.0f);
			}

			// カラー読み込み失敗時はフォールバック
		}

		{
			if (not((m_info.renderingMethod == FontMethod::Bitmap)
				&& (m_info.style & FontStyle::Bitmap)))
			{
				loadFlag |= FT_LOAD_NO_BITMAP;
			}

			// スタイル合成が必要
			if (m_info.style & (FontStyle::Bold | FontStyle::Italic))
			{
				// ロード失敗時は none
				if (::FT_Load_Glyph(m_face, glyphIndex, loadFlag))
				{
					return none;
				}

				if (m_info.style & FontStyle::Bold)
				{
					::FT_GlyphSlot_Embolden(m_face->glyph);
				}

				if (m_info.style & FontStyle::Italic)
				{
					::FT_GlyphSlot_Oblique(m_face->glyph);
				}

				return (m_face->glyph->metrics.horiAdvance / 64.0f);
			}
			else
			{
				::FT_Fixed advance = 0;

				if (::FT_Get_Advance(m_face, glyphIndex, loadFlag, &advance) == 0)
				{
					return (advance / 65536.0f);
				}

				// ロード失敗時は none
				return none;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getYAdvance
	//
	////////////////////////////////////////////////////////////////

	Optional<float> FontFace::getYAdvanceFromGlyphIndex(const GlyphIndex glyphIndex, const EnableHinting enableHinting)
	{
		const bool hasVertical = m_info.properties.hasVertical;

		::FT_Int32 loadFlag = ((enableHinting ? FT_LOAD_DEFAULT : FT_LOAD_NO_HINTING) | (hasVertical ? FT_LOAD_VERTICAL_LAYOUT : 0));

		if (m_info.properties.hasColor)
		{
			::FT_Fixed advance = 0;

			if (::FT_Get_Advance(m_face, glyphIndex, (loadFlag | FT_LOAD_COLOR), &advance) == 0)
			{
				return (advance / 65536.0f);
			}

			// カラー読み込み失敗時はフォールバック
		}

		{
			if (not((m_info.renderingMethod == FontMethod::Bitmap)
				&& (m_info.style & FontStyle::Bitmap)))
			{
				loadFlag |= FT_LOAD_NO_BITMAP;
			}

			// スタイル合成が必要
			if (m_info.style & (FontStyle::Bold | FontStyle::Italic))
			{
				// ロード失敗時は none
				if (::FT_Load_Glyph(m_face, glyphIndex, loadFlag))
				{
					return none;
				}

				if (m_info.style & FontStyle::Bold)
				{
					::FT_GlyphSlot_Embolden(m_face->glyph);
				}

				if (m_info.style & FontStyle::Italic)
				{
					::FT_GlyphSlot_Oblique(m_face->glyph);
				}

				return ((hasVertical ? m_face->glyph->metrics.vertAdvance : m_face->glyph->metrics.horiAdvance) / 64.0f);
			}
			else
			{
				::FT_Fixed advance = 0;

				if (::FT_Get_Advance(m_face, glyphIndex, loadFlag, &advance) == 0)
				{
					return (advance / 65536.0f);
				}

				// ロード失敗時は none
				return none;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphIndex FontFace::getGlyphIndex(const char32 codePoint)
	{
		return ::FT_Get_Char_Index(m_face, codePoint);
	}

	GlyphIndex FontFace::getGlyphIndex(const StringView ch)
	{
		if (const size_t length = ch.size();
			length == 0)
		{
			return GlyphIndexNotdef;
		}
		else if (length == 1)
		{
			return getGlyphIndex(ch[0]);
		}
		else
		{
			const HarfBuzzGlyphInfo glyphInfo = getHarfBuzzGlyphInfo(ch, EnableLigatures::Yes);

			if (glyphInfo.count != 1)
			{
				return GlyphIndexNotdef;
			}

			return glyphInfo.info[0].codepoint;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphNameByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	String FontFace::getGlyphNameByGlyphIndex(const GlyphIndex glyphIndex)
	{
		char glyphNameBuffer[256]{};

		if (::FT_Get_Glyph_Name(m_face, glyphIndex, glyphNameBuffer, sizeof(glyphNameBuffer)) != 0)
		{
			return{};
		}

		return Unicode::FromUTF8(glyphNameBuffer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

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
