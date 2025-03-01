//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	inline IReader::operator bool() const noexcept
	{
		return isOpen();
	}

	////////////////////////////////////////////////////////////////
	//
	//	read
	//
	////////////////////////////////////////////////////////////////

	bool IReader::read(Concept::TriviallyCopyable auto& dst)
	{
		return (read(std::addressof(dst), sizeof(dst)) == sizeof(dst));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	lookahead
	//
	////////////////////////////////////////////////////////////////

	bool IReader::lookahead(Concept::TriviallyCopyable auto& dst) const
	{
		return (lookahead(std::addressof(dst), sizeof(dst)) == sizeof(dst));
	}
}
