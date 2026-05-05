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
	////////////////////////////////////////////////////////////////
	//
	//	supportsLookahead
	//
	////////////////////////////////////////////////////////////////

	constexpr bool BinaryFileReader::supportsLookahead() const noexcept
	{
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	read
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::read(Concept::TriviallyCopyable auto& dst)
	{
		return (read(std::addressof(dst), sizeof(dst)) == sizeof(dst));
	}

	template <Concept::TriviallyCopyable Type>
	Optional<Type> BinaryFileReader::read()
	{
		Type value;
		
		if (read(value))
		{
			return value;
		}

		return none;
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookahead
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::lookahead(Concept::TriviallyCopyable auto& dst) const
	{
		return (lookahead(std::addressof(dst), sizeof(dst)) == sizeof(dst));
	}

	template <Concept::TriviallyCopyable Type>
	Optional<Type> BinaryFileReader::lookahead() const
	{
		Type value;

		if (lookahead(value))
		{
			return value;
		}

		return none;
	}
}
