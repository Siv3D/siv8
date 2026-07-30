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
# include "Common.hpp"
# include "Array.hpp"
# include "AnimatedImageFrame.hpp"

namespace s3d
{
	/// @brief アニメーション画像
	struct AnimatedImage
	{
		/// @brief 合成済みのフレーム一覧
		Array<AnimatedImageFrame> frames;

		/// @brief 総再生回数。0 は無限再生
		uint32 playCount = 1;

		/// @brief フレームが空であるかを返します。
		/// @return フレームが空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		/// @brief フレームを持っているかを返します。
		/// @return フレームを持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		/// @brief フレーム数を返します。
		/// @return フレーム数
		[[nodiscard]]
		size_t size() const noexcept;
	};
}

# include "detail/AnimatedImage.ipp"
