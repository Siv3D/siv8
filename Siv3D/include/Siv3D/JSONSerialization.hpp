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
# include "JSON.hpp"
# include "Int128.hpp"
# include "BigInt.hpp"
# include "BigFloat.hpp"
# include "HalfFloat.hpp"
# include "Date.hpp"
# include "DateTime.hpp"
# include "MD5Value.hpp"
# include "Mat3x2.hpp"
# include "Mat3x3.hpp"
# include "PointVector.hpp"
# include "Circular.hpp"
# include "OffsetCircular.hpp"
# include "2DShapesFwd.hpp"
# include "ColorHSV.hpp"
# include "LineString.hpp"
# include "Polygon.hpp"
# include "MultiPolygon.hpp"
# include "TriangleIndex.hpp"
# include "TriangleIndex32.hpp"
# include "Shape2D.hpp"
# include "UUIDValue.hpp"
# include "InputGroups.hpp"

////////////////////////////////////////////////////////////////
//
//	int128
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::int128>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::int128& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::int128& value);
};

////////////////////////////////////////////////////////////////
//
//	uint128
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::uint128>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::uint128& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::uint128& value);
};

////////////////////////////////////////////////////////////////
//
//	BigInt
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::BigInt>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::BigInt& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::BigInt& value);
};

////////////////////////////////////////////////////////////////
//
//	BigFloat
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::BigFloat>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::BigFloat& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::BigFloat& value);
};

////////////////////////////////////////////////////////////////
//
//	HalfFloat
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::HalfFloat>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::HalfFloat& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::HalfFloat& value);
};

////////////////////////////////////////////////////////////////
//
//	Date
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Date>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Date& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Date& value);
};

////////////////////////////////////////////////////////////////
//
//	DateTime
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::DateTime>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::DateTime& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::DateTime& value);
};

////////////////////////////////////////////////////////////////
//
//	MD5Value
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::MD5Value>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::MD5Value& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::MD5Value& value);
};

////////////////////////////////////////////////////////////////
//
//	Mat3x2
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Mat3x2>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Mat3x2& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Mat3x2& value);
};

////////////////////////////////////////////////////////////////
//
//	Mat3x3
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Mat3x3>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Mat3x3& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Mat3x3& value);
};

////////////////////////////////////////////////////////////////
//
//	LineString
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::LineString>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::LineString& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::LineString& value);
};

////////////////////////////////////////////////////////////////
//
//	Polygon
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Polygon>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Polygon& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Polygon& value);
};

////////////////////////////////////////////////////////////////
//
//	MultiPolygon
//
////////////////////////////////////////////////////////////////

//template <>
//struct JSONSerializer<s3d::MultiPolygon>
//{
//	static void to_json(s3d::JSON::json_base& j, const s3d::MultiPolygon& value);
//	static void from_json(const s3d::JSON::json_base& j, s3d::MultiPolygon& value);
//};

////////////////////////////////////////////////////////////////
//
//	Bezier2
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
//	Bezier3
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
//	TriangleIndex
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::TriangleIndex>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::TriangleIndex& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::TriangleIndex& value);
};

////////////////////////////////////////////////////////////////
//
//	TriangleIndex32
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::TriangleIndex32>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::TriangleIndex32& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::TriangleIndex32& value);
};

////////////////////////////////////////////////////////////////
//
//	Shape2D
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Shape2D>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Shape2D& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Shape2D& value);
};

////////////////////////////////////////////////////////////////
//
//	UUIDValue
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::UUIDValue>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::UUIDValue& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::UUIDValue& value);
};

////////////////////////////////////////////////////////////////
//
//	Input
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Input>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Input& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Input& value);
};

////////////////////////////////////////////////////////////////
//
//	InputCombination
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::InputCombination>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::InputCombination& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::InputCombination& value);
};

////////////////////////////////////////////////////////////////
//
//	InputGroup
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::InputGroup>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::InputGroup& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::InputGroup& value);
};

////////////////////////////////////////////////////////////////
//
//	Point
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Point>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Point& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Point& value);
};

////////////////////////////////////////////////////////////////
//
//	Float2
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Float2>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Float2& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Float2& value);
};

////////////////////////////////////////////////////////////////
//
//	Vec2
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Vec2>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Vec2& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Vec2& value);
};

////////////////////////////////////////////////////////////////
//
//	Float3
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Float3>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Float3& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Float3& value);
};

////////////////////////////////////////////////////////////////
//
//	Vec3
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Vec3>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Vec3& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Vec3& value);
};

////////////////////////////////////////////////////////////////
//
//	Float4
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Float4>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Float4& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Float4& value);
};

////////////////////////////////////////////////////////////////
//
//	Vec4
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Vec4>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Vec4& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Vec4& value);
};

////////////////////////////////////////////////////////////////
//
//	Color
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Color>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Color& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Color& value);
};

////////////////////////////////////////////////////////////////
//
//	ColorF
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::ColorF>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::ColorF& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::ColorF& value);
};

////////////////////////////////////////////////////////////////
//
//	HSV
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::HSV>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::HSV& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::HSV& value);
};

////////////////////////////////////////////////////////////////
//
//	CircularBase
//
////////////////////////////////////////////////////////////////

template <s3d::Concept::FloatingPoint Float, s3d::int32 Oclock>
struct JSONSerializer<s3d::CircularBase<Float, Oclock>>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::CircularBase<Float, Oclock>& value)
	{
		j = { { "r", value.r },{ "theta", value.theta } };
	}

	static void from_json(const s3d::JSON::json_base& j, s3d::CircularBase<Float, Oclock>& value)
	{
		j.at("r").get_to(value.r);
		j.at("theta").get_to(value.theta);
	}
};

////////////////////////////////////////////////////////////////
//
//	OffsetCircularBase
//
////////////////////////////////////////////////////////////////

template <s3d::Concept::FloatingPoint Float, s3d::int32 Oclock>
struct JSONSerializer<s3d::OffsetCircularBase<Float, Oclock>>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::OffsetCircularBase<Float, Oclock>& value)
	{
		j = { { "center", value.center }, { "r", value.r }, { "theta", value.theta } };
	}

	static void from_json(const s3d::JSON::json_base& j, s3d::OffsetCircularBase<Float, Oclock>& value)
	{
		j.at("center").get_to(value.center);
		j.at("r").get_to(value.r);
		j.at("theta").get_to(value.theta);
	}
};

////////////////////////////////////////////////////////////////
//
//	Line
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Line>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Line& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Line& value);
};

////////////////////////////////////////////////////////////////
//
//	Rect
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Rect>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Rect& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Rect& value);
};

////////////////////////////////////////////////////////////////
//
//	RectF
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::RectF>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::RectF& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::RectF& value);
};

////////////////////////////////////////////////////////////////
//
//	Circle
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Circle>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Circle& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Circle& value);
};

////////////////////////////////////////////////////////////////
//
//	Ellipse
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Ellipse>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Ellipse& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Ellipse& value);
};

////////////////////////////////////////////////////////////////
//
//	SuperEllipse
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::SuperEllipse>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::SuperEllipse& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::SuperEllipse& value);
};

////////////////////////////////////////////////////////////////
//
//	Triangle
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Triangle>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Triangle& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Triangle& value);
};

////////////////////////////////////////////////////////////////
//
//	Quad
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::Quad>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::Quad& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::Quad& value);
};

////////////////////////////////////////////////////////////////
//
//	RoundRect
//
////////////////////////////////////////////////////////////////

template <>
struct JSONSerializer<s3d::RoundRect>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::RoundRect& value);
	static void from_json(const s3d::JSON::json_base& j, s3d::RoundRect& value);
};
