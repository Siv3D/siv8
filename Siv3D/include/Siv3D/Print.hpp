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

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "PrintBuffer.hpp"
# include "Formatter.hpp"
# include "Format.hpp"

namespace s3d
{
	namespace detail
	{
		struct Print_impl
		{
			////////////////////////////////////////////////////////////////
			//
			//	write
			//
			////////////////////////////////////////////////////////////////

			void write(const char32_t* s) const;

			void write(StringView s) const;

			void write(const String& s) const;

			template <Concept::Formattable... Args>
			void write(const Args&... args) const;

			// Format できない値を Print.write() に渡すとコンパイルエラーになります。
			template <class... Args>
			void write(const Args&... args) const = delete;

			////////////////////////////////////////////////////////////////
			//
			//	writeln
			//
			////////////////////////////////////////////////////////////////

			void writeln(const char32* s) const;

			void writeln(StringView s) const;

			void writeln(const String& s) const;

			template <Concept::Formattable... Args>
			void writeln(const Args&... args) const;

			// Format できない値を Print.writeln() に渡すとコンパイルエラーになります。
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

			PrintBuffer operator <<(const Concept::Formattable auto& value) const;

			/// @brief Format できない値を Print に渡すとコンパイルエラーになります。
			template <class Type>
			PrintBuffer operator <<(const Type& value) const = delete;

			PrintBuffer operator <<(const char32* s) const;

			PrintBuffer operator <<(StringView s) const;

			PrintBuffer operator <<(const String& s) const;

			PrintBuffer operator <<(String&& s) const;
		};
	}

	void ClearPrint();

	inline namespace cpo
	{
		////////////////////////////////////////////////////////////////
		//
		//	Print
		//
		////////////////////////////////////////////////////////////////

		inline constexpr detail::Print_impl Print;
	}
}

# include "detail/Print.ipp"
