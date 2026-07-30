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

	/// @brief Animated GIF をデコードします。
	/// @param path GIF ファイルのパス
	/// @param options デコード設定
	/// @return デコード結果
	[[nodiscard]]
	AnimatedImageDecodeResult DecodeAnimatedGIF(
		FilePathView path,
		const AnimatedImageDecodeOptions& options = {});

	/// @brief Animated GIF をデコードします。
	/// @param reader Reader オブジェクト
	/// @param options デコード設定
	/// @return デコード結果
	[[nodiscard]]
	AnimatedImageDecodeResult DecodeAnimatedGIF(
		std::unique_ptr<IReader> reader,
		const AnimatedImageDecodeOptions& options = {});

	/// @brief APNG をデコードします。
	/// @param path PNG ファイルのパス
	/// @param options デコード設定
	/// @return デコード結果。通常の PNG の場合は `AnimatedImageDecodeError::NotAnimated`
	[[nodiscard]]
	AnimatedImageDecodeResult DecodeAPNG(
		FilePathView path,
		const AnimatedImageDecodeOptions& options = {});

	/// @brief APNG をデコードします。
	/// @param reader Reader オブジェクト
	/// @param options デコード設定
	/// @return デコード結果。通常の PNG の場合は `AnimatedImageDecodeError::NotAnimated`
	[[nodiscard]]
	AnimatedImageDecodeResult DecodeAPNG(
		std::unique_ptr<IReader> reader,
		const AnimatedImageDecodeOptions& options = {});
}
