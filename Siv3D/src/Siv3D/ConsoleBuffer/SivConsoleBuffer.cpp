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

# include <Siv3D/ConsoleBuffer.hpp>
# include <Siv3D/Console.hpp>

namespace s3d
{
	namespace detail
	{
		ConsoleBuffer::ConsoleBuffer()
			: formatData{ std::make_unique<String>() } {}

		ConsoleBuffer::ConsoleBuffer(ConsoleBuffer&& other) noexcept
			: formatData{ std::move(other.formatData) } {}

		ConsoleBuffer::~ConsoleBuffer()
		{
			if (formatData)
			{
				Console.writeln(*formatData);
			}
		}

		ConsoleBuffer& ConsoleBuffer::operator <<(const char32* s)
		{
			return operator <<(StringView{ s });
		}

		ConsoleBuffer& ConsoleBuffer::operator <<(const StringView s)
		{
			formatData->append(s);

			return *this;
		}

		ConsoleBuffer& ConsoleBuffer::operator <<(const String& s)
		{
			return operator <<(StringView{ s });
		}
	}
}
