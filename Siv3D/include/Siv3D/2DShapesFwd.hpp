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

namespace s3d
{
	struct Point;
	using Size		= Point;
	
	struct Point3D;
	using Size3D	= Point3D;
	
	template <class Type> struct Vector2D;
	using Float2	= Vector2D<float>;
	using Vec2		= Vector2D<double>;
	using SizeF		= Vec2;
	
	template <class Type> struct Vector3D;
	using Float3	= Vector3D<float>;
	using Vec3		= Vector3D<double>;	
	using Size3DF	= Vec3;
	
	template <class Type> struct Vector4D;
	using Float4	= Vector4D<float>;
	using Vec4		= Vector4D<double>;

	struct Line;
	struct Bezier2;
	struct Bezier3;
	struct Rect;
	struct RectF;
	struct Circle;
	struct Ellipse;
	struct Triangle;
	struct Quad;
	struct RoundRect;
	class Polygon;
	class MultiPolygon;
	class LineString;
	class Spline2D;
	class Shape2D;
}
