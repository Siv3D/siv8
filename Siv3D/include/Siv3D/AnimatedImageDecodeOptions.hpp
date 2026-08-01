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
# include "PredefinedYesNo.hpp"

namespace s3d
{
	/// @brief アニメーション画像のデコードで保持するフレームの既定上限 (512 MiB)
	inline constexpr uint64 DefaultMaxAnimatedImageDecodedBytes = (512ull * 1024 * 1024);

	/// @brief アニメーション画像のデコードで使用する作業メモリの既定上限 (512 MiB)
	inline constexpr uint64 DefaultMaxAnimatedImageWorkingMemoryBytes = (512ull * 1024 * 1024);

	/// @brief アニメーション画像の既定の最大フレーム数
	inline constexpr uint32 DefaultMaxAnimatedImageFrames = 10'000;

	/// @brief アニメーション画像のデコード設定
	struct AnimatedImageDecodeOptions
	{
		/// @brief デコード結果として保持する画像データのバイト数の上限
		uint64 maxDecodedBytes = DefaultMaxAnimatedImageDecodedBytes;

		/// @brief デコード中の作業メモリの上限
		uint64 maxWorkingMemoryBytes = DefaultMaxAnimatedImageWorkingMemoryBytes;

		/// @brief 最大フレーム数
		uint32 maxFrames = DefaultMaxAnimatedImageFrames;

		/// @brief 返却する各フレームにアルファ乗算処理を適用するか
		PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes;
	};
}
