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

	////////////////////////////////////////////////////////////////
	//
	//	lookahead
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::lookahead(Concept::TriviallyCopyable auto& dst) const
	{
		return (lookahead(std::addressof(dst), sizeof(dst)) == sizeof(dst));
	}
}
