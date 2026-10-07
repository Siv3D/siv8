//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "Spline2D.hpp"
# include "DistanceMode.hpp"
# include "detail/Bezier3ArcLengthTable.hpp"

namespace s3d
{
	/// @brief Spline2D 上の位置を、始点からの距離で繰り返し求めるためのクラスです。
	/// 例えば速度が毎秒 120 なら、距離に 120 * Scene::DeltaTime() を加えて pointAtDistance() を呼びます。
	/// @remark 曲線を所有するため、元の Spline2D を変更・破棄しても使い続けられます。形を変えた場合は作り直してください。
	/// @remark 作成時に長さを計算します。位置の検索時には領域の確保や経路全体の再計算を行いません。
	/// @remark 長さと距離からの位置は数値計算による近似です。描画用の折れ線の分割数には依存しません。
	class Spline2DMeasure
	{
	public:

		/// @brief 空の経路の計測結果を作成します。
		[[nodiscard]]
		Spline2DMeasure() = default;

		/// @brief 曲線を保存し、距離検索の準備をします。
		/// @param spline 計測する曲線。左辺値はコピーし、std::move() した曲線は移動して取り込みます。
		[[nodiscard]]
		explicit Spline2DMeasure(Spline2D spline);

		/// @brief 曲線と計測結果をコピーします。
		Spline2DMeasure(const Spline2DMeasure&) = default;

		/// @brief 曲線と計測結果を移動します。
		Spline2DMeasure(Spline2DMeasure&&) noexcept = default;

		/// @brief 曲線と計測結果を置き換えます。
		/// @return *this
		Spline2DMeasure& operator =(Spline2DMeasure other) noexcept;

		/// @brief 計測した経路が空の場合に true を返します。
		[[nodiscard]]
		bool isEmpty() const noexcept;

		/// @brief 経路全体の長さを返します。
		/// @return 座標と同じ単位の長さ。空の経路なら 0。
		[[nodiscard]]
		double length() const noexcept;

		/// @brief 計測に使った曲線を参照します。
		/// @return 所有する曲線。このオブジェクトの破棄・代入・swap() まで有効です。
		[[nodiscard]]
		const Spline2D& spline() const& noexcept;
		const Spline2D& spline() const&& = delete;

		/// @brief 始点から指定した距離にある区間とパラメータを求めます。
		/// @param distanceFromStart 始点から曲線に沿って測った距離。有限の値。
		/// @param mode 始点より前、または終点より先の距離の扱い
		/// @return 曲線上の位置。長さが 0 の経路では { 0, 0 }。
		/// @throws std::out_of_range 経路が空の場合
		/// @remark 内部の接続点では後ろの区間の t = 0 を選び、長さ 0 の区間を飛ばします。
		/// @remark Clamp の始点は最初の区間の t = 0、終点は最後の区間の t = 1 です。
		[[nodiscard]]
		SplineLocation locationAtDistance(double distanceFromStart, DistanceMode mode = DistanceMode::Clamp) const;

		/// @brief 始点から指定した距離にある座標を求めます。
		/// @param distanceFromStart 始点から曲線に沿って測った距離。有限の値。
		/// @param mode 始点より前、または終点より先の距離の扱い
		/// @return 曲線上の座標。長さが 0 の経路では始点。
		/// @throws std::out_of_range 経路が空の場合
		[[nodiscard]]
		Vec2 pointAtDistance(double distanceFromStart, DistanceMode mode = DistanceMode::Clamp) const;

		/// @brief 区間とパラメータで指定した位置までの距離を求めます。
		/// @param location 有効な区間番号と、0～1 の有限のパラメータ
		/// @return 始点からその位置まで、曲線に沿って測った距離
		/// @throws std::out_of_range 区間番号が経路の区間数以上の場合
		/// @remark 閉じた経路の最後の区間の t = 1 は、0 ではなく全長を返します。
		[[nodiscard]]
		double distanceAt(SplineLocation location) const;

		/// @brief 別の計測結果と、所有する曲線を含めて交換します。
		void swap(Spline2DMeasure& other) noexcept;

	private:

		Spline2D m_spline;
		Array<detail::Bezier3ArcLengthTable> m_tables;
		Array<double> m_prefixLengths;
	};
}

# include "detail/Spline2DMeasure.ipp"
