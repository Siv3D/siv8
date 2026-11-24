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
	/// @brief 経路の始点より前、または終点より先の距離をどう扱うかを指定します。
	enum class DistanceMode : uint8
	{
		/// @brief 負の距離は始点、全長以上の距離は終点として扱います。
		Clamp,

		/// @brief 全長ごとに始点へ戻ります。負の距離は終点側から数えます。
		/// @remark 長さ 100 の経路では、距離 120 は 20、-10 は 90、100 は 0 として扱います。
		/// @remark 開いた経路にも使えますが、終点から始点へ戻るときに位置が飛びます。
		Wrap,
	};
}
