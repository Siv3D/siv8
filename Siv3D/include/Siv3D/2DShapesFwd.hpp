﻿//-----------------------------------------------
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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Point, Size
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2 次元のベクトル（整数）
	struct Point;

	/// @brief 2 次元のサイズ（整数）
	using Size = Point;

	////////////////////////////////////////////////////////////////
	//
	//	Point3D, Size3D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3 次元のベクトル（整数）
	struct Point3D;

	/// @brief 3 次元のサイズ（整数）
	using Size3D = Point3D;

	////////////////////////////////////////////////////////////////
	//
	//	Vector2D, Float2, Vec2, SizeF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2 次元のベクトル
	/// @tparam Type ベクトルの成分の型
	template <Concept::FloatingPoint Float> struct Vector2D;
	
	/// @brief 成分が float 型の 2 次元ベクトル
	using Float2 = Vector2D<float>;

	/// @brief 成分が double 型の 2 次元ベクトル
	using Vec2 = Vector2D<double>;

	/// @brief 2 次元のサイズ
	using SizeF = Vec2;

	////////////////////////////////////////////////////////////////
	//
	//	Vector3D, Float3, Vec3, Size3DF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3 次元のベクトル
	/// @tparam Type ベクトルの成分の型
	template <Concept::FloatingPoint Float> struct Vector3D;

	/// @brief 成分が float 型の 3 次元ベクトル
	using Float3 = Vector3D<float>;

	/// @brief 成分が double 型の 3 次元ベクトル
	using Vec3 = Vector3D<double>;	

	/// @brief 3 次元のサイズ
	using Size3DF = Vec3;

	////////////////////////////////////////////////////////////////
	//
	//	Vector4D, Float4, Vec4
	//
	////////////////////////////////////////////////////////////////

	/// @brief 4 次元のベクトル
	/// @tparam Type ベクトルの成分の型
	template <Concept::FloatingPoint Float> struct Vector4D;

	/// @brief 成分が float 型の 4 次元ベクトル
	using Float4 = Vector4D<float>;

	/// @brief 成分が double 型の 4 次元ベクトル
	using Vec4= Vector4D<double>;

	////////////////////////////////////////////////////////////////
	//
	//	Line
	//
	////////////////////////////////////////////////////////////////

	struct Line;

	////////////////////////////////////////////////////////////////
	//
	//	Bezier2, Bezier3
	//
	////////////////////////////////////////////////////////////////

	struct Bezier2;

	struct Bezier3;

	////////////////////////////////////////////////////////////////
	//
	//	Rect, RectF
	//
	////////////////////////////////////////////////////////////////

	struct Rect;
	
	struct RectF;

	////////////////////////////////////////////////////////////////
	//
	//	Circle
	//
	////////////////////////////////////////////////////////////////

	struct Circle;

	////////////////////////////////////////////////////////////////
	//
	//	Ellipse
	//
	////////////////////////////////////////////////////////////////

	struct Ellipse;

	////////////////////////////////////////////////////////////////
	//
	//	Triangle
	//
	////////////////////////////////////////////////////////////////

	struct Triangle;

	////////////////////////////////////////////////////////////////
	//
	//	Quad
	//
	////////////////////////////////////////////////////////////////

	struct Quad;

	////////////////////////////////////////////////////////////////
	//
	//	RoundRect
	//
	////////////////////////////////////////////////////////////////

	struct RoundRect;

	////////////////////////////////////////////////////////////////
	//
	//	Polygon
	//
	////////////////////////////////////////////////////////////////

	class Polygon;

	////////////////////////////////////////////////////////////////
	//
	//	MultiPolygon
	//
	////////////////////////////////////////////////////////////////

	class MultiPolygon;

	////////////////////////////////////////////////////////////////
	//
	//	LineString
	//
	////////////////////////////////////////////////////////////////

	class LineString;

	////////////////////////////////////////////////////////////////
	//
	//	Spline2D
	//
	////////////////////////////////////////////////////////////////

	class Spline2D;

	////////////////////////////////////////////////////////////////
	//
	//	Shape2D
	//
	////////////////////////////////////////////////////////////////

	class Shape2D;
}
