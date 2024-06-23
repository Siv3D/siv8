//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <compare>
# include "Common.hpp"
# include "ISteadyClock.hpp"
# include "Duration.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	VariableSpeedStopwatch
	//
	////////////////////////////////////////////////////////////////

	/// @brief 変速ストップウォッチ
	class VariableSpeedStopwatch
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructors)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 変速ストップウォッチを作成します。
		/// @param startImmediately 即座に計測を開始する場合は `StartImmediately::Yes`
		/// @param pSteadyClock 基準時刻取得用のカスタム関数。nullptr の場合は `Time::GetMicrosec()` が使われます。
		[[nodiscard]]
		explicit VariableSpeedStopwatch(StartImmediately startImmediately = StartImmediately::No, ISteadyClock* pSteadyClock = nullptr) noexcept;

		/// @brief 変速ストップウォッチを作成します。
		/// @param speed 速度（1.0 で Stopwatch と同じ速度）
		/// @param startImmediately 即座に計測を開始する場合は `StartImmediately::Yes`
		/// @param pSteadyClock 基準時刻取得用のカスタム関数。nullptr の場合は `Time::GetMicrosec()` が使われます。
		[[nodiscard]]
		explicit VariableSpeedStopwatch(double speed, StartImmediately startImmediately = StartImmediately::No, ISteadyClock* pSteadyClock = nullptr) noexcept;

		/// @brief 指定した時間だけ経過し一時停止している状態の変速ストップウォッチを作成します。
		/// @param startTime 経過時間
		/// @param speed 速度（1.0 で Stopwatch と同じ速度）
		/// @param startImmediately 即座に計測を開始する場合は `StartImmediately::Yes`
		/// @param pSteadyClock 基準時刻取得用のカスタム関数。nullptr の場合は `Time::GetMicrosec()` が使われます。
		[[nodiscard]]
		VariableSpeedStopwatch(const Duration& startTime, double speed, StartImmediately startImmediately = StartImmediately::No, ISteadyClock* pSteadyClock = nullptr) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isStarted
		//
		////////////////////////////////////////////////////////////////

		/// @brief ストップウォッチが動作中であるかを示します（開始後の一時停止も動作中に含みます）。
		/// @return ストップウォッチが開始されている、または開始後一時停止中である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isStarted() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isPaused
		//
		////////////////////////////////////////////////////////////////

		/// @brief ストップウォッチが一時停止中であるかを示します。
		/// @return ストップウォッチが開始後一時停止中である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isPaused() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isRunning
		//
		////////////////////////////////////////////////////////////////

		/// @brief ストップウォッチが時間を計測中であるかを示します。
		/// @return ストップウォッチが開始されていて、なおかつ一時停止中でない場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isRunning() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	start
		//
		////////////////////////////////////////////////////////////////

		/// @brief ストップウォッチを開始・再開します。
		void start() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pause
		//
		////////////////////////////////////////////////////////////////

		/// @brief 開始しているストップウォッチを一時停止します。
		void pause() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	resume
		//
		////////////////////////////////////////////////////////////////

		/// @brief ストップウォッチが一時停止中である場合、再開します。
		void resume() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reset
		//
		////////////////////////////////////////////////////////////////

		/// @brief ストップウォッチを停止し、経過時間を 0 にリセットします。
		void reset() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	restart
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を 0 にリセットして、ストップウォッチを再び開始します。
		void restart() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief ストップウォッチの経過時間を変更します。
		/// @param time 新しく設定する経過時間
		/// @remark 指定した時間だけ経過している状態にします。計測中であるかの状態は引き継がれます。
		void set(const Duration& time) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setSpeed
		//
		////////////////////////////////////////////////////////////////

		void setSpeed(double speed) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getSpeed
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double getSpeed() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	d, d64, dF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を [日] で返します。
		/// @return 経過時間 [日]
		[[nodiscard]]
		int32 d() const noexcept;

		/// @brief 経過時間を [日] で返します。
		/// @return 経過時間 [日]
		[[nodiscard]]
		int64 d64() const noexcept;

		/// @brief 経過時間を [日] で返します。
		/// @return 経過時間 [日]
		[[nodiscard]]
		double dF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	h, h64, hF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を [時] で返します。
		/// @return 経過時間 [時]
		[[nodiscard]]
		int32 h() const noexcept;

		/// @brief 経過時間を [時] で返します。
		/// @return 経過時間 [時]
		[[nodiscard]]
		int64 h64() const noexcept;

		/// @brief 経過時間を [時] で返します。
		/// @return 経過時間 [時]
		[[nodiscard]]
		double hF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	min, mi64, miF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を [分] で返します。
		/// @return 経過時間 [分]
		[[nodiscard]]
		int32 min() const noexcept;

		/// @brief 経過時間を [分] で返します。
		/// @return 経過時間 [分]
		[[nodiscard]]
		int64 min64() const noexcept;

		/// @brief 経過時間を [分] で返します。
		/// @return 経過時間 [分]
		[[nodiscard]]
		double minF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	s, s64, sF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を [秒] で返します。
		/// @return 経過時間 [秒]
		[[nodiscard]]
		int32 s() const noexcept;

		/// @brief 経過時間を [秒] で返します。
		/// @return 経過時間 [秒]
		[[nodiscard]]
		int64 s64() const noexcept;

		/// @brief 経過時間を [秒] で返します。
		/// @return 経過時間 [秒]
		[[nodiscard]]
		double sF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ms, ms64, msF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を [ミリ秒] で返します。
		/// @return 経過時間 [ミリ秒]
		[[nodiscard]]
		int32 ms() const noexcept;

		/// @brief 経過時間を [ミリ秒] で返します。
		/// @return 経過時間 [ミリ秒]
		[[nodiscard]]
		int64 ms64() const noexcept;

		/// @brief 経過時間を [ミリ秒] で返します。
		/// @return 経過時間 [ミリ秒]
		[[nodiscard]]
		double msF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	us, us64, usF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を [マイクロ秒] で返します。
		/// @return 経過時間 [マイクロ秒]
		[[nodiscard]]
		int64 us() const noexcept;

		/// @brief 経過時間を [マイクロ秒] で返します。
		/// @return 経過時間 [マイクロ秒]
		[[nodiscard]]
		int64 us64() const noexcept;

		/// @brief 経過時間を [マイクロ秒] で返します。
		/// @return 経過時間 [マイクロ秒]
		[[nodiscard]]
		double usF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	elapsed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を返します。
		/// @return 経過時間
		[[nodiscard]]
		Duration elapsed() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を文字列に変換します。
		/// DD		日 (00-)
		/// D		日 (0-)
		/// dd		日 (00-)
		/// d		日 (0-)
		/// HH		時 (00-)
		/// H		時 (0-)
		/// hh		時 (00-24)
		/// h		時 (0-24)
		/// MM		分 (00-)
		/// M		分 (0-)
		/// mm		分 (00-59)
		/// m		分 (0-59)
		/// SS		秒 (00-)
		/// S		秒 (0-)
		/// ss		秒 (00-59)
		/// s		秒 (0-59)
		/// X		ミリ秒 (0-)
		/// x		小数点以下 1 桁秒 (0-9)
		/// xx		小数点以下 2 桁秒 (00-99)
		/// xxx		小数点以下 3 桁秒 (000-999)
		/// @param format フォーマット指定
		/// @return フォーマットされた時間
		[[nodiscard]]
		String format(StringView format = U"H:mm:ss.xx") const;

		////////////////////////////////////////////////////////////////
		//
		//	operator <=>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 経過時間を比較します。
		/// @param s ストップウォッチ
		/// @param timeUs 比較する時間
		/// @return 比較の結果
		[[nodiscard]]
		friend auto operator <=>(const VariableSpeedStopwatch& s, const MicrosecondsF& timeUs) noexcept
		{
			return (s.usF() <=> timeUs.count());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		friend std::ostream& operator <<(std::ostream& os, const VariableSpeedStopwatch& value);

		friend std::wostream& operator <<(std::wostream& os, const VariableSpeedStopwatch& value);

		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const VariableSpeedStopwatch& value);

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 
		/// @param formatData 
		/// @param value 
		friend void Formatter(FormatData& formatData, const VariableSpeedStopwatch& value);

	private:

		static constexpr double DefaultSpeed = 1.0;

		double m_speed = DefaultSpeed;

		mutable int64 m_lastTimeNanosec = 0;

		mutable int64 m_accumulatedNanosec = 0;

		ISteadyClock* m_pSteadyClock = nullptr;

		bool m_isStarted = false;

		bool m_pausing = true;

		[[nodiscard]]
		int64 ns() const noexcept;
	};
}

# include "detail/VariableSpeedStopwatch.ipp"
