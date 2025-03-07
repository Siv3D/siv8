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
# include "FormatData.hpp"
# include "Formatter.hpp"

namespace s3d
{
	namespace detail
	{
		struct Format_impl
		{
		public:

			////////////////////////////////////////////////////////////////
			//
			//	operator ()
			//
			////////////////////////////////////////////////////////////////

			/// @brief 一連の引数を文字列に変換します。
			/// @param ...args 変換する値
			/// @return 引数を文字列に変換して連結した文字列
			template <Concept::Formattable... Args>
			[[nodiscard]]
			static String operator ()(const Args&... args);

			/// @brief Format できない値が Format() に渡されたときに発生するエラーです
			template <class... Args>
			static String operator ()(const Args&...) = delete;

			/// @brief 引数を文字列に変換します。
			/// @param ch 変換する値
			/// @return 引数から変換された文字列	
			[[nodiscard]]
			static String operator ()(char32 ch);

			/// @brief 引数を文字列に変換します。
			/// @param s 変換する値
			/// @return 引数から変換された文字列	
			[[nodiscard]]
			static String operator ()(const char32* s) noexcept;

			/// @brief 引数を文字列に変換します。
			/// @param s 変換する値
			/// @return 引数から変換された文字列	
			[[nodiscard]]
			static String operator ()(StringView s) noexcept;

			/// @brief 引数を文字列に変換します。
			/// @param s 変換する値
			/// @return 引数から変換された文字列	
			[[nodiscard]]
			static String operator ()(const String& s);

			/// @brief 引数を文字列に変換します。
			/// @param s 変換する値
			/// @return 引数から変換された文字列	
			[[nodiscard]]
			static String operator ()(String&& s) noexcept;

		private:

			static constexpr void Apply(const FormatData&) noexcept;

			template <class Type, class... Args>
			static void Apply(FormatData& formatData, const Type& value, const Args&... args);
		};
	}

	inline namespace cpo
	{
		////////////////////////////////////////////////////////////////
		//
		//	Format
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列フォーマット
		inline constexpr detail::Format_impl Format;
	}
}

# include "detail/Format.ipp"
