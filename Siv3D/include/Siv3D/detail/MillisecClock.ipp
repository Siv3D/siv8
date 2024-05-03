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
	//	ms
	//
	////////////////////////////////////////////////////////////////

	inline int64 MillisecClock::ms() const noexcept
	{
		return (Time::GetMillisec() - m_start);
	}
}
