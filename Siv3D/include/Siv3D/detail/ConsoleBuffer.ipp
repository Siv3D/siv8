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
		inline ConsoleBuffer& ConsoleBuffer::operator <<(const Concept::Formattable auto& value)
		{
			Formatter(*formatData, value);
			return *this;
		}
	}
}