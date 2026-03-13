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
# include "Wave.hpp"
# include "IAudioDecoder.hpp"

namespace s3d
{
	namespace AudioDecoder
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetAudioFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief 音声ファイルのフォーマットを取得します。
		/// @param path 音声ファイルのパス
		/// @return 音声ファイルのフォーマット、取得に失敗した場合は AudioFormat::Unknown
		[[nodiscard]]
		AudioFormat GetAudioFormat(FilePathView path);

		/// @brief 音声ファイルのフォーマットを取得します。
		/// @param reader Reader オブジェクト
		/// @return 音声ファイルのフォーマット、取得に失敗した場合は AudioFormat::Unknown
		[[nodiscard]]
		AudioFormat GetAudioFormat(const IReader& reader);

		////////////////////////////////////////////////////////////////
		//
		//	Decode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 音声ファイルをデコードします。
		/// @param path 音声ファイルのパス
		/// @param audioFormat 音声フォーマット、指定しない場合はファイルの拡張子から判断
		/// @return デコードされた波形、デコードに失敗した場合は空の波形
		[[nodiscard]]
		Wave Decode(FilePathView path, AudioFormat audioFormat = AudioFormat::Unspecified);

		/// @brief 音声ファイルをデコードします。
		/// @param path 音声ファイルのパス
		/// @param decoderName 使用するデコーダの名前
		/// @return デコードされた波形、デコードに失敗した場合は空の波形
		[[nodiscard]]
		Wave Decode(FilePathView path, StringView decoderName);

		/// @brief 音声ファイルをデコードします。
		/// @param reader Reader オブジェクト
		/// @param audioFormat 音声フォーマット、指定しない場合はファイルの拡張子から判断
		/// @return デコードされた波形、デコードに失敗した場合は空の波形
		[[nodiscard]]
		Wave Decode(std::unique_ptr<IReader> reader, AudioFormat audioFormat = AudioFormat::Unspecified);

		/// @brief 音声ファイルをデコードします。
		/// @param reader Reader オブジェクト
		/// @param decoderName 使用するデコーダの名前
		/// @return デコードされた波形、デコードに失敗した場合は空の波形
		[[nodiscard]]
		Wave Decode(std::unique_ptr<IReader> reader, StringView decoderName);

		/// @brief 音声ファイルをデコードします。
		/// @tparam Reader Reader オブジェクトの型
		/// @param reader Reader オブジェクト
		/// @param audioFormat 音声フォーマット、指定しない場合はファイルの拡張子から判断
		/// @return デコードされた波形、デコードに失敗した場合は空の波形
		template <ReaderObject Reader>
		[[nodiscard]]
		Wave Decode(Reader&& reader, AudioFormat audioFormat = AudioFormat::Unspecified);

		/// @brief 音声ファイルをデコードします。
		/// @tparam Reader Reader オブジェクトの型
		/// @param reader Reader オブジェクト
		/// @param decoderName 使用するデコーダの名前
		/// @return デコードされた波形、デコードに失敗した場合は空の波形
		template <ReaderObject Reader>
		[[nodiscard]]
		Wave Decode(Reader&& reader, StringView decoderName);

		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		/// @brief 音声デコーダを登録します。
		/// @tparam AudioDecoder 音声デコーダの型
		/// @return 登録に成功した場合 true, それ以外の場合は false
		template <class AudioDecoder>
			requires std::derived_from<AudioDecoder, IAudioDecoder>
		bool Add();

		/// @brief 音声デコーダを登録します。
		/// @param decoder 音声デコーダ
		/// @return 登録に成功した場合 true, それ以外の場合は false
		bool Add(std::unique_ptr<IAudioDecoder> decoder);

		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////

		/// @brief 音声デコーダを登録解除します。
		/// @param decoderName 音声デコーダの名前
		void Remove(StringView decoderName);

		////////////////////////////////////////////////////////////////
		//
		//	IsRegistered
		//
		////////////////////////////////////////////////////////////////

		/// @brief 音声デコーダが登録されているかを返します。
		/// @param decoderName 音声デコーダの名前
		/// @return 登録されている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsRegistered(StringView decoderName) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Enum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録されている音声デコーダの一覧を返します。
		/// @return 登録されている音声デコーダの一覧
		[[nodiscard]]
		const Array<std::unique_ptr<IAudioDecoder>>& Enum() noexcept;
	}
}

# include "detail/AudioDecoder.ipp"
