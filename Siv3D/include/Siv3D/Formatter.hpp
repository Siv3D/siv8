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
# include <compare>
# include "FormatData.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, bool value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, int8 value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, uint8 value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, int16 value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, uint16 value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, int32 value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, uint32 value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, long value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, unsigned long value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, long long value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, unsigned long long value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, float value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, double value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, long double value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param ch 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, char ch);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param ch 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, char8_t ch);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param ch 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, char16_t ch);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param ch 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, wchar_t ch);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param ch 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, char32_t ch);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, std::nullptr_t value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, const void* value);

	void Formatter(FormatData& formatData, const char*) = delete;

	void Formatter(FormatData& formatData, const char8_t*) = delete;

	void Formatter(FormatData& formatData, const char16_t*) = delete;

	void Formatter(FormatData& formatData, const wchar_t*) = delete;

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param s 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, const char32_t* s);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param s 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, const std::u32string& s);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, std::strong_ordering value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, std::weak_ordering value);

	/// @brief 値を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 値
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, std::partial_ordering value);

	////////////////////////////////////////////////////////////////
	//
	//	Concept::Formattable
	//
	////////////////////////////////////////////////////////////////

	namespace Concept
	{
		/// @brief 値がフォーマット可能であるかを表すコンセプト | Concept for formattable values
		/// @tparam Type 値の型 | Value type
		template <class Type>
		concept Formattable = requires (FormatData& formatData, Type value)
		{
			{ Formatter(formatData, value) } -> std::same_as<void>;
		};
	}
}
