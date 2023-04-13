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
# include <memory>
# include "FormatData.hpp"
# include "Formatter.hpp"


namespace s3d
{
	namespace detail
	{
		struct ConsoleBuffer
		{
			std::unique_ptr<FormatData> formatData;

			ConsoleBuffer();

			ConsoleBuffer(ConsoleBuffer&& other) noexcept;

			explicit ConsoleBuffer(const char32* s);

			explicit ConsoleBuffer(StringView s);

			explicit ConsoleBuffer(const String& s);

			explicit ConsoleBuffer(String&& s);

			~ConsoleBuffer();

			ConsoleBuffer& operator <<(const char32* s);

			ConsoleBuffer& operator <<(StringView s);

			ConsoleBuffer& operator <<(const String& s);

			ConsoleBuffer& operator <<(const Concept::Formattable auto& value);
		};
	}
}

# include "detail/ConsoleBuffer.ipp"
