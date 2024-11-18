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

# include "GeometryCommon.hpp"

namespace s3d
{
	namespace detail
	{
		size_t CalculateCircleQuality(const double r) noexcept
		{
			if (r <= 2.0)
			{
				return 6;
			}
			else if (r <= 4.0)
			{
				return 10;
			}
			else if (r < 8.0)
			{
				return 12;
			}
			else
			{
				return Min((12 + static_cast<int32>((r - 8.0) / 2.0)), 256);
			}
		}
	}
}
