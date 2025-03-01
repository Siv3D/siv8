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
	namespace ImageProcessing
	{
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
