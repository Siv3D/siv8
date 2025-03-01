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
# include <memory>
# include "FormatData.hpp"
# include "Formatter.hpp"

namespace s3d
{
	namespace detail
	{
		struct ConsoleBuffer
		{
			std::unique_ptr<FormatData> formatData;

			////////////////////////////////////////////////////////////////
			//
			//	(constructor)
			//
			////////////////////////////////////////////////////////////////

			ConsoleBuffer();

			ConsoleBuffer(ConsoleBuffer&& other) noexcept;

			explicit ConsoleBuffer(const char32* s);

			explicit ConsoleBuffer(StringView s);

			explicit ConsoleBuffer(const String& s);

			explicit ConsoleBuffer(String&& s);

			////////////////////////////////////////////////////////////////
			//
			//	(destructor)
			//
			////////////////////////////////////////////////////////////////

			~ConsoleBuffer();

			////////////////////////////////////////////////////////////////
			//
			//	operator <<
			//
			////////////////////////////////////////////////////////////////

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			/// @return *this
			ConsoleBuffer& operator <<(const char32* s);

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			/// @return *this
			ConsoleBuffer& operator <<(StringView s);

			/// @brief 文字列をコンソールに出力し、改行します。
			/// @param s 出力する文字列
			/// @return *this
			ConsoleBuffer& operator <<(const String& s);

			/// @brief 値をコンソールに出力し、改行します。
			/// @param value 出力する値
			/// @return *this
			ConsoleBuffer& operator <<(const Concept::Formattable auto& value);

			/// @brief Format できない値を Console に渡すとコンパイルエラーになります。
			template <class Type>
			ConsoleBuffer& operator <<(const Type& value) = delete;
		};
	}
}

# include "detail/ConsoleBuffer.ipp"
