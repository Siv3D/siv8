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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	AudioFormat
	//
	////////////////////////////////////////////////////////////////

	/// @brief 音声フォーマット
	enum class AudioFormat : uint8
	{
		/// @brief 不明
		Unknown,

		/// @brief WAV
		WAV,

		/// @brief MP3
		MP3,

		/// @brief AAC
		AAC,

		/// @brief Ogg Vorbis
		OggVorbis,

		/// @brief Opus
		Opus,

		/// @brief WMA
		WMA,

		/// @brief AIFF
		AIFF,

		/// @brief FLAC
		FLAC,

		/// @brief MIDI
		MIDI,

		/// @brief 指定しない（データと拡張子から判断）
		Unspecified = Unknown,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 音声フォーマットを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 音声フォーマット
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, AudioFormat value);
}
