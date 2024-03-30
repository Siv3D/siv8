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
# include "ConsoleBuffer.hpp"
# include "Formatter.hpp"
# include "Format.hpp"

namespace s3d
{
	namespace detail
	{
		struct Console_impl
		{
			////////////////////////////////////////////////////////////////
			//
			//	open
			//
			////////////////////////////////////////////////////////////////

			/// @brief コンソールウィンドウを開きます。
			/// @remark Windows 以外では何もしません。
			void open() const;

			////////////////////////////////////////////////////////////////
			//
			//	write
			//
			////////////////////////////////////////////////////////////////

			/// @brief 文字列をコンソールに出力します。
			/// @param s 出力する文字列
			void write(const char32* s) const;

			/// @brief 文字列をコンソールに出力します。
			/// @param s 出力する文字列
			void write(StringView s) const;

			/// @brief 文字列をコンソールに出力します。
			/// @param s 出力する文字列
			void write(const String& s) const;

			/// @brief 値をコンソールに出力します。
			/// @tparam ...Args 値の型
			/// @param ...args 出力する値
			template <Concept::Formattable... Args>
			void write(const Args&... args) const;

			// Format できない値を Console.write() に渡すとコンパイルエラーになります。
			template <class... Args>
			void write(const Args&... args) const = delete;

			////////////////////////////////////////////////////////////////
			//
			//	writeln
			//
			////////////////////////////////////////////////////////////////

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			void writeln(const char32* s) const;

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			void writeln(StringView s) const;

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			void writeln(const String& s) const;

			template <Concept::Formattable... Args>
			void writeln(const Args&... args) const;

			// Format できない値を Console.writeln() に渡すとコンパイルエラーになります。
			template <class... Args>
			void writeln(const Args&... args) const = delete;

			////////////////////////////////////////////////////////////////
			//
			//	operator ()
			//
			////////////////////////////////////////////////////////////////

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			void operator ()(const char32* s) const;

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			void operator ()(StringView s) const;

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			void operator ()(const String& s) const;

			template <Concept::Formattable... Args>
			void operator ()(const Args&... args) const;

			////////////////////////////////////////////////////////////////
			//
			//	operator <<
			//
			////////////////////////////////////////////////////////////////

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			/// @return ConsoleBuffer
			ConsoleBuffer operator <<(const char32* s) const;

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			/// @return ConsoleBuffer
			ConsoleBuffer operator <<(StringView s) const;

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			/// @return ConsoleBuffer
			ConsoleBuffer operator <<(const String& s) const;

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			/// @return ConsoleBuffer
			ConsoleBuffer operator <<(String&& s) const;

			/// @brief 値をコンソールに出力し、改行します。
			/// @param value 出力する値
			/// @return ConsoleBuffer
			ConsoleBuffer operator <<(const Concept::Formattable auto& value) const;

			/// @brief Format できない値を Console に渡すとコンパイルエラーになります。
			template <class Type>
			ConsoleBuffer operator <<(const Type& value) const = delete;

			////////////////////////////////////////////////////////////////
			//
			//	read
			//
			////////////////////////////////////////////////////////////////

			/// @brief コンソールから入力を読み取ります。
			/// @tparam Type 読み取る値の型
			/// @return 読み取った値
			template <class Type>
			Type read() const;

			////////////////////////////////////////////////////////////////
			//
			//	readLine
			//
			////////////////////////////////////////////////////////////////
			
			/// @brief コンソールから一行分の入力を読み取ります。
			/// @tparam Type 読み取る値の型
			/// @return 読み取った値
			template <class Type>
			Type readLine() const;

			////////////////////////////////////////////////////////////////
			//
			//	operator >>
			//
			////////////////////////////////////////////////////////////////

			/// @brief コンソールから入力を読み取ります。
			/// @tparam Type 読み取る値の型
			/// @param value 格納先の変数
			/// @return *this
			template <class Type>
			const Console_impl& operator >>(Type& value) const;
		};
	}

	inline namespace cpo
	{
		/// @brief コンソール入出力 | Console I/O
		inline constexpr detail::Console_impl Console;
	}
}

# include "detail/Console.ipp"
