//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "IIcon.hpp"
# include <Siv3D/Font/FontUtility.hpp>
# if SIV3D_PLATFORM(WINDOWS) | SIV3D_PLATFORM(MACOS) | SIV3D_PLATFORM(WEB)
#	include <ThirdParty-prebuilt/harfbuzz/hb.h>
#	include <ThirdParty-prebuilt/harfbuzz/hb-ft.h>
# else
#	include <harfbuzz/hb.h>
#	include <harfbuzz/hb-ft.h>
# endif

namespace s3d
{
	class CIcon final : public ISiv3DIcon
	{
	public:

		~CIcon() override;

		void init() override;

		[[nodiscard]]
		bool hasGlyph(char32 icon) const override;

		[[nodiscard]]
		GlyphIndex getGlyphIndex(char32 icon) const override;

		[[nodiscard]]
		Image render(GlyphIndex glyphIndex, int32 size) override;

		[[nodiscard]]
		Image renderMSDF(GlyphIndex glyphIndex, int32 size, int32 bufferSize) override;

	private:

		bool m_available = false;

		FT_Face m_face = nullptr;

		hb_font_t* m_hbFont = nullptr;

		hb_buffer_t* m_hbBuffer = nullptr;

		FontFaceInfo m_faceInfo{};
	};
}
