//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	constexpr HardwareRNG::result_type HardwareRNG::min() noexcept
	{
		return engine_type::min();
	}

	constexpr HardwareRNG::result_type HardwareRNG::max() noexcept
	{
		return engine_type::max();
	}

	inline HardwareRNG::result_type HardwareRNG::operator()()
	{
		return m_engine();
	}
}
