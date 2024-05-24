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
	//	CompareFunction
	//
	////////////////////////////////////////////////////////////////

	enum class CompareFunction : uint8
	{
		Never			= 1,

		Less			= 2,

		Equal			= 3,

		LessEqual		= 4,

		Greater			= 5,

		NotEqual		= 6,

		GreaterEqual	= 7,

		Always			= 8,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, CompareFunction value);
}
