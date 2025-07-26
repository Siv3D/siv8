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

namespace s3d
{
	constexpr Vec2 GlyphInfo::getOffset(const double scale) const noexcept
	{
		return{ ((left - bufferThickness) * scale), ((ascender - top - bufferThickness) * scale) };
	}

	constexpr Vec2 GlyphInfo::getBase(const double scale) const noexcept
	{
		return{ ((left - bufferThickness) * scale), ((-top - bufferThickness) * scale) };
	}
}
