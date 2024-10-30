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
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "JoinStyle.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	struct PatternParameters;

	////////////////////////////////////////////////////////////////
	//
	//	Triangle
	//
	////////////////////////////////////////////////////////////////

	/// @brief 三角形
	struct Triangle
	{
		/// @brief 三角形の座標を表現する型
		using position_type = Vec2;

		/// @brief 三角形の座標の成分の型
		using value_type	= position_type::value_type;

		/// @brief 三角形の点 0 の座標
		position_type p0;

		/// @brief 三角形の点 1 の座標
		position_type p1;

		/// @brief 三角形の点 2 の座標
		position_type p2;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Triangle() = default;

		/// @brief 正三角形を作成します。
		/// @param sides 一辺の長さ
		[[nodiscard]]
		explicit constexpr Triangle(value_type sides) noexcept;

		/// @brief 正三角形を作成します。
		/// @param sides 一辺の長さ
		[[nodiscard]]
		explicit constexpr Triangle(Concept::Arithmetic auto sides) noexcept;

		/// @brief 正三角形を作成します。
		/// @param sides 一辺の長さ
		/// @param angle 重心から見た点 0 の方向を表す角度（ラジアン）
		[[nodiscard]]
		Triangle(value_type sides, value_type angle) noexcept;

		/// @brief 正三角形を作成します。
		/// @param sides 一辺の長さ
		/// @param angle 重心から見た点 0 の方向を表す角度（ラジアン）
		[[nodiscard]]
		Triangle(Concept::Arithmetic auto sides, Concept::Arithmetic auto angle) noexcept;

		/// @brief 正三角形を作成します。
		/// @param x 重心の X 座標
		/// @param y 重心の Y 座標
		/// @param sides 一辺の長さ
		[[nodiscard]]
		constexpr Triangle(value_type x, value_type y, value_type sides) noexcept;

		/// @brief 正三角形を作成します。
		/// @param x 重心の X 座標
		/// @param y 重心の Y 座標
		/// @param sides 一辺の長さ
		[[nodiscard]]
		constexpr Triangle(Concept::Arithmetic auto x, Concept::Arithmetic auto y, Concept::Arithmetic auto sides) noexcept;

		/// @brief 正三角形を作成します。
		/// @param pos 重心の座標
		/// @param sides 一辺の長さ
		[[nodiscard]]
		constexpr Triangle(const position_type& pos, value_type sides) noexcept;

		/// @brief 正三角形を作成します。
		/// @param pos 重心の座標
		/// @param sides 一辺の長さ
		[[nodiscard]]
		constexpr Triangle(const position_type& pos, Concept::Arithmetic auto sides) noexcept;

		/// @brief 正三角形を作成します。
		/// @param x 重心の X 座標
		/// @param y 重心の Y 座標
		/// @param sides 一辺の長さ
		/// @param angle 重心から見た点 0 の方向を表す角度（ラジアン）
		[[nodiscard]]
		Triangle(value_type x, value_type y, value_type sides, value_type angle) noexcept;

		/// @brief 正三角形を作成します。
		/// @param x 重心の X 座標
		/// @param y 重心の Y 座標
		/// @param sides 一辺の長さ
		/// @param angle 重心から見た点 0 の方向を表す角度（ラジアン）
		[[nodiscard]]
		Triangle(Concept::Arithmetic auto x, Concept::Arithmetic auto y, Concept::Arithmetic auto sides, value_type angle) noexcept;

		/// @brief 正三角形を作成します。
		/// @param pos 重心の座標
		/// @param sides 一辺の長さ
		/// @param angle 重心から見た点 0 の方向を表す角度（ラジアン）
		[[nodiscard]]
		Triangle(const position_type& pos, value_type sides, value_type angle) noexcept;

		/// @brief 正三角形を作成します。
		/// @param pos 重心の座標
		/// @param sides 一辺の長さ
		/// @param angle 重心から見た点 0 の方向を表す角度（ラジアン）
		[[nodiscard]]
		Triangle(const position_type& pos, Concept::Arithmetic auto sides, value_type angle) noexcept;

		/// @brief 三角形を作成します。
		/// @param x0 点 0 の X 座標
		/// @param y0 点 0 の Y 座標
		/// @param x1 点 1 の X 座標
		/// @param y1 点 1 の Y 座標
		/// @param x2 点 2 の X 座標
		/// @param y2 点 2 の Y 座標
		[[nodiscard]]
		constexpr Triangle(value_type x0, value_type y0, value_type x1, value_type y1, value_type x2, value_type y2) noexcept;

		/// @brief 三角形を作成します。
		/// @param x0 点 0 の X 座標
		/// @param y0 点 0 の Y 座標
		/// @param x1 点 1 の X 座標
		/// @param y1 点 1 の Y 座標
		/// @param x2 点 2 の X 座標
		/// @param y2 点 2 の Y 座標
		[[nodiscard]]
		constexpr Triangle(Concept::Arithmetic auto x0, Concept::Arithmetic auto y0, Concept::Arithmetic auto x1, Concept::Arithmetic auto y1, Concept::Arithmetic auto x2, Concept::Arithmetic auto y2) noexcept;

		/// @brief 三角形を作成します。
		/// @param _p0 点 0 の座標
		/// @param _p1 点 1 の座標
		/// @param _p2 点 2 の座標
		[[nodiscard]]
		constexpr Triangle(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Triangle が等しいかを返します。
		/// @param lhs 一方の Triangle
		/// @param rhs もう一方の Triangle
		/// @return 2 つの Triangle が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const Triangle& lhs, const Triangle& rhs) noexcept
		{
			return ((lhs.p0 == rhs.p0)
				 && (lhs.p1 == rhs.p1)
				 && (lhs.p2 == rhs.p2));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withP0
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の点 0 の座標を変更した新しい三角形を返します。
		/// @param _x 新しい点 0 の X 座標
		/// @param _y 新しい点 0 の Y 座標
		/// @return 新しい点 0 の座標を持つ新しい三角形
		[[nodiscard]]
		constexpr Triangle withP0(value_type _x, value_type _y) const noexcept;

		/// @brief 三角形の点 0 の座標を変更した新しい三角形を返します。
		/// @param _p0 新しい点 0 の座標
		/// @return 新しい点 0 の座標を持つ新しい三角形
		[[nodiscard]]
		constexpr Triangle withP0(position_type _p0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の点 1 の座標を変更した新しい三角形を返します。
		/// @param _x 新しい点 1 の X 座標
		/// @param _y 新しい点 1 の Y 座標
		/// @return 新しい点 1 の座標を持つ新しい三角形
		[[nodiscard]]
		constexpr Triangle withP1(value_type _x, value_type _y) const noexcept;

		/// @brief 三角形の点 1 の座標を変更した新しい三角形を返します。
		/// @param _p1 新しい点 1 の座標
		/// @return 新しい点 1 の座標を持つ新しい三角形
		[[nodiscard]]
		constexpr Triangle withP1(position_type _p1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP2
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の点 2 の座標を変更した新しい三角形を返します。
		/// @param _x 新しい点 2 の X 座標
		/// @param _y 新しい点 2 の Y 座標
		/// @return 新しい点 2 の座標を持つ新しい三角形
		[[nodiscard]]
		constexpr Triangle withP2(value_type _x, value_type _y) const noexcept;

		/// @brief 三角形の点 2 の座標を変更した新しい三角形を返します。
		/// @param _p2 新しい点 2 の座標
		/// @return 新しい点 2 の座標を持つ新しい三角形
		[[nodiscard]]
		constexpr Triangle withP2(position_type _p2) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withCentroid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標が重心となるように三角形を平行移動した新しい三角形を返します。
		/// @param _x 新しい重心の X 座標
		/// @param _y 新しい重心の Y 座標
		/// @return 新しい重心を持つ新しい三角形
		[[nodiscard]]
		constexpr Triangle withCentroid(value_type _x, value_type _y) const noexcept;

		/// @brief 指定した座標が重心となるように三角形を平行移動した新しい三角形を返します。
		/// @param _centroid 新しい重心の座標
		/// @return 新しい重心を持つ新しい三角形
		[[nodiscard]]
		constexpr Triangle withCentroid(position_type _centroid) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 三角形の各頂点の座標を変更します。
		/// @param x0 点 0 の X 座標
		/// @param y0 点 0 の Y 座標
		/// @param x1 点 1 の X 座標
		/// @param y1 点 1 の Y 座標
		/// @param x2 点 2 の X 座標
		/// @param y2 点 2 の Y 座標
		/// @return *this
		constexpr Triangle& set(value_type x0, value_type y0, value_type x1, value_type y1, value_type x2, value_type y2) noexcept;

		/// @brief 三角形の各頂点の座標を変更します。
		/// @param _p0 点 0 の座標
		/// @param _p1 点 1 の座標
		/// @param _p2 点 2 の座標
		/// @return *this
		constexpr Triangle& set(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept;

		/// @brief 三角形を変更します。
		/// @param triangle 新しい三角形
		/// @return *this
		constexpr Triangle& set(const Triangle& triangle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setCentroid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標が重心となるように三角形を平行移動します。
		/// @param x 新しい重心の X 座標
		/// @param y 新しい重心の Y 座標
		/// @return *this
		constexpr Triangle& setCentroid(value_type x, value_type y) noexcept;

		/// @brief 指定した座標が重心となるように三角形を平行移動します。
		/// @param pos 新しい重心の座標
		/// @return *this
		constexpr Triangle& setCentroid(position_type pos) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を平行移動した新しい三角形を返します。
		/// @param x X 方向の移動距離
		/// @param y Y 方向の移動距離
		/// @return 平行移動した新しい三角形
		[[nodiscard]]
		constexpr Triangle movedBy(value_type x, value_type y) const noexcept;

		/// @brief 三角形を平行移動した新しい三角形を返します。
		/// @param v 移動距離
		/// @return 平行移動した新しい三角形
		[[nodiscard]]
		constexpr Triangle movedBy(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を平行移動します。
		/// @param x X 方向の移動距離
		/// @param y Y 方向の移動距離
		/// @return *this
		constexpr Triangle& moveBy(value_type x, value_type y) noexcept;

		/// @brief 三角形を平行移動します。
		/// @param v 移動距離
		/// @return *this
		constexpr Triangle& moveBy(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		/// @brief 辺を外側に拡大または内側に縮小した新しい三角形を返します。
		/// @param size 拡大縮小の量（正の値で外側に拡大、負の値で内側に縮小）
		/// @return 拡大縮小した新しい三角形
		[[nodiscard]]
		Triangle stretched(value_type size) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 重心を中心に回転させた新しい三角形を返します。
		/// @param angle 回転角度（ラジアン）
		/// @return 回転させた新しい三角形
		[[nodiscard]]
		Triangle rotated(value_type angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を中心に回転させた新しい三角形を返します。
		/// @param x 回転の中心の X 座標
		/// @param y 回転の中心の Y 座標
		/// @param angle 回転角度（ラジアン）
		/// @return 回転させた新しい三角形
		[[nodiscard]]
		Triangle rotatedAt(value_type x, value_type y, value_type angle) const noexcept;

		/// @brief 指定した座標を中心に回転させた新しい三角形を返します。
		/// @param pos 回転の中心の座標
		/// @param angle 回転角度（ラジアン）
		/// @return 回転させた新しい三角形
		[[nodiscard]]
		Triangle rotatedAt(position_type pos, value_type angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を基準に拡大縮小した新しい三角形を返します。
		/// @param s 拡大倍率
		/// @return 拡大縮小した新しい三角形
		[[nodiscard]]
		constexpr Triangle scaledFromOrigin(value_type s) const noexcept;

		/// @brief 原点を基準に拡大縮小した新しい三角形を返します。
		/// @param sx X 方向の拡大倍率
		/// @param sy Y 方向の拡大倍率
		/// @return 拡大縮小した新しい三角形
		[[nodiscard]]
		constexpr Triangle scaledFromOrigin(value_type sx, value_type sy) const noexcept;

		/// @brief 原点を基準に拡大縮小した新しい三角形を返します。
		/// @param s X 方向および Y 方向の拡大倍率
		/// @return 拡大縮小した新しい三角形
		[[nodiscard]]
		constexpr Triangle scaledFromOrigin(Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を基準に拡大縮小します。
		/// @param s 拡大倍率
		/// @return *this
		constexpr Triangle& scaleFromOrigin(value_type s) noexcept;

		/// @brief 原点を基準に拡大縮小します。
		/// @param sx X 方向の拡大倍率
		/// @param sy Y 方向の拡大倍率
		/// @return *this
		constexpr Triangle& scaleFromOrigin(value_type sx, value_type sy) noexcept;

		/// @brief 原点を基準に拡大縮小します。
		/// @param s X 方向および Y 方向の拡大倍率
		/// @return *this
		constexpr Triangle& scaleFromOrigin(Vec2 s) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を基準に拡大縮小した新しい三角形を返します。
		/// @param pos 基準となる座標
		/// @param s 拡大倍率
		/// @return 拡大縮小した新しい三角形
		[[nodiscard]]
		constexpr Triangle scaledFrom(position_type pos, value_type s) const noexcept;

		/// @brief 指定した座標を基準に拡大縮小した新しい三角形を返します。
		/// @param pos 基準となる座標
		/// @param sx X 方向の拡大倍率
		/// @param sy Y 方向の拡大倍率
		/// @return 拡大縮小した新しい三角形
		[[nodiscard]]
		constexpr Triangle scaledFrom(position_type pos, value_type sx, value_type sy) const noexcept;

		/// @brief 指定した座標を基準に拡大縮小した新しい三角形を返します。
		/// @param pos 基準となる座標
		/// @param s X 方向および Y 方向の拡大倍率
		/// @return 拡大縮小した新しい三角形
		[[nodiscard]]
		constexpr Triangle scaledFrom(position_type pos, Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を基準に拡大縮小します。
		/// @param pos 基準となる座標
		/// @param s 拡大倍率
		/// @return *this
		constexpr Triangle& scaleFrom(position_type pos, value_type s) noexcept;

		/// @brief 指定した座標を基準に拡大縮小します。
		/// @param pos 基準となる座標
		/// @param sx X 方向の拡大倍率
		/// @param sy Y 方向の拡大倍率
		/// @return *this
		constexpr Triangle& scaleFrom(position_type pos, value_type sx, value_type sy) noexcept;

		/// @brief 指定した座標を基準に拡大縮小します。
		/// @param pos 基準となる座標
		/// @param s X 方向および Y 方向の拡大倍率
		/// @return *this
		constexpr Triangle& scaleFrom(position_type pos, Vec2 s) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated180AlongP0P1, rotated180AlongP1P2, rotated180AlongP2P0
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を辺 p0 p1 が重なるよう時計回りに 180° 回転させた新しい三角形を返します。
		/// @return 作成した三角形
		[[nodiscard]]
		constexpr Triangle rotated180AlongP0P1() const noexcept;
		
		/// @brief 三角形を辺 p1 p2 が重なるよう時計回りに 180° 回転させた新しい三角形を返します。
		/// @return 作成した三角形
		[[nodiscard]]
		constexpr Triangle rotated180AlongP1P2() const noexcept;

		/// @brief 三角形を辺 p2 p0 が重なるよう時計回りに 180° 回転させた新しい三角形を返します。
		/// @return 作成した三角形
		[[nodiscard]]
		constexpr Triangle rotated180AlongP2P0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mirroredAlongP0P1, mirroredAlongP1P2, mirroredAlongP2P0
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を辺 p0 p1 を軸にミラー反転した新しい三角形を返します。
		/// @return 作成した三角形
		[[nodiscard]]
		Triangle mirroredAlongP0P1() const noexcept;
		
		/// @brief 三角形を辺 p1 p2 を軸にミラー反転した新しい三角形を返します。
		/// @return 作成した三角形
		[[nodiscard]]
		Triangle mirroredAlongP1P2() const noexcept;
		
		/// @brief 三角形を辺 p2 p0 を軸にミラー反転した新しい三角形を返します。
		/// @return 作成した三角形
		[[nodiscard]]
		Triangle mirroredAlongP2P0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isClockwise
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の頂点が時計回りであるかを返します。
		/// @return 頂点が時計回りである場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isClockwise() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasArea
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形が面積を持つかを返します。
		/// @return 三角形が面積を持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasArea() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	centroidX, centroidY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の重心の X 座標を返します。
		/// @return 三角形の重心の X 座標
		[[nodiscard]]
		constexpr value_type centroidX() const noexcept;

		/// @brief 三角形の重心の Y 座標を返します。
		/// @return 三角形の重心の Y 座標
		[[nodiscard]]
		constexpr value_type centroidY() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	centroid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の重心を返します。
		/// @return 三角形の重心
		[[nodiscard]]
		constexpr position_type centroid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		position_type& pointAtIndex(size_t index);
	
		[[nodiscard]]
		const position_type& pointAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の周上の指定した距離に対応する座標を返します。
		/// @param length 距離（p0-p1-p2 の順）
		/// @return 三角形の周上の指定した距離に対応する座標
		[[nodiscard]]
		Vec2 pointAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	interpolatedPointAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の周の長さを 1 として、周上の指定した位置に対応する座標を返します。
		/// @param t 位置（p0-p1-p2 の順、周の長さを 1 とした場合の位置）
		/// @return 三角形の周上の指定した位置に対応する座標
		[[nodiscard]]
		Vec2 interpolatedPointAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sideAtIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの辺を Line として返します。
		/// @param index インデックス（0 が p0-p1, 1 が p1-p2, 2 が p2-p0）
		/// @return 指定したインデックスの辺
		/// @throw std::out_of_range index が範囲外の場合
		[[nodiscard]]
		constexpr Line sideAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	sideIndexAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の周上の指定した距離に対応する辺のインデックスを返します。
		/// @param length 距離（p0-p1-p2 の順）
		/// @return 三角形の周上の指定した距離に対応する辺のインデックス（0 が p0-p1, 1 が p1-p2, 2 が p2-p0）
		[[nodiscard]]
		size_t sideIndexAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	p0p1, p1p2, p2p0
		//
		////////////////////////////////////////////////////////////////

		/// @brief p0 から p1 への線分を返します。
		/// @return p0 から p1 への線分
		[[nodiscard]]
		constexpr Line p0p1() const noexcept;

		/// @brief p1 から p2 への線分を返します。
		/// @return p1 から p2 への線分
		[[nodiscard]]
		constexpr Line p1p2() const noexcept;
		
		/// @brief p2 から p0 への線分を返します。
		/// @return p2 から p0 への線分
		[[nodiscard]]
		constexpr Line p2p0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	p1p0, p2p1, p0p2
		//
		////////////////////////////////////////////////////////////////

		/// @brief p1 から p0 への線分を返します。
		/// @return p1 から p0 への線分
		[[nodiscard]]
		constexpr Line p1p0() const noexcept;

		/// @brief p2 から p1 への線分を返します。
		/// @return p2 から p1 への線分
		[[nodiscard]]
		constexpr Line p2p1() const noexcept;

		/// @brief p0 から p2 への線分を返します。
		/// @return p0 から p2 への線分
		[[nodiscard]]
		constexpr Line p0p2() const noexcept;
	
		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の面積を返します。
		/// @return 三角形の面積
		[[nodiscard]]
		constexpr value_type area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perimeter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の周の長さを返します。
		/// @return 三角形の周の長さ
		[[nodiscard]]
		value_type perimeter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inradius
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の内接円の半径を返します。
		/// @return 三角形の内接円の半径
		[[nodiscard]]
		value_type inradius() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を囲む最小の矩形を返します。
		/// @return 三角形を囲む最小の矩形
		[[nodiscard]]
		RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingCircle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を囲む最小の円を返します。
		/// @return 三角形を囲む最小の円
		[[nodiscard]]
		Circle boundingCircle() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getCircumscribedCircle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の外接円を返します。
		/// @return 三角形の外接円
		[[nodiscard]]
		Circle getCircumscribedCircle() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getInscribedCircle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の内接円を返します。
		/// @return 三角形の内接円
		[[nodiscard]]
		Circle getInscribedCircle() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	outline
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//LineString outline(CloseRing closeRing = CloseRing::No) const;

		//[[nodiscard]]
		//LineString outline(double distanceFromOrigin, double length) const;

		////////////////////////////////////////////////////////////////
		//
		//	asPolygon
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を Polygon として返します。
		/// @return 三角形の Polygon
		[[nodiscard]]
		Polygon asPolygon() const;

		////////////////////////////////////////////////////////////////
		//
		//	rounded
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角を丸めて作成した、新しい多角形を返します。分割数は半径に応じて自動的に決定されます。
		/// @param round 角の半径
		/// @param qualityFactor 品質係数。大きいほど分割数が増えます。
		/// @return 新しい多角形
		[[nodiscard]]
		Polygon rounded(double round, const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;

		////////////////////////////////////////////////////////////////
		//
		//	calculateRoundBuffer
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を丸く太らせて作成した、新しい多角形を返します。分割数は半径に応じて自動的に決定されます。
		/// @param distance 太らせる距離
		/// @param qualityFactor 品質係数。大きいほど分割数が増えます。
		/// @return 新しい多角形。distance が 0 以下の場合は空の多角形
		[[nodiscard]]
		Polygon calculateRoundBuffer(double distance, const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の三角形との線形補間を計算します。
		/// @param other 別の三角形
		/// @param f 補間係数
		/// @return 線形補間された三角形
		[[nodiscard]]
		constexpr Triangle lerp(const Triangle& other, double f) const noexcept;

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
		//	intersectsAt
		//
		////////////////////////////////////////////////////////////////

		//template <class Shape2DType>
		//[[nodiscard]]
		//Optional<Array<Vec2>> intersectsAt(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	contains
		//
		////////////////////////////////////////////////////////////////

		//template <class Shape2DType>
		//[[nodiscard]]
		//bool contains(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	leftClicked, leftPressed, leftReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形が現在のフレームで左クリックされ始めたかを返します。
		/// @return 三角形が現在のフレームで左クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftClicked() const noexcept;

		/// @brief 三角形が左クリックされているかを返します。
		/// @return 三角形が左クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftPressed() const noexcept;

		/// @brief 現在のフレームで三角形への左クリックが離されたかを返します。
		/// @return 現在のフレームで三角形への左クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightClicked, rightPressed, rightReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形が現在のフレームで右クリックされ始めたかを返します。
		/// @return 三角形が現在のフレームで右クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightClicked() const noexcept;

		/// @brief 三角形が右クリックされているかを返します。
		/// @return 三角形が右クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightPressed() const noexcept;

		/// @brief 現在のフレームで三角形への右クリックが離されたかを返します。
		/// @return 現在のフレームで三角形への右クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseOver
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形上にマウスカーソルがあるかを返します。
		/// @return 三角形上にマウスカーソルがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool mouseOver() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		//const Triangle& paint(Image& dst, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		//const Triangle& overwrite(Image& dst, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintFrame
		//
		////////////////////////////////////////////////////////////////

		//const Triangle& paintFrame(Image& dst, int32 thickness, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwriteFrame
		//
		////////////////////////////////////////////////////////////////

		//const Triangle& overwriteFrame(Image& dst, int32 thickness, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を描きます。
		/// @param color 三角形の色
		/// @return *this
		const Triangle& draw(const ColorF& color = Palette::White) const;

		/// @brief 三角形を描きます。
		/// @param color0 頂点 0 の色
		/// @param color1 頂点 1 の色
		/// @param color2 頂点 2 の色
		/// @return *this
		const Triangle& draw(const ColorF& color0, const ColorF& color1, const ColorF& color2) const;

		/// @brief 三角形を描きます。
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const Triangle& draw(const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param color 枠の色
		/// @param joinStyle 線分の結合スタイル
		/// @return *this
		const Triangle& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White, JoinStyle joinStyle = JoinStyle::Default) const;

		///// @brief 三角形の枠を描きます。
		///// @param innerThickness 基準の三角形から内側方向への枠の太さ（ピクセル）
		///// @param outerThickness 基準の三角形から外側方向への枠の太さ（ピクセル）
		///// @param color 枠の色
		///// @return *this
		//const Triangle& drawFrame(double innerThickness, double outerThickness, const ColorF& color = Palette::White) const;

		/// @brief 三角形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		/// @param joinStyle 線分の結合スタイル
		/// @return *this
		const Triangle& drawFrame(double thickness, const PatternParameters& pattern, JoinStyle joinStyle = JoinStyle::Default) const;

		///// @brief 三角形の枠を描きます。
		///// @param innerThickness 基準の三角形から内側方向への枠の太さ（ピクセル）
		///// @param outerThickness 基準の三角形から外側方向への枠の太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		///// @return *this
		//const Triangle& drawFrame(double innerThickness, double outerThickness, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	FromBaseCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 底辺の中心座標と頂点の座標、底辺の長さから三角形を作成します。
		/// @param baseCenter 底辺の中心座標
		/// @param top 頂点の座標
		/// @param baseLength 底辺の長さ
		/// @return 作成された三角形
		[[nodiscard]]
		static Triangle FromBaseCenter(const position_type& baseCenter, const position_type& top, double baseLength) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに Triangle の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value Triangle
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Triangle& value)
		{
			return output << CharType('(')
				<< value.p0 << CharType(',') << CharType(' ')
				<< value.p1 << CharType(',') << CharType(' ')
				<< value.p2 << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから Triangle の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value Triangle の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Triangle& value)
		{
			CharType unused;
			return input >> unused
				>> value.p0 >> unused
				>> value.p1 >> unused
				>> value.p2 >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Triangle を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value Triangle
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Triangle& value);

	private:

		[[noreturn]]
		static void ThrowPointAtIndexOutOfRange();

		[[noreturn]]
		static void ThrowSideAtIndexOutOfRange();
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Triangle>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Triangle& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Triangle, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Triangle& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Triangle>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Triangle& value) const noexcept
	{
		return value.hash();
	}
};
