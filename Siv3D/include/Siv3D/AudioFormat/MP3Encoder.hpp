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
# include <Siv3D/IAudioEncoder.hpp>
# include <Siv3D/MP3Quality.hpp>

namespace s3d
{
	/// @brief MP3 形式音声データのエンコーダ
	class MP3Encoder : public IAudioEncoder
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////

		/// @brief エンコーダの対応形式 `U"MP3"` を返します。
		/// @return 文字列 `U"MP3"`
		[[nodiscard]]
		StringView name() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	audioFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief MP3 形式を示す AudioFormat を返します。
		/// @return `AudioFormat::MP3`
		[[nodiscard]]
		AudioFormat audioFormat() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	possibleExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief MP3 形式のファイルに想定される拡張子一覧 `{ U"mp3" }` を返します。
		/// @return 配列 `{ U"mp3" }`
		[[nodiscard]]
		std::span<const StringView> possibleExtensions() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief Wave を MP3 形式でエンコードしてファイルに保存します。
		/// @param wave エンコードする Wave
		/// @param path 保存するファイルのパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool save(const Wave& wave, FilePathView path) const override;

		bool save(const Wave& wave, FilePathView path, MP3Quality quality) const;

		////////////////////////////////////////////////////////////////
		//
		//	encode
		//
		////////////////////////////////////////////////////////////////

		/// @brief Wave を MP3 形式でエンコードして書き出します。
		/// @param wave エンコードする Wave
		/// @param writer 書き出し先
		/// @return  書き出しに成功した場合 true, それ以外の場合は false
		bool encode(const Wave& wave, IWriter& writer) const override;

		bool encode(const Wave& wave, IWriter& writer, MP3Quality quality) const;

		/// @brief Wave を MP3 形式でエンコードした結果を Blob で返します。
		/// @param wave エンコードする Wave
		/// @return エンコード結果
		[[nodiscard]]
		Blob encode(const Wave& wave) const override;

		[[nodiscard]]
		Blob encode(const Wave& wave, MP3Quality MP3Quality) const;

		////////////////////////////////////////////////////////////////
		//
		//	availableSampleRates
		//
		////////////////////////////////////////////////////////////////

		Array<uint32> availableSampleRates() const;
	};
}
