//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "Common.hpp"

namespace s3d
{
	/// @brief Catmull–Rom 曲線を通過点の間でどのように曲げるかを指定します。
	enum class CatmullRomParameterization : uint8
	{
		/// @brief 点の間隔を考慮します。間隔が不均一な経路の作成に適しています。
		/// @remark 各区間内の不要なループを避ける方式です。離れた区間同士が交差することはあります。
		Centripetal,

		/// @brief 点の間隔によらず、前後の点から接線を求めます。
		/// @remark Spline::CatmullRom() と同じ補間方式です。点の間隔によっては大きく膨らんだり、ループしたりします。
		Uniform,
	};
}
