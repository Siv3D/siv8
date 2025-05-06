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
# include <Siv3D/FontMethod.hpp>
# include <Siv3D/FontStyle.hpp>
# include "IFont.hpp"

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

namespace s3d
{
	class FontData
	{
	public:

		struct Null {};

		FontData() = default;

		explicit FontData(Null);

		FontData(::FT_Library library, FilePathView path, size_t faceIndex, StringView styleName, FontMethod fontMethod, int32 baseSize, FontStyle style);

		[[nodiscard]]
		bool isInitialized() const noexcept;

		[[nodiscard]]
		String toString() const;

	private:

		bool m_initialized = false;
	};
}
