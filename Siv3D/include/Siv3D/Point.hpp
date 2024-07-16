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
# include <cassert>
# include <cmath>
# include "Common.hpp"
# include "Concepts.hpp"
# include "2DShapesFwd.hpp"
# include "Utility.hpp"
# include "MinMax.hpp"
# include "CommonFloat.hpp"
# include "MathConstants.hpp"
# include "FormatLiteral.hpp"

namespace s3d
{
	struct FormatData;
	class Image;
	struct Color;

	////////////////////////////////////////////////////////////////
	//
	//	Point
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2 次元のベクトル（整数）
	struct Point
	{
		/// @brief ベクトルの要素の型
		using value_type = int32;

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
		Point() = default;

		[[nodiscard]]
		constexpr Point(value_type _x, value_type _y) noexcept;

		[[nodiscard]]
		constexpr Point(Concept::Integral auto _x, Concept::Integral auto _y) noexcept;

		template <class X, class Y>
		constexpr Point(X _x, Y _y) noexcept requires((not Concept::Integral<X>) || (not Concept::Integral<Y>)) = delete;

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
		constexpr Point operator +() const noexcept;

		[[nodiscard]]
		constexpr Point operator +(Point p) const noexcept;

		template <class Type>
		[[nodiscard]]
		constexpr Vector2D<Type> operator +(Vector2D<Type> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Point operator -() const noexcept;

		[[nodiscard]]
		constexpr Point operator -(Point p) const noexcept;

		template <class Type>
		[[nodiscard]]
		constexpr Vector2D<Type> operator -(Vector2D<Type> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Point operator *(int32 s) const noexcept;

		[[nodiscard]]
		constexpr Float2 operator *(float s) const noexcept;

		[[nodiscard]]
		constexpr Vec2 operator *(double s) const noexcept;

		[[nodiscard]]
		constexpr Point operator *(Point p) const noexcept;

		template <class Type>
		[[nodiscard]]
		constexpr Vector2D<Type> operator *(Vector2D<Type> v) const noexcept;

		[[nodiscard]]
		friend constexpr Point operator *(int32 s, Point p) noexcept
		{
			return{ (s * p.x), (s * p.y) };
		}

		[[nodiscard]]
		friend constexpr Vector2D<float> operator *(float s, Point p) noexcept
		{
			return{ (s * p.x), (s * p.y) };
		}

		[[nodiscard]]
		friend constexpr Vector2D<double> operator *(double s, Point p) noexcept
		{
			return{ (s * p.x), (s * p.y) };
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator /
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Point operator /(int32 s) const noexcept;

		[[nodiscard]]
		constexpr Float2 operator /(float s) const noexcept;

		[[nodiscard]]
		constexpr Vec2 operator /(double s) const noexcept;

		[[nodiscard]]
		constexpr Point operator /(Point p) const noexcept;

		template <class Type>
		[[nodiscard]]
		constexpr Vector2D<Type> operator /(Vector2D<Type> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator %
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Point operator %(int32 s) const noexcept;

		[[nodiscard]]
		constexpr Point operator %(Point p) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		constexpr Point& operator +=(Point p) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		constexpr Point& operator -=(Point p) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		constexpr Point& operator *=(int32 s) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator /=
		//
		////////////////////////////////////////////////////////////////

		constexpr Point& operator /=(int32 s) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator %=
		//
		////////////////////////////////////////////////////////////////

		constexpr Point& operator %=(int32 s) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Point が等しいかを返します。
		/// @param lhs 一方の Point
		/// @param rhs もう一方の Point
		/// @return 2 つの Point が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const Point lhs, const Point rhs) noexcept
		{
			return ((lhs.x == rhs.x)
				 && (lhs.y == rhs.y));
		}

		////////////////////////////////////////////////////////////////
		//
		//	isZero
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての成分が 0 であるかを返します。
		/// @return　すべての成分が 0 である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isZero() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minComponent, maxComponent
		//
		////////////////////////////////////////////////////////////////

		/// @brief 最大の成分を返します。
		/// @remark Point{ 3, 2 } の場合、3 を返します。
		/// @return 最大の成分
		[[nodiscard]]
		constexpr value_type minComponent() const noexcept;

		/// @brief 最小の成分を返します。
		/// @remark Point{ 3, 2 } の場合、2 を返します。
		/// @return 最小の成分
		[[nodiscard]]
		constexpr value_type maxComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての成分を 0 にします。
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
		constexpr Point withX(value_type _x) const noexcept;

		/// @brief y 成分のみを変更した自身のコピーを返します。
		/// @param _y y 成分
		/// @return y 成分を変更したコピー
		[[nodiscard]]
		constexpr Point withY(value_type _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		constexpr Point& set(value_type _x, value_type _y) noexcept;

		constexpr Point& set(Point p) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の座標から移動した座標を返します。
		/// @param _x X 方向の移動量
		/// @param _y Y 方向の移動量
		/// @return 現在の座標から移動した座標
		[[nodiscard]]
		constexpr Point movedBy(value_type _x, value_type _y) const noexcept;

		/// @brief 現在の座標から移動した座標を返します。
		/// @param p 移動量
		/// @return 現在の座標から移動した座標
		[[nodiscard]]
		constexpr Point movedBy(Point p) const noexcept;

		/// @brief 現在の座標から移動した座標を返します。
		/// @tparam Type 移動量を表す二次元ベクトルの要素の型
		/// @param v 移動量
		/// @return 現在の座標から移動した座標
		template <class Type>
		[[nodiscard]]
		constexpr Vector2D<Type> movedBy(Vector2D<Type> v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の座標から移動します。
		/// @param _x X 方向の移動量
		/// @param _y Y 方向の移動量
		/// @return *this
		constexpr Point& moveBy(value_type _x, value_type _y) noexcept;

		/// @brief 現在の座標から移動します。
		/// @param p 移動量
		/// @return *this
		constexpr Point& moveBy(Point p) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	horizontalAspectRatio
		//
		////////////////////////////////////////////////////////////////

		/// @brief 水平方向のアスペクト比を返します。
		/// @tparam Type 戻り値の型
		/// @remark Point{ 3, 2 } の場合、1.5 を返します。
		/// @return 水平方向のアスペクト比
		template <class Type = double>
		[[nodiscard]]
		constexpr Type horizontalAspectRatio() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	length
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの大きさ（長さ）を返します。
		/// @tparam Type 結果の型
		/// @return ベクトルの大きさ（長さ）
		template <class Type = double>
		[[nodiscard]]
		Type length() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lengthSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの大きさ（長さ）の二乗を返します。
		/// @tparam Type 結果の型
		/// @remark 平方根を計算しないため `length()` より高速です。
		/// @return ベクトルの大きさ（長さ）の二乗
		template <class Type = double>
		[[nodiscard]]
		constexpr Type lengthSq() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	manhattanLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点からこの座標までのマンハッタン距離を返します。
		/// @return 原点からのマンハッタン距離
		[[nodiscard]]
		constexpr int32 manhattanLength() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	manhattanDistanceFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の位置ベクトルからのマンハッタン距離を返します。
		/// @param _x 別の座標の X 成分
		/// @param _y 別の座標の Y 成分
		/// @return 別の座標からのマンハッタン距離
		[[nodiscard]]
		constexpr int32 manhattanDistanceFrom(int32 _x, int32 _y) const noexcept;

		/// @brief 別の座標からのマンハッタン距離を返します。
		/// @param p 別の座標
		/// @return 別の座標からのマンハッタン距離
		[[nodiscard]]
		constexpr int32 manhattanDistanceFrom(Point p) const noexcept;

		/// @brief 別の座標からのマンハッタン距離を返します。
		/// @param p 別の座標
		/// @return 別の座標からのマンハッタン距離
		template <class Type>
		[[nodiscard]]
		constexpr Type manhattanDistanceFrom(Vector2D<Type> p) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の座標からの距離を返します。
		/// @param _x 別の座標の X 成分
		/// @param _y 別の座標の Y 成分
		/// @return 別の座標からの距離
		[[nodiscard]]
		double distanceFrom(double _x, double _y) const noexcept;

		/// @brief 別の座標からの距離を返します。
		/// @param p 別の座標
		/// @return 別の座標からの距離
		[[nodiscard]]
		double distanceFrom(Point p) const noexcept;

		/// @brief 別の座標からの距離を返します。
		/// @tparam Type 別の座標の成分の型
		/// @param p 別の座標
		/// @return 別の座標からの距離
		template <class Type>
		[[nodiscard]]
		Type distanceFrom(Vector2D<Type> p) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	distanceFromSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の座標からの距離の二乗を返します。
		/// @param _x 別の座標の X 成分
		/// @param _y 別の座標の Y 成分
		/// @remark 平方根を計算しないため `distanceFrom()` より高速です。
		/// @return 別の座標からの距離の二乗
		[[nodiscard]]
		constexpr double distanceFromSq(double _x, double _y) const noexcept;

		/// @brief 別の座標からの距離の二乗を返します。
		/// @param p 別の座標
		/// @remark 平方根を計算しないため `distanceFrom()` より高速です。
		/// @return 別の座標からの距離の二乗
		[[nodiscard]]
		constexpr double distanceFromSq(Point p) const noexcept;

		/// @brief 別の座標からの距離の二乗を返します。
		/// @tparam Type 別の座標の成分の型
		/// @param p 別の座標
		/// @remark 平方根を計算しないため `distanceFrom()` より高速です。
		/// @return 別の座標からの距離の二乗
		template <class Type>
		[[nodiscard]]
		constexpr Type distanceFromSq(Vector2D<Type> p) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		/// @brief 幅 x, 高さ y の長方形の面積を返します。
		/// @return 幅 x, 高さ y の長方形の面積
		/// @remark `x * y` を返します。要素の符号によっては負の値を返すことがあります。
		template <class Type = int64>
		[[nodiscard]]
		constexpr Type area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotate90, rotated90
		//
		////////////////////////////////////////////////////////////////

		/// @brief 自身を時計回りに 90°* n 回転します。
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return *this
		constexpr Point& rotate90(int32 n = 1) noexcept;

		/// @brief 時計回りに 90°* n 回転した座標を返します。
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return 時計回りに 90°* n 回転した座標
		[[nodiscard]]
		constexpr Point rotated90(int32 n = 1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotate90, rotated90
		//
		////////////////////////////////////////////////////////////////

		/// @brief centerを中心とし、自身を時計回りに 90°* n 回転します。
		/// @param center 回転の中心座標
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return *this
		constexpr Point& rotate90At(Point center, int32 n = 1) noexcept;

		/// @brief centerを中心とし、時計回りに 90°* n 回転した座標を返します。
		/// @param center 回転の中心座標
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return centerを中心とし、時計回りに 90°* n 回転した座標
		[[nodiscard]]
		constexpr Point rotated90At(Point center, int32 n = 1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getAngle
		//
		////////////////////////////////////////////////////////////////

		template <class Type = double>
		[[nodiscard]]
		Type getAngle() const noexcept;

		template <class Type>
		[[nodiscard]]
		Type getAngle(Vector2D<Type> other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPerpendicularCW, getPerpendicularCCW
		//
		////////////////////////////////////////////////////////////////

		/// @brief 時計回りに 90° 回転したベクトルを返します。
		/// @return 時計回りに 90° 回転したベクトル
		[[nodiscard]]
		constexpr Point getPerpendicularCW() const noexcept;

		/// @brief 反時計回りに 90° 回転したベクトルを返します。
		/// @return 反時計回りに 90° 回転したベクトル
		[[nodiscard]]
		constexpr Point getPerpendicularCCW() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getMidpoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の座標との中間にある座標を返します。
		/// @tparam Type 戻り値の二次元座標の要素の型
		/// @param other 別の座標
		/// @return 別の座標との中間にある座標
		template <class Type = double>
		[[nodiscard]]
		constexpr Vector2D<Type> getMidpoint(Point other) const noexcept;

		/// @brief 別の座標との中間にある座標を返します。
		/// @tparam Type 戻り値の二次元座標の要素の型
		/// @param other 別の座標
		/// @return 別の座標との中間にある座標
		template <class Type>
		[[nodiscard]]
		constexpr Vector2D<Type> getMidpoint(Vector2D<Type> other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointByAngleAndDistance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した角度に向かって指定した距離だけ進んだ座標を返します。
		/// @param angle 角度（ラジアン）
		/// @param distance 進む距離
		/// @return 指定した角度に向かって指定した距離だけ進んだ座標
		[[nodiscard]]
		auto getPointByAngleAndDistance(Concept::Arithmetic auto angle, Concept::Arithmetic auto distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の座標との線形補間を返します。
		/// @tparam Type 戻り値の二次元座標の要素の型
		/// @param other 別の座標
		/// @param f 補間係数
		/// @return 線形補間された座標
		template <class Type = double>
		[[nodiscard]]
		constexpr Vector2D<Type> lerp(Point other, double f) const noexcept;

		/// @brief 別の座標との線形補間を返します。
		/// @tparam Type 戻り値の二次元座標の要素の型
		/// @param other 別の座標
		/// @param f 補間係数
		/// @return 線形補間された座標
		template <class Type>
		[[nodiscard]]
		constexpr Vector2D<Type> lerp(Vector2D<Type> other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asCircle
		//
		////////////////////////////////////////////////////////////////

		/// @brief この座標を中心とした円を作成して返します。
		/// @param r 円の半径
		/// @return この座標を中心とした円
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
		//	leftClicked, leftPressed, leftReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 座標の点が現在のフレームで左クリックされ始めたかを返します。
		/// @return 座標の点が現在のフレームで左クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftClicked() const noexcept;

		/// @brief 座標の点が現在のフレームで左クリックされているかを返します。
		/// @return 座標の点が現在のフレームで左クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftPressed() const noexcept;

		/// @brief 現在のフレームで座標の点への左クリックが離されたかを返します。
		/// @return 現在のフレームで座標の点への左クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightClicked, rightPressed, rightReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 座標の点が現在のフレームで右クリックされ始めたかを返します。
		/// @return 座標の点が現在のフレームで右クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightClicked() const noexcept;

		/// @brief 座標の点が現在のフレームで右クリックされているかを返します。
		/// @return 座標の点が現在のフレームで右クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightPressed() const noexcept;

		/// @brief 現在のフレームで座標の点への右クリックが離されたかを返します。
		/// @return 現在のフレームで座標の点への右クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseOver
		//
		////////////////////////////////////////////////////////////////

		/// @brief 座標の点の上にマウスカーソルがあるかを返します。
		/// @return 座標の点の上にマウスカーソルがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool mouseOver() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		// [Siv3D ToDo]
		//const Point& paint(Image& dst, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		const Point& overwrite(Image& dst, const Color& color) const noexcept;

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

		/// @brief Point{ x, x } を返します。
		/// @return Point{ x, x }
		[[nodiscard]]
		constexpr Point xx() const noexcept;

		/// @brief Point{ x, y } を返します。
		/// @return Point{ x, y }
		[[nodiscard]]
		constexpr Point xy() const noexcept;

		/// @brief Point{ y x } を返します。
		/// @return Point{ y, x }
		[[nodiscard]]
		constexpr Point yx() const noexcept;

		/// @brief Point{ y, y } を返します。
		/// @return Point{ y, y }
		[[nodiscard]]
		constexpr Point yy() const noexcept;

		/// @brief Point{ x, 0 } を返します。
		/// @return Point{ x, 0 }
		[[nodiscard]]
		constexpr Point x0() const noexcept;

		/// @brief Point{ y, 0 } を返します。
		/// @return Point{ y, 0 }
		[[nodiscard]]
		constexpr Point y0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		/// @brief Point{ 0, 0 } を返します。
		/// @return Point{ 0, 0 }
		[[nodiscard]]
		static constexpr Point Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	One
		//
		////////////////////////////////////////////////////////////////

		/// @brief Point{ 1, 1 } を返します。
		/// @return Point{ 1, 1 }
		[[nodiscard]]
		static constexpr Point One() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	All
		//
		////////////////////////////////////////////////////////////////

		/// @brief Point{ value, value } を返します。
		/// @param value 値
		/// @return Point{ value, value }
		[[nodiscard]]
		static constexpr Point All(value_type value = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UnitX, UnitY
		//
		////////////////////////////////////////////////////////////////

		/// @brief Point{ 1, 0 } を返します。
		/// @return Point{ 1, 0 }
		[[nodiscard]]
		static constexpr Point UnitX() noexcept;

		/// @brief Point{ 0, 1 } を返します。
		/// @return Point{ 0, 1 }
		[[nodiscard]]
		static constexpr Point UnitY() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Left, Right, Up, Down
		//
		////////////////////////////////////////////////////////////////

		/// @brief Point{ -length, 0 } を返します。
		/// @param length 値
		/// @return Point{ -length, 0 }
		[[nodiscard]]
		static constexpr Point Left(value_type length = 1) noexcept;

		/// @brief Point{ length, 0 } を返します。
		/// @param length 値
		/// @return Point{ length, 0 }
		[[nodiscard]]
		static constexpr Point Right(value_type length = 1) noexcept;

		/// @brief Point{ 0, -length } を返します。
		/// @param length 値
		/// @return Point{ 0, -length }
		[[nodiscard]]
		static constexpr Point Up(value_type length = 1) noexcept;

		/// @brief Point{ 0, length } を返します。
		/// @param length 値
		/// @return Point{ 0, length }
		[[nodiscard]]
		static constexpr Point Down(value_type length = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに Point の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value Point
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Point& value)
		{
			return output << CharType('(')
				<< value.x << CharType(',') << CharType(' ')
				<< value.y << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから Point の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value Point
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Point& value)
		{
			CharType unused;
			return input >> unused
				>> value.x >> unused
				>> value.y >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Point を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value Point
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Point& value);
	};

	using Size = Point;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Point>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Point& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Point, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Point& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Point>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Point& value) const noexcept
	{
		return value.hash();
	}
};
