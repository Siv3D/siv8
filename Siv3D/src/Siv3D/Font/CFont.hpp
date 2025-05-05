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
# include "IFont.hpp"
# include <Siv3D/Font.hpp>
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>
# include "FontData.hpp"

# if SIV3D_PLATFORM(WINDOWS) | SIV3D_PLATFORM(MACOS)
#	include <ThirdParty-prebuilt/harfbuzz/hb.h>
#	include <ThirdParty-prebuilt/harfbuzz/hb-ft.h>
# else
#	include <harfbuzz/hb.h>
#	include <harfbuzz/hb-ft.h>
# endif

namespace s3d
{
	class CFont final : public ISiv3DFont
	{
	public:

		CFont();

		~CFont();

		void init() override;

	private:

		FT_Library m_freeType = nullptr;

		AssetHandleManager<Font::IDType, FontData> m_fonts{ "Font" };
	};
}
