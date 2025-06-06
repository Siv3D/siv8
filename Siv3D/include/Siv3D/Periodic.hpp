﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "Duration.hpp"
# include "Scene.hpp"

namespace s3d
{
	//
	// [Sine0_1]
	// 
	// 1.0 | **      **      **      **
	//     |*  *    *  *    *  *    *
	// 0.5 *    *  *    *  *    *  *
	//     |    *  *    *  *    *  *
	// 0.0 |     **      **      **
	//     ----------------------------
	//     0       1       2       3
	//
	// [Square0_1]
	// 
	// 1.0 ****    ****    ****    ****
	//     |
	// 0.5 |
	//     |  
	// 0.0 |   ****    ****    ****   
	//     ----------------------------
	//     0       1       2       3
	//
	// [Triangle0_1]
	// 
	// 1.0 |   *       *       *       
	//     |  * *     * *     * *     *
	// 0.5 | *   *   *   *   *   *   *
	//     |*     * *     * *     * *
	// 0.0 *       *       *       *
	//     ----------------------------
	//     0       1       2       3
	//
	// [Sawtooth0_1]
	// 
	// 1.0 |       -       -       -
	//     |     *       *       *
	// 0.5 |   *       *       *       *
	//     | *       *       *       *
	// 0.0 *       +       +       +
	//     ----------------------------
	//     0       1       2       3
	//
	// [Jump0_1]
	// 
	// 1.0 |  -^-     -^-     -^-     -
	//     | *   *   *   *   *   *   *
	// 0.5 |*     * *     * *     * *
	//     |*     * *     * *     * *
	// 0.0 *       *       *       *
	//     ----------------------------
	//     0       1       2       3
	//
	// 
	// 
	// [Sine1_1]
	// 
	// 1.0 | **      **      **      **
	//     |*  *    *  *    *  *    *
	// 0.0 *    *  *    *  *    *  *
	//     |    *  *    *  *    *  *
	//-1.0 |     **      **      **
	//     ----------------------------
	//     0       1       2       3
	//
	// [Square1_1]
	// 
	// 1.0 ****    ****    ****    ****
	//     |
	// 0.5 |
	//     |  
	//-1.0 |   ****    ****    ****   
	//     ----------------------------
	//     0       1       2       3
	//
	// [Triangle1_1]
	// 
	// 1.0 |   *       *       *       
	//     |  * *     * *     * *     *
	// 0.0 | *   *   *   *   *   *   *
	//     |*     * *     * *     * *
	//-1.0 *       *       *       *
	//     ----------------------------
	//     0       1       2       3
	//
	// [Sawtooth1_1]
	// 
	// 1.0 |       -       -       -
	//     |     *       *       *
	// 0.0 |   *       *       *       *
	//     | *       *       *       *
	//-1.0 *       +       +       +
	//     ----------------------------
	//     0       1       2       3
	//
	// [Jump1_1]
	// 
	// 1.0 |  -^-     -^-     -^-     -
	//     | *   *   *   *   *   *   *
	// 0.0 |*     * *     * *     * *
	//     |*     * *     * *     * *
	//-1.0 *       *       *       *
	//     ----------------------------
	//     0       1       2       3
	//

	namespace Periodic
	{
		////////////////////////////////////////////////////////////////
		//
		//	Sine0_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief サインカーブに従って、周期的に [0.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]
		double Sine0_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief サインカーブに従って、周期的に [0.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]
		double Sine0_1(const Duration& period, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Square0_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 矩形波に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]	
		double Square0_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief 矩形波に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]
		double Square0_1(const Duration& period, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Pulse0_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 矩形波に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param dutyCycle デューティー比、周期内で波形の大きさが 1.0 である時間の割合
		/// @param t 経過時間（秒）
		/// @return 0.0 または 1.0
		/// @remark デューティー比が 0.5 のとき `Square0_1()` と一致します。
		[[nodiscard]]
		double Pulse0_1(double periodSec, double dutyCycle, double t = Scene::Time()) noexcept;

		/// @brief 矩形波に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param dutyCycle デューティー比、周期内で波形の大きさが 1.0 である時間の割合
		/// @param t 経過時間（秒）
		/// @return 0.0 または 1.0
		/// @remark デューティー比が 0.5 のとき `Square0_1()` と一致します。
		[[nodiscard]]
		double Pulse0_1(const Duration& period, double dutyCycle, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Triangle0_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角波に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]
		double Triangle0_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief 三角波に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]
		double Triangle0_1(const Duration& period, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Sawtooth0_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief のこぎり波に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]
		double Sawtooth0_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief のこぎり波に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]
		double Sawtooth0_1(const Duration& period, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Jump0_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief ジャンプする運動に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]
		double Jump0_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief ジャンプする運動に従って、周期的に [0.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [0.0, 1.0] の範囲の値
		[[nodiscard]]
		double Jump0_1(const Duration& period, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Sine1_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief サインカーブに従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Sine1_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief サインカーブに従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Sine1_1(const Duration& period, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Square1_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 矩形波に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Square1_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief 矩形波に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Square1_1(const Duration& period, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Pulse1_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 矩形波に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param dutyCycle デューティー比、周期内で波形の大きさが 1.0 である時間の割合
		/// @param t 経過時間（秒）
		/// @return -1.0 または 1.0
		/// @remark デューティー比が 0.5 のとき `Square1_1()` と一致します。
		[[nodiscard]]
		double Pulse1_1(double periodSec, double dutyCycle, double t = Scene::Time()) noexcept;

		/// @brief 矩形波に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param dutyCycle デューティー比、周期内で波形の大きさが 1.0 である時間の割合
		/// @param t 経過時間（秒）
		/// @return -1.0 または 1.0
		/// @remark デューティー比が 0.5 のとき `Square1_1()` と一致します。
		[[nodiscard]]
		double Pulse1_1(const Duration& period, double dutyCycle, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Triangle1_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角波に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Triangle1_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief 三角波に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Triangle1_1(const Duration& period, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Sawtooth1_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief のこぎり波に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Sawtooth1_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief のこぎり波に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Sawtooth1_1(const Duration& period, double t = Scene::Time()) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Jump1_1
		//
		////////////////////////////////////////////////////////////////

		/// @brief ジャンプする運動に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param periodSec 周期（秒）
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Jump1_1(double periodSec, double t = Scene::Time()) noexcept;

		/// @brief ジャンプする運動に従って、周期的に [-1.0, 1.0] の値を返します。
		/// @param period 周期
		/// @param t 経過時間（秒）
		/// @return [-1.0, 1.0] の範囲の値
		[[nodiscard]]
		double Jump1_1(const Duration& period, double t = Scene::Time()) noexcept;
	}
}

# include "detail/Periodic.ipp"
