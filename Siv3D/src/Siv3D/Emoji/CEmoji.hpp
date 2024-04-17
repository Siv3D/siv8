//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "IEmoji.hpp"
# if SIV3D_PLATFORM(WINDOWS) | SIV3D_PLATFORM(MACOS) | SIV3D_PLATFORM(WEB)
#	include <ThirdParty-prebuilt/harfbuzz/hb.h>
#	include <ThirdParty-prebuilt/harfbuzz/hb-ft.h>
# else
#	include <harfbuzz/hb.h>
#	include <harfbuzz/hb-ft.h>
# endif
# include <ThirdParty/skia/include/core/SkFont.h>
# include <ThirdParty/skia/include/core/SkStream.h>
# include <ThirdParty/skia/include/core/SkBitmap.h>
# include <ThirdParty/skia/src/ports/SkTypeface_FreeType.h>

namespace s3d
{
	class CEmoji final : public ISiv3DEmoji
	{
	public:

		~CEmoji() override;

		void init() override;

		bool hasEmoji(StringView emoji) const override;

		Image renderEmoji(StringView emoji, int32 size) override;

	private:

		FT_Library m_freeType = nullptr;

		FT_Face m_face = nullptr;

		hb_font_t* m_hbFont = nullptr;

		hb_buffer_t* m_hbBuffer = nullptr;

		std::unique_ptr<SkStreamAsset> m_fileStream;

		sk_sp<SkTypeface> m_typeface;

		SkFont m_font;
	};
}
