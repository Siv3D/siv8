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

# include <ThirdParty/skia/include/core/SkStream.h>

namespace s3d
{
	namespace
	{
		static constexpr std::array<hb_feature_t, 5> NoLigatureFeatures{ {
			{ HB_TAG('l','i','g','a'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // 標準合字
			{ HB_TAG('r','l','i','g'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // 必須合字
			{ HB_TAG('d','l','i','g'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // 分音記号合字
			{ HB_TAG('c','a','l','t'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // コンテキスト依存代替字形
			{ HB_TAG('c','l','i','g'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END }, // コンテキスト
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

	bool FontFace::init(const ::FT_Library library, const MappedMemoryView& memoryView, ::FT_Face face, const StringView styleName, const FontMethod fontMethod, int32 baseSize, const FontStyle style)
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
		m_info.hinting			= ((m_info.renderingMethod == FontMethod::Bitmap) ? EnableHinting::No : EnableHinting::Yes);

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

		if (m_info.properties.isCOLRv1)
		{
			m_colrv1 = std::make_unique<COLRv1>();
			std::unique_ptr<SkStreamAsset> stream = SkMemoryStream::MakeDirect(memoryView.data, memoryView.size);

			for (const auto& axes : m_info.properties.variationAxes)
			{
				const std::string tag = axes.tag.toUTF8();
				const float value = static_cast<float>(axes.value);

				if (tag.size() == 4)
				{
					const uint32 tagValue = SkSetFourByteTag(tag[0], tag[1], tag[2], tag[3]);
					m_colrv1->variationCoordinates.emplace_back(tagValue, value);
				}
			}

			SkFontArguments fontArgs{};

			if (m_colrv1->variationCoordinates)
			{
				SkFontArguments::VariationPosition variationPosition{};
				variationPosition.coordinates = m_colrv1->variationCoordinates.data();
				variationPosition.coordinateCount = static_cast<uint32_t>(m_colrv1->variationCoordinates.size());
				fontArgs.setVariationDesignPosition(variationPosition);
			}

			m_colrv1->skTypeface = SkTypeface_FreeType::MakeFromStream(std::move(stream), fontArgs);		
			m_colrv1->skFont.setTypeface(m_colrv1->skTypeface);
		}

		m_face					= face;
		m_info.baseSize			= static_cast<int16>(baseSize);
		m_info.style			= style;
		m_info.ascender			= (m_face->size->metrics.ascender / 64.0f);
		m_info.descender		= -(m_face->size->metrics.descender / 64.0f);

		{
			const GlyphIndex glyphIndex = getGlyphIndex(U' ');
			m_info.spaceXAdvance = getXAdvanceByGlyphIndex(glyphIndex, m_info.hinting).value_or(m_info.baseSize);
			m_info.spaceYAdvance = getYAdvanceByGlyphIndex(glyphIndex).value_or(m_info.baseSize);
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

	HarfBuzzGlyphInfo FontFace::getHarfBuzzGlyphInfo(const StringView s, const EnableLigatures enableLigatures, const ReadingDirection readingDirection) const
	{
		if (not m_face)
		{
			return{};
		}

		const auto& hbObjects = *m_hbObjects;

		::hb_buffer_clear_contents(hbObjects.hbBuffer);

		if (readingDirection == ReadingDirection::LeftToRight)
		{
			::hb_buffer_set_direction(hbObjects.hbBuffer, HB_DIRECTION_LTR);
		}
		else // ReadingDirection::TopToBottom
		{
			::hb_buffer_set_direction(hbObjects.hbBuffer, HB_DIRECTION_TTB);
		}

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
	//	getXAdvanceByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	Optional<float> FontFace::getXAdvanceByGlyphIndex(const GlyphIndex glyphIndex, const EnableHinting enableHinting)
	{
		if (not m_face)
		{
			return none;
		}

		::FT_Int32 loadFlag = (enableHinting ? FT_LOAD_DEFAULT : FT_LOAD_NO_HINTING);

		if (m_info.properties.hasColor)
		{
			::FT_Fixed advance = 0;

			if (::FT_Get_Advance(m_face, glyphIndex, loadFlag, &advance) == 0)
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
	//	getYAdvanceByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	Optional<float> FontFace::getYAdvanceByGlyphIndex(const GlyphIndex glyphIndex)
	{
		if (not m_face)
		{
			return none;
		}

		const ::hb_position_t vAdvance = ::hb_font_get_glyph_v_advance(m_hbObjects->hbFont, glyphIndex);
		return (-vAdvance / 64.0f);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getYAdvance
	//
	////////////////////////////////////////////////////////////////

	float FontFace::getYAdvance(const StringView ch)
	{
		if (not m_face)
		{
			return 0.0f;
		}

		const auto& hbObjects = *m_hbObjects;

		::hb_buffer_clear_contents(hbObjects.hbBuffer);
		::hb_buffer_set_direction(hbObjects.hbBuffer, HB_DIRECTION_TTB);
		::hb_buffer_set_script(hbObjects.hbBuffer, HB_SCRIPT_COMMON);

		const int32 textLength = static_cast<int32>(ch.length());
		::hb_buffer_add_utf32(hbObjects.hbBuffer, reinterpret_cast<const uint32_t*>(ch.data()), textLength, 0, textLength);
		::hb_buffer_guess_segment_properties(hbObjects.hbBuffer);

		::hb_shape(hbObjects.hbFont, hbObjects.hbBuffer, nullptr, 0);

		uint32 glyphCount = 0;
		const hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(hbObjects.hbBuffer, &glyphCount);

		if (glyphCount)
		{
			return (-glyphPos[0].y_advance / 64.0f);
		}
		else
		{
			return 0.0f;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphIndex FontFace::getGlyphIndex(const char32 codePoint)
	{
		if (not m_face)
		{
			return GlyphIndexNotdef;
		}

		return ::FT_Get_Char_Index(m_face, codePoint);
	}

	GlyphIndex FontFace::getGlyphIndex(const StringView ch, const ReadingDirection readingDirection)
	{
		if (not m_face)
		{
			return GlyphIndexNotdef;
		}

		if (const size_t length = ch.size();
			length == 0)
		{
			return GlyphIndexNotdef;
		}
		else
		{
			const HarfBuzzGlyphInfo glyphInfo = getHarfBuzzGlyphInfo(ch, EnableLigatures::Yes, readingDirection);

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
		if (not m_face)
		{
			return{};
		}

		char glyphNameBuffer[256]{};

		if (::FT_Get_Glyph_Name(m_face, glyphIndex, glyphNameBuffer, sizeof(glyphNameBuffer)) != 0)
		{
			return{};
		}

		return Unicode::FromUTF8(glyphNameBuffer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFace
	//
	////////////////////////////////////////////////////////////////

	::FT_Face FontFace::getFace() const noexcept
	{
		return m_face;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSkFont
	//
	////////////////////////////////////////////////////////////////

	SkFont* FontFace::getSkFont() const noexcept
	{
		if (m_colrv1)
		{
			return &m_colrv1->skFont;
		}
		else
		{
			return nullptr;
		}
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
