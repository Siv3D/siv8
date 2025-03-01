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
# include "IEmoji.hpp"
# if SIV3D_PLATFORM(WINDOWS) | SIV3D_PLATFORM(MACOS) | SIV3D_PLATFORM(WEB)
#	include <ThirdParty-prebuilt/harfbuzz/hb.h>
#	include <ThirdParty-prebuilt/harfbuzz/hb-ft.h>
# else
#	include <harfbuzz/hb.h>
#	include <harfbuzz/hb-ft.h>
# endif
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4244)
# include <ThirdParty/skia/include/core/SkFont.h>
# include <ThirdParty/skia/include/core/SkStream.h>
# include <ThirdParty/skia/include/core/SkBitmap.h>
# include <ThirdParty/skia/src/ports/SkTypeface_FreeType.h>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	class CEmoji final : public ISiv3DEmoji
	{
	public:

		~CEmoji() override;

		void init() override;

		bool hasEmoji(StringView emoji) const override;

		GlyphIndex getGlyphIndex(StringView emoji) const override;

		Image renderEmoji(GlyphIndex emoji, int32 size) override;

		Image renderFilledEmoji(GlyphIndex emoji, int32 size) override;

	private:

		bool m_available = false;

		FT_Library m_freeType = nullptr;

		FT_Face m_face = nullptr;

		hb_font_t* m_hbFont = nullptr;

		hb_buffer_t* m_hbBuffer = nullptr;

		sk_sp<SkTypeface> m_typeface;

		SkFont m_font;
	};
}
