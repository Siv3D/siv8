//-----------------------------------------------
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
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "PredefinedNamedParameter.hpp"

namespace s3d
{
	struct PatternParameters;

	struct SuperEllipse
	{
		/// @brief スーパー楕円の座標を表現する型
		using position_type	= Vec2;

		/// @brief スーパー楕円の半径を表現する型
		using size_type		= SizeF;

		/// @brief スーパー楕円の座標や半径の要素の型
		using value_type	= position_type::value_type;

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			/// @brief 楕円の中心座標
			position_type center;

			struct
			{
				/// @brief 楕円の中心の X 座標
				value_type x;

				/// @brief 楕円の中心の Y 座標
				value_type y;
			};
		};

		union
		{
			/// @brief 楕円の X 軸 Y 軸に沿った半径
			size_type axes;

			struct
			{
				/// @brief 楕円の X 軸上の半径
				value_type a;

				/// @brief 楕円の Y 軸上の半径
				value_type b;
			};
		};

		/// @brief スーパー楕円の形状を決定する指数（n = 2 で通常の楕円）
		value_type n = 2.0;

	SIV3D_DISABLE_MSVC_WARNINGS_POP()

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		SuperEllipse() = default;

		[[nodiscard]]
		constexpr SuperEllipse(value_type _x, value_type _y, value_type _a, value_type _b, value_type _n) noexcept;

		[[nodiscard]]
		constexpr SuperEllipse(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _a, Concept::Arithmetic auto _b, Concept::Arithmetic auto _n) noexcept;

		[[nodiscard]]
		constexpr SuperEllipse(const position_type& _center, value_type _a, value_type _b, value_type _n) noexcept;

		[[nodiscard]]
		constexpr SuperEllipse(const position_type& _center, Concept::Arithmetic auto _a, Concept::Arithmetic auto _b, Concept::Arithmetic auto _n) noexcept;

		[[nodiscard]]
		constexpr SuperEllipse(value_type _x, value_type _y, const size_type& _axes, value_type _n) noexcept;

		[[nodiscard]]
		constexpr SuperEllipse(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, const size_type& _axes, Concept::Arithmetic auto _n) noexcept;

		[[nodiscard]]
		constexpr SuperEllipse(const position_type& _center, const size_type& _axes, Concept::Arithmetic auto _n) noexcept;

		[[nodiscard]]
		constexpr SuperEllipse(const Ellipse& ellipse, Concept::Arithmetic auto _n) noexcept;

		[[nodiscard]]
		constexpr SuperEllipse(const RectF& rect, Concept::Arithmetic auto _n) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの SuperEllipse が等しいかを返します。
		/// @param lhs 一方の SuperEllipse
		/// @param rhs もう一方の SuperEllipse
		/// @return 2 つの SuperEllipse が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const SuperEllipse& lhs, const SuperEllipse& rhs) noexcept
		{
			return ((lhs.center == rhs.center)
				&& (lhs.axes == rhs.axes)
				&& (lhs.n == rhs.n));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withX
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心の X 座標を変更した新しいスーパー楕円を返します。
		/// @param _x 新しい X 座標
		/// @return 中心の X 座標を変更した新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withX(value_type _x) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心の Y 座標を変更した新しいスーパー楕円を返します。
		/// @param _y 新しい Y 座標
		/// @return 中心の Y 座標を変更した新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withY(value_type _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を変更した新しいスーパー楕円を返します。
		/// @param _x 新しい X 座標
		/// @param _y 新しい Y 座標
		/// @return 中心座標を変更した新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withCenter(value_type _x, value_type _y) const noexcept;

		/// @brief 中心座標を変更した新しいスーパー楕円を返します。
		/// @param _center 新しい中心座標
		/// @return 中心座標を変更した新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withCenter(position_type _center) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withA
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸上の半径を変更した新しいスーパー楕円を返します。
		/// @param _a 新しい X 軸上の半径
		/// @return X 軸上の半径を変更した新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withA(value_type _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withB
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸上の半径を変更した新しいスーパー楕円を返します。
		/// @param _b 新しい Y 軸上の半径
		/// @return Y 軸上の半径を変更した新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withB(value_type _b) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withAxes
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸上の半径と Y 軸上の半径を変更した新しいスーパー楕円を返します。
		/// @param _a 新しい X 軸上の半径
		/// @param _b 新しい Y 軸上の半径
		/// @return X 軸上の半径と Y 軸上の半径を変更した新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withAxes(value_type _a, value_type _b) const noexcept;

		/// @brief X 軸上の半径と Y 軸上の半径を変更した新しいスーパー楕円を返します。
		/// @param _axes 新しい X 軸上の半径と Y 軸上の半径
		/// @return X 軸上の半径と Y 軸上の半径を変更した新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withAxes(size_type _axes) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withN
		//
		////////////////////////////////////////////////////////////////

		/// @brief 形状を決定する指数を変更した新しいスーパー楕円を返します。
		/// @param _n 新しい形状を決定する指数
		///	@return 形状を決定する指数を変更した新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withN(value_type _n) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円の中心座標を変更します。
		/// @param _x 新しい X 座標
		/// @param _y 新しい Y 座標
		/// @return *this
		/// @remark `.setCenter(_x, _y)` と同じです。
		constexpr SuperEllipse& setPos(value_type _x, value_type _y) noexcept;

		/// @brief スーパー楕円の中心座標を変更します。
		/// @param _center 新しい中心座標
		/// @return *this
		/// @remark `.setCenter(_center)` と同じです。
		constexpr SuperEllipse& setPos(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円の中心座標を変更します。
		/// @param _x 新しい X 座標
		/// @param _y 新しい Y 座標
		/// @return *this
		/// @remark `.setPos(_x, _y)` と同じです。
		constexpr SuperEllipse& setCenter(value_type _x, value_type _y) noexcept;

		/// @brief スーパー楕円の中心座標を変更します。
		/// @param _center 新しい中心座標
		/// @return *this
		/// @remark `.setPos(_center)` と同じです。
		constexpr SuperEllipse& setCenter(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setA
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸上の半径を変更します。
		/// @param _a 新しい X 軸上の半径
		/// @return *this
		constexpr SuperEllipse& setA(value_type _a) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setB
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸上の半径を変更します。
		/// @param _b 新しい Y 軸上の半径
		/// @return *this
		constexpr SuperEllipse& setB(value_type _b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setAxes
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸上の半径と Y 軸上の半径を変更します。
		/// @param _a 新しい X 軸上の半径
		/// @param _b 新しい Y 軸上の半径
		/// @return *this
		constexpr SuperEllipse& setAxes(value_type _a, value_type _b) noexcept;

		/// @brief X 軸上の半径と Y 軸上の半径を変更します。
		/// @param _axes 新しい X 軸上の半径と Y 軸上の半径
		/// @return *this
		constexpr SuperEllipse& setAxes(size_type _axes) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setN
		//
		////////////////////////////////////////////////////////////////

		/// @brief 形状を決定する指数を変更します。
		/// @param _n 新しい形状を決定する指数
		/// @return *this
		constexpr SuperEllipse& setN(value_type _n) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円の中心座標と半径を変更します。
		/// @param _x 新しい中心の X 座標
		/// @param _y 新しい中心の Y 座標
		/// @param _a 新しい X 軸上の半径
		/// @param _b 新しい Y 軸上の半径
		/// @param _n 新しい形状を決定する指数
		/// @return *this
		constexpr SuperEllipse& set(value_type _x, value_type _y, value_type _a, value_type _b, value_type _n) noexcept;

		/// @brief スーパー楕円の中心座標と半径を変更します。
		/// @param _center 新しい中心座標
		/// @param _a 新しい X 軸上の半径
		/// @param _b 新しい Y 軸上の半径
		/// @param _n 新しい形状を決定する指数
		/// @return *this
		constexpr SuperEllipse& set(const position_type& _center, value_type _a, value_type _b, value_type _n) noexcept;

		/// @brief スーパー楕円の中心座標と半径を変更します。
		/// @param _x 新しい中心の X 座標
		/// @param _y 新しい中心の Y 座標
		/// @param _axes 新しい X 軸上の半径と Y 軸上の半径
		/// @param _n 新しい形状を決定する指数
		/// @return *this
		constexpr SuperEllipse& set(value_type _x, value_type _y, const size_type& _axes, value_type _n) noexcept;

		/// @brief スーパー楕円の中心座標と半径を変更します。
		/// @param _center 新しい中心座標
		/// @param _axes 新しい X 軸上の半径と Y 軸上の半径
		/// @param _n 新しい形状を決定する指数
		/// @return *this
		constexpr SuperEllipse& set(const position_type& _center, const size_type& _axes, value_type _n) noexcept;

		/// @brief スーパー楕円の中心座標と半径を変更します。
		/// @param ellipse 新しいスーパー楕円
		/// @return *this
		constexpr SuperEllipse& set(const SuperEllipse& ellipse) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を移動した新しいスーパー楕円を返します。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse movedBy(value_type _x, value_type _y) const noexcept;

		/// @brief 中心座標を移動した新しいスーパー楕円を返します。
		/// @param v 移動量
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse movedBy(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を移動させます。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return *this
		constexpr SuperEllipse& moveBy(value_type _x, value_type _y) noexcept;

		/// @brief 中心座標を移動させます。
		/// @param v 移動量
		/// @return *this
		constexpr SuperEllipse& moveBy(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		/// @brief 半径を拡大縮小した新しいスーパー楕円を返します。
		/// @param size X 軸、Y 軸 4 方向それぞれの拡大縮小量
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse stretched(value_type size) const noexcept;

		/// @brief 半径を拡大縮小した新しいスーパー楕円を返します。
		/// @param _x X 軸の 2 方向にそれぞれの拡大縮小量
		/// @param _y Y 軸の 2 方向にそれぞれの拡大縮小量
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse stretched(double _x, double _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 半径を拡大縮小した新しいスーパー楕円を返します。
		/// @param s 拡大倍率
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse scaled(double s) const noexcept;

		/// @brief 半径を拡大縮小した新しいスーパー楕円を返します。
		/// @param sx X 軸方向の拡大倍率
		/// @param sy Y 軸方向の拡大倍率
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse scaled(double sx, double sy) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	top, right, bottom, left
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円の周上で最も上にある点の座標を返します。
		/// @return 最も上にある点の座標
		[[nodiscard]]
		constexpr position_type top() const noexcept;

		/// @brief スーパー楕円の周上で最も右にある点の座標を返します。
		/// @return 最も右にある点の座標
		[[nodiscard]]
		constexpr position_type right() const noexcept;

		/// @brief スーパー楕円の周上で最も下にある点の座標を返します。
		/// @return 最も下にある点の座標
		[[nodiscard]]
		constexpr position_type bottom() const noexcept;

		/// @brief スーパー楕円の周上で最も左にある点の座標を返します。
		/// @return 最も左にある点の座標
		[[nodiscard]]
		constexpr position_type left() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	horizontalDiameter, verticalDiameter
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸に平行な直径（線分）を返します。
		/// @return X 軸に平行な直径（線分）
		/// @remark `Line{ left(), right() }` と同じです。
		[[nodiscard]]
		constexpr Line horizontalDiameter() const noexcept;

		/// @brief Y 軸に平行な直径（線分）を返します。
		/// @return Y 軸に平行な直径（線分）
		/// @remark `Line{ top(), bottom() }` と同じです。
		[[nodiscard]]
		constexpr Line verticalDiameter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円の面積を返します。
		/// @return スーパー楕円の面積
		[[nodiscard]]
		value_type area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingCircle
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円に外接する円を返します。
		/// @return スーパー楕円に外接する円
		[[nodiscard]]
		constexpr Circle boundingCircle() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円に外接する長方形を返します。
		/// @return スーパー楕円に外接する長方形
		[[nodiscard]]
		constexpr RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointByAngle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円周上の点をラジアン (12 時方向が 0, 3 時方向が π/2, 6 時方向が π) で取得します。
		/// @param angle 方向（ラジアン）
		/// @return 円周上の点
		[[nodiscard]]
		position_type getPointByAngle(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	outer
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<Vec2> outer(const PointsPerCircle& pointsPerCircle) const;

		[[nodiscard]]
		Array<Vec2> outer(const QualityFactor& qualityFactor) const;

		////////////////////////////////////////////////////////////////
		//
		//	asPolygon
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円から Polygon を作成します。
		/// @param pointsPerCircle 頂点数
		/// @return Polygon
		[[nodiscard]]
		Polygon asPolygon(const PointsPerCircle& pointsPerCircle) const;

		/// @brief スーパー楕円から Polygon を作成します。分割数は半径に応じて自動的に決定されます。
		/// @param qualityFactor 品質係数。大きいほど分割数が増えます。
		/// @return Polygon
		[[nodiscard]]
		Polygon asPolygon(const QualityFactor& qualityFactor) const;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別のスーパー楕円との線形補間を返します。
		/// @param other 別のスーパー楕円
		/// @param f 補間係数
		/// @return 線形補間されたスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse lerp(const SuperEllipse& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief ハッシュ値を返します。
		/// @return ハッシュ値
		[[nodiscard]]
		uint64 hash() const noexcept;
	};
}
