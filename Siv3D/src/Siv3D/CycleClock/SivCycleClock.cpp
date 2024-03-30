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

# include <Siv3D/CycleClock.hpp>
# include <Siv3D/Logger.hpp>
# include <Siv3D/Console.hpp>
# include <Siv3D/IntToString.hpp>
//# include <Siv3D/Print.hpp>

namespace s3d
{
	void CycleClock::log() const
	{
		const auto c = cycles();

		Logger.writeln(ToString(c).append(U" cycles"_sv));
	}

	void CycleClock::console() const
	{
		const auto c = cycles();

		Console.writeln(ToString(c).append(U" cycles"_sv));
	}

	//void CycleClock::print() const
	//{
	//	const auto c = cycles();

	//	Print.writeln(ToString(c).append(U" cycles"_sv));
	//}
}
