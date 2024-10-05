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
# include "2DShapes.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	/// @brief 長方形の詰め込み結果
	struct RectanglePack
	{
		/// @brief 長方形の配置
		Array<Rect> rects;

		/// @brief 全体の幅と高さ
		Size size{ 0, 0 };

		/// @brief 長方形の詰め込み結果が空であるかを返します。
		/// @return 長方形の詰め込み結果が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept;

		/// @brief 長方形の詰め込み結果が空でないかを返します。
		/// @return 長方形の詰め込み結果が空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;

		/// @brief 長方形の配列から詰め込み結果を作成します。
		/// @param rects 長方形の配列
		/// @param maxSide 詰め込み先の長方形の長辺の最大サイズ（ピクセル）
		[[nodiscard]]
		static RectanglePack Pack(const Array<Rect>& rects, int32 maxSide) noexcept;

		/// @brief 長方形の配列から詰め込み結果を作成します。
		/// @param rects 長方形の配列
		/// @param maxSide 詰め込み先の長方形の長辺の最大サイズ（ピクセル）
		/// @param allowFlip 詰め込む長方形の 90° 回転を許可するか
		[[nodiscard]]
		static RectanglePack Pack(const Array<Rect>& rects, int32 maxSide, AllowFlip allowFlip) noexcept;
	};
}

# include "detail/RectanglePack.ipp"
