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

# include <Siv3D/LoggerBuffer.hpp>
# include <Siv3D/Logger.hpp>

namespace s3d
{
	namespace detail
	{
		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		LoggerBuffer::LoggerBuffer()
			: formatData{ std::make_unique<FormatData>() } {}

		LoggerBuffer::LoggerBuffer(LoggerBuffer&& other) noexcept
			: formatData{ std::move(other.formatData) } {}

		LoggerBuffer::LoggerBuffer(const char32* s)
			: formatData{ std::make_unique<FormatData>(s) } {}

		LoggerBuffer::LoggerBuffer(const StringView s)
			: formatData{ std::make_unique<FormatData>(s) } {}

		LoggerBuffer::LoggerBuffer(const String& s)
			: formatData{ std::make_unique<FormatData>(s) } {}

		LoggerBuffer::LoggerBuffer(String&& s)
			: formatData{ std::make_unique<FormatData>(std::move(s)) } {}

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		LoggerBuffer::~LoggerBuffer()
		{
			// デストラクタで Logger に書き出す
			Logger.writeln(formatData->string);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		LoggerBuffer& LoggerBuffer::operator <<(const char32* s)
		{
			formatData->string.append(s);
			return *this;
		}

		LoggerBuffer& LoggerBuffer::operator <<(const StringView s)
		{
			formatData->string.append(s);
			return *this;
		}

		LoggerBuffer& LoggerBuffer::operator <<(const String& s)
		{
			formatData->string.append(s);
			return *this;
		}
	}
}
