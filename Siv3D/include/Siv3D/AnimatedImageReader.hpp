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
# include "IReader.hpp"
# include "AnimatedImageDecodeOptions.hpp"
# include "AnimatedImageReadResult.hpp"

namespace s3d
{
	/// @brief アニメーション画像のストリームリーダー
	/// @remark GIF および APNG に対応します。
	class AnimatedImageReader
	{
	public:

		/// @brief 空のストリームリーダーを作成します。
		[[nodiscard]]
		AnimatedImageReader();

		/// @brief アニメーション画像ファイルをオープンします。
		/// @param path GIF または APNG ファイルのパス
		/// @param options デコード設定
		[[nodiscard]]
		explicit AnimatedImageReader(
			FilePathView path,
			const AnimatedImageDecodeOptions& options = {});

		/// @brief Reader オブジェクトからアニメーション画像をオープンします。
		/// @param reader Reader オブジェクト。リーダーが所有権を取得します。
		/// @param options デコード設定
		[[nodiscard]]
		explicit AnimatedImageReader(
			std::unique_ptr<IReader> reader,
			const AnimatedImageDecodeOptions& options = {});

		/// @brief Reader オブジェクトからアニメーション画像をオープンします。
		/// @tparam Reader Reader オブジェクトの型
		/// @param reader Reader オブジェクト
		/// @param options デコード設定
		template <ReaderObject Reader>
		[[nodiscard]]
		explicit AnimatedImageReader(
			Reader&& reader,
			const AnimatedImageDecodeOptions& options = {});

		AnimatedImageReader(const AnimatedImageReader&) = delete;

		/// @brief ムーブコンストラクタ
		AnimatedImageReader(AnimatedImageReader&& other) noexcept;

		/// @brief デストラクタ
		~AnimatedImageReader();

		AnimatedImageReader& operator =(const AnimatedImageReader&) = delete;

		/// @brief ムーブ代入演算子
		/// @return *this
		AnimatedImageReader& operator =(AnimatedImageReader&& other) noexcept;

		/// @brief アニメーション画像ファイルをオープンします。
		/// @param path GIF または APNG ファイルのパス
		/// @param options デコード設定
		/// @return オープンに成功した場合 true, それ以外の場合は false
		bool open(
			FilePathView path,
			const AnimatedImageDecodeOptions& options = {});

		/// @brief Reader オブジェクトからアニメーション画像をオープンします。
		/// @param reader Reader オブジェクト。リーダーが所有権を取得します。
		/// @param options デコード設定
		/// @return オープンに成功した場合 true, それ以外の場合は false
		bool open(
			std::unique_ptr<IReader> reader,
			const AnimatedImageDecodeOptions& options = {});

		/// @brief Reader オブジェクトからアニメーション画像をオープンします。
		/// @tparam Reader Reader オブジェクトの型
		/// @param reader Reader オブジェクト
		/// @param options デコード設定
		/// @return オープンに成功した場合 true, それ以外の場合は false
		template <ReaderObject Reader>
		bool open(
			Reader&& reader,
			const AnimatedImageDecodeOptions& options = {});

		/// @brief ストリームをクローズします。
		void close();

		/// @brief ストリームがオープンされているかを返します。
		/// @return オープンされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOpen() const noexcept;

		/// @brief ストリームがオープンされているかを返します。
		/// @return オープンされている場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		/// @brief アニメーション画像のサイズを返します。
		/// @return アニメーション画像のサイズ。画像情報を取得できなかった場合は (0, 0)
		[[nodiscard]]
		Size imageSize() const noexcept;

		/// @brief 総再生回数を返します。
		/// @return 総再生回数。0 は無限再生、または画像情報を取得できなかったことを示す
		[[nodiscard]]
		uint32 playCount() const noexcept;

		/// @brief 次のフレームを読み込みます。
		/// @param frame 読み込んだフレームの格納先
		/// @remark frame の画像が十分な容量を持つ場合、そのメモリを再利用します。
		/// @remark frame の内容は戻り値が `AnimatedImageReadStatus::Frame` の場合に有効です。
		/// @return 読み込み状態
		[[nodiscard]]
		AnimatedImageReadStatus readFrame(AnimatedImageFrame& frame);

		/// @brief 次のフレームを読み込みます。
		/// @return 読み込み結果
		[[nodiscard]]
		AnimatedImageReadResult readFrame();

		/// @brief 最後に発生したデコードエラーを返します。
		/// @return デコードエラー
		[[nodiscard]]
		AnimatedImageDecodeError error() const noexcept;

	private:

		class AnimatedImageReaderDetail;

		std::unique_ptr<AnimatedImageReaderDetail> pImpl;
	};
}

# include "detail/AnimatedImageReader.ipp"
