﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "Time.hpp"

namespace s3d
{
	class ISteadyClock
	{
	public:

		virtual ~ISteadyClock() = default;

		virtual uint64 getMicrosec() noexcept = 0;

		virtual uint64 getNanosec() noexcept;

		static uint64 GetMicrosec(ISteadyClock* pSteadyClock) noexcept;

		static uint64 GetNanosec(ISteadyClock* pSteadyClock) noexcept;
	};
}

# include "detail/ISteadyClock.ipp"
