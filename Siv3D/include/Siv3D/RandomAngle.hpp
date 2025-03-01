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
# include "Random.hpp"
# include "Duration.hpp"
# include "MathConstants.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RandomAngle
	//
	////////////////////////////////////////////////////////////////
	
	/// @brief 現在のスレッドの乱数エンジンを用いて、0 以上 2π 未満の範囲の角度を生成して返します。
	/// @return 生成されたランダムな角度
	[[nodiscard]]
	double RandomAngle() noexcept;

	/// @brief 0 以上 2π 未満の範囲の角度を生成して返します。
	/// @param urbg 乱数エンジン
	/// @return 生成されたランダムな角度
	[[nodiscard]]
	double RandomAngle(Concept::UniformRandomBitGenerator auto&& urbg) noexcept;
}

# include "detail/RandomAngle.ipp"
