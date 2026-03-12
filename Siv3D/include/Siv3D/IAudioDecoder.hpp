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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	IAudioDecoder
	//
	////////////////////////////////////////////////////////////////

	struct IAudioDecoder
	{
		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief デストラクタ
		virtual ~IAudioDecoder() = default;

		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief デコーダの名前を返します。
		/// @return デコーダの名前
		[[nodiscard]]
		virtual StringView name() const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	audioFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief デコーダがサポートする音声フォーマットを返します。
		/// @return デコーダがサポートする音声フォーマット
		[[nodiscard]]
		virtual AudioFormat audioFormat() const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	isHeader
		//
		////////////////////////////////////////////////////////////////

		/// @brief 与えられたバイト列が、このデコーダがサポートする音声フォーマットのヘッダであるかを返します。
		/// @param bytes バイト列
		/// @return 与えられたバイト列が、このデコーダがサポートする音声フォーマットのヘッダである場合 true, それ以外の場合は false
		[[nodiscard]]
		virtual bool isHeader(const uint8(&bytes)[48]) const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	possibleExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief このデコーダがサポートする音声ファイルの拡張子の一覧を返します。
		/// @return このデコーダがサポートする音声ファイルの拡張子の一覧
		[[nodiscard]]
		virtual std::span<const StringView> possibleExtensions() const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	decode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 音声ファイルをデコードします。
		/// @param path 音声ファイルのパス
		/// @return デコードされた波形
		[[nodiscard]]
		virtual Wave decode(FilePathView path) const;

		/// @brief 音声ファイルをデコードします。
		/// @param reader Reader オブジェクト
		/// @param pathHint 音声ファイルのパス（わかる場合）
		/// @return デコードされた波形
		[[nodiscard]]
		virtual Wave decode(std::unique_ptr<IReader> reader, FilePathView pathHint) const = 0;
	};
}
