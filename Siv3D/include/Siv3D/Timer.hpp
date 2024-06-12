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
# include "String.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Timer
	//
	////////////////////////////////////////////////////////////////

	/// @brief タイマー
	class Timer
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructors)
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーを作成します。
		/// @param duration カウントダウン時間
		/// @param startImmediately 即座に計測を開始する場合は `StartImmediately::Yes`
		/// @param pSteadyClock 基準時刻取得用のカスタム関数。nullptr の場合は `Time::GetMicrosec()` が使われます。
		[[nodiscard]]
		explicit Timer(const Duration& duration = SecondsF{ 0 }, StartImmediately startImmediately = StartImmediately::No, ISteadyClock* pSteadyClock = nullptr) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isStarted
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーが動作中であるかを示します（開始後の一時停止も動作中に含みます）。
		/// @return タイマーが開始されている、または開始後一時停止中である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isStarted() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isPaused
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーが一時停止中であるかを示します。
		/// @return タイマーが開始後一時停止中である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isPaused() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isRunning
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーが時間を計測中であるかを示します。
		/// @return タイマーが開始されていて、なおかつ一時停止中でない場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isRunning() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reachedZero
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーの残り時間が 0 に達しているかを返します。
		/// @return タイマーの残り時間が 0 に達している場合 true, それ以外の場合は false
		[[nodiscard]]
		bool reachedZero() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	start
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーを開始・再開します。
		void start() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pause
		//
		////////////////////////////////////////////////////////////////

		/// @brief 開始しているタイマーを一時停止します。
		void pause() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	resume
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーが一時停止中である場合、再開します。
		void resume() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reset
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーを停止し、残り時間を最初の状態にリセットします。
		void reset() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	restart
		//
		////////////////////////////////////////////////////////////////

		/// @brief 残り時間を最初の状態にリセットしてタイマーを再び開始します。
		void restart() noexcept;

		/// @brief カウントダウン時間を変更して、タイマーを再び最初から開始します。
		/// @param duration 
		void restart(const Duration& duration) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーのカウントダウン時間を変更し、残り時間も最初からにリセットします。
		/// @param duration 新しいカウントダウン時間
		void set(const Duration& duration) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setRemaining
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーの残り時間を変更します。
		/// @param remaining タイマーの新しい残り時間
		void setRemaining(const Duration& remaining) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	d, d64, dF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 残り時間を [日] で返します。
		/// @return 残り時間 [日]
		[[nodiscard]]
		int32 d() const noexcept;

		/// @brief 残り時間を [日] で返します。
		/// @return 残り時間 [日]
		[[nodiscard]]
		int64 d64() const noexcept;

		/// @brief 残り時間を [日] で返します。
		/// @return 残り時間 [日]
		[[nodiscard]]
		double dF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	h, h64, hF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 残り時間を [時] で返します。
		/// @return 残り時間 [時]
		[[nodiscard]]
		int32 h() const noexcept;

		/// @brief 残り時間を [時] で返します。
		/// @return 残り時間 [時]
		[[nodiscard]]
		int64 h64() const noexcept;

		/// @brief 残り時間を [時] で返します。
		/// @return 残り時間 [時]
		[[nodiscard]]
		double hF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	min, mi64, miF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 残り時間を [分] で返します。
		/// @return 残り時間 [分]
		[[nodiscard]]
		int32 min() const noexcept;

		/// @brief 残り時間を [分] で返します。
		/// @return 残り時間 [分]
		[[nodiscard]]
		int64 min64() const noexcept;

		/// @brief 残り時間を [分] で返します。
		/// @return 残り時間 [分]
		[[nodiscard]]
		double minF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	s, s64, sF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 残り時間を [秒] で返します。
		/// @return 残り時間 [秒]
		[[nodiscard]]
		int32 s() const noexcept;

		/// @brief 残り時間を [秒] で返します。
		/// @return 残り時間 [秒]
		[[nodiscard]]
		int64 s64() const noexcept;

		/// @brief 残り時間を [秒] で返します。
		/// @return 残り時間 [秒]
		[[nodiscard]]
		double sF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ms, ms64, msF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 残り時間を [ミリ秒] で返します。
		/// @return 残り時間 [ミリ秒]
		[[nodiscard]]
		int32 ms() const noexcept;

		/// @brief 残り時間を [ミリ秒] で返します。
		/// @return 残り時間 [ミリ秒]
		[[nodiscard]]
		int64 ms64() const noexcept;

		/// @brief 残り時間を [ミリ秒] で返します。
		/// @return 残り時間 [ミリ秒]
		[[nodiscard]]
		double msF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	us, us64, usF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 残り時間を [マイクロ秒] で返します。
		/// @return 残り時間 [マイクロ秒]
		[[nodiscard]]
		int64 us() const noexcept;

		/// @brief 残り時間を [マイクロ秒] で返します。
		/// @return 残り時間 [マイクロ秒]
		[[nodiscard]]
		int64 us64() const noexcept;

		/// @brief 残り時間を [マイクロ秒] で返します。
		/// @return 残り時間 [マイクロ秒]
		[[nodiscard]]
		double usF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	duration
		//
		////////////////////////////////////////////////////////////////

		/// @brief 設定されているカウントダウン時間を返します。
		/// @return 設定されているカウントダウン時間
		[[nodiscard]]
		Duration duration() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	remaining
		//
		////////////////////////////////////////////////////////////////

		/// @brief 残り時間を返します。
		/// @return 残り時間
		[[nodiscard]]
		Duration remaining() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	progress1_0
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーの進み具合を 1 から 0 の範囲で返します。
		/// @return タイマーの進み具合 (1.0 から始まり、0.0 で終わる), カウントダウン時間が 0 の場合は 0
		[[nodiscard]]
		double progress1_0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	progress0_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイマーの進み具合を 0 から 1 の範囲で返します。
		/// @return タイマーの進み具合 (0.0 から始まり、1.0 で終わる), カウントダウン時間が 0 の場合は 1
		[[nodiscard]]
		double progress0_1() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief 残り時間を文字列に変換します。
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

		/// @brief 残り時間を比較します。
		/// @param t タイマー
		/// @param timeUs 比較する時間
		/// @return 比較の結果
		[[nodiscard]]
		friend auto operator <=>(const Timer& t, const MicrosecondsF& timeUs) noexcept
		{
			return (t.usF() <=> timeUs.count());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 
		/// @tparam CharType 
		/// @param output 
		/// @param value 
		/// @return 
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Timer& value)
		{
			return output << value.format();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 
		/// @param formatData 
		/// @param value 
		friend void Formatter(FormatData& formatData, const Timer& value);

	private:

		int64 m_durationMicrosec = 0;

		int64 m_startTimeMicrosec = 0;

		int64 m_remainingMicrosec = 0;

		ISteadyClock* m_pSteadyClock = nullptr;

		bool m_isStarted = false;

		bool m_pausing = true;
	};
}

# include "detail/Timer.ipp"
