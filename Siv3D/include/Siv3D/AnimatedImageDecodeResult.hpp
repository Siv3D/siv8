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
# include "AnimatedImage.hpp"
# include "AnimatedImageDecodeError.hpp"

namespace s3d
{
	/// @brief アニメーション画像のデコード結果
	struct AnimatedImageDecodeResult
	{
		/// @brief デコードされたアニメーション画像
		AnimatedImage image;

		/// @brief エラー
		AnimatedImageDecodeError error = AnimatedImageDecodeError::None;

		/// @brief デコードに成功したかを返します。
		/// @return デコードに成功した場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;
	};
}

# include "detail/AnimatedImageDecodeResult.ipp"
