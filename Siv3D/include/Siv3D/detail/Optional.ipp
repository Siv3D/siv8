//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	template <class Type>
	inline void Formatter(FormatData& formatData, const Optional<Type>& value)
	{
		if (value)
		{
			formatData.string.append(U"(Optional)"_sv);
			Formatter(formatData, *value);
		}
		else
		{
			Formatter(formatData, none);
		}
	}
}
