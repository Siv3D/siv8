//-----------------------------------------------
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

namespace s3d
{
	template <class DurationTo, class DurationFrom>
	constexpr DurationTo DurationCast(const DurationFrom& duration) noexcept
	{
		return std::chrono::duration_cast<DurationTo>(duration);
	}

	namespace Literals
	{
		namespace DurationLiterals
		{
			constexpr Days operator ""_d(const unsigned long long days)
			{
				return Days(days);
			}

			constexpr DaysF operator ""_d(const long double days)
			{
				return DaysF(days);
			}
		}
	}
}
