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

# include <iostream>
# include <Siv3D/Error.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	Error::Error(const char32* message)
		: Error{ StringView{ message } } {}

	Error::Error(const StringView message)
		: std::exception{ Unicode::ToUTF8(message).c_str() } {}

	String Error::type() const noexcept
	{
		return U"Error";
	}

	String Error::message() const noexcept
	{
		return Unicode::FromUTF8(what());
	}

	std::ostream& operator <<(std::ostream& os, const Error& value)
	{
		return (os << '[' << value.type() << "] " << value.what());
	}

	std::wostream& operator <<(std::wostream& os, const Error& value)
	{
		return (os << L'[' << value.type() << L"] " << Unicode::ToWstring(value.what()));
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const Error& value)
	{
		return os << (U'[' + value.type() + U"] " + value.message());
	}

	void Formatter(FormatData& formatData, const Error& value)
	{
		formatData.string.append(U"["_sv);
		formatData.string.append(value.type());
		formatData.string.append(U"] "_sv);
		formatData.string.append(value.message());
	}

	const char* Error::what() const noexcept
	{
		return std::exception::what();
	}

	String ParseError::type() const noexcept
	{
		return U"ParseError";
	}

	String NotImplementedError::type() const noexcept
	{
		return U"NotImplementedError";
	}

	String EngineError::type() const noexcept
	{
		return U"EngineError";
	}
}
