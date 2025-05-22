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
# include <Siv3D/FontFaceProperties.hpp>
# include <Siv3D/GlyphInfo.hpp>
# include <Siv3D/FontMethod.hpp>
# include <Siv3D/FontStyle.hpp>
# include "FontCommon.hpp"

namespace s3d
{
	struct FontFaceInfo
	{
		FontFaceProperties properties;

		int16 baseSize = 16;

		FontMethod renderingMethod = FontMethod::Bitmap;

		EnableHinting hinting = EnableHinting::Yes;

		FontStyle style = FontStyle::Normal;

		int16 tabSize = 8;

		float ascender = 0.0f;

		float descender = 0.0f;

		float spaceXAdvance = 0.0f;

		float spaceYAdvance = 0.0f;
	};

	////////////////////////////////////////////////////////////////
	//
	//	GetFontFaceProperties
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	FontFaceProperties GetFontFaceProperties(::FT_Face face, const FT_Fixed* namedStyleCoords);

	////////////////////////////////////////////////////////////////
	//
	//	GetGlyphInfo
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	GlyphInfo GetGlyphInfo(::FT_Face face, GlyphIndex glyphIndex, const FontFaceInfo& info, int16 bufferThickness);
}
