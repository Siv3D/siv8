//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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
		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		ConsoleBuffer::ConsoleBuffer()
			: formatData{ std::make_unique<FormatData>() } {}

		ConsoleBuffer::ConsoleBuffer(ConsoleBuffer&& other) noexcept
			: formatData{ std::move(other.formatData) } {}

		ConsoleBuffer::ConsoleBuffer(const char32* s)
			: formatData{ std::make_unique<FormatData>(s) } {}

		ConsoleBuffer::ConsoleBuffer(const StringView s)
			: formatData{ std::make_unique<FormatData>(s) } {}

		ConsoleBuffer::ConsoleBuffer(const String& s)
			: formatData{ std::make_unique<FormatData>(s) } {}

		ConsoleBuffer::ConsoleBuffer(String&& s)
			: formatData{ std::make_unique<FormatData>(std::move(s)) } {}
			
		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		ConsoleBuffer::~ConsoleBuffer()
		{
			// デストラクタで Console に書き出す
			Console.writeln(formatData->string);
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		ConsoleBuffer& ConsoleBuffer::operator <<(const char32* s)
		{
			formatData->string.append(s);
			return *this;
		}

		ConsoleBuffer& ConsoleBuffer::operator <<(const StringView s)
		{
			formatData->string.append(s);
			return *this;
		}

		ConsoleBuffer& ConsoleBuffer::operator <<(const String& s)
		{
			formatData->string.append(s);
			return *this;
		}
	}
}
