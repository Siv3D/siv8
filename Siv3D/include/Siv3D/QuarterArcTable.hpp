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
# include <span>
# include "Common.hpp"
# include "PointVector.hpp"

namespace s3d
{
	namespace QuarterArcTable
	{
		inline constexpr uint32 MinQuality = 1;

		inline constexpr uint32 MaxQuality = 63;

		[[nodiscard]]
		std::span<const Float2> GetUnitVectors(uint32 quality) noexcept;
	}
}
