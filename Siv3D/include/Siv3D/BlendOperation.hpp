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

	/// @brief ブレンド操作
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

	void Formatter(FormatData& formatData, BlendOperation value);
}
