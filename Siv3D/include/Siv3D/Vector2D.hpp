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
# include "Concepts.hpp"
# include "2DShapesFwd.hpp"
# include "FormatLiteral.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Vector2D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2 次元のベクトル
	/// @tparam Type ベクトルの要素の型
	template <class Type>
	struct Vector2D
	{
		/// @brief ベクトルの要素の型
		using value_type = Type;

		/// @brief ベクトルの次元数
		static constexpr size_t Dimension = 2;

		/// @brief ベクトルの X 成分
		value_type x;

		/// @brief ベクトルの Y 成分
		value_type y;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vector2D() = default;

		[[nodiscard]]
		constexpr Vector2D(value_type _x, value_type _y) noexcept;

		[[nodiscard]]
		constexpr Vector2D(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y) noexcept;

		[[nodiscard]]
		constexpr Vector2D(Point p) noexcept;

		template <class U>
		[[nodiscard]]
		constexpr Vector2D(const Vector2D<U>& v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	elem
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの要素を返します。
		/// @param index インデックス（0 または 1）
		/// @return 指定したインデックスの要素。index が 2 以上の場合は 0 を返します。
		[[nodiscard]]
		constexpr value_type elem(size_t index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointer
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分へのポインタを返します。
		/// @remark 戻り値に対して [0] で x 成分、[1] で y 成分にアクセスできます。
		/// @return x 成分へのポインタ
		[[nodiscard]]
		constexpr value_type* getPointer() noexcept;

		/// @brief x 成分へのポインタを返します。
		/// @remark 戻り値に対して [0] で x 成分、[1] で y 成分にアクセスできます。
		/// @return x 成分へのポインタ
		[[nodiscard]]
		constexpr const value_type* getPointer() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector2D operator +() const noexcept;

		[[nodiscard]]
		constexpr Vector2D operator +(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector2D operator -() const noexcept;

		[[nodiscard]]
		constexpr Vector2D operator -(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector2D operator *(value_type s) const noexcept;

		[[nodiscard]]
		constexpr Vector2D operator *(Vector2D v) const noexcept;
		
		template <Concept::Arithmetic U> // IntelliSense のエラー回避のため template を使う (17.12 Preview 2)
		[[nodiscard]]
		friend constexpr Vector2D operator *(const U s, const Vector2D v) noexcept
		{
			return (v * static_cast<value_type>(s));
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator /
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector2D operator /(value_type s) const noexcept;

		[[nodiscard]]
		constexpr Vector2D operator /(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector2D& operator +=(Vector2D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector2D& operator -=(Vector2D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector2D& operator *=(value_type s) noexcept;

		constexpr Vector2D& operator *=(Vector2D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator /=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector2D& operator /=(value_type s) noexcept;

		constexpr Vector2D& operator /=(Vector2D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのベクトルが等しいかを返します。
		/// @param lhs 一方のベクトル
		/// @param rhs もう一方のベクトル
		/// @return 2 つのベクトルが等しい場合 true, それ以外の場合は false
		/// @remark 浮動小数点数誤差を考慮する場合は `.epsilonEquals()` を使用してください。
		[[nodiscard]]
		friend constexpr bool operator ==(const Vector2D lhs, const Vector2D rhs) noexcept
		{
			return ((lhs.x == rhs.x)
				 && (lhs.y == rhs.y));
		}

		////////////////////////////////////////////////////////////////
		//
		//	epsilonEquals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した別のベクトルとの各成分の差の絶対値が epsilon 以下であるかを返します。
		/// @param other 別のベクトル
		/// @param epsilon 差の絶対値の許容量
		/// @return 別のベクトルとの各成分の差の絶対値が epsilon 以下である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool epsilonEquals(Vector2D other, value_type epsilon) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasSameDirection
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した別のベクトルとのなす角が 90° 未満であるかを返します。
		/// @param other 別のベクトル
		/// @return 別のベクトルとのなす角が 90° 未満である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasSameDirection(Vector2D other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasOppositeDirection
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した別のベクトルとのなす角が 90° より大きいかを返します。
		/// @param other 別のベクトル
		/// @return 別のベクトルとのなす角が 90° より大きい場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasOppositeDirection(Vector2D other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isZero
		//
		////////////////////////////////////////////////////////////////

		/// @brief ゼロベクトルであるかを返します。
		/// @return ゼロベクトルである場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isZero() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasNaN
		//
		////////////////////////////////////////////////////////////////

		/// @brief NaN である成分を持つかを返します。
		/// @return NaN である成分を持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasNaN() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minComponent, maxComponent
		//
		////////////////////////////////////////////////////////////////

		/// @brief 最小の成分を返します。
		/// @return 最小の成分
		[[nodiscard]]
		constexpr value_type minComponent() const noexcept;

		/// @brief 最大の成分を返します。
		/// @return 最大の成分
		[[nodiscard]]
		constexpr value_type maxComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分を 0 にセットします。
		constexpr void clear() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withX, withY
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分のみを変更した自身のコピーを返します。
		/// @param _x x 成分
		/// @return x 成分を変更したコピー
		[[nodiscard]]
		constexpr Vector2D withX(value_type _x) const noexcept;

		/// @brief y 成分のみを変更した自身のコピーを返します。
		/// @param _y y 成分
		/// @return y 成分を変更したコピー
		[[nodiscard]]
		constexpr Vector2D withY(value_type _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分を変更します。
		/// @param _x 新しい X 成分
		/// @param _y 新しい Y 成分
		/// @return *this
		constexpr Vector2D& set(value_type _x, value_type _y) noexcept;

		/// @brief ベクトルを変更します。
		/// @param v 新しいベクトル
		/// @return *this
		constexpr Vector2D& set(Vector2D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 位置ベクトルを移動させた新しいベクトルを返します。
		/// @param _x X 成分の移動量
		/// @param _y Y 成分の移動量
		/// @return 移動後の新しいベクトル
		[[nodiscard]]
		constexpr Vector2D movedBy(value_type _x, value_type _y) const noexcept;

		/// @brief 位置ベクトルを移動させた新しいベクトルを返します。
		/// @param v 移動量
		/// @return 移動後の新しいベクトル
		[[nodiscard]]
		constexpr Vector2D movedBy(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 位置ベクトルを移動させます。
		/// @param _x X 成分の移動量
		/// @param _y Y 成分の移動量
		/// @return *this
		constexpr Vector2D& moveBy(value_type _x, value_type _y) noexcept;

		/// @brief 位置ベクトルを移動させます。
		/// @param v 移動量
		/// @return *this
		constexpr Vector2D& moveBy(Vector2D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	clamp, clamped
		//
		////////////////////////////////////////////////////////////////

		/// @brief 座標を指定した長方形の範囲内にクランプします。
		/// @param rect クランプする範囲
		/// @return *this
		Vector2D& clamp(const RectF& rect) noexcept;

		/// @brief 指定した長方形の範囲内にクランプした座標を返します。
		/// @param rect クランプする範囲
		/// @return クランプされた座標
		[[nodiscard]]
		Vector2D clamped(const RectF& rect) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	dot
		//
		////////////////////////////////////////////////////////////////

		/// @brief 内積を返します。
		/// @tparam U もう一方のベクトルの型
		/// @param v もう一方のベクトル
		/// @return 内積
		template <class U>
		[[nodiscard]]
		constexpr auto dot(Vector2D<U> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cross
		//
		////////////////////////////////////////////////////////////////

		/// @brief 外積を返します。
		/// @tparam U もう一方のベクトルの型
		/// @param v もう一方のベクトル
		/// @return 外積
		/// @remark `(x * v.y) - (y * v.x)` を返します。
		template <class U>
		[[nodiscard]]
		constexpr auto cross(Vector2D<U> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	horizontalAspectRatio
		//
		////////////////////////////////////////////////////////////////

		/// @brief 水平方向のアスペクト比を返します。
		/// @return 水平方向のアスペクト比
		/// @remark `x / y` を返します。y が 0 の場合は 0 を返します。
		[[nodiscard]]
		constexpr value_type horizontalAspectRatio() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	length
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの大きさ（長さ）を返します。
		/// @return ベクトルの大きさ（長さ）
		[[nodiscard]]
		value_type length() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lengthSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの大きさ（長さ）の二乗を返します。
		/// @remark 平方根を計算しないため `length()` より高速です。
		/// @return ベクトルの大きさ（長さ）の二乗
		[[nodiscard]]
		constexpr value_type lengthSq() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	invLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの長さの逆数 (1 / length())を返します。
		/// @return ベクトルの長さの逆数
		[[nodiscard]]
		value_type invLength() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	manhattanLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点からこの位置ベクトルまでのマンハッタン距離を返します。
		/// @return 原点からのマンハッタン距離
		[[nodiscard]]
		constexpr value_type manhattanLength() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	manhattanDistanceFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルからのマンハッタン距離を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @return 別の位置ベクトルからのマンハッタン距離
		/// @remark `.manhattanDistanceTo(_x, _y)` と同じです。
		[[nodiscard]]
		constexpr value_type manhattanDistanceFrom(value_type _x, value_type _y) const noexcept;

		/// @brief 別の位置ベクトルからのマンハッタン距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルからのマンハッタン距離
		/// @remark `.manhattanDistanceTo(v)` と同じです。
		[[nodiscard]]
		constexpr value_type manhattanDistanceFrom(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	manhattanDistanceTo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルへのマンハッタン距離を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @return 別の位置ベクトルへのマンハッタン距離
		/// @remark `.manhattanDistanceFrom(_x, _y)` と同じです。
		[[nodiscard]]
		constexpr value_type manhattanDistanceTo(value_type _x, value_type _y) const noexcept;

		/// @brief 別の位置ベクトルへのマンハッタン距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルへのマンハッタン距離
		/// @remark `.manhattanDistanceFrom(v)` と同じです。
		[[nodiscard]]
		constexpr value_type manhattanDistanceTo(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルからの距離を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @return 別の位置ベクトルからの距離
		/// @remark `.distanceTo(_x, _y)` と同じです。
		[[nodiscard]]
		value_type distanceFrom(value_type _x, value_type _y) const noexcept;

		/// @brief 別の位置ベクトルからの距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルからの距離
		/// @remark `.distanceTo(v)` と同じです。
		[[nodiscard]]
		value_type distanceFrom(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceFromSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルからの距離の二乗を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @return 別の位置ベクトルからの距離の二乗
		/// @remark 平方根を計算しないため `distanceFrom()` より高速です。
		/// @remark `.distanceToSq(_x, _y)` と同じです。
		[[nodiscard]]
		constexpr value_type distanceFromSq(value_type _x, value_type _y) const noexcept;

		/// @brief 別の位置ベクトルからの距離の二乗を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルからの距離の二乗
		/// @remark 平方根を計算しないため `distanceFrom()` より高速です。
		/// @remark `.distanceToSq(v)` と同じです。
		[[nodiscard]]
		constexpr value_type distanceFromSq(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceTo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルへの距離を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @return 別の位置ベクトルへの距離
		/// @remark `.distanceFrom(_x, _y)` と同じです。
		[[nodiscard]]
		value_type distanceTo(value_type _x, value_type _y) const noexcept;

		/// @brief 別の位置ベクトルへの距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルへの距離
		/// @remark `.distanceFrom(v)` と同じです。
		[[nodiscard]]
		value_type distanceTo(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceToSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルへの距離の二乗を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @return 別の位置ベクトルへの距離の二乗
		/// @remark 平方根を計算しないため `distanceTo()` より高速です。
		/// @remark `.distanceFromSq(_x, _y)` と同じです。
		[[nodiscard]]
		constexpr value_type distanceToSq(value_type _x, value_type _y) const noexcept;

		/// @brief 別の位置ベクトルへの距離の二乗を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルへの距離の二乗
		/// @remark 平方根を計算しないため `distanceTo()` より高速です。
		/// @remark `.distanceFromSq(v)` と同じです。
		[[nodiscard]]
		constexpr value_type distanceToSq(Vector2D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 向きが同じで大きさ（長さ）を変更した新しいベクトルを返します。
		/// @param _length ベクトルの大きさ（長さ）
		/// @return 新しいベクトル。自身がゼロベクトルの場合はゼロベクトル
		[[nodiscard]]
		Vector2D withLength(value_type _length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの向きはそのままで、大きさ（長さ）だけを変更します。
		/// @param _length ベクトルの大きさ（長さ）
		/// @remark 自身がゼロベクトルの場合は何もしません。
		/// @return *this
		Vector2D& setLength(value_type _length) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withMaxLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 向きが同じで大きさ（長さ）を一定の値以下にした新しいベクトルを返します。
		/// @param maxLength ベクトルの最大の大きさ（長さ）
		/// @return 大きさ（長さ）を一定の値以下にした新しいベクトル
		[[nodiscard]]
		Vector2D withMaxLength(value_type maxLength) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setMaxLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの向きはそのままで、大きさ（長さ）を一定の値以下にします
		/// @param maxLength ベクトルの最大の大きさ（長さ）
		/// @return *this
		Vector2D& setMaxLength(value_type maxLength) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化した（大きさを 1 にした）ベクトルを返します。
		/// @return 正規化した（大きさを 1 にした）ベクトル
		[[nodiscard]]
		Vector2D normalized() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalized_or
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化した（大きさを 1 にした）ベクトルを返します。ゼロベクトルの場合は valueIfZero を返します。
		/// @param valueIfZero ゼロベクトルの場合に返すベクトル
		/// @return 正規化した（大きさを 1 にした）ベクトル、または valueIfZero
		[[nodiscard]]
		Vector2D normalized_or(Vector2D valueIfZero) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 自身を正規化（大きさを 1 に）します。
		/// @return *this
		Vector2D& normalize() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated, rotate
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vector2D rotated(value_type angle) const noexcept;

		Vector2D& rotate(value_type angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAt, rotateAt
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vector2D rotatedAt(Vector2D center, value_type angle) const noexcept;

		Vector2D& rotateAt(Vector2D center, value_type angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated90, rotate90
		//
		////////////////////////////////////////////////////////////////

		/// @brief 時計回りに 90°* n 回転したベクトルを返します。
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return 時計回りに 90°* n 回転したベクトル
		[[nodiscard]]
		constexpr Vector2D rotated90(int32 n = 1) const noexcept;

		/// @brief 自身を時計回りに 90°* n 回転します。
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return *this
		constexpr Vector2D& rotate90(int32 n = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated90At, rotate90At
		//
		////////////////////////////////////////////////////////////////

		/// @brief centerを中心とし、時計回りに 90°* n 回転したベクトルを返します。
		/// @param center 回転の中心座標
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return centerを中心とし、時計回りに 90°* n 回転したベクトル
		[[nodiscard]]
		constexpr Vector2D rotated90At(Vector2D center, int32 n = 1) const noexcept;

		/// @brief centerを中心とし、自身を時計回りに 90°* n 回転します。
		/// @param center 回転の中心座標
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return *this
		constexpr Vector2D& rotate90At(Vector2D center, int32 n = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getAngle
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		value_type getAngle() const noexcept;

		[[nodiscard]]
		value_type getAngle(Vector2D other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPerpendicularCW, getPerpendicularCCW
		//
		////////////////////////////////////////////////////////////////

		/// @brief 時計回りに 90° 回転したベクトルを返します。
		/// @return 時計回りに 90° 回転したベクトル
		[[nodiscard]]
		constexpr Vector2D getPerpendicularCW() const noexcept;

		/// @brief 反時計回りに 90° 回転したベクトルを返します。
		/// @return 反時計回りに 90° 回転したベクトル
		[[nodiscard]]
		constexpr Vector2D getPerpendicularCCW() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getMidpoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の座標との中点を返します。
		/// @param other 別の座標
		/// @return 別の座標との中点
		[[nodiscard]]
		constexpr Vector2D getMidpoint(Vector2D other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	projection
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector2D projection(Vector2D onto) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointByAngleAndDistance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の座標を中心とし、指定した角度と距離にある座標を返します。
		/// @param angle 角度（ラジアン）
		/// @param distance 距離
		/// @return 指定した角度と距離にある座標
		[[nodiscard]]
		Vector2D getPointByAngleAndDistance(value_type angle, value_type distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別のベクトルとの線形補間を返します。
		/// @param other 別のベクトル
		/// @param f 補間係数
		/// @return 線形補間されたベクトル
		[[nodiscard]]
		constexpr Vector2D lerp(Vector2D other, value_type f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asPoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec2 を Point 型に変換します。小数点数以下は切り捨てられます。
		/// @return Point
		[[nodiscard]]
		constexpr Point asPoint() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asCircle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 点を中心とした円を作成します。
		/// @param r 円の半径
		/// @return 円
		[[nodiscard]]
		Circle asCircle(double r) const noexcept;

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
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief ハッシュ値を返します。
		/// @return ハッシュ値
		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xx, xy, yx, yy, x0, y0
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ x, x } を返します。
		/// @return Vector2D{ x, x }
		[[nodiscard]]
		constexpr Vector2D xx() const noexcept;

		/// @brief Vector2D{ x, y } を返します。
		/// @return Vector2D{ x, y }
		[[nodiscard]]
		constexpr Vector2D xy() const noexcept;

		/// @brief Vector2D{ y, x } を返します。
		/// @return Vector2D{ y, x }
		[[nodiscard]]
		constexpr Vector2D yx() const noexcept;

		/// @brief Vector2D{ y, y } を返します。
		/// @return Vector2D{ y, y }
		[[nodiscard]]
		constexpr Vector2D yy() const noexcept;

		/// @brief Vector2D{ x, 0 } を返します。
		/// @return Vector2D{ x, 0 }
		[[nodiscard]]
		constexpr Vector2D x0() const noexcept;

		/// @brief Vector2D{ y, 0 } を返します。
		/// @return Vector2D{ y, 0 }
		[[nodiscard]]
		constexpr Vector2D y0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xy0
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector3D{ x, y, 0 } を返します。
		/// @return Vector3D{ x, y, 0 }
		[[nodiscard]]
		constexpr Vector3D<Type> xy0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xy00
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector4D{ x, y, 0, 0 } を返します。
		/// @return Vector4D{ x, y, 0, 0 }
		[[nodiscard]]
		constexpr Vector4D<Type> xy00() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 0, 0 } を返します。
		/// @return Vector2D{ 0, 0 }
		[[nodiscard]]
		static constexpr Vector2D Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	One
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 1, 1 } を返します。
		/// @return Vector2D{ 1, 1 }
		[[nodiscard]]
		static constexpr Vector2D One() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	All
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ value, value } を返します。
		/// @param value 値
		/// @return Vector2D{ value, value }
		[[nodiscard]]
		static constexpr Vector2D All(value_type value = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UnitX, UnitY
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 1, 0 } を返します。
		/// @return Vector2D{ 1, 0 }
		[[nodiscard]]
		static constexpr Vector2D UnitX() noexcept;

		/// @brief Vector2D{ 0, 1 } を返します。
		/// @return Vector2D{ 0, 1 }
		[[nodiscard]]
		static constexpr Vector2D UnitY() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Left, Right, Up, Down
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ -length, 0 } を返します。
		/// @param length 値
		/// @return Vector2D{ -length, 0 }
		[[nodiscard]]
		static constexpr Vector2D Left(value_type length = 1) noexcept;

		/// @brief Vector2D{ length, 0 } を返します。
		/// @param length 値
		/// @return Vector2D{ length, 0 }
		[[nodiscard]]
		static constexpr Vector2D Right(value_type length = 1) noexcept;

		/// @brief Vector2D{ 0, -length } を返します。
		/// @param length 値
		/// @return Vector2D{ 0, -length }
		[[nodiscard]]
		static constexpr Vector2D Up(value_type length = 1) noexcept;

		/// @brief Vector2D{ 0, length } を返します。
		/// @return Vector2D{ 0, length }
		[[nodiscard]]
		static constexpr Vector2D Down(value_type length = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AnchorCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 0.5, 0.5 } を返します。
		/// @return Vector2D{ 0.5, 0.5 }
		[[nodiscard]]
		static constexpr Vector2D AnchorCenter() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AnchorTopLeft
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 0, 0 } を返します。
		/// @return Vector2D{ 0, 0 }
		[[nodiscard]]
		static constexpr Vector2D AnchorTopLeft() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AnchorTopCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 0.5, 0 } を返します。
		/// @return Vector2D{ 0.5, 0 }
		[[nodiscard]]
		static constexpr Vector2D AnchorTopCenter() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AnchorTopRight
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 1, 0 } を返します。
		/// @return Vector2D{ 1, 0 }
		[[nodiscard]]
		static constexpr Vector2D AnchorTopRight() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AnchorRightCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 1, 0.5 } を返します。
		/// @return Vector2D{ 1, 0.5 }
		[[nodiscard]]
		static constexpr Vector2D AnchorRightCenter() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AnchorBottomRight
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 1, 1 } を返します。
		/// @return Vector2D{ 1, 1 }
		[[nodiscard]]
		static constexpr Vector2D AnchorBottomRight() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AnchorBottomCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 0.5, 1 } を返します。
		/// @return Vector2D{ 0.5, 1 }
		[[nodiscard]]
		static constexpr Vector2D AnchorBottomCenter() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AnchorBottomLeft
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 0, 1 } を返します。
		/// @return Vector2D{ 0, 1 }
		[[nodiscard]]
		static constexpr Vector2D AnchorBottomLeft() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AnchorLeftCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ 0, 0.5 } を返します。
		/// @return Vector2D{ 0, 0.5 }
		[[nodiscard]]
		static constexpr Vector2D AnchorLeftCenter() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Vector2D& value)
		{
			return output  << CharType('(')
				<< value.x << CharType(',') << CharType(' ')
				<< value.y << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Vector2D& value)
		{
			CharType unused;
			return input   >> unused
				>> value.x >> unused
				>> value.y >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Vector2D& value)
		{
			_Formatter(formatData, value);
		}

		static void _Formatter(FormatData& formatData, const Vector2D& value);
	};

	using Float2	= Vector2D<float>;
	using Vec2		= Vector2D<double>;
	using SizeF		= Vec2;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Float2>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Float2& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Vec2>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Vec2& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Float2, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Float2& value, s3d::BufferContext& ctx);
};

template <>
struct fmt::formatter<s3d::Vec2, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Vec2& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Float2>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Float2& value) const noexcept
	{
		return value.hash();
	}
};

template <>
struct std::hash<s3d::Vec2>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Vec2& value) const noexcept
	{
		return value.hash();
	}
};
