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

		/// @brief スーパー楕円を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		/// @param _n 形状を決定する指数（n = 2 で通常の楕円）
		[[nodiscard]]
		constexpr SuperEllipse(value_type _x, value_type _y, value_type _a, value_type _b, value_type _n) noexcept;

		/// @brief スーパー楕円を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		/// @param _n 形状を決定する指数（n = 2 で通常の楕円）
		[[nodiscard]]
		constexpr SuperEllipse(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _a, Concept::Arithmetic auto _b, Concept::Arithmetic auto _n) noexcept;

		/// @brief スーパー楕円を作成します。
		/// @param _center 中心座標
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		/// @param _n 形状を決定する指数（n = 2 で通常の楕円）
		[[nodiscard]]
		constexpr SuperEllipse(const position_type& _center, value_type _a, value_type _b, value_type _n) noexcept;

		/// @brief スーパー楕円を作成します。
		/// @param _center 中心座標
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		/// @param _n 形状を決定する指数（n = 2 で通常の楕円）
		[[nodiscard]]
		constexpr SuperEllipse(const position_type& _center, Concept::Arithmetic auto _a, Concept::Arithmetic auto _b, Concept::Arithmetic auto _n) noexcept;

		/// @brief スーパー楕円を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param _axes X 軸 Y 軸に沿った半径
		/// @param _n 形状を決定する指数（n = 2 で通常の楕円）
		[[nodiscard]]
		constexpr SuperEllipse(value_type _x, value_type _y, const size_type& _axes, value_type _n) noexcept;

		/// @brief スーパー楕円を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param _axes X 軸 Y 軸に沿った半径
		/// @param _n 形状を決定する指数（n = 2 で通常の楕円）
		[[nodiscard]]
		constexpr SuperEllipse(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, const size_type& _axes, Concept::Arithmetic auto _n) noexcept;

		/// @brief スーパー楕円を作成します。
		/// @param _center 中心座標
		/// @param _axes X 軸 Y 軸に沿った半径
		/// @param _n 形状を決定する指数（n = 2 で通常の楕円）
		[[nodiscard]]
		constexpr SuperEllipse(const position_type& _center, const size_type& _axes, Concept::Arithmetic auto _n) noexcept;

		/// @brief 楕円からスーパー楕円を作成します。
		/// @param ellipse 元になる楕円
		/// @param _n 形状を決定する指数（n = 2 で通常の楕円）
		[[nodiscard]]
		constexpr SuperEllipse(const Ellipse& ellipse, Concept::Arithmetic auto _n) noexcept;

		/// @brief 長方形に内接するスーパー楕円を作成します。
		/// @param rect スーパー楕円が内接する長方形
		/// @param _n 形状を決定する指数（n = 2 で通常の楕円）
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
		//	withOffset
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標をオフセットした新しいスーパー楕円を返します。
		/// @param _x X 軸方向のオフセット量
		///	@param _y Y 軸方向のオフセット量
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withOffset(value_type _x, value_type _y) const noexcept;

		/// @brief 中心座標をオフセットした新しいスーパー楕円を返します。
		/// @param v オフセット量
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withOffset(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetX, withOffsetY
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸方向にオフセットした新しいスーパー楕円を返します。
		/// @param _x オフセット量
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withOffsetX(value_type _x) const noexcept;

		/// @brief Y 軸方向にオフセットした新しいスーパー楕円を返します。
		/// @param _y オフセット量
		/// @return 新しいスーパー楕円
		[[nodiscard]]
		constexpr SuperEllipse withOffsetY(value_type _y) const noexcept;

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

		/// @brief スーパー楕円の外周を表現する頂点配列を返します。
		/// @param pointsPerCircle 円周の分割数
		/// @return スーパー楕円の外周を表現する頂点配列
		[[nodiscard]]
		Array<Vec2> outer(const PointsPerCircle& pointsPerCircle) const;

		/// @brief スーパー楕円の外周を表現する頂点配列を返します。分割数は半径と qualityFactor に応じて自動的に決定されます。
		/// @param qualityFactor 品質係数。大きいほど分割数が増えます。
		/// @return スーパー楕円の外周を表現する頂点配列
		[[nodiscard]]
		Array<Vec2> outer(const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;

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
		Polygon asPolygon(const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;

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

		////////////////////////////////////////////////////////////////
		//
		//	distanceTo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形との距離を返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形との距離
		template <class Shape2DType>
		[[nodiscard]]
		double distanceTo(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	intersects
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形と交差しているかを返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形と交差している場合 true, それ以外の場合は false
		template <class Shape2DType>
		[[nodiscard]]
		constexpr bool intersects(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	overlaps
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形と交差する領域が面積を持つかを返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形と交差する領域が面積を持つ場合 true, それ以外の場合は false
		template <class Shape2DType>
		[[nodiscard]]
		constexpr bool overlaps(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	contains
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形を完全に含んでいるかを返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形を完全に含んでいる場合 true, それ以外の場合は false
		template <class Shape2DType>
		[[nodiscard]]
		constexpr bool contains(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	intersectsAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形と点で交差している場合、その座標を返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形と点で交差している場合、その座標の配列を返します。交差が存在しても、一次元以上の共有部分しかない場合は空の配列を返します。交差していない場合は none を返します。
		template <class Shape2DType>
		[[nodiscard]]
		Optional<Array<Vec2>> intersectsAt(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	leftClicked, leftPressed, leftReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円が現在のフレームで左クリックされ始めたかを返します。
		/// @return スーパー楕円が現在のフレームで左クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftClicked() const noexcept;

		/// @brief スーパー楕円が左クリックされているかを返します。
		/// @return スーパー楕円が左クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftPressed() const noexcept;

		/// @brief 現在のフレームでスーパー楕円への左クリックが離されたかを返します。
		/// @return 現在のフレームでスーパー楕円への左クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightClicked, rightPressed, rightReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円が現在のフレームで右クリックされ始めたかを返します。
		/// @return スーパー楕円が現在のフレームで右クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightClicked() const noexcept;

		/// @brief スーパー楕円が右クリックされているかを返します。
		/// @return スーパー楕円が右クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightPressed() const noexcept;

		/// @brief 現在のフレームでスーパー楕円への右クリックが離されたかを返します。
		/// @return 現在のフレームでスーパー楕円への右クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseOver
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円上にマウスカーソルがあるかを返します。
		/// @return スーパー楕円上にマウスカーソルがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool mouseOver() const noexcept;

		//////////////////////////////////////////////////////////////////
		////
		////	paint
		////
		//////////////////////////////////////////////////////////////////

		////const Ellipse& paint(Image& dst, const Color& color) const;

		//////////////////////////////////////////////////////////////////
		////
		////	overwrite
		////
		//////////////////////////////////////////////////////////////////

		////const Ellipse& overwrite(Image& dst, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円を描きます。
		/// @param color スーパー楕円の色
		/// @return *this
		const SuperEllipse& draw(const ColorF& color = Palette::White) const;

		/// @brief スーパー楕円を描きます。
		/// @param innerColor スーパー楕円の内側の色
		/// @param outerColor スーパー楕円の外側の色
		/// @return *this
		const SuperEllipse& draw(const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief スーパー楕円を描きます。
		/// @param topColor スーパー楕円の上側の色
		/// @param bottomColor スーパー楕円の下側の色
		/// @return *this
		const SuperEllipse& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief スーパー楕円を描きます。
		/// @param leftColor スーパー楕円の左側の色
		/// @param rightColor スーパー楕円の右側の色
		/// @return *this
		const SuperEllipse& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief スーパー楕円を描きます。
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const SuperEllipse& draw(const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief スーパー楕円の枠を描画します。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param color 色
		/// @return *this
		const SuperEllipse& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief スーパー楕円の枠を描画します。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const SuperEllipse& drawFrame(double thickness, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに SuperEllipse の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value SuperEllipse
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const SuperEllipse& value)
		{
			return output << CharType('(')
				<< value.x << CharType(',') << CharType(' ')
				<< value.y << CharType(',') << CharType(' ')
				<< value.a << CharType(',') << CharType(' ')
				<< value.b << CharType(')') << CharType(',')
				<< value.n << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから SuperEllipse の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value SuperEllipse の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, SuperEllipse& value)
		{
			CharType unused;
			return input >> unused
				>> value.x >> unused
				>> value.y >> unused
				>> value.a >> unused
				>> value.b >> unused
				>> value.n >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief SuperEllipse を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value SuperEllipse
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const SuperEllipse& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::SuperEllipse>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::SuperEllipse& value, format_context& ctx) const;
};

template <>
struct fmt::formatter<s3d::SuperEllipse, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::SuperEllipse& value, s3d::BufferContext& ctx) const;
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::SuperEllipse>
{
	[[nodiscard]]
	size_t operator ()(const s3d::SuperEllipse& value) const noexcept
	{
		return value.hash();
	}
};
