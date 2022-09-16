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
# include <memory>
# include "String.hpp"

namespace s3d
{
	namespace detail
	{
		struct ConsoleBuffer
		{
			std::unique_ptr<String> formatData;

			ConsoleBuffer();

			ConsoleBuffer(ConsoleBuffer&& other) noexcept;

			~ConsoleBuffer();

			ConsoleBuffer& operator <<(const char32* s);

			ConsoleBuffer& operator <<(StringView s);

			ConsoleBuffer& operator <<(const String& s);

			//SIV3D_CONCEPT_FORMATTABLE
			//ConsoleBuffer& operator <<(const Formattable& value)
			//{
			//	Formatter(*formatData, value);

			//	return *this;
			//}
		};
	}
}
