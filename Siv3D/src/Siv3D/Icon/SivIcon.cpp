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

# include <Siv3D/Icon.hpp>
# include <Siv3D/Icon/IIcon.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	bool Icon::HasGlyph(const char32 code)
	{
		return SIV3D_ENGINE(Icon)->hasGlyph(code);
	}

	GlyphIndex Icon::GetGlyphIndex(const char32 code)
	{
		return SIV3D_ENGINE(Icon)->getGlyphIndex(code);
	}

	Image Icon::CreateImage(const char32 code, const int32 size)
	{
		return SIV3D_ENGINE(Icon)->render(code, size);
	}

	Image Icon::CreateMSDFImage(const char32 code, const int32 size, const int32 bufferSize)
	{
		return SIV3D_ENGINE(Icon)->renderMSDF(code, size, bufferSize);
	}
}
