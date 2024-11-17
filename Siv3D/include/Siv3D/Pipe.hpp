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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Pipe
	//
	////////////////////////////////////////////////////////////////

	enum class Pipe : uint8
	{
		None,

		StdIn,

		StdOut,

		StdInOut,
	};

	void Formatter(FormatData& formatData, Pipe value);
}
