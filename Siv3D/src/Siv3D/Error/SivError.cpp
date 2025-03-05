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

# include <ostream>
# include <Siv3D/Error.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Formatter.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatLiteral.hpp>

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Error::Error(const char* message, const std::source_location& location)
		: std::runtime_error{ message }
		, m_sourceLocation{ location } {}

	Error::Error(const std::string_view message, const std::source_location& location)
		: Error{ std::string{ message }, location } {}

	Error::Error(const std::string& message, const std::source_location& location)
		: std::runtime_error{ message.c_str() }
		, m_sourceLocation{ location } {}

	Error::Error(const StringView message, const std::source_location& location)
		: std::runtime_error{ Unicode::ToUTF8(message).c_str() }
		, m_sourceLocation{ location } {}
		
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
	//	location
	//
	////////////////////////////////////////////////////////////////

	const std::source_location& Error::location() const noexcept
	{
		return m_sourceLocation;
	}

	////////////////////////////////////////////////////////////////
	//
	//	locationString
	//
	////////////////////////////////////////////////////////////////

	String Error::locationString() const noexcept
	{
		const FilePath path = Unicode::FromUTF8(m_sourceLocation.file_name());
		const FilePath fileName = FileSystem::FileName(path);
		const int32 line = m_sourceLocation.line();
		const int32 column = m_sourceLocation.column();
		const String functionName = Unicode::FromUTF8(m_sourceLocation.function_name());
		return U"{}({},{}) {}"_fmt(fileName, line, column, functionName);
	}

	////////////////////////////////////////////////////////////////
	//
	//	locationStringUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string Error::locationStringUTF8() const noexcept
	{
		const FilePath path = Unicode::FromUTF8(m_sourceLocation.file_name());
		const FilePath fileName = FileSystem::FileName(path);
		const int32 line = m_sourceLocation.line();
		const int32 column = m_sourceLocation.column();
		return fmt::format("{}({},{}) {}", fileName, line, column, m_sourceLocation.function_name());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& output, const Error& value)
	{
		return (output << '[' << value.type() << "] " << value.what());
	}

	std::wostream& operator <<(std::wostream& output, const Error& value)
	{
		return (output << L'[' << value.type() << L"] " << Unicode::ToWstring(value.what()));
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const Error& value)
	{
		return output << (U'[' + value.type() + U"] " + value.message());
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Error& value)
	{
		formatData.string.append(value.locationString());
		formatData.string.append(U"\n["_sv);
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
