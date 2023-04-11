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
# include <iostream>
# include "ConsoleBuffer.hpp"

namespace s3d
{
	namespace detail
	{
		struct Console_impl
		{
			void open() const;

			void write(const char32* s) const;

			void write(StringView s) const;

			void write(const String& s) const;

			void writeln(const char32* s) const;

			void writeln(StringView s) const;

			void writeln(const String& s) const;

			void operator ()(const char32* s) const;

			void operator ()(StringView s) const;

			void operator ()(const String& s) const;

			template <Concept::Formattable... Args>
			void write(const Args&... args) const;

			// Format できない値が Console.write() に渡されたときに発生するエラーです
			template <class... Args>
			void write(const Args&... args) const = delete;

			template <Concept::Formattable... Args>
			void writeln(const Args&... args) const;

			// Format できない値が Console.writeln() に渡されたときに発生するエラーです
			template <class... Args>
			void writeln(const Args&... args) const = delete;

			template <Concept::Formattable... Args>
			void operator ()(const Args&... args) const;

			// Format できない値が Console() に渡されたときに発生するエラーです
			template <class... Args>
			void operator ()(const Args&... args) const = delete;

			template <Concept::Formattable Formattable>
			ConsoleBuffer operator <<(const Formattable& value) const;

			ConsoleBuffer operator <<(const char32* s) const;

			ConsoleBuffer operator <<(StringView s) const;

			ConsoleBuffer operator <<(const String& s) const;

			ConsoleBuffer operator <<(String&& s) const;

			//template <class Type>
			//Type read() const;

			////template <class Type>
			////Type readLine() const;

			//template <class Type>
			//const Console_impl& operator >>(Type& value) const;
		};
	}

	inline namespace cpo
	{
		/// @brief コンソール入出力 | Console I/O
		inline constexpr detail::Console_impl Console;
	}
}

# include "detail/Console.ipp"
