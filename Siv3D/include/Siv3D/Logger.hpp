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

# pragma once
# include <iostream>
# include "Common.hpp"
# include "String.hpp"
# include "LoggerBuffer.hpp"
# include "Formatter.hpp"
# include "Format.hpp"

namespace s3d
{
	enum class LogLevel : uint8;

	namespace detail
	{
		struct Logger_impl
		{
			////////////////////////////////////////////////////////////////
			//
			//	write
			//
			////////////////////////////////////////////////////////////////

			void write(const char* s) const;

			void write(std::string_view s) const;

			void write(const std::string& s) const;

			void write(const char32* s) const;

			void write(StringView s) const;

			void write(const String& s) const;

			template <Concept::Formattable... Args>
			void write(const Args&... args) const;

			// Format できない値を Logger.write() に渡すとコンパイルエラーになります。
			template <class... Args>
			void write(const Args&... args) const = delete;

			////////////////////////////////////////////////////////////////
			//
			//	writeln
			//
			////////////////////////////////////////////////////////////////

			void writeln(const char* s) const;

			void writeln(std::string_view s) const;

			void writeln(const std::string& s) const;

			void writeln(const char32* s) const;

			void writeln(StringView s) const;

			void writeln(const String& s) const;

			template <Concept::Formattable... Args>
			void writeln(const Args&... args) const;

			// Format できない値を Logger.writeln() に渡すとコンパイルエラーになります。
			template <class... Args>
			void writeln(const Args&... args) const = delete;

			////////////////////////////////////////////////////////////////
			//
			//	operator ()
			//
			////////////////////////////////////////////////////////////////

			void operator ()(const char32* s) const;

			void operator ()(StringView s) const;

			void operator ()(const String& s) const;

			template <Concept::Formattable... Args>
			void operator ()(const Args&... args) const;

			////////////////////////////////////////////////////////////////
			//
			//	operator <<
			//
			////////////////////////////////////////////////////////////////

			LoggerBuffer operator <<(const Concept::Formattable auto& value) const;

			// Format できない値を Logger に渡すとコンパイルエラーになります。
			template <class Type>
			LoggerBuffer operator <<(const Type& value) const = delete;

			LoggerBuffer operator <<(const char32* s) const;

			LoggerBuffer operator <<(StringView s) const;

			LoggerBuffer operator <<(const String& s) const;

			LoggerBuffer operator <<(String&& s) const;

			////////////////////////////////////////////////////////////////
			//
			//	setOutputLevel
			//
			////////////////////////////////////////////////////////////////

			void setOutputLevel(LogLevel logLevel) const noexcept;

			////////////////////////////////////////////////////////////////
			//
			//	getOutputLevel
			//
			////////////////////////////////////////////////////////////////

			[[nodiscard]]
			LogLevel getOutputLevel() const noexcept;
		};
	}

	inline namespace cpo
	{
		/// @brief ログ出力 | Logger
		inline constexpr detail::Logger_impl Logger;
	}
}

# include "detail/Logger.ipp"
