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
# include "Common.hpp"

namespace s3d
{
	struct FormatData;

	/// @brief 2 次元のベクトル（整数）
	struct Point
	{
		/// @brief ベクトルの要素の型
		using value_type = int32;

		/// @brief ベクトルの次元数
		static constexpr size_t Dimension = 2;

		/// @brief ベクトルの X 成分
		value_type x;

		/// @brief ベクトルの Y 成分
		value_type y;










		friend void Formatter(FormatData& formatData, const Point& value);
	};

	using Size = Point;
}
