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
# include <Siv3D/StringView.hpp>
# include <Siv3D/FontStyle.hpp>
# include <Siv3D/FontFaceProperties.hpp>
# include "FontCommon.hpp"

namespace s3d
{
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

	private:

		::FT_Face m_face = nullptr;

		FontFaceProperties m_properties;

		int32 m_baseSize = 16;

		FontStyle m_style = FontStyle::Default;
	};
}
