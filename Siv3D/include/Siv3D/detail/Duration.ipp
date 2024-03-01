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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DurationCast
	//
	////////////////////////////////////////////////////////////////

	template <class DurationTo, class DurationFrom>
	constexpr DurationTo DurationCast(const DurationFrom& duration) noexcept
	{
		return std::chrono::duration_cast<DurationTo>(duration);
	}

	inline namespace Literals
	{
		inline namespace DurationLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_d
			//
			////////////////////////////////////////////////////////////////

			constexpr Days operator ""_d(const unsigned long long days)
			{
				return Days{ static_cast<int>(days) };
			}

			constexpr DaysF operator ""_d(const long double days)
			{
				return DaysF{ static_cast<double>(days) };
			}
		}
	}
}
