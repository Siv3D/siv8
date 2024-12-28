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
	/// @tparam Float ベクトルの要素の型
	template <Concept::FloatingPoint Float>
	struct Vector4D
	{
		/// @brief ベクトルの要素の型
		using value_type = Float;

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

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Vector4D() = default;

		/// @brief 4 次元のベクトルを作成します。
		/// @param _x X 成分
		/// @param _y Y 成分
		/// @param _z Z 成分
		/// @param _w W 成分
		[[nodiscard]]
		constexpr Vector4D(value_type _x, value_type _y, value_type _z, value_type _w) noexcept;

		/// @brief 4 次元のベクトルを作成します。
		/// @param _x X 成分
		/// @param _y Y 成分
		/// @param _z Z 成分
		/// @param _w W 成分
		[[nodiscard]]
		constexpr Vector4D(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _z, Concept::Arithmetic auto _w) noexcept;

		/// @brief 4 次元のベクトルをコピーして作成します。
		/// @tparam U コピーするベクトルの要素の型
		/// @param v コピーするベクトル
		template <Concept::FloatingPoint U>
		[[nodiscard]]
		constexpr Vector4D(const Vector4D<U>& v) noexcept;

		/// @brief 4 次元のベクトルを作成します。
		/// @param xy XY 成分
		/// @param _z Z 成分
		/// @param _w W 成分
		[[nodiscard]]
		constexpr Vector4D(const Vector2D<value_type>& xy, Concept::Arithmetic auto _z, Concept::Arithmetic auto _w) noexcept;

		/// @brief 4 次元のベクトルを作成します。
		/// @param _x X 成分
		/// @param yz YZ 成分
		/// @param _w W 成分
		[[nodiscard]]
		constexpr Vector4D(Concept::Arithmetic auto _x, const Vector2D<value_type>& yz, Concept::Arithmetic auto _w) noexcept;

		/// @brief 4 次元のベクトルを作成します。
		/// @param _x X 成分
		/// @param _y Y 成分
		/// @param zw ZW 成分
		[[nodiscard]]
		constexpr Vector4D(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, const Vector2D<value_type>& zw) noexcept;

		/// @brief 4 次元のベクトルを作成します。
		/// @param xy XY 成分
		/// @param zw ZW 成分
		[[nodiscard]]
		constexpr Vector4D(const Vector2D<value_type>& xy, const Vector2D<value_type>& zw) noexcept;

		/// @brief 4 次元のベクトルを作成します。
		/// @param xyz XYZ 成分
		/// @param _w W 成分
		[[nodiscard]]
		constexpr Vector4D(const Vector3D<value_type>& xyz, Concept::Arithmetic auto _w) noexcept;

		/// @brief 4 次元のベクトルを作成します。
		/// @param _x X 成分
		/// @param yzw YZW 成分
		[[nodiscard]]
		constexpr Vector4D(Concept::Arithmetic auto _x, const Vector3D<value_type>& yzw) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	elem
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの要素を返します。
		/// @param index インデックス
		/// @return 指定したインデックスの要素。index が範囲外の場合は 0
		/// @remark index が 0 の場合は x, 1 の場合は y, 2 の場合は z, 3 の場合は w を返します。
		[[nodiscard]]
		constexpr value_type elem(size_t index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointer
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分へのポインタを返します。
		/// @return x 成分へのポインタ
		/// @remark 戻り値に対して [0] で x 成分、[1] で y 成分、[2] で z 成分、[3] で w 成分にアクセスできます。
		[[nodiscard]]
		constexpr value_type* getPointer() noexcept;

		/// @brief x 成分へのポインタを返します。
		/// @return x 成分へのポインタ
		/// @remark 戻り値に対して [0] で x 成分、[1] で y 成分、[2] で z 成分、[3] で w 成分にアクセスできます。
		[[nodiscard]]
		constexpr const value_type* getPointer() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		/// @brief 自身を返します。
		/// @return *this
		[[nodiscard]]
		constexpr Vector4D operator +() const noexcept;

		/// @brief ベクトルを加算した結果を返します。
		/// @param v 加算するベクトル
		/// @return 加算した結果
		[[nodiscard]]
		constexpr Vector4D operator +(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		/// @brief 符号を反転したベクトルを返します。
		/// @return 符号を反転したベクトル
		[[nodiscard]]
		constexpr Vector4D operator -() const noexcept;

		/// @brief ベクトルを減算した結果を返します。
		/// @param v 減算するベクトル
		/// @return 減算した結果
		[[nodiscard]]
		constexpr Vector4D operator -(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの各成分にスカラーを乗算した結果を返します。
		/// @param s スカラー
		/// @return 乗算した結果
		[[nodiscard]]
		constexpr Vector4D operator *(value_type s) const noexcept;

		/// @brief ベクトル同士の各成分同士を乗算した結果を返します。
		/// @param v 乗算するベクトル
		/// @return 乗算した結果
		[[nodiscard]]
		constexpr Vector4D operator *(Vector4D v) const noexcept;

		/// @brief ベクトルの各成分にスカラーを乗算した結果を返します。
		/// @tparam U スカラーの型
		/// @param s スカラー
		/// @param v ベクトル
		/// @return 乗算した結果
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

		/// @brief ベクトルの各成分をスカラーで除算した結果を返します。
		/// @param s スカラー
		/// @return 除算した結果
		[[nodiscard]]
		constexpr Vector4D operator /(value_type s) const noexcept;

		/// @brief ベクトル同士の各成分同士を除算した結果を返します。
		/// @param v 除算するベクトル
		/// @return 除算した結果
		[[nodiscard]]
		constexpr Vector4D operator /(Vector4D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルを加算します。
		/// @param v 加算するベクトル
		/// @return *this
		constexpr Vector4D& operator +=(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルを減算します。
		/// @param v 減算するベクトル
		/// @return *this
		constexpr Vector4D& operator -=(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの各成分にスカラーを乗算します。
		/// @param s スカラー
		/// @return *this
		constexpr Vector4D& operator *=(value_type s) noexcept;

		/// @brief ベクトル同士の各成分同士を乗算します。
		/// @param v 乗算するベクトル
		/// @return *this
		constexpr Vector4D& operator *=(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator /=
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの各成分をスカラーで除算します。
		/// @param s スカラー
		/// @return *this
		constexpr Vector4D& operator /=(value_type s) noexcept;

		/// @brief ベクトル同士の各成分同士を除算します。
		/// @param v 除算するベクトル
		/// @return *this
		constexpr Vector4D& operator /=(Vector4D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのベクトルが等しいかを返します。
		/// @param lhs 一方のベクトル
		/// @param rhs もう一方のベクトル
		/// @return 2 つのベクトルが等しい場合 true, それ以外の場合は false
		/// @remark 誤差を許容して比較したい場合は `.epsilonEquals()` を使用してください。
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
		/// @remark Vec4{ 3, -2, 0, 0 } の場合、-2 を返します。
		/// @return 最小の成分
		[[nodiscard]]
		constexpr value_type minComponent() const noexcept;

		/// @brief 最大の成分を返します。
		/// @remark Vec4{ 3, -4, 0, 0 } の場合、3 を返します。
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

		/// @brief 各成分を変更します。
		/// @param xy 新しい XY 成分
		/// @param zw 新しい ZW 成分
		/// @return *this
		constexpr Vector4D& set(const Vector2D<value_type>& xy, const Vector2D<value_type>& zw) noexcept;

		/// @brief 各成分を変更します。
		/// @param xy 新しい XY 成分
		/// @param _z 新しい Z 成分
		/// @param _w 新しい W 成分
		/// @return *this
		constexpr Vector4D& set(const Vector2D<value_type>& xy, value_type _z, value_type _w) noexcept;

		/// @brief 各成分を変更します。
		/// @param _x 新しい X 成分
		/// @param yz 新しい YZ 成分
		/// @param _w 新しい W 成分
		/// @return *this
		constexpr Vector4D& set(value_type _x, const Vector2D<value_type>& yz, value_type _w) noexcept;

		/// @brief 各成分を変更します。
		/// @param _x 新しい X 成分
		/// @param _y 新しい Y 成分
		/// @param zw 新しい ZW 成分
		/// @return *this
		constexpr Vector4D& set(value_type _x, value_type _y, const Vector2D<value_type>& zw) noexcept;

		/// @brief 各成分を変更します。
		/// @param xyz 新しい XYZ 成分
		/// @param _w 新しい W 成分
		/// @return *this
		constexpr Vector4D& set(const Vector3D<value_type>& xyz, value_type _w) noexcept;

		/// @brief 各成分を変更します。
		/// @param _x 新しい X 成分
		/// @param yzw 新しい YZW 成分
		/// @return *this
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

		/// @brief ベクトル同士の内積を返します。
		/// @tparam U もう一方のベクトルの要素の型
		/// @param v もう一方のベクトル
		/// @return 内積
		template <Concept::FloatingPoint U>
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

		/// @brief 2 つのベクトルの中間点を返します。
		/// @param other もう一方のベクトル
		/// @return 2 つのベクトルの中間点
		[[nodiscard]]
		constexpr Vector4D getMidpoint(Vector4D other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのベクトル間を線形補間したベクトルを返します。
		/// @param other もう一方のベクトル
		/// @param f 補間係数
		/// @return 2 つのベクトル間を線形補間したベクトル
		[[nodiscard]]
		constexpr Vector4D lerp(Vector4D other, value_type f) const noexcept;

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
		//	xx, xy, xz, xw, yx, yy, yz, yw, zx, zy, zz, zw, wx, wy, wz, ww
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector2D{ x, x } を返します。
		/// @return Vector2D{ x, x }
		[[nodiscard]]
		constexpr Vector2D<value_type> xx() const noexcept;

		/// @brief Vector2D{ x, y } を返します。
		/// @return Vector2D{ x, y }
		[[nodiscard]]
		constexpr Vector2D<value_type> xy() const noexcept;

		/// @brief Vector2D{ x, z } を返します。
		/// @return Vector2D{ x, z }
		[[nodiscard]]
		constexpr Vector2D<value_type> xz() const noexcept;

		/// @brief Vector2D{ x, w } を返します。
		/// @return Vector2D{ x, w }
		[[nodiscard]]
		constexpr Vector2D<value_type> xw() const noexcept;

		/// @brief Vector2D{ y, x } を返します。
		/// @return Vector2D{ y, x }
		[[nodiscard]]
		constexpr Vector2D<value_type> yx() const noexcept;

		/// @brief Vector2D{ y, y } を返します。
		/// @return Vector2D{ y, y }
		[[nodiscard]]
		constexpr Vector2D<value_type> yy() const noexcept;

		/// @brief Vector2D{ y, z } を返します。
		/// @return Vector2D{ y, z }
		[[nodiscard]]
		constexpr Vector2D<value_type> yz() const noexcept;

		/// @brief Vector2D{ y, w } を返します。
		/// @return Vector2D{ y, w }
		[[nodiscard]]
		constexpr Vector2D<value_type> yw() const noexcept;

		/// @brief Vector2D{ z, x } を返します。
		/// @return Vector2D{ z, x }
		[[nodiscard]]
		constexpr Vector2D<value_type> zx() const noexcept;

		/// @brief Vector2D{ z, y } を返します。
		/// @return Vector2D{ z, y }
		[[nodiscard]]
		constexpr Vector2D<value_type> zy() const noexcept;

		/// @brief Vector2D{ z, z } を返します。
		/// @return Vector2D{ z, z }
		[[nodiscard]]
		constexpr Vector2D<value_type> zz() const noexcept;

		/// @brief Vector2D{ z, w } を返します。
		/// @return Vector2D{ z, w }
		[[nodiscard]]
		constexpr Vector2D<value_type> zw() const noexcept;

		/// @brief Vector2D{ w, x } を返します。
		/// @return Vector2D{ w, x }
		[[nodiscard]]
		constexpr Vector2D<value_type> wx() const noexcept;

		/// @brief Vector2D{ w, y } を返します。
		/// @return Vector2D{ w, y }
		[[nodiscard]]
		constexpr Vector2D<value_type> wy() const noexcept;

		/// @brief Vector2D{ w, z } を返します。
		/// @return Vector2D{ w, z }
		[[nodiscard]]
		constexpr Vector2D<value_type> wz() const noexcept;

		/// @brief Vector2D{ w, w } を返します。
		/// @return Vector2D{ w, w }
		[[nodiscard]]
		constexpr Vector2D<value_type> ww() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xxx, xyz, yyy, yxz, yzw, zzz, zyx, www, wzy
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector3D{ x, x, x } を返します。
		/// @return Vector3D{ x, x, x }
		[[nodiscard]]
		constexpr Vector3D<value_type> xxx() const noexcept;

		/// @brief Vector3D{ x, y, z } を返します。
		/// @return Vector3D{ x, y, z }
		[[nodiscard]]
		constexpr Vector3D<value_type> xyz() const noexcept;

		/// @brief Vector3D{ y, y, y } を返します。
		/// @return Vector3D{ y, y, y }
		[[nodiscard]]
		constexpr Vector3D<value_type> yyy() const noexcept;

		/// @brief Vector3D{ y, x, z } を返します。
		/// @return Vector3D{ y, x, z }
		[[nodiscard]]
		constexpr Vector3D<value_type> yxz() const noexcept;

		/// @brief Vector3D{ y, z, w } を返します。
		/// @return Vector3D{ y, z, w }
		[[nodiscard]]
		constexpr Vector3D<value_type> yzw() const noexcept;

		/// @brief Vector3D{ z, z, z } を返します。
		/// @return Vector3D{ z, z, z }
		[[nodiscard]]
		constexpr Vector3D<value_type> zzz() const noexcept;

		/// @brief Vector3D{ z, y, x } を返します。
		/// @return Vector3D{ z, y, x }
		[[nodiscard]]
		constexpr Vector3D<value_type> zyx() const noexcept;

		/// @brief Vector3D{ w, w, w } を返します。
		/// @return Vector3D{ w, w, w }
		[[nodiscard]]
		constexpr Vector3D<value_type> www() const noexcept;

		/// @brief Vector3D{ w, z, y } を返します。
		/// @return Vector3D{ w, z, y }
		[[nodiscard]]
		constexpr Vector3D<value_type> wzy() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xyz0, xyz1
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector4D{ x, y, z, 0 } を返します。
		/// @return Vector4D{ x, y, z, 0 }
		[[nodiscard]]
		constexpr Vector4D xyz0() const noexcept;

		/// @brief Vector4D{ x, y, z, 1 } を返します。
		/// @return Vector4D{ x, y, z, 1 }
		[[nodiscard]]
		constexpr Vector4D xyz1() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xyzw, xxxx, yyyy, zzzz, wwww, wzyx
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector4D{ x, y, z, w } を返します。
		/// @return Vector4D{ x, y, z, w }
		[[nodiscard]]
		constexpr Vector4D xyzw() const noexcept;

		/// @brief Vector4D{ x, x, x, x } を返します。
		/// @return Vector4D{ x, x, x, x }
		[[nodiscard]]
		constexpr Vector4D xxxx() const noexcept;

		/// @brief Vector4D{ y, y, y, y } を返します。
		/// @return Vector4D{ y, y, y, y }
		[[nodiscard]]
		constexpr Vector4D yyyy() const noexcept;

		/// @brief Vector4D{ z, z, z, z } を返します。
		/// @return Vector4D{ z, z, z, z }
		[[nodiscard]]
		constexpr Vector4D zzzz() const noexcept;

		/// @brief Vector4D{ w, w, w, w } を返します。
		/// @return Vector4D{ w, w, w, w }
		[[nodiscard]]
		constexpr Vector4D wwww() const noexcept;

		/// @brief Vector4D{ w, z, y, x } を返します。
		/// @return Vector4D{ w, z, y, x }
		[[nodiscard]]
		constexpr Vector4D wzyx() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		/// @brief ゼロベクトル Vector4D{ 0, 0, 0, 0 } を返します。
		/// @return Vector4D{ 0, 0, 0, 0 }
		[[nodiscard]]
		static constexpr Vector4D Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	One
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての成分が 1 のベクトル Vector4D{ 1, 1, 1, 1 } を返します。
		/// @return Vector4D{ 1, 1, 1, 1 }
		[[nodiscard]]
		static constexpr Vector4D One() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	All
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての成分が value のベクトル Vector4D{ value, value, value, value } を返します。
		/// @param value 成分の値
		/// @return Vector4D{ value, value, value, value }
		[[nodiscard]]
		static constexpr Vector4D All(value_type value = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UnitX, UnitY, UnitZ, UnitW
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 成分が 1 でそれ以外が 0 のベクトル Vector4D{ 1, 0, 0, 0 } を返します。
		/// @return Vector4D{ 1, 0, 0, 0 }
		[[nodiscard]]
		static constexpr Vector4D UnitX() noexcept;

		/// @brief Y 成分が 1 でそれ以外が 0 のベクトル Vector4D{ 0, 1, 0, 0 } を返します。
		/// @return Vector4D{ 0, 1, 0, 0 }
		[[nodiscard]]
		static constexpr Vector4D UnitY() noexcept;

		/// @brief Z 成分が 1 でそれ以外が 0 のベクトル Vector4D{ 0, 0, 1, 0 } を返します。
		/// @return Vector4D{ 0, 0, 1, 0 }
		[[nodiscard]]
		static constexpr Vector4D UnitZ() noexcept;

		/// @brief W 成分が 1 でそれ以外が 0 のベクトル Vector4D{ 0, 0, 0, 1 } を返します。
		/// @return Vector4D{ 0, 0, 0, 1 }
		[[nodiscard]]
		static constexpr Vector4D UnitW() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルを出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value ベクトル
		/// @return 出力ストリーム
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

		/// @brief ベクトルを入力します。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value 入力した値を格納するベクトル
		/// @return 入力ストリーム
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

		/// @brief ベクトルを文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value ベクトル
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Vector4D& value)
		{
			_Formatter(formatData, value);
		}

	private:

		static void _Formatter(FormatData& formatData, const Vector4D& value);
	};

	////////////////////////////////////////////////////////////////
	//
	//	Float4, Vec4
	//
	////////////////////////////////////////////////////////////////

	/// @brief 成分が float 型の 4 次元ベクトル
	using Float4 = Vector4D<float>;

	/// @brief 成分が double 型の 4 次元ベクトル
	using Vec4 = Vector4D<double>;
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
