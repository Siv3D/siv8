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
# include "Optional.hpp"
# include "PointVector.hpp"
# include "ColorHSV.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ScopedColorMul2D
	//
	////////////////////////////////////////////////////////////////

	/// @brief スコープ内の 2D 描画に適用する乗算カラーを設定します。
	/// @remark Target::Push は現在の乗算カラーと RGBA 各成分を乗算し、Target::Set は置き換えます。
	/// @remark 加算カラーとは独立に累積します。入力はアルファ乗算前の値で、累積値はクランプしません。
	/// @remark 破棄時に以前の乗算カラーを復元します。スコープは作成と逆の順序で終了してください。
	class ScopedColorMul2D
	{
	public:

		/// @brief 現在のカラーとの合成方法
		enum class Target : uint8
		{
			Push, ///< 現在の乗算カラーに RGBA 各成分を乗算します。
			Set,  ///< 現在の乗算カラーを置き換えます。
		};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 描画状態を変更しないスコープを作成します。
		[[nodiscard]]
		ScopedColorMul2D() = default;

		/// @brief 乗算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param color 乗算カラー
		/// @param target 現在の乗算カラーとの合成方法
		[[nodiscard]]
		explicit ScopedColorMul2D(const ColorF& color, Target target = Target::Push);

		/// @brief 乗算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param color RGBA 各成分の乗算係数
		/// @param target 現在の乗算カラーとの合成方法
		[[nodiscard]]
		explicit ScopedColorMul2D(const Float4& color, Target target = Target::Push);

		/// @brief 乗算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param rgb RGB 各成分の乗算係数
		/// @param a アルファの乗算係数
		/// @param target 現在の乗算カラーとの合成方法
		[[nodiscard]]
		explicit ScopedColorMul2D(const Float3& rgb, double a = 1.0, Target target = Target::Push);

		/// @brief 乗算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param rgb RGB 各成分の乗算係数。アルファの乗算係数は 1.0
		/// @param target 現在の乗算カラーとの合成方法
		[[nodiscard]]
		ScopedColorMul2D(const Float3& rgb, Target target);

		/// @brief 乗算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param rgb RGB 共通の乗算係数
		/// @param a アルファの乗算係数
		/// @param target 現在の乗算カラーとの合成方法
		[[nodiscard]]
		explicit ScopedColorMul2D(double rgb, double a = 1.0, Target target = Target::Push);

		/// @brief 乗算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param rgb RGB 共通の乗算係数。アルファの乗算係数は 1.0
		/// @param target 現在の乗算カラーとの合成方法
		[[nodiscard]]
		ScopedColorMul2D(double rgb, Target target);

		/// @brief 乗算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param r 赤の乗算係数
		/// @param g 緑の乗算係数
		/// @param b 青の乗算係数
		/// @param a アルファの乗算係数
		/// @param target 現在の乗算カラーとの合成方法
		[[nodiscard]]
		ScopedColorMul2D(double r, double g, double b, double a = 1.0, Target target = Target::Push);

		/// @brief 乗算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param r 赤の乗算係数
		/// @param g 緑の乗算係数
		/// @param b 青の乗算係数。アルファの乗算係数は 1.0
		/// @param target 現在の乗算カラーとの合成方法
		[[nodiscard]]
		ScopedColorMul2D(double r, double g, double b, Target target);

		/// @brief 状態を復元する責任を移動します。移動元の破棄では復元しません。
		/// @param other 別のカラースコープ
		[[nodiscard]]
		ScopedColorMul2D(ScopedColorMul2D&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 保存した乗算カラーを復元します。
		~ScopedColorMul2D();

		////////////////////////////////////////////////////////////////
		//
		//	(deleted)
		//
		////////////////////////////////////////////////////////////////

		ScopedColorMul2D(const ScopedColorMul2D&) = delete;

		ScopedColorMul2D& operator =(const ScopedColorMul2D&) = delete;

	private:

		Optional<Float4> m_oldColor;

		void clear() noexcept;
	};
}
