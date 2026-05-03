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
# include <Siv3D/IAudioDecoder.hpp>

namespace s3d
{
	/// @brief WAV 形式音声データのデコーダ
	class WAVDecoder : public IAudioDecoder
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////

		/// @brief デコーダの対応形式 `U"WAV"` を返します。
		/// @return 文字列 `U"WAV"`
		[[nodiscard]]
		StringView name() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	audioFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief WAV 形式を示す AudioFormat を返します。
		/// @return `AudioFormat::WAV`
		[[nodiscard]]
		AudioFormat audioFormat() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	isHeader
		//
		////////////////////////////////////////////////////////////////

		/// @brief WAV 形式の先頭データであるかを返します。
		/// @param bytes データの先頭 48 バイト
		/// @return WAV 形式と推測される場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	possibleExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief WAV 形式のファイルに想定される拡張子一覧 `{ U"wav" }` を返します。
		/// @return 配列 `{ U"wav" }`
		[[nodiscard]]
		std::span<const StringView> possibleExtensions() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	decode
		//
		////////////////////////////////////////////////////////////////

		/// @brief WAV 形式の音声ファイルをデコードして Wave を作成します。
		/// @param path 音声ファイルのパス
		/// @return 作成した Wave
		[[nodiscard]]
		Wave decode(FilePathView path) const override;

		/// @brief WAVE 形式の音声データをデコードして Wave を作成します。
		/// @param reader Reader オブジェクト
		/// @param pathHint ファイルパス（オプション）
		/// @return 作成した Wave
		[[nodiscard]]
		Wave decode(std::unique_ptr<IReader> reader, FilePathView pathHint = {}) const override;
	};
}
