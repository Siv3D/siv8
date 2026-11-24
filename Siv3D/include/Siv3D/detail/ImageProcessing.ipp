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
# include <bit>

namespace s3d
{
	namespace ImageProcessing
	{
		constexpr Size CalculateMipmapSize(const Size baseSize, const uint32 level) noexcept
		{
			if ((baseSize.x <= 0) || (baseSize.y <= 0)
				|| (static_cast<uint32>(std::bit_width(static_cast<uint32>((baseSize.x < baseSize.y) ? baseSize.y : baseSize.x))) <= level))
			{
				return{ 0, 0 };
			}
			const int32 width = (baseSize.x >> level);
			const int32 height = (baseSize.y >> level);
			return{ (width ? width : 1), (height ? height : 1) };
		}

		constexpr uint8 CalculateMipmapLevel(const size_t width, const size_t height) noexcept
		{
			if ((width == 0) || (height == 0))
			{
				return 0;
			}

			uint8 levels = 1;

			while ((width | height) >> levels)
			{
				++levels;
			}

			return levels;
		}
	}
}
