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
# include "IAudioEncoder.hpp"

namespace s3d
{
	namespace AudioEncoder
	{	
		////////////////////////////////////////////////////////////////
		//
		//	Save
		//
		////////////////////////////////////////////////////////////////

		/// @brief 波形をエンコードしてファイルに保存します。
		/// @param wave 波形
		/// @param format 音声フォーマット
		/// @param path 保存先のファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool Save(const Wave& wave, AudioFormat format, FilePathView path);

		/// @brief 波形をエンコードしてファイルに保存します。
		/// @param wave 波形
		/// @param encoderName 使用するエンコーダ名 
		/// @param path 保存先のファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool Save(const Wave& wave, StringView encoderName, FilePathView path);
	
		////////////////////////////////////////////////////////////////
		//
		//	Encode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 波形をエンコードします。
		/// @param wave 波形
		/// @param format 音声フォーマット
		/// @param writer 音声ファイルを書き込む IWriter
		/// @return エンコードに成功した場合 true, それ以外の場合は false
		bool Encode(const Wave& wave, AudioFormat format, IWriter& writer);

		/// @brief 波形をエンコードします。
		/// @param wave 波形
		/// @param encoderName 使用するエンコーダ名
		/// @param writer 音声ファイルを書き込む IWriter
		/// @return エンコードに成功した場合 true, それ以外の場合は false
		bool Encode(const Wave& wave, StringView encoderName, IWriter& writer);

		/// @brief 波形をエンコードします。
		/// @param wave 波形
		/// @param format 音声フォーマット
		/// @return エンコードされた音声データ
		[[nodiscard]]
		Blob Encode(const Wave& wave, AudioFormat format);

		/// @brief 波形をエンコードします。
		/// @param wave 波形
		/// @param encoderName 使用するエンコーダ名
		/// @return エンコードされた音声データ
		[[nodiscard]]
		Blob Encode(const Wave& wave, StringView encoderName);
	
		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		/// @brief 音声エンコーダを登録します。
		/// @tparam AudioEncoder 音声エンコーダの型
		/// @return 登録に成功した場合 true, それ以外の場合は false
		template <class AudioEncoder>
			requires std::derived_from<AudioEncoder, IAudioEncoder>
		bool Add();

		/// @brief 音声エンコーダを登録します。
		/// @param encoder 音声エンコーダ
		/// @return 登録に成功した場合 true, それ以外の場合は false
		bool Add(std::unique_ptr<IAudioEncoder> encoder);
	
		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////

		/// @brief 音声エンコーダを登録解除します。
		/// @param encoderName 音声エンコーダの名前
		void Remove(StringView encoderName);
	
		////////////////////////////////////////////////////////////////
		//
		//	IsRegistered
		//
		////////////////////////////////////////////////////////////////

		/// @brief 音声エンコーダが登録されているかを返します。
		/// @param encoderName 音声エンコーダの名前
		/// @return 登録されている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsRegistered(StringView encoderName) noexcept;
	
		////////////////////////////////////////////////////////////////
		//
		//	Enum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録されている音声エンコーダの一覧を返します。
		/// @return 登録されている音声エンコーダの一覧
		[[nodiscard]]
		const Array<std::unique_ptr<IAudioEncoder>>& Enum() noexcept;
	}
}

# include "detail/AudioEncoder.ipp"
