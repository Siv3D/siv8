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
# include "Array.hpp"
# include "String.hpp"
# include "Number.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Serial
	//
	////////////////////////////////////////////////////////////////

	/// @brief シリアル通信
	class Serial
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	DataBits
		//
		////////////////////////////////////////////////////////////////

		/// @brief データビット長
		enum class DataBits : uint8
		{
			Five	= 5,
			Six		= 6,
			Seven	= 7,
			Eight	= 8,
		};

		////////////////////////////////////////////////////////////////
		//
		//	Parity
		//
		////////////////////////////////////////////////////////////////

		/// @brief パリティ
		enum class Parity : uint8
		{
			NoParity	= 0,
			Odd			= 1,
			Even		= 2,
			Mark		= 3,
			Space		= 4,
		};

		////////////////////////////////////////////////////////////////
		//
		//	StopBits
		//
		////////////////////////////////////////////////////////////////

		/// @brief ストップビット
		enum class StopBits : uint8
		{
			One				= 1,
			Two				= 2,
			OnePointFive	= 3,
		};

		////////////////////////////////////////////////////////////////
		//
		//	FlowControl
		//
		////////////////////////////////////////////////////////////////

		/// @brief フロー制御
		enum class FlowControl : uint8
		{
			NoFlowControl	= 0,
			Software		= 1,
			Hardware		= 2,
		};

		////////////////////////////////////////////////////////////////
		//
		//	Timeout
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイムアウトの設定
		struct Timeout
		{
			/// @brief 最初の 1 バイトの受信後に次のバイトが到着するまでの最大経過時間（ミリ秒）。この時間を超えるとタイムアウトになります。
			/// @remark この値を `Timeout::Max`, 他の read タイムアウトを 0 に設定すると、ノンブロッキングモードになります。
			uint32 readIntervalTimeoutMillisec = 0;

			/// @brief 読み込み全体の固定タイムアウト追加時間（ミリ秒）
			uint32 readTotalTimeoutMillisec = 0;

			/// @brief 読み込み 1 バイトあたりのタイムアウト追加乗数（ミリ秒）
			uint32 readTotalTimeoutMultiplier = 0;

			/// @brief 書き込み全体の固定タイムアウト追加時間（ミリ秒）
			uint32 writeTotalTimeoutMillisec = 0;

			/// @brief 書き込み 1 バイトあたりのタイムアウト追加乗数（ミリ秒）
			uint32 writeTotalTimeoutMultiplier = 0;

			/// @brief タイムアウトの最大値
			static constexpr uint32 Max = Largest<uint32>;

			/// @brief ブロッキングモードのタイムアウト設定を返します。
			/// @return ブロッキングモードのタイムアウト設定
			[[nodiscard]]
			static constexpr Timeout Blocking();

			/// @brief 読み込みをノンブロッキングモードにするタイムアウト設定を返します。
			/// @return 読み込みをノンブロッキングモードにするタイムアウト設定
			[[nodiscard]]
			static constexpr Timeout NonBlocking();

			/// @brief 固定タイムアウトの設定を返します。
			/// @param timeout タイムアウト時間（ミリ秒）
			/// @return 固定タイムアウトの設定
			/// @remark 要求したバイト数が揃うまで、最大 `timeout` ミリ秒待ちます。
			[[nodiscard]]
			static constexpr Timeout Fixed(uint32 timeout);
		};

		////////////////////////////////////////////////////////////////
		//
		//	Config
		//
		////////////////////////////////////////////////////////////////

		/// @brief シリアル通信の設定
		struct Config
		{
			/// @brief データビット長
			DataBits dataBits = DataBits::Eight;

			/// @brief パリティ
			Parity parity = Parity::NoParity;

			/// @brief ストップビット
			StopBits stopBits = StopBits::One;

			/// @brief フロー制御
			FlowControl flowControl = FlowControl::NoFlowControl;

			/// @brief タイムアウトの設定
			Timeout timeout = Timeout::Blocking();

			Config();
		};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Serial();

		/// @brief シリアルポートに接続します。
		/// @param port シリアルポートの名前
		/// @param baudrate ボーレート
		/// @param config その他の通信設定
		/// @remark Arduino IDE のシリアルモニターを開いている場合、接続に失敗することがあります。シリアルモニターを閉じてから再度接続してください。
		[[nodiscard]]
		explicit Serial(StringView port, int32 baudrate = 9600, const Config& config = {});

		Serial(const Serial& other) = delete;

		/// @brief ムーブコンストラクタ
		/// @param other ムーブする Serial
		Serial(Serial&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		/// @remark シリアルポートに接続している場合は切断します。
		~Serial();

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		Serial& operator =(const Serial& other) = delete;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする Serial
		/// @return *this
		Serial& operator =(Serial&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	open
		//
		////////////////////////////////////////////////////////////////

		/// @brief シリアルポートに接続します。
		/// @param port シリアルポートの名前
		/// @param baudrate ボーレート
		/// @param config その他の通信設定
		/// @return 接続に成功した場合 true, それ以外の場合は false
		/// @remark Arduino IDE のシリアルモニターを開いている場合、接続に失敗することがあります。シリアルモニターを閉じてから再度接続してください。
		bool open(StringView port, int32 baudrate = 9600, const Config& config = {});

		////////////////////////////////////////////////////////////////
		//
		//	close
		//
		////////////////////////////////////////////////////////////////

		/// @brief シリアルポートとの接続を切断します。
		void close();

		////////////////////////////////////////////////////////////////
		//
		//	isOpen
		//
		////////////////////////////////////////////////////////////////

		/// @brief シリアルポートに接続されているかを返します。
		/// @return 接続されている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOpen() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief シリアルポートに接続されているかを返します。
		/// @remark `isOpen()` と同じです。
		/// @return 接続されている場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const;

		////////////////////////////////////////////////////////////////
		//
		//	baudrate, setBaudrate
		//
		////////////////////////////////////////////////////////////////

		/// @brief ボーレートを返します。
		/// @return ボーレート
		[[nodiscard]]
		int32 baudrate() const noexcept;

		/// @brief ボーレートを設定します。
		/// @param baudrate ボーレート
		/// @return 設定に成功した場合 true, それ以外の場合は false
		bool setBaudrate(int32 baudrate);

		////////////////////////////////////////////////////////////////
		//
		//	setConfig
		//
		////////////////////////////////////////////////////////////////

		/// @brief 通信設定を変更します。
		/// @param config 新しい通信設定
		/// @return 設定に成功した場合 true, それ以外の場合は false
		bool setConfig(const Config& config);

		////////////////////////////////////////////////////////////////
		//
		//	port
		//
		////////////////////////////////////////////////////////////////

		/// @brief シリアルポートの名前を返します。
		/// @return シリアルポートの名前
		[[nodiscard]]
		const String& port() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	available
		//
		////////////////////////////////////////////////////////////////

		/// @brief 受信済みのデータ（読み込みバッファ）のサイズ（バイト）を返します。
		/// @return 受信済みのデータ（読み込みバッファ）のサイズ（バイト）
		/// @remark この関数は即時に結果を返します。
		[[nodiscard]]
		size_t available();

		////////////////////////////////////////////////////////////////
		//
		//	clearInput, clearOutput, clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 読み込みバッファの内容を消去します。
		void clearInput();

		/// @brief 書き込みバッファの内容を消去します。
		void clearOutput();

		/// @brief 読み込みバッファと書き込みバッファの内容を消去します。
		void clear();

		////////////////////////////////////////////////////////////////
		//
		//	read
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した最大サイズ分だけデータを読み込みます。
		/// @param dst 読み込み先
		/// @param size 読み込むサイズ（バイト）
		/// @return 実際に読み込んだサイズ（バイト）
		/// @remark `.available() < size` の場合、タイムアウト設定にしたがって動作します。
		size_t read(void* dst, size_t size);

		/// @brief 受信済みのデータ（読み込みバッファ）からブロッキングせずに読み込みます。
		/// @tparam TriviallyCopyable 読み込む値の型
		/// @param to 読み込み先
		/// @return 読み込みに成功した場合 true, それ以外の場合は false
		/// @remark `.available() < sizeof(to)` の場合、読み込みに失敗します。
		bool read(Concept::TriviallyCopyable auto& to);

		////////////////////////////////////////////////////////////////
		//
		//	readBytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 受信済みのデータ（読み込みバッファ）をブロッキングせずに読み込みます。
		/// @return 読み込んだデータ
		Array<uint8> readBytes();

		/// @brief 受信済みのデータ（読み込みバッファ）をブロッキングせずに読み込みます。
		/// @param dst 読み込み先
		/// @return 読み込みに成功した場合 true, それ以外の場合は false
		bool readBytes(Array<uint8>& dst);

		////////////////////////////////////////////////////////////////
		//
		//	readLine
		//
		////////////////////////////////////////////////////////////////

		/// @brief 受信済みのデータ（読み込みバッファ）からブロッキングせずに 1 行分の文字列を読み込みます。
		/// @param line 読み込み先（上書きされます）
		/// @return 読み込みに成功した場合 true, それ以外の場合は false
		/// @remark 受信済みのデータに '\n' が含まれていない場合、読み込みに失敗し、line の内容は消去されます。
		/// @remark 行末の改行コード '\n' および '\r' は格納されません。
		bool readLine(String& line);

		////////////////////////////////////////////////////////////////
		//
		//	write
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータを書き込みます。
		/// @param src 書き込むデータの先頭ポインタ
		/// @param size 書き込むデータのサイズ（バイト）
		/// @return 書き込みに成功したバイト数
		size_t write(const void* src, size_t size);

		/// @brief バイナリデータを書き込みます。
		/// @tparam TriviallyCopyable 書き込む値の型
		/// @param from 書き込むデータ
		/// @return 書き込みに成功した場合 true, それ以外の場合は false
		bool write(const Concept::TriviallyCopyable auto& from);

		////////////////////////////////////////////////////////////////
		//
		//	writeByte
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 バイトのデータを書き込みます。
		/// @param byte 書き込むデータ
		/// @return 書き込みに成功した場合 true, それ以外の場合は false
		bool writeByte(uint8 byte);

		/// @brief 1 バイトのデータを書き込みます。
		/// @param byte 書き込むデータ
		/// @return 書き込みに成功した場合 true, それ以外の場合は false
		bool writeByte(Byte byte);

		////////////////////////////////////////////////////////////////
		//
		//	write, writeln
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を書き込みます。
		/// @param s 書き込む文字列
		/// @return 書き込みに成功した場合 true, それ以外の場合は false
		bool write(StringView s);

		/// @brief 文字列を書き込み、最後に改行コードを付加します。
		/// @param s 書き込む文字列
		/// @return 書き込みに成功した場合 true, それ以外の場合は false
		bool writeln(StringView s);

		////////////////////////////////////////////////////////////////
		//
		//	setBreak
		//
		////////////////////////////////////////////////////////////////

		/// @brief RS-232 のブレーク信号の状態を変更します。
		/// @param level on にする場合 true, off にする場合 false
		void setBreak(bool level);

		////////////////////////////////////////////////////////////////
		//
		//	setRTS
		//
		////////////////////////////////////////////////////////////////

		/// @brief RTS ハンドシェイクの状態を変更します。
		/// @param level on にする場合 true, off にする場合 false
		/// @remark デフォルトでは on です。
		void setRTS(bool level);

		////////////////////////////////////////////////////////////////
		//
		//	setDTR
		//
		////////////////////////////////////////////////////////////////

		/// @brief DTR ハンドシェイクの状態を変更します。
		/// @param level on にする場合 true, off にする場合 false
		/// @remark デフォルトでは on です。
		void setDTR(bool level);

		////////////////////////////////////////////////////////////////
		//
		//	waitForChange
		//
		////////////////////////////////////////////////////////////////

		/// @brief CTS, DSR, RI, CD の変更、もしくは何らかの割り込みまで待機します。
		/// @return 変更があった場合は true, それ以外の何かが起こった場合は false
		bool waitForChange();

		////////////////////////////////////////////////////////////////
		//
		//	getCTS
		//
		////////////////////////////////////////////////////////////////

		/// @brief CTS の状態を返します。
		/// @return CTS の状態
		[[nodiscard]]
		bool getCTS() const;

		////////////////////////////////////////////////////////////////
		//
		//	getDSR
		//
		////////////////////////////////////////////////////////////////

		/// @brief DSR の状態を返します。
		/// @return DSR の状態
		[[nodiscard]]
		bool getDSR() const;

		////////////////////////////////////////////////////////////////
		//
		//	getRI
		//
		////////////////////////////////////////////////////////////////

		/// @brief RI の状態を返します。
		/// @return RI の状態
		[[nodiscard]]
		bool getRI() const;

		////////////////////////////////////////////////////////////////
		//
		//	getCD
		//
		////////////////////////////////////////////////////////////////

		/// @brief CD の状態を返します。
		/// @return CD の状態
		[[nodiscard]]
		bool getCD() const;

	private:

		class SerialDetail;

		std::unique_ptr<SerialDetail> pImpl;
	};
}

# include "detail/Serial.ipp"
