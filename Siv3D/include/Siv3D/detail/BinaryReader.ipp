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
	//	supportsLookahead
	//
	////////////////////////////////////////////////////////////////

	constexpr bool BinaryReader::supportsLookahead() const noexcept
	{
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	read
	//
	////////////////////////////////////////////////////////////////

	bool BinaryReader::read(Concept::TriviallyCopyable auto& dst)
	{
		return (read(std::addressof(dst), sizeof(dst)) == sizeof(dst));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookahead
	//
	////////////////////////////////////////////////////////////////

	bool BinaryReader::lookahead(Concept::TriviallyCopyable auto& dst) const
	{
		return (lookahead(std::addressof(dst), sizeof(dst)) == sizeof(dst));
	}
}
