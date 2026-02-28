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

namespace s3d
{
	/// @brief シリアル通信
	class Serial
	{
	public:

		/// @brief データビット長
		enum class DataBits : uint8
		{
			Five	= 5,
			Six		= 6,
			Seven	= 7,
			Eight	= 8,
		};

		/// @brief パリティ
		enum class Parity : uint8
		{
			NoParity	= 0,
			Odd			= 1,
			Even		= 2,
			Mark		= 3,
			Space		= 4,
		};

		/// @brief ストップビット
		enum class StopBits : uint8
		{
			One				= 1,
			Two				= 2,
			OnePointFive	= 3,
		};

		/// @brief フロー制御
		enum class FlowControl : uint8
		{
			NoFlowControl	= 0,
			Software		= 1,
			Hardware		= 2,
		};

		/// @brief シリアル通信の設定
		struct Config
		{
			/// @brief データビット長
			DataBits dataBits		= DataBits::Eight;
			
			/// @brief パリティ
			Parity parity			= Parity::NoParity;
			
			/// @brief ストップビット
			StopBits stopBits		= StopBits::One;
			
			/// @brief フロー制御
			FlowControl flowControl	= FlowControl::NoFlowControl;

			/// @brief 読み込みのタイムアウト時間（ミリ秒）
			uint32 readTimeoutMillisec = 0;

			/// @brief 書き込みのタイムアウト時間（ミリ秒）
			uint32 writeTimeoutMillisec = 0;
		};

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Serial();

		/// @brief シリアルポートに接続します。
		/// @param port シリアルポートの名前
		/// @param baudrate ボーレート
		/// @param config その他の通信設定
		[[nodiscard]]
		explicit Serial(StringView port, int32 baudrate = 9600, const Config& config = {});

		/// @brief デストラクタ
		/// @remark シリアルポートに接続している場合は切断します。
		~Serial();

		/// @brief シリアルポートに接続します。
		/// @param port シリアルポートの名前
		/// @param baudrate ボーレート
		/// @param config その他の通信設定
		/// @return 接続に成功した場合 true, それ以外の場合は false
		bool open(StringView port, int32 baudrate = 9600, const Config& config = {});

		/// @brief シリアルポートとの接続を切断します。
		void close();

		/// @brief シリアルポートに接続されているかを返します。
		/// @return 接続されている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOpen() const;

		/// @brief シリアルポートに接続されているかを返します。
		/// @remark `isOpen()` と同じです。
		/// @return 接続されている場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const;

		/// @brief ボーレートを返します。
		/// @return ボーレート
		[[nodiscard]]
		int32 baudrate() const noexcept;

		/// @brief ボーレートを設定します。
		/// @param baudrate ボーレート
		void setBaudrate(int32 baudrate);

		/// @brief 通信設定を変更します。
		/// @param config 新しい通信設定
		void setConfig(const Config& config);

		/// @brief シリアルポートの名前を返します。
		/// @return シリアルポートの名前
		[[nodiscard]]
		const String& port() const noexcept;

		/// @brief 受信済みで読み込み可能なデータのサイズ（バイト）を返します。
		/// @return 読み込み可能なデータのサイズ（バイト）
		[[nodiscard]]
		size_t available();

		/// @brief 読み込みバッファを消去します。
		void clearInput();

		/// @brief 書き込みバッファを消去します。
		void clearOutput();

		/// @brief 読み込みバッファと書き込みバッファを消去します。
		void clear();

		/// @brief 受信したデータを読み込みます。
		/// @param dst 読み込み先
		/// @param size 読み込むサイズ（バイト）
		/// @return 実際に読み込んだサイズ（バイト）
		size_t read(void* dst, size_t size);

		/// @brief 受信したデータを読み込みます。
		/// @return 読み込んだデータ
		Array<uint8> readBytes();

		/// @brief 受信したデータを読み込みます。
		/// @param dst 読み込み先
		/// @return 読み込みに成功した場合 true, それ以外の場合は false
		bool readBytes(Array<uint8>& dst);

		/// @brief 受信したデータを読み込みます。
		/// @tparam TriviallyCopyable 読み込む値の型
		/// @param to 読み込み先
		/// @return 読み込みに成功した場合 true, それ以外の場合は false
		bool read(Concept::TriviallyCopyable auto& to);

		/// @brief 改行文字を受信するまで文字列を読み込みます。
		/// @return 読み込んだ文字列
		String readLine();

		/// @brief バイナリデータを書き込みます。
		/// @param src 書き込むデータの先頭ポインタ
		/// @param size 書き込むデータのサイズ（バイト）
		/// @return 書き込みに成功したバイト数
		size_t write(const void* src, size_t size);

		/// @brief 1 バイトのデータを書き込みます。
		/// @param byte 書き込むデータ
		/// @return 書き込みに成功した場合 true, それ以外の場合は false
		bool writeByte(uint8 byte);

		/// @brief 1 バイトのデータを書き込みます。
		/// @param byte 書き込むデータ
		/// @return 書き込みに成功した場合 true, それ以外の場合は false
		bool writeByte(Byte byte);

		/// @brief バイナリデータを書き込みます。
		/// @tparam TriviallyCopyable 書き込む値の型
		/// @param from 書き込むデータ
		/// @return 書き込みに成功した場合 true, それ以外の場合は false
		bool write(const Concept::TriviallyCopyable auto& from);

		/// @brief 文字列を書き込みます。
		/// @param s 書き込む文字列
		/// @return 書き込みに成功したバイト数
		size_t write(std::string_view s);

		/// @brief 文字列を書き込みます。
		/// @param s 書き込む文字列
		/// @return 書き込みに成功したバイト数
		size_t write(StringView s);

		/// @brief RS-232 のブレーク信号を送信します。
		void sendBreak();

		/// @brief RS-232 のブレーク信号の状態を変更します。
		/// @param level on にする場合 true, off にする場合 false
		void setBreak(bool level);

		/// @brief RTS ハンドシェイクの状態を変更します。
		/// @param level on にする場合 true, off にする場合 false
		/// @remark デフォルトでは on です。
		void setRTS(bool level);

		/// @brief DTR ハンドシェイクの状態を変更します。
		/// @param level on にする場合 true, off にする場合 false
		/// @remark デフォルトでは on です。
		void setDTR(bool level);

		/// @brief CTS, DSR, RI, CD の変更、もしくは何らかの割り込みまで待機します。
		/// @return 変更があった場合は true, それ以外の何かが起こった場合は false
		bool waitForChange();

		/// @brief CTS の状態を返します。
		/// @return CTS の状態
		[[nodiscard]]
		bool getCTS();

		/// @brief DSR の状態を返します。
		/// @return DSR の状態
		[[nodiscard]]
		bool getDSR();

		/// @brief RI の状態を返します。
		/// @return RI の状態
		[[nodiscard]]
		bool getRI();

		/// @brief CD の状態を返します。
		/// @return CD の状態
		[[nodiscard]]
		bool getCD();

	private:

		class SerialDetail;

		std::shared_ptr<SerialDetail> pImpl;
	};
}
