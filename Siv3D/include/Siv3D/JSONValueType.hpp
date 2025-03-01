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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	JSONValueType
	//
	////////////////////////////////////////////////////////////////

	/// @brief JSON の要素の型 | JSON value type
	enum class JSONValueType : uint8
	{
		Null,

		Object,

		Array,

		String,

		Bool,

		NumberInt,

		NumberUint,

		NumberFloat,

		Binary,

		Invalid,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief JSON の要素の型を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value JSON の要素の型
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, JSONValueType value);
}
