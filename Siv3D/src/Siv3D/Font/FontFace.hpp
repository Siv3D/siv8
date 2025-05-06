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

# pragma once
# include <Siv3D/PredefinedYesNo.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FontStyle.hpp>
# include <Siv3D/FontFaceProperties.hpp>
# include "FontCommon.hpp"

namespace s3d
{
	struct HarfBuzzGlyphInfo
	{
		const hb_glyph_info_t* info = nullptr;

		size_t count = 0;
	};

	class FontFace
	{
	public:

		[[nodiscard]]
		FontFace() = default;

		~FontFace();

		[[nodiscard]]
		bool init(::FT_Library library, ::FT_Face face, StringView styleName, int32 baseSize, FontStyle style);

		[[nodiscard]]
		const FontFaceProperties& getProperties() const noexcept;

		[[nodiscard]]
		int32 getBaseSize() const noexcept;
		
		[[nodiscard]]
		FontStyle getStyle() const noexcept;

		[[nodiscard]]
		HarfBuzzGlyphInfo getHarfBuzzGlyphInfo(StringView s, Ligature ligature) const;

	private:

		struct HarfBuzzObjects
		{
			::hb_font_t* hbFont = nullptr;

			::hb_buffer_t* hbBuffer = nullptr;

			~HarfBuzzObjects();

			bool init(::FT_Face face);
		};

		::FT_Face m_face = nullptr;

		std::unique_ptr<HarfBuzzObjects> m_hbObjects;

		FontFaceProperties m_properties;

		int32 m_baseSize = 16;

		FontStyle m_style = FontStyle::Normal;

		int16 m_tabSize = 8;

		float m_ascender = 0.0f;

		float m_descender = 0.0f;

		float m_spaceWidth = 0.0f;
	};
}
