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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	JSONValueType
	//
	////////////////////////////////////////////////////////////////

	/// @brief JSON が持つ要素の値の種類を表す列挙体
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

	void Formatter(FormatData& formatData, JSONValueType value);
}
