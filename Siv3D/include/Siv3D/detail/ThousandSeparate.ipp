﻿//-----------------------------------------------
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
		String ThousandSeparateInt(const String& value, char32 separator);
	}

	inline String ThousandSeparate(const Concept::Integral auto value, const char32 separator)
	{
		return detail::ThousandSeparateInt(ToString(value), separator);
	}

	inline String ThousandSeparate(const Concept::FloatingPoint auto value, const char32 separator)
	{

	}
}