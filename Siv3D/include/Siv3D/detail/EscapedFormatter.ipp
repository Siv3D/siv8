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
	//	DebugFormatter
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Formattable Type>
	void DebugFormatter(FormatData& formatData, const Type& value)
	{
		if constexpr (Concept::Escapable<Type>)
		{
			EscapedFormatter(formatData, value);
		}
		else
		{
			Formatter(formatData, value);
		}
	}
}
