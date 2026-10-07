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
	/// @brief スプライン曲線上の位置を、区間番号と区間内のパラメータで表します。
	/// @remark 例えば { 2, 0.5 } は 3 番目の区間の t = 0.5 です。区間の長さの半分とは限りません。
	/// @remark 元の曲線を作り直したり反転したりすると、同じ値でも別の位置を表します。
	struct SplineLocation
	{
		/// @brief 0 から始まる区間番号
		size_t segment = 0;

		/// @brief 区間内のパラメータ。0 が区間の始点、1 が終点です。
		double t = 0.0;

		/// @brief 区間番号とパラメータが両方等しいかを調べます。
		[[nodiscard]]
		friend constexpr bool operator ==(const SplineLocation&, const SplineLocation&) noexcept = default;
	};
}
