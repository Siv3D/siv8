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
# include "FontCommon.hpp"

namespace s3d
{
	FontFaceProperties GetFontFaceProperties(const ::FT_Face face, const FT_Fixed* namedStyleCoords);
}
