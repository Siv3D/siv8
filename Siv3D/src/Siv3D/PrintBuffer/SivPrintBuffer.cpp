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

# include <Siv3D/PrintBuffer.hpp>
# include <Siv3D/Print.hpp>

namespace s3d
{
	namespace detail
	{
		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		PrintBuffer::PrintBuffer()
			: formatData{ std::make_unique<FormatData>() } {}

		PrintBuffer::PrintBuffer(PrintBuffer&& other) noexcept
			: formatData{ std::move(other.formatData) } {}

		PrintBuffer::PrintBuffer(const char32* s)
			: formatData{ std::make_unique<FormatData>(s) } {}

		PrintBuffer::PrintBuffer(const StringView s)
			: formatData{ std::make_unique<FormatData>(s) } {}

		PrintBuffer::PrintBuffer(const String& s)
			: formatData{ std::make_unique<FormatData>(s) } {}

		PrintBuffer::PrintBuffer(String&& s)
			: formatData{ std::make_unique<FormatData>(std::move(s)) } {}

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		PrintBuffer::~PrintBuffer()
		{
			// デストラクタで Print に書き出す
			Print.writeln(formatData->string);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		PrintBuffer& PrintBuffer::operator <<(const char32* s)
		{
			formatData->string.append(s);
			return *this;
		}

		PrintBuffer& PrintBuffer::operator <<(const StringView s)
		{
			formatData->string.append(s);
			return *this;
		}

		PrintBuffer& PrintBuffer::operator <<(const String& s)
		{
			formatData->string.append(s);
			return *this;
		}
	}
}
