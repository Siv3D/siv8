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
	inline String Pad(const Type& value, std::pair<int32, char32> padding)
	{
		String result = Format(value);

		if (result.length() < padding.first)
		{
			result.insert(result.begin(), (padding.first - result.length()), padding.second);
		}

		return result;
	}
}
