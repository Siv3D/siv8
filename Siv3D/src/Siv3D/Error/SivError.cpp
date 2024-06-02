//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Error::Error(const char* message)
		: std::runtime_error{ message } {}

	Error::Error(const std::string_view message)
		: Error{ std::string{ message } } {}

	Error::Error(const std::string& message)
		: std::runtime_error{ message.c_str() } {}

	Error::Error(const char32* message)
		: Error{ StringView{ message } } {}

	Error::Error(const StringView message)
		: std::runtime_error{ Unicode::ToUTF8(message).c_str() } {}
		
	////////////////////////////////////////////////////////////////
	//
	//	type
	//
	////////////////////////////////////////////////////////////////

	String Error::type() const
	{
		return U"Error";
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	message
	//
	////////////////////////////////////////////////////////////////

	String Error::message() const
	{
		return Unicode::FromUTF8(what());
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	messageUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string Error::messageUTF8() const
	{
		return what();
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

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
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Error& value)
	{
		formatData.string.append(U"["_sv);
		formatData.string.append(value.type());
		formatData.string.append(U"] "_sv);
		formatData.string.append(value.message());
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	what
	//
	////////////////////////////////////////////////////////////////

	const char* Error::what() const noexcept
	{
		return std::runtime_error::what();
	}
}
