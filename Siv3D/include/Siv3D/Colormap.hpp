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
# include "ColorHSV.hpp"
# include "ColorScale.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ColorMap
	//
	////////////////////////////////////////////////////////////////

	/// @brief 0.0 から 1.0 の範囲でのカラーマップの色を返します。
	/// @param t 0.0 から 1.0 の範囲の値
	/// @param colorScale 使用するカラースケールの種類
	/// @return カラーマップの色
	[[nodiscard]]
	ColorF Colormap(double t, ColorScale colorScale = ColorScale::Turbo);

	/// @brief min から max の範囲でのカラーマップの色を返します。
	/// @param t min から max の範囲の値
	/// @param min 最小値
	/// @param max 最大値
	/// @param colorScale 使用するカラースケールの種類
	/// @return カラーマップの色
	[[nodiscard]]
	ColorF Colormap(double t, double min, double max, ColorScale colorScale = ColorScale::Turbo);
}
