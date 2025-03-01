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
		/// @brief プロパティの値
		String value;

		/// @brief セクション内でのインデックス（小さいほど上位）
		int32 index = 0;

		////////////////////////////////////////////////////////////////
		//
		//	get
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を Type 型として取得します。
		/// @tparam Type 値の型
		/// @return 値
		/// @throw Error 値を Type 型に変換できない場合
		template <class Type>
		[[nodiscard]]
		Type get() const;

		////////////////////////////////////////////////////////////////
		//
		//	getOr
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を Type 型として取得します。取得できなかった場合は defaultValue を返します。
		/// @tparam Type 値の型
		/// @tparam U 取得できなかった場合に返す値の型
		/// @param defaultValue 取得できなかった場合に返す値
		/// @return 値
		template <class Type, class U>
		[[nodiscard]]
		Type getOr(U&& defaultValue) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOpt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を Type 型として取得します。
		/// @tparam Type 値の型
		/// @return 値。取得できなかった場合は none
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
