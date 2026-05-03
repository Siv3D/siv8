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
# include "StringView.hpp"
# include "ColorHSV.hpp"

namespace osc
{
	class OutboundPacketStream;
}

namespace s3d
{
	class Blob;

	////////////////////////////////////////////////////////////////
	//
	//	OSCMessage
	//
	////////////////////////////////////////////////////////////////

	/// @brief OSC 送信メッセージ
	class OSCMessage
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief OSC 送信メッセージ用のバッファを構築します。
		/// @param bufferSize メッセージ用に確保するバッファサイズ
		[[nodiscard]]
		explicit OSCMessage(size_t bufferSize = 1024);

		////////////////////////////////////////////////////////////////
		//
		//	beginBundle, endBundle
		//
		////////////////////////////////////////////////////////////////

		/// @brief バンドルを開始します。
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& beginBundle();

		/// @brief バンドルを終了します。
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& endBundle();

		////////////////////////////////////////////////////////////////
		//
		//	beginMessage, endMessage
		//
		////////////////////////////////////////////////////////////////

		/// @brief メッセージを開始します。
		/// @param addressPattern アドレスパターン
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& beginMessage(StringView addressPattern);

		/// @brief メッセージを終了します。
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& endMessage();

		////////////////////////////////////////////////////////////////
		//
		//	addBool
		//
		////////////////////////////////////////////////////////////////

		/// @brief bool 型の値を引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addBool(bool value);

		////////////////////////////////////////////////////////////////
		//
		//	addChar
		//
		////////////////////////////////////////////////////////////////

		/// @brief ASCII 文字を引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addChar(char value);

		////////////////////////////////////////////////////////////////
		//
		//	addInt32
		//
		////////////////////////////////////////////////////////////////

		/// @brief int32 型の値を引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addInt32(int32 value);

		////////////////////////////////////////////////////////////////
		//
		//	addInt64
		//
		////////////////////////////////////////////////////////////////

		/// @brief int64 型の値を引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addInt64(int64 value);

		////////////////////////////////////////////////////////////////
		//
		//	addFloat
		//
		////////////////////////////////////////////////////////////////

		/// @brief float 型の値を引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addFloat(float value);

		////////////////////////////////////////////////////////////////
		//
		//	addDouble
		//
		////////////////////////////////////////////////////////////////

		/// @brief double 型の値を引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addDouble(double value);

		////////////////////////////////////////////////////////////////
		//
		//	addMIDIMessage
		//
		////////////////////////////////////////////////////////////////

		/// @brief MIDI メッセージを引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addMIDIMessage(uint32 value);

		////////////////////////////////////////////////////////////////
		//
		//	addTimeTag
		//
		////////////////////////////////////////////////////////////////

		/// @brief 時刻タグを引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addTimeTag(uint64 value);

		////////////////////////////////////////////////////////////////
		//
		//	addRGBA
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA カラーを引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addRGBA(const Color& value);

		////////////////////////////////////////////////////////////////
		//
		//	addString
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addString(StringView value);

		////////////////////////////////////////////////////////////////
		//
		//	addSymbol
		//
		////////////////////////////////////////////////////////////////

		/// @brief シンボル文字列を引数として追加します。
		/// @param value 追加する値
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addSymbol(StringView value);

		////////////////////////////////////////////////////////////////
		//
		//	addBlob
		//
		////////////////////////////////////////////////////////////////

		/// @brief blob を引数として追加します。
		/// @param blob 追加するデータ
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addBlob(const Blob& blob);

		/// @brief blob を引数として追加します。
		/// @param data 追加するデータの先頭ポインタ
		/// @param size 追加するデータのサイズ（バイト）
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addBlob(const void* data, size_t size);

		/// @brief blob を引数として追加します。
		/// @param data 追加するデータ
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addBlob(std::span<const Byte> data);

		////////////////////////////////////////////////////////////////
		//
		//	addNil
		//
		////////////////////////////////////////////////////////////////

		/// @brief Nil 値を引数として追加します。
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addNil();

		////////////////////////////////////////////////////////////////
		//
		//	addInf
		//
		////////////////////////////////////////////////////////////////

		/// @brief 無限を表す値を引数として追加します。
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& addInf();

		////////////////////////////////////////////////////////////////
		//
		//	beginArray, endArray
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列を開始します。
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& beginArray();

		/// @brief 配列を終了します。
		/// @throws Error バッファが不足している場合
		/// @return *this
		OSCMessage& endArray();

		////////////////////////////////////////////////////////////////
		//
		//	_detail
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const osc::OutboundPacketStream* _detail() const noexcept;

	public:

		class OSCMessageDetail;

		std::shared_ptr<OSCMessageDetail> pImpl;
	};
}
