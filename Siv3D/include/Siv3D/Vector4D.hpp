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
	//	Vector4D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 4 次元のベクトル
	/// @tparam Type ベクトルの要素の型
	template <class Type>
	struct Vector4D
	{
		/// @brief ベクトルの要素の型
		using value_type = Type;

		/// @brief ベクトルの次元数
		static constexpr size_t Dimension = 4;

		/// @brief ベクトルの X 成分
		value_type x;

		/// @brief ベクトルの Y 成分
		value_type y;

		/// @brief ベクトルの Z 成分
		value_type z;

		/// @brief ベクトルの W 成分
		value_type w;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vector4D() = default;

		[[nodiscard]]
		constexpr Vector4D(value_type _x, value_type _y, value_type _z, value_type _w) noexcept;

		[[nodiscard]]
		constexpr Vector4D(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _z, Concept::Arithmetic auto _w) noexcept;

		template <class U>
		[[nodiscard]]
		constexpr Vector4D(const Vector4D<U>& v) noexcept;

		[[nodiscard]]
		constexpr Vector4D(const Vector2D<value_type>& xy, Concept::Arithmetic auto _z, Concept::Arithmetic auto _w) noexcept;

		[[nodiscard]]
		constexpr Vector4D(Concept::Arithmetic auto _x, const Vector2D<value_type>& yz, Concept::Arithmetic auto _w) noexcept;

		[[nodiscard]]
		constexpr Vector4D(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, const Vector2D<value_type>& zw) noexcept;

		[[nodiscard]]
		constexpr Vector4D(const Vector2D<value_type>& xy, const Vector2D<value_type>& zw) noexcept;

		[[nodiscard]]
		constexpr Vector4D(const Vector3D<value_type>& xyz, Concept::Arithmetic auto _z) noexcept;

		[[nodiscard]]
		constexpr Vector4D(Concept::Arithmetic auto _x, const Vector3D<value_type>& yzw) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	elem
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr value_type elem(size_t index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointer
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr value_type* getPointer() noexcept;

		[[nodiscard]]
		constexpr const value_type* getPointer() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector4D operator +() const noexcept;

		[[nodiscard]]
		constexpr Vector4D operator +(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector4D operator -() const noexcept;

		[[nodiscard]]
		constexpr Vector4D operator -(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector4D operator *(value_type s) const noexcept;

		[[nodiscard]]
		constexpr Vector4D operator *(Vector4D v) const noexcept;

		template <Concept::Arithmetic U> // IntelliSense のエラー回避のため template を使う (17.12 Preview 2)
		[[nodiscard]]
		friend constexpr Vector4D operator *(const U s, const Vector4D v) noexcept
		{
			return (v * static_cast<value_type>(s));
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator /
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector4D operator /(value_type s) const noexcept;

		[[nodiscard]]
		constexpr Vector4D operator /(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector4D& operator +=(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector4D& operator -=(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector4D& operator *=(value_type s) noexcept;

		constexpr Vector4D& operator *=(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator /=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector4D& operator /=(value_type s) noexcept;

		constexpr Vector4D& operator /=(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const Vector4D lhs, const Vector4D rhs) noexcept
		{
			return ((lhs.x == rhs.x)
				&&  (lhs.y == rhs.y)
				&&  (lhs.z == rhs.z)
				&&  (lhs.w == rhs.w));
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
		constexpr bool epsilonEquals(Vector4D other, value_type epsilon) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasSameDirection
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した別のベクトルとの内積が正であるかを返します。
		/// @param other 別のベクトル
		/// @return 別のベクトルとの内積が正である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasSameDirection(Vector4D other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasOppositeDirection
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した別のベクトルとの内積が負であるかを返します。
		/// @param other 別のベクトル
		/// @return 別のベクトルとの内積が負である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasOppositeDirection(Vector4D other) const noexcept;

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
		//	withX, withY, withZ, withW
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分のみを変更した自身のコピーを返します。
		/// @param _x x 成分
		/// @return x 成分を変更したコピー
		[[nodiscard]]
		constexpr Vector4D withX(value_type _x) const noexcept;

		/// @brief y 成分のみを変更した自身のコピーを返します。
		/// @param _y y 成分
		/// @return y 成分を変更したコピー
		[[nodiscard]]
		constexpr Vector4D withY(value_type _y) const noexcept;

		/// @brief z 成分のみを変更した自身のコピーを返します。
		/// @param _z z 成分
		/// @return z 成分を変更したコピー
		[[nodiscard]]
		constexpr Vector4D withZ(value_type _z) const noexcept;

		/// @brief w 成分のみを変更した自身のコピーを返します。
		/// @param _w w 成分
		/// @return w 成分を変更したコピー
		[[nodiscard]]
		constexpr Vector4D withW(value_type _w) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector4D& set(const Vector2D<value_type>& xy, const Vector2D<value_type>& zw) noexcept;

		constexpr Vector4D& set(const Vector2D<value_type>& xy, value_type _z, value_type _w) noexcept;

		constexpr Vector4D& set(value_type _x, const Vector2D<value_type>& yz, value_type _w) noexcept;

		constexpr Vector4D& set(value_type _x, value_type _y, const Vector2D<value_type>& zw) noexcept;

		constexpr Vector4D& set(const Vector3D<value_type>& xyz, value_type _w) noexcept;

		constexpr Vector4D& set(value_type _x, const Vector3D<value_type>& yzw) noexcept;

		/// @brief 各成分を変更します。
		/// @param _x 新しい X 成分
		/// @param _y 新しい Y 成分
		/// @param _z 新しい Z 成分
		/// @param _w 新しい W 成分
		/// @return *this
		constexpr Vector4D& set(value_type _x, value_type _y, value_type _z, value_type _w) noexcept;

		/// @brief ベクトルを変更します。
		/// @param v 新しいベクトル
		/// @return *this
		constexpr Vector4D& set(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 位置ベクトルを移動させた新しいベクトルを返します。
		/// @param _x X 成分の移動量
		/// @param _y Y 成分の移動量
		/// @param _z Z 成分の移動量
		/// @param _w W 成分の移動量
		/// @return 移動後の新しいベクトル
		[[nodiscard]]
		constexpr Vector4D movedBy(value_type _x, value_type _y, value_type _z, value_type _w) const noexcept;

		/// @brief 位置ベクトルを移動させた新しいベクトルを返します。
		/// @param v 移動量
		/// @return 移動後の新しいベクトル
		[[nodiscard]]
		constexpr Vector4D movedBy(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 位置ベクトルを移動させます。
		/// @param _x X 成分の移動量
		/// @param _y Y 成分の移動量
		/// @param _z Z 成分の移動量
		/// @param _w W 成分の移動量
		/// @return *this
		constexpr Vector4D& moveBy(value_type _x, value_type _y, value_type _z, value_type _w) noexcept;

		/// @brief 位置ベクトルを移動させます。
		/// @param v 移動量
		/// @return *this
		constexpr Vector4D& moveBy(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	dot
		//
		////////////////////////////////////////////////////////////////

		template <class U>
		[[nodiscard]]
		constexpr auto dot(Vector4D<U> v) const noexcept;

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
		/// @param _z 別の位置ベクトルの Z 成分
		/// @param _w 別の位置ベクトルの W 成分
		/// @return 別の位置ベクトルからのマンハッタン距離
		/// @remark `.manhattanDistanceTo(_x, _y, _z, _w)` と同じです。
		[[nodiscard]]
		constexpr value_type manhattanDistanceFrom(value_type _x, value_type _y, value_type _z, value_type _w) const noexcept;

		/// @brief 別の位置ベクトルからのマンハッタン距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルからのマンハッタン距離
		/// @remark `.manhattanDistanceTo(v)` と同じです。
		[[nodiscard]]
		constexpr value_type manhattanDistanceFrom(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	manhattanDistanceTo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルへのマンハッタン距離を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @param _z 別の位置ベクトルの Z 成分
		/// @param _w 別の位置ベクトルの W 成分
		/// @return 別の位置ベクトルへのマンハッタン距離
		/// @remark `.manhattanDistanceFrom(_x, _y, _z, _w)` と同じです。
		[[nodiscard]]
		constexpr value_type manhattanDistanceTo(value_type _x, value_type _y, value_type _z, value_type _w) const noexcept;

		/// @brief 別の位置ベクトルへのマンハッタン距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルへのマンハッタン距離
		/// @remark `.manhattanDistanceFrom(v)` と同じです。
		[[nodiscard]]
		constexpr value_type manhattanDistanceTo(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルからの距離を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @param _z 別の位置ベクトルの Z 成分
		/// @param _w 別の位置ベクトルの W 成分
		/// @return 別の位置ベクトルからの距離
		/// @remark `.distanceTo(_x, _y, _z, _w)` と同じです。
		[[nodiscard]]
		value_type distanceFrom(value_type _x, value_type _y, value_type _z, value_type _w) const noexcept;

		/// @brief 別の位置ベクトルからの距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルからの距離
		/// @remark `.distanceTo(v)` と同じです。
		[[nodiscard]]
		value_type distanceFrom(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceFromSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルからの距離の二乗を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @param _z 別の位置ベクトルの Z 成分
		/// @param _w 別の位置ベクトルの W 成分
		/// @return 別の位置ベクトルからの距離の二乗
		/// @remark 平方根を計算しないため `distanceFrom()` より高速です。
		/// @remark `.distanceToSq(_x, _y, _z, _w)` と同じです。
		[[nodiscard]]
		constexpr value_type distanceFromSq(value_type _x, value_type _y, value_type _z, value_type _w) const noexcept;

		/// @brief 別の位置ベクトルからの距離の二乗を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルからの距離の二乗
		/// @remark 平方根を計算しないため `distanceFrom()` より高速です。
		/// @remark `.distanceToSq(v)` と同じです。
		[[nodiscard]]
		constexpr value_type distanceFromSq(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceTo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルへの距離を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @param _z 別の位置ベクトルの Z 成分
		/// @param _w 別の位置ベクトルの W 成分
		/// @return 別の位置ベクトルへの距離
		/// @remark `.distanceFrom(_x, _y, _z, _w)` と同じです。
		[[nodiscard]]
		value_type distanceTo(value_type _x, value_type _y, value_type _z, value_type _w) const noexcept;

		/// @brief 別の位置ベクトルへの距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルへの距離
		/// @remark `.distanceFrom(v)` と同じです。
		[[nodiscard]]
		value_type distanceTo(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceToSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルへの距離の二乗を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @param _z 別の位置ベクトルの Z 成分
		/// @param _w 別の位置ベクトルの W 成分
		/// @return 別の位置ベクトルへの距離の二乗
		/// @remark 平方根を計算しないため `distanceTo()` より高速です。
		/// @remark `.distanceFromSq(_x, _y, _z, _w)` と同じです。
		[[nodiscard]]
		constexpr value_type distanceToSq(value_type _x, value_type _y, value_type _z, value_type _w) const noexcept;

		/// @brief 別の位置ベクトルへの距離の二乗を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルへの距離の二乗
		/// @remark 平方根を計算しないため `distanceTo()` より高速です。
		/// @remark `.distanceToSq(v)` と同じです。
		[[nodiscard]]
		constexpr value_type distanceToSq(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 向きが同じで大きさ（長さ）を変更した新しいベクトルを返します。
		/// @param _length ベクトルの大きさ（長さ）
		/// @return 新しいベクトル。自身がゼロベクトルの場合はゼロベクトル
		[[nodiscard]]
		Vector4D withLength(value_type _length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの向きはそのままで、大きさ（長さ）だけを変更します。
		/// @param _length ベクトルの大きさ（長さ）
		/// @remark 自身がゼロベクトルの場合は何もしません。
		/// @return *this
		Vector4D& setLength(value_type _length) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withMaxLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 向きが同じで大きさ（長さ）を一定の値以下にした新しいベクトルを返します。
		/// @param maxLength ベクトルの最大の大きさ（長さ）
		/// @return 大きさ（長さ）を一定の値以下にした新しいベクトル
		[[nodiscard]]
		Vector4D withMaxLength(value_type maxLength) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setMaxLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの向きはそのままで、大きさ（長さ）を一定の値以下にします
		/// @param maxLength ベクトルの最大の大きさ（長さ）
		/// @return *this
		Vector4D& setMaxLength(value_type maxLength) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化した（大きさを 1 にした）ベクトルを返します。
		/// @return 正規化した（大きさを 1 にした）ベクトル
		[[nodiscard]]
		Vector4D normalized() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalized_or
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化した（大きさを 1 にした）ベクトルを返します。ゼロベクトルの場合は valueIfZero を返します。
		/// @param valueIfZero ゼロベクトルの場合に返すベクトル
		/// @return 正規化した（大きさを 1 にした）ベクトル、または valueIfZero
		[[nodiscard]]
		Vector4D normalized_or(Vector4D valueIfZero) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 自身を正規化（大きさを 1 に）します。
		/// @return *this
		Vector4D& normalize() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getMidpoint
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector4D getMidpoint(Vector4D other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector4D lerp(Vector4D other, value_type f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xx, xy, xz, xw, yx, yy, yz, yw, zx, zy, zz, zw, wx, wy, wz, ww
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector2D<value_type> xx() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> xy() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> xz() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> xw() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> yx() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> yy() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> yz() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> yw() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> zx() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> zy() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> zz() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> zw() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> wx() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> wy() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> wz() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> ww() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xxx, xyz, yyy, yxz, yzw, zzz, zyx, www, wzy
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector3D<value_type> xxx() const noexcept;

		[[nodiscard]]
		constexpr Vector3D<value_type> xyz() const noexcept;

		[[nodiscard]]
		constexpr Vector3D<value_type> yyy() const noexcept;

		[[nodiscard]]
		constexpr Vector3D<value_type> yxz() const noexcept;

		[[nodiscard]]
		constexpr Vector3D<value_type> yzw() const noexcept;

		[[nodiscard]]
		constexpr Vector3D<value_type> zzz() const noexcept;

		[[nodiscard]]
		constexpr Vector3D<value_type> zyx() const noexcept;

		[[nodiscard]]
		constexpr Vector3D<value_type> www() const noexcept;

		[[nodiscard]]
		constexpr Vector3D<value_type> wzy() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xyz0, xyz1
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector4D xyz0() const noexcept;

		[[nodiscard]]
		constexpr Vector4D xyz1() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xyzw, xxxx, yyyy, zzzz, wwww, wzyx
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector4D xyzw() const noexcept;

		[[nodiscard]]
		constexpr Vector4D xxxx() const noexcept;

		[[nodiscard]]
		constexpr Vector4D yyyy() const noexcept;

		[[nodiscard]]
		constexpr Vector4D zzzz() const noexcept;

		[[nodiscard]]
		constexpr Vector4D wwww() const noexcept;

		[[nodiscard]]
		constexpr Vector4D wzyx() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Vector4D Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	One
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Vector4D One() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	All
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Vector4D All(value_type value = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UnitX, UnitY, UnitZ, UnitW
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Vector4D UnitX() noexcept;

		[[nodiscard]]
		static constexpr Vector4D UnitY() noexcept;

		[[nodiscard]]
		static constexpr Vector4D UnitZ() noexcept;

		[[nodiscard]]
		static constexpr Vector4D UnitW() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Vector4D& value)
		{
			return output  << CharType('(')
				<< value.x << CharType(',') << CharType(' ')
				<< value.y << CharType(',') << CharType(' ')
				<< value.z << CharType(',') << CharType(' ')
				<< value.w << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Vector4D& value)
		{
			CharType unused;
			return input   >> unused
				>> value.x >> unused
				>> value.y >> unused
				>> value.z >> unused
				>> value.w >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Vector4D& value)
		{
			_Formatter(formatData, value);
		}

		static void _Formatter(FormatData& formatData, const Vector4D& value);
	};

	using Float4	= Vector4D<float>;
	using Vec4		= Vector4D<double>;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Float4>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Float4& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Vec4>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Vec4& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Float4, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Float4& value, s3d::BufferContext& ctx);
};

template <>
struct fmt::formatter<s3d::Vec4, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Vec4& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Float4>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Float4& value) const noexcept
	{
		return value.hash();
	}
};

template <>
struct std::hash<s3d::Vec4>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Vec4& value) const noexcept
	{
		return value.hash();
	}
};
