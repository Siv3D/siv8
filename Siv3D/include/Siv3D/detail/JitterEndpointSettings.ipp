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

namespace s3d
{
	constexpr JitterEndpointSettings JitterEndpointSettings::Free() noexcept
	{
		return{ .mode = JitterEndpointMode::Free, .taperRatio = 0.0 };
	}

	constexpr JitterEndpointSettings JitterEndpointSettings::Tapered(const double taperRatio) noexcept
	{
		return{ .mode = JitterEndpointMode::Tapered, .taperRatio = Clamp(taperRatio, 0.0, 0.5) };
	}

	constexpr JitterEndpointSettings JitterEndpointSettings::Locked() noexcept
	{
		return{ .mode = JitterEndpointMode::Locked, .taperRatio = 0.0 };
	}
}
