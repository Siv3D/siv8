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

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	OSCTypeTag
	//
	////////////////////////////////////////////////////////////////

	/// @brief OSC 型タグ
	enum class OSCTypeTag : char
	{
		/// @brief False
		False		= 'F',

		/// @brief True
		True		= 'T',

		/// @brief ASCII 文字
		Char		= 'c',

		/// @brief int32
		Int32		= 'i',

		/// @brief int64
		Int64		= 'h',

		/// @brief float32
		Float		= 'f',

		/// @brief float64
		Double		= 'd',

		/// @brief MIDI メッセージ
		MIDIMessage	= 'm',

		/// @brief 時刻タグ
		TimeTag		= 't',

		/// @brief RGBA カラー
		RGBAColor	= 'r',

		/// @brief OSC 文字列
		String		= 's',

		/// @brief OSC 文字列（シンボル）
		Symbol		= 'S',

		/// @brief blob
		Blob		= 'b',

		/// @brief Nil
		Nil			= 'N',

		/// @brief 無限
		Infinitum	= 'I',

		/// @brief 配列の開始
		ArrayBegin	= '[',

		/// @brief 配列の終了
		ArrayEnd	= ']'
	};

	/// @brief OSC 型タグを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value OSC 型タグ
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, OSCTypeTag value);
}
