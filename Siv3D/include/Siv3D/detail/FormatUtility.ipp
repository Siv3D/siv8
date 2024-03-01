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

namespace s3d
{
	namespace detail
	{
		String PadLeft(String&& s, const std::pair<int32, char32>& padding);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Pad
	//
	////////////////////////////////////////////////////////////////

	String Pad(const auto& value, const std::pair<int32, char32>& padding)
	{
		return detail::PadLeft(Format(value), padding);
	}
}
