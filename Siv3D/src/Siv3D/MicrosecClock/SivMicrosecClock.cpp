﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/MicrosecClock.hpp>
# include <Siv3D/Duration.hpp>
# include <Siv3D/Logger.hpp>
# include <Siv3D/Console.hpp>
//# include <Siv3D/Print.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	log
	//
	////////////////////////////////////////////////////////////////

	void MicrosecClock::log() const
	{
		Logger(Microseconds{ us() });
	}

	////////////////////////////////////////////////////////////////
	//
	//	console
	//
	////////////////////////////////////////////////////////////////

	void MicrosecClock::console() const
	{
		Console(Microseconds{ us() });
	}

	////////////////////////////////////////////////////////////////
	//
	//	print
	//
	////////////////////////////////////////////////////////////////

	//void MicrosecClock::print() const
	//{
	//	Print(Microseconds{ us() });
	//}
}
