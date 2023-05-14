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
# include "Common.hpp"
# include "Error.hpp"
# include "Unicode.hpp"
# include "ParseBool.hpp"
# include "ParseInt.hpp"

namespace s3d
{
	/// @brief 文字列をパースしてデータ型に変換します。
	/// @tparam Type 変換先のデータ型
	/// @param s 変換する文字列
	/// @throw ParseError パースに失敗した場合
	/// @return 文字列から変換されたデータ
	template <class Type>
	[[nodiscard]]
	Type Parse(std::string_view s);

	/// @brief 文字列をパースしてデータ型に変換します。
	/// @tparam Type 変換先のデータ型
	/// @param s 変換する文字列
	/// @throw ParseError パースに失敗した場合
	/// @return 文字列から変換されたデータ
	template <class Type>
	[[nodiscard]]
	Type Parse(StringView s);

	/// @brief 文字列をパースしてデータ型に変換します。
	/// @tparam Type 変換先のデータ型
	/// @tparam U パースに失敗したときに代わりに返す値を構築する引数の型
	/// @param s 変換する文字列
	/// @param defaultValue パースに失敗したときに代わりに返す値を構築する引数
	/// @remark この関数はパースに失敗しても例外を投げません。
	/// @return 文字列から変換されたデータ、失敗した場合は defaultValue から構築した Type 型の値
	template <class Type, class U>
	[[nodiscard]]
	Type ParseOr(std::string_view s, U&& defaultValue);

	/// @brief 文字列をパースしてデータ型に変換します。
	/// @tparam Type 変換先のデータ型
	/// @tparam U パースに失敗したときに代わりに返す値を構築する引数の型
	/// @param s 変換する文字列
	/// @param defaultValue パースに失敗したときに代わりに返す値を構築する引数
	/// @remark この関数はパースに失敗しても例外を投げません。
	/// @return 文字列から変換されたデータ、失敗した場合は defaultValue から構築した Type 型の値
	template <class Type, class U>
	[[nodiscard]]
	Type ParseOr(StringView s, U&& defaultValue);

	/// @brief 文字列をパースしてデータ型に変換します。
	/// @tparam Type 変換先のデータ型
	/// @param s 変換する文字列
	/// @remark この関数はパースに失敗しても例外を投げません。
	/// @return 文字列から変換されたデータの Optional, 失敗した場合は none
	template <class Type>
	[[nodiscard]]
	Optional<Type> ParseOpt(std::string_view s) noexcept;

	/// @brief 文字列をパースしてデータ型に変換します。
	/// @tparam Type 変換先のデータ型
	/// @param s 変換する文字列
	/// @remark この関数はパースに失敗しても例外を投げません。
	/// @return 文字列から変換されたデータの Optional, 失敗した場合は none
	template <class Type>
	[[nodiscard]]
	Optional<Type> ParseOpt(StringView s) noexcept;

	/// @brief 文字列をパースしてデータ型に変換します。
	/// @tparam Type 変換先のデータ型
	/// @param s 変換する文字列
	/// @remark この関数はパースに失敗しても例外を投げません。
	/// @return 文字列から変換されたデータの Expected, 失敗した場合はエラーの詳細
	template <class Type>
	[[nodiscard]]
	Expected<Type, ParseErrorReason> ParseChecked(std::string_view s) noexcept;

	/// @brief 文字列をパースしてデータ型に変換します。
	/// @tparam Type 変換先のデータ型
	/// @param s 変換する文字列
	/// @remark この関数はパースに失敗しても例外を投げません。
	/// @return 文字列から変換されたデータの Expected, 失敗した場合はエラーの詳細
	template <class Type>
	[[nodiscard]]
	Expected<Type, ParseErrorReason> ParseChecked(StringView s) noexcept;
}

# include "detail/Parse.ipp"
