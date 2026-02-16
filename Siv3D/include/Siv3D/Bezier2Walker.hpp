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
# include "Bezier.hpp"

namespace s3d
{
	/// @brief 二次ベジェ曲線上を距離で進めるためのユーティリティ
	///
	/// 現在のパラメータ t（0.0～1.0）を保持し、「指定した距離だけ進む / 戻る」操作で t を更新します。
	/// 曲線上の移動を簡単に扱いたい（毎フレーム一定距離で進めたい等）用途向けの軽量ユーティリティです。
	///
	/// @remark
	/// 更新は弧長を厳密に解くのではなく、曲線の速度 |B'(t)| を用いた近似で行います。
	/// 高精度が必要な場合は `Bezier2::tAtLength()` などの利用を検討してください。
	struct Bezier2Walker
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Bezier2Walker() = default;

		/// @brief 二次ベジェ曲線からウォーカーを作成します。
		/// @param bezier 二次ベジェ曲線
		///
		/// @remark
		/// 内部では導関数 `B'(t) = m_d0 * t + m_d1` の係数のみを保持します。
		[[nodiscard]]
		explicit constexpr Bezier2Walker(const Bezier2& bezier) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setT, t
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のパラメータ t を設定します。
		/// @param t パラメータ（0.0～1.0）
		constexpr void setT(const double t) noexcept;

		/// @brief 現在のパラメータ t を返します。
		/// @return パラメータ t（0.0～1.0）
		[[nodiscard]]
		constexpr double t() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	advanceBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線上を指定距離だけ前進/後退し、t を更新します（近似）。
		/// @param distance 進める距離（負の場合は後退）
		/// @param iterations 反復回数（大きいほど高精度、最低 1）
		/// @return 更新後のパラメータ t（0.0～1.0）
		double advanceBy(const double distance, int32 iterations = 8) noexcept;

		/// @brief 曲線上を指定距離だけ前進/後退し、t を更新します（近似）。
		/// @param distance 進める距離（負の場合は後退）
		/// @param remainder 終端で止まったときの余り距離（distance と同符号）
		/// @param iterations 反復回数（大きいほど高精度、最低 1）
		/// @return 更新後のパラメータ t（0.0～1.0）
		double advanceBy(const double distance, double& remainder, int32 iterations = 8) noexcept;

	private:

		/// @brief 導関数の一次係数（B'(t) = m_d0 * t + m_d1）
		Vec2 m_d0{ 0, 0 };

		/// @brief 導関数の定数項（B'(t) = m_d0 * t + m_d1）
		Vec2 m_d1{ 0, 0 };

		/// @brief 現在のパラメータ（0.0～1.0）
		double m_t = 0.0;
	};
}

# include "detail/Bezier2Walker.ipp"
