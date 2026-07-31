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
# include <memory>
# include "Common.hpp"
# include "AnimatedImageDecodeOptions.hpp"
# include "AnimatedImageDecodeResult.hpp"

namespace s3d
{
	class IReader;

	/// @brief アニメーション画像をデコードします。
	/// @param path GIF または APNG ファイルのパス
	/// @param options デコード設定
	/// @return デコード結果。通常の PNG の場合は `AnimatedImageDecodeError::NotAnimated`
	[[nodiscard]]
	AnimatedImageDecodeResult DecodeAnimatedImage(
		FilePathView path,
		const AnimatedImageDecodeOptions& options = {});

	/// @brief アニメーション画像をデコードします。
	/// @param reader Reader オブジェクト
	/// @param options デコード設定
	/// @return デコード結果。通常の PNG の場合は `AnimatedImageDecodeError::NotAnimated`
	[[nodiscard]]
	AnimatedImageDecodeResult DecodeAnimatedImage(
		std::unique_ptr<IReader> reader,
		const AnimatedImageDecodeOptions& options = {});
}
