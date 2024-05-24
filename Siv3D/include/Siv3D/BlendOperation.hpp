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
	//	BlendOperation
	//
	////////////////////////////////////////////////////////////////

	enum class BlendOperation : uint8
	{
		Add				= 1,

		Subtract		= 2,

		ReverseSubtract	= 3,

		Min				= 4,

		Max				= 5,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, BlendOperation value);
}
