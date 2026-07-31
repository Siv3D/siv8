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
# include "AnimatedImageFrame.hpp"
# include "AnimatedImageReadStatus.hpp"
# include "AnimatedImageDecodeError.hpp"

namespace s3d
{
	/// @brief アニメーション画像の 1 フレームのストリーム読み込み結果
	struct AnimatedImageReadResult
	{
		/// @brief 読み込み状態
		AnimatedImageReadStatus status = AnimatedImageReadStatus::Error;

		/// @brief 読み込まれたフレーム。status が `AnimatedImageReadStatus::Frame` の場合に有効
		AnimatedImageFrame frame;

		/// @brief エラー
		AnimatedImageDecodeError error = AnimatedImageDecodeError::None;

		/// @brief フレームの読み込みに成功したかを返します。
		/// @return フレームの読み込みに成功した場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;
	};
}

# include "detail/AnimatedImageReadResult.ipp"
