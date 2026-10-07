//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "Bezier.hpp"
# include "LineString.hpp"
# include "Optional.hpp"
# include "CatmullRomParameterization.hpp"
# include "SplineClosestPoint2D.hpp"

namespace s3d
{
	/// @brief 複数の三次ベジェ曲線をつないだ 2D の経路です。
	/// 通過させたい点列や、端点がつながった Bezier3 の列から作成できます。
	/// @remark 距離に従って物体を移動させる場合は、この曲線から Spline2DMeasure を作成してください。
	/// @remark 座標には有限の値を指定します。区間の接続は保証しますが、接線や曲率の連続性は生成方法によります。
	class Spline2D
	{
	public:

		/// @brief 区間を持たない空の経路を作成します。
		[[nodiscard]]
		Spline2D() = default;

		/// @brief 順番にすべての点を通る曲線を作成します。
		/// @param points 通過点の列。入力の点はコピーされ、作成後に変更しても曲線には影響しません。
		/// @param closeRing 最後の点から最初の点へ曲線をつなぐ場合は Yes
		/// @param parameterization 点の間隔に応じた曲げ方
		/// @return 作成した曲線。開いた経路で、連続する同一点を除いた点が 2 個未満なら空です。
		/// @throws std::invalid_argument 閉じた経路で、重複を整理した通過点が 3 個未満の場合
		/// @remark 連続する同一点を 1 点にまとめます。閉じた経路では、末尾に重複して置いた始点も取り除きます。
		/// @remark 開いた経路の両端は、隣の点へ向かう片側の接線を使います。2 点の場合は直線になります。
		/// @remark 閉じた経路では、先頭と末尾も隣り合う通過点として接線を求めます。
		[[nodiscard]]
		static Spline2D FromCatmullRom(std::span<const Vec2> points, CloseRing closeRing = CloseRing::No,
			CatmullRomParameterization parameterization = CatmullRomParameterization::Centripetal);

		/// @brief 端点がつながった三次ベジェ曲線の列から経路を作成します。
		/// @param segments 接続する区間の列
		/// @param closeRing 始点と終点がつながる閉じた経路として扱う場合は Yes
		/// @return 入力をコピーした経路。区間がなければ空です。
		/// @throws std::invalid_argument 隣り合う区間の p3 と p0 が一致しない場合。または閉じた経路の終点と始点が一致しない場合。
		/// @remark 座標を補正したり、すき間を埋める区間を追加したりしません。接続点で曲がる経路も作成できます。
		[[nodiscard]]
		static Spline2D FromBezierSegments(std::span<const Bezier3> segments, CloseRing closeRing = CloseRing::No);

		/// @brief 区間数を返します。通過点の数ではありません。
		[[nodiscard]]
		size_t segmentCount() const noexcept;

		/// @brief 区間がない場合に true を返します。
		[[nodiscard]]
		bool isEmpty() const noexcept;

		/// @brief 閉じた経路として作成され、区間がある場合に true を返します。
		/// @remark 始点と終点の座標が同じだけでは、閉じた経路とはみなしません。
		[[nodiscard]]
		bool isClosed() const noexcept;

		/// @brief 指定した区間を参照します。
		/// @param index 0 から始まる区間番号
		/// @return 指定区間の三次ベジェ曲線。接線・曲率・区間の分割などに利用できます。
		/// @throws std::out_of_range index が segmentCount() 以上の場合
		/// @remark 参照は経路の破棄・代入・clear()・反転まで有効です。
		[[nodiscard]]
		const Bezier3& segment(size_t index) const&;
		const Bezier3& segment(size_t index) const&& = delete;

		/// @brief 全区間を読み取り専用の span で参照します。
		/// @return 区間の列。経路の破棄・代入・clear()・反転まで有効です。
		[[nodiscard]]
		std::span<const Bezier3> segments() const& noexcept;
		std::span<const Bezier3> segments() const&& = delete;

		/// @brief 区間番号とパラメータから曲線上の座標を求めます。
		/// @param location 有効な区間番号と、0～1 の有限のパラメータ
		/// @return 曲線上の座標
		/// @throws std::out_of_range 区間番号が segmentCount() 以上の場合
		[[nodiscard]]
		Vec2 pointAt(SplineLocation location) const;

		/// @brief 指定した点に最も近い曲線上の位置を求めます。
		/// @param point 検索の基準となる点。座標は有限の値。
		/// @return 区間・パラメータ・座標・距離の二乗。空の経路なら none。
		/// @remark 計算結果の距離が等しい候補がある場合は、区間番号が小さいものを選びます。
		[[nodiscard]]
		Optional<SplineClosestPoint2D> computeClosestPoint(Vec2 point) const;

		/// @brief 経路全体を囲む軸平行の長方形を求めます。
		/// @return 曲線を囲む長方形。空の経路なら none。
		[[nodiscard]]
		Optional<RectF> computeBoundingRect() const;

		/// @brief 各区間のパラメータを等分し、折れ線に変換します。
		/// @param subdivisionsPerSegment 区間ごとの分割数。1 未満は 1 として扱います。
		/// @return 折れ線。空の経路なら空です。
		/// @remark 閉じた経路では末尾に始点を重複させません。描画には drawClosed() を使います。
		[[nodiscard]]
		LineString toLineString(int32 subdivisionsPerSegment = 24) const;

		/// @brief 各区間のパラメータを等分し、既存の折れ線へ書き込みます。
		/// @param destination 結果で置き換える折れ線。確保済みの領域を再利用します。
		/// @param subdivisionsPerSegment 区間ごとの分割数。1 未満は 1 として扱います。
		/// @remark 空の経路なら destination を空にします。閉じた経路では末尾に始点を重複させません。
		void toLineString(LineString& destination, int32 subdivisionsPerSegment = 24) const;

		/// @brief 曲がりの大きい部分を細かく分割し、描画用の折れ線を作成します。
		/// @param maxError 折れ線とのずれの目安。座標と同じ単位の有限の正の値。
		/// @param maxDepth 区間ごとの最大分割深さ。0～20。0 は各区間の両端だけを使います。
		/// @return 折れ線。空の経路なら空です。閉じた経路では末尾に始点を重複させません。
		/// @throws std::invalid_argument maxError が 0 以下、または maxDepth が 0～20 の範囲外の場合
		/// @remark 分割上限に達した部分では maxError を超えるずれが残る場合があります。
		[[nodiscard]]
		LineString toLineStringAdaptive(double maxError = 0.48, int32 maxDepth = 10) const;

		/// @brief 曲がりに応じて分割した折れ線を、既存の領域へ書き込みます。
		/// @param destination 結果で置き換える折れ線。確保済みの領域を再利用します。
		/// @param maxError 折れ線とのずれの目安。座標と同じ単位の有限の正の値。
		/// @param maxDepth 区間ごとの最大分割深さ。0～20。
		/// @throws std::invalid_argument maxError が 0 以下、または maxDepth が 0～20 の範囲外の場合。destination は変更しません。
		/// @remark 空の経路なら destination を空にします。閉じた経路では末尾に始点を重複させません。
		/// @remark 分割上限に達した部分では maxError を超えるずれが残る場合があります。
		void toLineStringAdaptive(LineString& destination, double maxError = 0.48, int32 maxDepth = 10) const;

		/// @brief 曲線を折れ線に変換して描画します。
		/// @param thickness 線の太さ
		/// @param color 線の色
		/// @param maxError 折れ線とのずれの目安。座標と同じ単位の有限の正の値。
		/// @param maxDepth 最大分割深さ。0～20。
		/// @return *this
		/// @throws std::invalid_argument maxError が 0 以下、または maxDepth が 0～20 の範囲外の場合
		/// @remark 同じ曲線を繰り返し描画する場合は、toLineStringAdaptive() の結果を保存して描画すると変換を省けます。
		const Spline2D& draw(double thickness = 1.0, const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 10) const;

		/// @brief 始点と終点を入れ替え、経路の向きを反転します。
		/// @return *this
		Spline2D& reverse() noexcept;

		/// @brief 向きを反転した経路を返します。
		[[nodiscard]]
		Spline2D reversed() const;

		/// @brief 区間を取り除き、空の経路にします。
		void clear() noexcept;

		/// @brief 別の経路と内容を交換します。
		void swap(Spline2D& other) noexcept;

	private:

		Array<Bezier3> m_segments;
		bool m_closed = false;
	};
}

# include "detail/Spline2D.ipp"
