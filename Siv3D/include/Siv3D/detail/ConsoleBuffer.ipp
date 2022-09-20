//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	namespace detail
	{
		template <Concept::Formattable Formattable>
		inline ConsoleBuffer& ConsoleBuffer::operator <<(const Formattable& value)
		{
			Formatter(*formatData, value);
			return *this;
		}
	}
}
