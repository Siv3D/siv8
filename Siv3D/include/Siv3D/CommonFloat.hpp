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
# include "Concepts.hpp"

namespace s3d
{
	/// @brief 異なる数値型どうしの計算結果として使う浮動小数点数型
	template <Concept::Arithmetic T, Concept::Arithmetic U>
	struct CommonFloat
	{
		using type = std::conditional_t<(not std::is_floating_point_v<T>) && (not std::is_floating_point_v<U>), double, std::common_type_t<T, U>>;
	};

	/// @brief 異なる数値型どうしの計算結果として使う浮動小数点数型
	template <Concept::Arithmetic T, Concept::Arithmetic U>
	using CommonFloat_t = typename CommonFloat<T, U>::type;
}
