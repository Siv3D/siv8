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
# include "Common.hpp"
# include "Time.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ISteadyClock
	//
	////////////////////////////////////////////////////////////////

	/// @brief タイムポイント提供インタフェース
	/// @remark Stopwatch や Timer クラスが時間を計測する際に参照するタイムポイントを提供します。
	class ISteadyClock
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~ISteadyClock() = default;

		////////////////////////////////////////////////////////////////
		//
		//	getMicrosec
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイムポイント（マイクロ秒）を返します。
		/// @return タイムポイント（マイクロ秒）
		[[nodiscard]]
		virtual uint64 getMicrosec() noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getNanosec
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイムポイント（ナノ秒）を返します。
		/// @return タイムポイント（ナノ秒）
		[[nodiscard]]
		virtual uint64 getNanosec() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetMicrosec
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイムポイント（マイクロ秒）を返します。
		/// @param pSteadyClock タイムポイント提供オブジェクトのポインタ
		/// @return タイムポイント（マイクロ秒）
		[[nodiscard]]
		static uint64 GetMicrosec(ISteadyClock* pSteadyClock) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetNanosec
		//
		////////////////////////////////////////////////////////////////

		/// @brief タイムポイント（ナノ秒）を返します。
		/// @param pSteadyClock タイムポイント提供オブジェクトのポインタ
		/// @return タイムポイント（ナノ秒）
		[[nodiscard]]
		static uint64 GetNanosec(ISteadyClock* pSteadyClock) noexcept;
	};
}

# include "detail/ISteadyClock.ipp"
