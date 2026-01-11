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
# include "Common.hpp"
# include "MultiPolygon.hpp"
# include "GlyphInfo.hpp"

namespace s3d
{
	struct PolygonGlyph : GlyphInfo
	{
		MultiPolygon polygons;
	};
}
