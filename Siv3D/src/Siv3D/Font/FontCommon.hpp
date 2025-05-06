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
#	include <ThirdParty-prebuilt/freetype/freetype/ftsnames.h>
#	include <ThirdParty-prebuilt/freetype/freetype/ttnameid.h>
#	include <ThirdParty-prebuilt/freetype/freetype/ftmm.h>
# else
#	include <harfbuzz/hb.h>
#	include <harfbuzz/hb-ft.h>
#	include <freetype/freetype/ftsnames.h>
#	include <freetype/freetype/ttnameid.h>
#	include <freetype/freetype/ftmm.h>
# endif
