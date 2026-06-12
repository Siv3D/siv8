//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "Optional.hpp"
# include "Parse.hpp"
# include "Format.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	INIItem
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI のプロパティの値 | INI property value
	struct INIItem
	{
		/// @brief プロパティの値 | Property value
		String value;

		/// @brief セクション内でのインデックス（小さいほど上位） | Index in the section. Smaller values appear earlier.
		int32 index = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getString
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を文字列として取得します。 | Returns the value as a string.
		/// @return 値 | Value
		[[nodiscard]]
		const String& getString() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を UTF-8 文字列として取得します。 | Returns the value as a UTF-8 string.
		/// @return UTF-8 文字列 | UTF-8 string
		[[nodiscard]]
		std::string getUTF8() const;

		////////////////////////////////////////////////////////////////
		//
		//	get
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を Type 型として取得します。 | Returns the value as Type.
		/// @tparam Type 値の型 | Value type
		/// @return 値 | Value
		/// @throw Error 値を Type 型に変換できない場合 | Throws Error if the value cannot be converted to Type.
		template <class Type>
		[[nodiscard]]
		Type get() const;

		////////////////////////////////////////////////////////////////
		//
		//	getOr
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を Type 型として取得します。取得できなかった場合は defaultValue を返します。 | Returns the value as Type, or defaultValue if conversion fails.
		/// @tparam Type 値の型 | Value type
		/// @tparam U 取得できなかった場合に返す値の型 | Default value type
		/// @param defaultValue 取得できなかった場合に返す値 | Default value
		/// @return 値 | Value
		template <class Type, class U>
		[[nodiscard]]
		Type getOr(U&& defaultValue) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOpt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を Type 型として取得します。 | Returns the value as Type.
		/// @tparam Type 値の型 | Value type
		/// @return 値。取得できなかった場合は none | Value, or none if conversion fails
		template <class Type>
		[[nodiscard]]
		Optional<Type> getOpt() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		INIItem& operator =(const Concept::Formattable auto& newValue);
	};
}

# include "detail/INIItem.ipp"
