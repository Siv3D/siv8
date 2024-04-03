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

# include <Siv3D/MemoryViewReader.hpp>
# include <Siv3D/Error.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void MemoryViewReader::ThrowReadDstError()
	{
		throw Error{ "MemoryViewReader::read(): A non-null destination pointer is required when readSize is greater than 0." };
	}

	void MemoryViewReader::ThrowReadPosError()
	{
		throw Error{ "MemoryViewReader::read(): `pos` must be a non-negative value." };
	}

	void MemoryViewReader::ThrowLookaheadDstError()
	{
		throw Error{ "MemoryViewReader::lookahead(): A non-null destination pointer is required when readSize is greater than 0." };
	}

	void MemoryViewReader::ThrowLookaheadPosError()
	{
		throw Error{ "MemoryViewReader::lookahead(): `pos` must be a non-negative value." };
	}
}
