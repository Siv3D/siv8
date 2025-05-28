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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	ReadingDirection
	//
	////////////////////////////////////////////////////////////////

	enum class ReadingDirection : uint8
	{
		LeftToRight,
		
		TopToBottom,

		RightToLeft,
	};

	void Formatter(FormatData& formatData, ReadingDirection value);
}
