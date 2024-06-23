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
	//	CompareFunction
	//
	////////////////////////////////////////////////////////////////

	/// @brief 比較関数
	enum class CompareFunction : uint8
	{
		/// @brief 常にテストをパスしない
		Never			= 1,

		/// @brief 新しい値が現在の値未満の場合にテストをパスする
		Less			= 2,

		/// @brief 新しい値が現在の値と等しい場合にテストをパスする
		Equal			= 3,

		/// @brief 新しい値が現在の値以下の場合にテストをパスする
		LessEqual		= 4,

		/// @brief 新しい値が現在の値より大きい場合にテストをパスする
		Greater			= 5,

		/// @brief 新しい値が現在の値と等しくない場合にテストをパスする
		NotEqual		= 6,

		/// @brief 新しい値が現在の値以上の場合にテストをパスする
		GreaterEqual	= 7,

		/// @brief 常にテストをパスする
		Always			= 8,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, CompareFunction value);
}
