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
# include "String.hpp"
# include "AudioFormat.hpp"
# include "Wave.hpp"
# include "Blob.hpp"

namespace s3d
{
	class IWriter;

	////////////////////////////////////////////////////////////////
	//
	//	IAudioEncoder
	//
	////////////////////////////////////////////////////////////////

	struct IAudioEncoder
	{
		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		virtual ~IAudioEncoder() = default;

		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////

		/// @brief エンコーダの名前を返します。
		/// @return エンコーダの名前
		[[nodiscard]]
		virtual StringView name() const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	audioFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief エンコーダがサポートする音声フォーマットを返します。
		/// @return エンコーダがサポートする音声フォーマット
		[[nodiscard]]
		virtual AudioFormat audioFormat() const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	possibleExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエンコーダがサポートするファイルの拡張子一覧を返します。
		/// @return このエンコーダがサポートするファイルの拡張子一覧
		[[nodiscard]]
		virtual std::span<const StringView> possibleExtensions() const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief 波形をエンコードしてファイルに保存します。
		/// @param wave 保存する波形
		/// @param path 保存先のファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		virtual bool save(const Wave& wave, FilePathView path) const = 0;

		////////////////////////////////////////////////////////////////
		//
		//	encode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 波形をエンコードします。
		/// @param wave エンコードする波形
		/// @param writer 書き込み先の IWriter
		/// @return エンコードに成功した場合 true, それ以外の場合は false
		virtual bool encode(const Wave& wave, IWriter& writer) const = 0;

		/// @brief 波形をエンコードします。
		/// @param wave エンコードする波形
		/// @return エンコードされたデータ
		[[nodiscard]]
		virtual Blob encode(const Wave& wave) const = 0;
	};
}
