//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "2DShapes.hpp"

namespace s3d
{
	/// @brief テキストのレイアウト結果
	struct TextLayoutResult
	{
		/// @brief テキスト全体が指定領域内に収まったか
		bool fits = false;

		/// @brief 実際に描画された、または描画可能なテキスト領域
		RectF region{};

		/// @brief テキスト全体が指定領域内に収まったかを返します。
		/// @return テキスト全体が指定領域内に収まった場合 true, そうでない場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept
		{
			return fits;
		}
	};
}
