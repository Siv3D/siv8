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
	namespace detail
	{
		String Lpad(String&& s, const std::pair<int32, char32>& padding);

		String ThousandSeparateInt(const String& value, char32 separator);
	}

	inline String Pad(const auto& value, const std::pair<int32, char32>& padding)
	{
		return detail::Lpad(Format(value), padding);
	}

	inline String ThousandSeparate(const Concept::Integral auto value, const char32 separator)
	{
		return detail::ThousandSeparateInt(ToString(value), separator);
	}
}
