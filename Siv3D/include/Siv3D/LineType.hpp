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
# include "Common.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	LineType
	//
	////////////////////////////////////////////////////////////////

	enum class LineType : uint8
	{
		Solid,

		Dotted,

		Dashed,

		LongDash,

		DashDot,

		RoundDot,
	};

	////////////////////////////////////////////////////////////////
	//
	//	LineType
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, LineType value);
}
