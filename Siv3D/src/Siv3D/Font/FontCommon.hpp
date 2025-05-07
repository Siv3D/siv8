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
# if SIV3D_PLATFORM(WINDOWS) | SIV3D_PLATFORM(MACOS)
#	include <ThirdParty-prebuilt/harfbuzz/hb.h>
#	include <ThirdParty-prebuilt/harfbuzz/hb-ft.h>
# else
#	include <harfbuzz/hb.h>
#	include <harfbuzz/hb-ft.h>
# endif

extern "C"
{
# include <ft2build.h>
# include FT_FREETYPE_H
# include FT_SYNTHESIS_H
# include FT_MULTIPLE_MASTERS_H
# include FT_SFNT_NAMES_H
# include FT_TRUETYPE_IDS_H
}
