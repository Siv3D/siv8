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
	//	BlendOperation
	//
	////////////////////////////////////////////////////////////////

	/// @brief ブレンド操作 | Blend operation
	enum class BlendOperation : uint8
	{
		/// @brief 加算
		/// @remark Result = (Source + Destination)
		Add				= 1,

		/// @brief 減算
		/// @remark Result = (Source - Destination)
		Subtract		= 2,

		/// @brief 逆減算
		/// @remark Result = (Destination - Source)
		ReverseSubtract	= 3,

		/// @brief 最小値
		/// @remark Result = Min(Source, Destination)
		Min				= 4,

		/// @brief 最大値
		/// @remark Result = Max(Source, Destination)
		Max				= 5,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief ブレンド操作を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value ブレンド操作
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, BlendOperation value);
}
