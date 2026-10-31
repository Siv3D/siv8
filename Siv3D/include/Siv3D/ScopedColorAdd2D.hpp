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
	//	ScopedColorAdd2D
	//
	////////////////////////////////////////////////////////////////

	/// @brief スコープ内の 2D 描画に適用する加算カラーを設定します。
	/// @remark Target::Push は現在の加算カラーと RGB 各成分を加算し、Target::Set は置き換えます。
	/// @remark 乗算カラーとは独立に累積します。入力はアルファ乗算前の値で、累積値はクランプしません。
	/// @remark 描画のアルファは変更しません。ColorF / Float4 のアルファ成分は無視します。
	/// @remark 破棄時に以前の加算カラーを復元します。スコープは作成と逆の順序で終了してください。
	class ScopedColorAdd2D
	{
	public:

		/// @brief 現在のカラーとの合成方法
		enum class Target : uint8
		{
			Push, ///< 現在の加算カラーに RGB 各成分を加算します。
			Set,  ///< 現在の加算カラーを置き換えます。
		};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 描画状態を変更しないスコープを作成します。
		[[nodiscard]]
		ScopedColorAdd2D() = default;

		/// @brief 加算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param rgb RGB 各成分の加算量
		/// @param target 現在の加算カラーとの合成方法
		[[nodiscard]]
		explicit ScopedColorAdd2D(const ColorF& rgb, Target target = Target::Push);

		/// @brief 加算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param rgb RGB 各成分の加算量
		/// @param target 現在の加算カラーとの合成方法
		[[nodiscard]]
		explicit ScopedColorAdd2D(const Float4& rgb, Target target = Target::Push);

		/// @brief 加算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param rgb RGB 各成分の加算量
		/// @param target 現在の加算カラーとの合成方法
		[[nodiscard]]
		explicit ScopedColorAdd2D(const Float3& rgb, Target target = Target::Push);

		/// @brief 加算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param rgb RGB 共通の加算量
		/// @param target 現在の加算カラーとの合成方法
		[[nodiscard]]
		explicit ScopedColorAdd2D(double rgb, Target target = Target::Push);

		/// @brief 加算カラーを適用し、破棄時に以前の状態を復元します。
		/// @param r 赤の加算量
		/// @param g 緑の加算量
		/// @param b 青の加算量
		/// @param target 現在の加算カラーとの合成方法
		[[nodiscard]]
		ScopedColorAdd2D(double r, double g, double b, Target target = Target::Push);

		/// @brief 状態を復元する責任を移動します。移動元の破棄では復元しません。
		/// @param other 別のカラースコープ
		[[nodiscard]]
		ScopedColorAdd2D(ScopedColorAdd2D&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 保存した加算カラーを復元します。
		~ScopedColorAdd2D();

		////////////////////////////////////////////////////////////////
		//
		//	(deleted)
		//
		////////////////////////////////////////////////////////////////

		ScopedColorAdd2D(const ScopedColorAdd2D&) = delete;

		ScopedColorAdd2D& operator =(const ScopedColorAdd2D&) = delete;

	private:

		Optional<Float3> m_oldColor;

		void clear() noexcept;
	};
}
