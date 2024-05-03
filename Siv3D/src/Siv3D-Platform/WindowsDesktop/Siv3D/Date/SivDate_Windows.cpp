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

# include <Siv3D/Common.hpp>
# include <Siv3D/Date.hpp>
# include <Siv3D/Windows/MinWindows.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Today
	//
	////////////////////////////////////////////////////////////////

	Date Date::Today() noexcept
	{
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);
		return{ sysTime.wYear, sysTime.wMonth, sysTime.wDay };
	}
}
