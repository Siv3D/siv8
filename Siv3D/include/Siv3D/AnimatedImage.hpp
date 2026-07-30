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
# include "Array.hpp"
# include "Image.hpp"
# include "Duration.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	class IReader;

	/// @brief アニメーション画像の 1 フレーム
	struct AnimatedImageFrame
	{
		/// @brief 合成済みのフルキャンバス画像
		Image image;

		/// @brief フレームの表示時間
		Duration duration{ 0 };
	};

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
		bool isEmpty() const noexcept
		{
			return frames.empty();
		}

		/// @brief フレームを持っているかを返します。
		/// @return フレームを持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept
		{
			return (not frames.empty());
		}

		/// @brief フレーム数を返します。
		/// @return フレーム数
		[[nodiscard]]
		size_t size() const noexcept
		{
			return frames.size();
		}
	};

	/// @brief アニメーション画像のデコードで保持するフレームの既定上限 (512 MiB)
	inline constexpr uint64 DefaultMaxAnimatedImageDecodedBytes = (512ull * 1024 * 1024);

	/// @brief アニメーション画像のデコードで使用する作業メモリの既定上限 (512 MiB)
	inline constexpr uint64 DefaultMaxAnimatedImageWorkingMemoryBytes = (512ull * 1024 * 1024);

	/// @brief アニメーション画像の既定の最大フレーム数
	inline constexpr uint32 DefaultMaxAnimatedImageFrames = 10'000;

	/// @brief アニメーション画像のデコード設定
	struct AnimatedImageDecodeOptions
	{
		/// @brief 返却する全フレームの合計バイト数の上限
		uint64 maxTotalDecodedBytes = DefaultMaxAnimatedImageDecodedBytes;

		/// @brief デコード中の作業メモリの上限
		uint64 maxWorkingMemoryBytes = DefaultMaxAnimatedImageWorkingMemoryBytes;

		/// @brief 最大フレーム数
		uint32 maxFrames = DefaultMaxAnimatedImageFrames;

		/// @brief 返却する各フレームにアルファ乗算処理を適用するか
		PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes;
	};

	/// @brief アニメーション画像のデコードエラー
	enum class AnimatedImageDecodeError : uint8
	{
		/// @brief エラーなし
		None,

		/// @brief 指定された画像形式ではない
		InvalidFormat,

		/// @brief PNG だが APNG ではない
		NotAnimated,

		/// @brief 画像データが不正
		InvalidData,

		/// @brief 画像データが途中で終わっている
		TruncatedInput,

		/// @brief Reader からの読み込みに失敗
		ReadError,

		/// @brief 未対応の画像機能を使用している
		UnsupportedFeature,

		/// @brief フレーム数の上限を超えた
		TooManyFrames,

		/// @brief デコード済みフレームの合計バイト数上限を超えた
		DecodedBytesLimitExceeded,

		/// @brief 作業メモリ上限を超えた
		WorkingMemoryLimitExceeded,

		/// @brief メモリ確保に失敗
		OutOfMemory,
	};

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
		explicit operator bool() const noexcept
		{
			return ((error == AnimatedImageDecodeError::None)
				&& (not image.frames.empty()));
		}
	};

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
