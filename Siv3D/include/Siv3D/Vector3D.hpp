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
	//	Vector3D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3 次元のベクトル
	/// @tparam Type ベクトルの要素の型
	template <class Type>
	struct Vector3D
	{
		/// @brief ベクトルの要素の型
		using value_type = Type;

		/// @brief ベクトルの次元数
		static constexpr size_t Dimension = 3;

		/// @brief ベクトルの X 成分
		value_type x;

		/// @brief ベクトルの Y 成分
		value_type y;

		/// @brief ベクトルの Z 成分
		value_type z;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vector3D() = default;

		[[nodiscard]]
		constexpr Vector3D(value_type _x, value_type _y, value_type _z) noexcept;

		[[nodiscard]]
		constexpr Vector3D(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _z) noexcept;

		[[nodiscard]]
		constexpr Vector3D(Point3D p) noexcept;

		template <class U>
		[[nodiscard]]
		constexpr Vector3D(const Vector3D<U>& v) noexcept;

		[[nodiscard]]
		constexpr Vector3D(const Vector2D<value_type>& xy, Concept::Arithmetic auto _z) noexcept;

		[[nodiscard]]
		constexpr Vector3D(Concept::Arithmetic auto _x, const Vector2D<value_type>& yz) noexcept;

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
		constexpr Vector3D operator +() const noexcept;

		[[nodiscard]]
		constexpr Vector3D operator +(Vector3D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector3D operator -() const noexcept;

		[[nodiscard]]
		constexpr Vector3D operator -(Vector3D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector3D operator *(value_type s) const noexcept;

		[[nodiscard]]
		constexpr Vector3D operator *(Vector3D v) const noexcept;

		template <Concept::Arithmetic U> // IntelliSense のエラー回避のため template を使う (17.10 Preview 1)
		[[nodiscard]]
		friend constexpr Vector3D operator *(const U s, const Vector3D v) noexcept
		{
			return (v * static_cast<value_type>(s));
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator /
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector3D operator /(value_type s) const noexcept;

		[[nodiscard]]
		constexpr Vector3D operator /(Vector3D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector3D& operator +=(Vector3D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector3D& operator -=(Vector3D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector3D& operator *=(value_type s) noexcept;

		constexpr Vector3D& operator *=(Vector3D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator /=
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector3D& operator /=(value_type s) noexcept;

		constexpr Vector3D& operator /=(Vector3D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const Vector3D lhs, const Vector3D rhs) noexcept
		{
			return ((lhs.x == rhs.x)
				&&  (lhs.y == rhs.y)
				&&  (lhs.z == rhs.z));
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
		constexpr bool epsilonEquals(Vector3D other, value_type epsilon) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasSameDirection
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した別のベクトルとの内積が正であるかを返します。
		/// @param other 別のベクトル
		/// @return 別のベクトルとの内積が正である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasSameDirection(Vector3D other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasOppositeDirection
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した別のベクトルとの内積が負であるかを返します。
		/// @param other 別のベクトル
		/// @return 別のベクトルとの内積が負である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasOppositeDirection(Vector3D other) const noexcept;

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
		//	withX, withY, withZ
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分のみを変更した自身のコピーを返します。
		/// @param _x x 成分
		/// @return x 成分を変更したコピー
		[[nodiscard]]
		constexpr Vector3D withX(value_type _x) const noexcept;

		/// @brief y 成分のみを変更した自身のコピーを返します。
		/// @param _y y 成分
		/// @return y 成分を変更したコピー
		[[nodiscard]]
		constexpr Vector3D withY(value_type _y) const noexcept;

		/// @brief z 成分のみを変更した自身のコピーを返します。
		/// @param _z z 成分
		/// @return z 成分を変更したコピー
		[[nodiscard]]
		constexpr Vector3D withZ(value_type _z) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		constexpr Vector3D& set(const Vector2D<value_type>& xy, value_type _z) noexcept;

		constexpr Vector3D& set(value_type _x, const Vector2D<value_type>& yz) noexcept;

		/// @brief 各成分を変更します。
		/// @param _x 新しい X 成分
		/// @param _y 新しい Y 成分
		/// @param _z 新しい Z 成分
		/// @return *this
		constexpr Vector3D& set(value_type _x, value_type _y, value_type _z) noexcept;

		/// @brief ベクトルを変更します。
		/// @param v 新しいベクトル
		/// @return *this
		constexpr Vector3D& set(Vector3D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 位置ベクトルを移動させた新しいベクトルを返します。
		/// @param _x X 成分の移動量
		/// @param _y Y 成分の移動量
		/// @param _z Z 成分の移動量
		/// @return 移動後の新しいベクトル
		[[nodiscard]]
		constexpr Vector3D movedBy(value_type _x, value_type _y, value_type _z) const noexcept;

		/// @brief 位置ベクトルを移動させた新しいベクトルを返します。
		/// @param v 移動量
		/// @return 移動後の新しいベクトル
		[[nodiscard]]
		constexpr Vector3D movedBy(Vector3D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 位置ベクトルを移動させます。
		/// @param _x X 成分の移動量
		/// @param _y Y 成分の移動量
		/// @param _z Z 成分の移動量
		/// @return *this
		constexpr Vector3D& moveBy(value_type _x, value_type _y, value_type _z) noexcept;

		/// @brief 位置ベクトルを移動させます。
		/// @param v 移動量
		/// @return *this
		constexpr Vector3D& moveBy(Vector3D v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	clamp, clamped
		//
		////////////////////////////////////////////////////////////////

		//Vector3D& clamp(const Box& box) noexcept;

		//[[nodiscard]]
		//Vector3D clamped(const Box& box) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	dot
		//
		////////////////////////////////////////////////////////////////

		template <class U>
		[[nodiscard]]
		constexpr auto dot(Vector3D<U> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cross
		//
		////////////////////////////////////////////////////////////////

		template <class U>
		[[nodiscard]]
		constexpr auto cross(Vector3D<U> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	angleTo
		//
		////////////////////////////////////////////////////////////////

		template <class U>
		[[nodiscard]]
		auto angleTo(Vector3D<U> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	projectOnVector
		//
		////////////////////////////////////////////////////////////////

		template <class U>
		[[nodiscard]]
		constexpr auto projectOnVector(Vector3D<U> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	projectOnPlane
		//
		////////////////////////////////////////////////////////////////

		template <class U>
		[[nodiscard]]
		constexpr auto projectOnPlane(Vector3D<U> planeNormal) const noexcept;

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
		/// @return 別の位置ベクトルからのマンハッタン距離
		[[nodiscard]]
		constexpr value_type manhattanDistanceFrom(value_type _x, value_type _y, value_type _z) const noexcept;

		/// @brief 別の位置ベクトルからのマンハッタン距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルからのマンハッタン距離
		[[nodiscard]]
		constexpr value_type manhattanDistanceFrom(Vector3D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルからの距離を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @param _z 別の位置ベクトルの Z 成分
		/// @return 別の位置ベクトルからの距離
		[[nodiscard]]
		value_type distanceFrom(value_type _x, value_type _y, value_type _z) const noexcept;

		/// @brief 別の位置ベクトルからの距離を返します。
		/// @param v 別の位置ベクトル
		/// @return 別の位置ベクトルからの距離
		[[nodiscard]]
		value_type distanceFrom(Vector3D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceFromSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルからの距離の二乗を返します。
		/// @param _x 別の位置ベクトルの X 成分
		/// @param _y 別の位置ベクトルの Y 成分
		/// @param _z 別の位置ベクトルの Z 成分
		/// @remark 平方根を計算しないため `distanceFrom()` より高速です。
		/// @return 別の位置ベクトルからの距離の二乗
		[[nodiscard]]
		constexpr value_type distanceFromSq(value_type _x, value_type _y, value_type _z) const noexcept;

		/// @brief 別の位置ベクトルからの距離の二乗を返します。
		/// @param v 別の位置ベクトル
		/// @remark 平方根を計算しないため `distanceFrom()` より高速です。
		/// @return 別の位置ベクトルからの距離の二乗
		[[nodiscard]]
		constexpr value_type distanceFromSq(Vector3D v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 向きが同じで大きさ（長さ）を変更した新しいベクトルを返します。
		/// @param _length ベクトルの大きさ（長さ）
		/// @return 新しいベクトル。自身がゼロベクトルの場合はゼロベクトル
		[[nodiscard]]
		Vector3D withLength(value_type _length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの向きはそのままで、大きさ（長さ）だけを変更します。
		/// @param _length ベクトルの大きさ（長さ）
		/// @remark 自身がゼロベクトルの場合は何もしません。
		/// @return *this
		Vector3D& setLength(value_type _length) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withMaxLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 向きが同じで大きさ（長さ）を一定の値以下にした新しいベクトルを返します。
		/// @param maxLength ベクトルの最大の大きさ（長さ）
		/// @return 大きさ（長さ）を一定の値以下にした新しいベクトル
		[[nodiscard]]
		Vector3D withMaxLength(value_type maxLength) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setMaxLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの向きはそのままで、大きさ（長さ）を一定の値以下にします
		/// @param maxLength ベクトルの最大の大きさ（長さ）
		/// @return *this
		Vector3D& setMaxLength(value_type maxLength) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化した（大きさを 1 にした）ベクトルを返します。
		/// @return 正規化した（大きさを 1 にした）ベクトル
		[[nodiscard]]
		Vector3D normalized() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalized_or
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化した（大きさを 1 にした）ベクトルを返します。ゼロベクトルの場合は valueIfZero を返します。
		/// @param valueIfZero ゼロベクトルの場合に返すベクトル
		/// @return 正規化した（大きさを 1 にした）ベクトル、または valueIfZero
		[[nodiscard]]
		Vector3D normalized_or(Vector3D valueIfZero) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 自身を正規化（大きさを 1 に）します。
		/// @return *this
		Vector3D& normalize() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getMidpoint
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector3D getMidpoint(Vector3D other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector3D lerp(Vector3D other, value_type f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asPoint3D
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec3 を Point3D 型に変換します。小数点数以下は切り捨てられます。
		/// @return Point3D
		[[nodiscard]]
		constexpr Point3D asPoint3D() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asSphere
		//
		////////////////////////////////////////////////////////////////

		///// @brief 点を中心とした球を作成します。
		///// @param r 球の半径
		///// @return 球
		//[[nodiscard]]
		//Sphere asSphere(double r) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xx, xy, xz, yx, yy, yz, zx, zy, zz
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector2D<value_type> xx() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> xy() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> xz() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> yx() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> yy() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> yz() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> zx() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> zy() const noexcept;

		[[nodiscard]]
		constexpr Vector2D<value_type> zz() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xxx, xyz, xzy, yyy, yxz, yzx, zzz, zxy, zyx
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector3D xxx() const noexcept;

		[[nodiscard]]
		constexpr Vector3D xyz() const noexcept;

		[[nodiscard]]
		constexpr Vector3D xzy() const noexcept;

		[[nodiscard]]
		constexpr Vector3D yyy() const noexcept;

		[[nodiscard]]
		constexpr Vector3D yxz() const noexcept;

		[[nodiscard]]
		constexpr Vector3D yzx() const noexcept;

		[[nodiscard]]
		constexpr Vector3D zzz() const noexcept;

		[[nodiscard]]
		constexpr Vector3D zxy() const noexcept;

		[[nodiscard]]
		constexpr Vector3D zyx() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xyz0, xyz1
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vector4D<Type> xyz0() const noexcept;

		[[nodiscard]]
		constexpr Vector4D<Type> xyz1() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector3D{ 0, 0, 0 } を返します。
		/// @return Vector3D{ 0, 0, 0 }
		[[nodiscard]]
		static constexpr Vector3D Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	One
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector3D{ 1, 1, 1 } を返します。
		/// @return Vector3D{ 1, 1, 1 }
		[[nodiscard]]
		static constexpr Vector3D One() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	All
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector3D{ value, value, value } を返します。
		/// @return Vector3D{ value, value, value }
		[[nodiscard]]
		static constexpr Vector3D All(value_type value = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UnitX, UnitY, UnitZ
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector3D{ 1, 0, 0 } を返します。
		/// @return Vector3D{ 1, 0, 0 }
		[[nodiscard]]
		static constexpr Vector3D UnitX() noexcept;

		/// @brief Vector3D{ 0, 1, 0 } を返します。
		/// @return Vector3D{ 0, 1, 0 }
		[[nodiscard]]
		static constexpr Vector3D UnitY() noexcept;

		/// @brief Vector3D{ 0, 0, 1 } を返します。
		/// @return Vector3D{ 0, 0, 1 }
		[[nodiscard]]
		static constexpr Vector3D UnitZ() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Left, Right, Up, Down, Forward, Backward
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vector3D{ -length, 0, 0 } を返します。
		/// @return Vector3D{ -length, 0, 0 }
		[[nodiscard]]
		static constexpr Vector3D Left(value_type length = 1) noexcept;

		/// @brief Vector3D{ length, 0, 0 } を返します。
		/// @return Vector3D{ length, 0, 0 }
		[[nodiscard]]
		static constexpr Vector3D Right(value_type length = 1) noexcept;

		/// @brief Vector3D{ 0, length, 0 } を返します。
		/// @return Vector3D{ 0, length, 0 }
		[[nodiscard]]
		static constexpr Vector3D Up(value_type length = 1) noexcept;

		/// @brief Vector3D{ 0, -length, 0 } を返します。
		/// @return Vector3D{ 0, -length, 0 }
		[[nodiscard]]
		static constexpr Vector3D Down(value_type length = 1) noexcept;

		/// @brief Vector3D{ 0, 0, length } を返します。
		/// @return Vector3D{ 0, 0, length }
		[[nodiscard]]
		static constexpr Vector3D Forward(value_type length = 1) noexcept;

		/// @brief Vector3D{ 0, 0, -length } を返します。
		/// @return Vector3D{ 0, 0, -length }
		[[nodiscard]]
		static constexpr Vector3D Backward(value_type length = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Vector3D& value)
		{
			return output  << CharType('(')
				<< value.x << CharType(',') << CharType(' ')
				<< value.y << CharType(',') << CharType(' ')
				<< value.z << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Vector3D& value)
		{
			CharType unused;
			return input   >> unused
				>> value.x >> unused
				>> value.y >> unused
				>> value.z >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Vector3D& value)
		{
			_Formatter(formatData, value);
		}

		static void _Formatter(FormatData& formatData, const Vector3D& value);
	};

	using Float3	= Vector3D<float>;
	using Vec3		= Vector3D<double>;
	using Size3DF	= Vec3;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Float3>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Float3& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Vec3>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Vec3& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Float3, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Float3& value, s3d::BufferContext& ctx);
};

template <>
struct fmt::formatter<s3d::Vec3, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Vec3& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Float3>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Float3& value) const noexcept
	{
		return value.hash();
	}
};

template <>
struct std::hash<s3d::Vec3>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Vec3& value) const noexcept
	{
		return value.hash();
	}
};
