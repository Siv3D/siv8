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
	inline Serial::Config::Config() = default;

	constexpr Serial::Timeout Serial::Timeout::Blocking()
	{
		return{ 0, 0, 0, 0, 0 };
	}

	constexpr Serial::Timeout Serial::Timeout::NonBlocking()
	{
		return{ Max, 0, 0, 0, 0 };
	}

	constexpr Serial::Timeout Serial::Timeout::Fixed(uint32 timeout)
	{
		return{ 0, timeout, 0, timeout, 0 };
	}

	bool Serial::read(Concept::TriviallyCopyable auto& to)
	{
		if (available() < sizeof(to))
		{
			return false;
		}

		return (read(std::addressof(to), sizeof(to)) == sizeof(to));
	}

	bool Serial::write(const Concept::TriviallyCopyable auto& from)
	{
		return (write(std::addressof(from), sizeof(from))) == sizeof(from);
	}
}
