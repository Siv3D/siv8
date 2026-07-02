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

# include <Siv3D/JSONSerialization.hpp>

////////////////////////////////////////////////////////////////
//
//	int128
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::int128>::to_json(s3d::JSON::json_base& j, const s3d::int128& value)
{
	j = { { "high", absl::Int128High64(value) },
		  { "low", absl::Uint128Low64(value) } };
}

void JSONSerializer<s3d::int128>::from_json(const s3d::JSON::json_base& j, s3d::int128& value)
{
	const s3d::int64 high = j.at("high").get<s3d::int64>();
	const s3d::uint64 low = j.at("low").get<s3d::uint64>();
	value = s3d::MakeInt128(high, low);
}

////////////////////////////////////////////////////////////////
//
//	uint128
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::uint128>::to_json(s3d::JSON::json_base& j, const s3d::uint128& value)
{
	j = { { "high", absl::Uint128High64(value) },
		  { "low", absl::Uint128Low64(value) } };
}

void JSONSerializer<s3d::uint128>::from_json(const s3d::JSON::json_base& j, s3d::uint128& value)
{
	const s3d::uint64 high = j.at("high").get<s3d::uint64>();
	const s3d::uint64 low = j.at("low").get<s3d::uint64>();
	value = s3d::MakeUint128(high, low);
}

////////////////////////////////////////////////////////////////
//
//	BigInt
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::BigInt>::to_json(s3d::JSON::json_base& j, const s3d::BigInt& value)
{
	j = value.to_string();
}

void JSONSerializer<s3d::BigInt>::from_json(const s3d::JSON::json_base& j, s3d::BigInt& value)
{
	value = s3d::BigInt{ j.get<std::string_view>() };
}

////////////////////////////////////////////////////////////////
//
//	BigFloat
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::BigFloat>::to_json(s3d::JSON::json_base& j, const s3d::BigFloat& value)
{
	j = value.to_string();
}

void JSONSerializer<s3d::BigFloat>::from_json(const s3d::JSON::json_base& j, s3d::BigFloat& value)
{
	value = s3d::BigFloat{ j.get<std::string_view>() };
}

////////////////////////////////////////////////////////////////
//
//	HalfFloat
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::HalfFloat>::to_json(s3d::JSON::json_base& j, const s3d::HalfFloat& value)
{
	j = static_cast<float>(value);
}

void JSONSerializer<s3d::HalfFloat>::from_json(const s3d::JSON::json_base& j, s3d::HalfFloat& value)
{
	value = j.get<float>();
}

////////////////////////////////////////////////////////////////
//
//	Date
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Date>::to_json(s3d::JSON::json_base& j, const s3d::Date& value)
{
	j = { { "year", value.year },
		  { "month", value.month },
		  { "day", value.day } };
}

void JSONSerializer<s3d::Date>::from_json(const s3d::JSON::json_base& j, s3d::Date& value)
{
	j.at("year").get_to(value.year);
	j.at("month").get_to(value.month);
	j.at("day").get_to(value.day);
}

////////////////////////////////////////////////////////////////
//
//	DateTime
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::DateTime>::to_json(s3d::JSON::json_base& j, const s3d::DateTime& value)
{
	j = { { "year", value.year },
		  { "month", value.month },
		  { "day", value.day },
		  { "hour", value.hour },
		  { "minute", value.minute },
		  { "second", value.second },
		  { "millisecond", value.millisecond } };
}

void JSONSerializer<s3d::DateTime>::from_json(const s3d::JSON::json_base& j, s3d::DateTime& value)
{
	j.at("year").get_to(value.year);
	j.at("month").get_to(value.month);
	j.at("day").get_to(value.day);
	j.at("hour").get_to(value.hour);
	j.at("minute").get_to(value.minute);
	j.at("second").get_to(value.second);
	j.at("millisecond").get_to(value.millisecond);
}

////////////////////////////////////////////////////////////////
//
//	MD5Value
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::MD5Value>::to_json(s3d::JSON::json_base& j, const s3d::MD5Value& value)
{
	j = value.to_string();
}

void JSONSerializer<s3d::MD5Value>::from_json(const s3d::JSON::json_base& j, s3d::MD5Value& value)
{
	if (const auto md5 = s3d::MD5Value::Parse(j.get<std::string_view>()))
	{
		value = *md5;
		return;
	}
	else
	{
		throw s3d::JSON::json_base::parse_error::create(101, 0, "Invalid MD5 string", nullptr);
	}
}

////////////////////////////////////////////////////////////////
//
//	Mat3x2
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Mat3x2>::to_json(s3d::JSON::json_base& j, const s3d::Mat3x2& value)
{
	j = { { "_11", value._11 }, { "_12", value._12 },
		  { "_21", value._21 }, { "_22", value._22 },
		  { "_31", value._31 }, { "_32", value._32 } };
}

void JSONSerializer<s3d::Mat3x2>::from_json(const s3d::JSON::json_base& j, s3d::Mat3x2& value)
{
	j.at("_11").get_to(value._11);
	j.at("_12").get_to(value._12);
	j.at("_21").get_to(value._21);
	j.at("_22").get_to(value._22);
	j.at("_31").get_to(value._31);
	j.at("_32").get_to(value._32);
}

////////////////////////////////////////////////////////////////
//
//	Mat3x3
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Mat3x3>::to_json(s3d::JSON::json_base& j, const s3d::Mat3x3& value)
{
	j = { { "_11", value._11 }, { "_12", value._12 }, { "_13", value._13 },
		  { "_21", value._21 }, { "_22", value._22 }, { "_23", value._23 },
		  { "_31", value._31 }, { "_32", value._32 }, { "_33", value._33 } };
}

void JSONSerializer<s3d::Mat3x3>::from_json(const s3d::JSON::json_base& j, s3d::Mat3x3& value)
{
	j.at("_11").get_to(value._11);
	j.at("_12").get_to(value._12);
	j.at("_13").get_to(value._13);
	j.at("_21").get_to(value._21);
	j.at("_22").get_to(value._22);
	j.at("_23").get_to(value._23);
	j.at("_31").get_to(value._31);
	j.at("_32").get_to(value._32);
	j.at("_33").get_to(value._33);
}

////////////////////////////////////////////////////////////////
//
//	LineString
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::LineString>::to_json(s3d::JSON::json_base& j, const s3d::LineString& value)
{
	j = value.asArray();
}

void JSONSerializer<s3d::LineString>::from_json(const s3d::JSON::json_base& j, s3d::LineString& value)
{
	value = s3d::LineString{ j.get<s3d::Array<s3d::Vec2>>() };
}

////////////////////////////////////////////////////////////////
//
//	Polygon
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Polygon>::to_json(s3d::JSON::json_base& j, const s3d::Polygon& value)
{
	j = { { "outer", value.outer() },
		  { "inners", value.inners().asArray() },
		  { "vertices", value.vertices() },
		  { "indices", value.indices() },
		  { "boundingRect", value.boundingRect() }
	};
}

void JSONSerializer<s3d::Polygon>::from_json(const s3d::JSON::json_base& j, s3d::Polygon& value)
{
	s3d::Array<s3d::Vec2> outer = j.at("outer").get<s3d::Array<s3d::Vec2>>();
	s3d::Array<s3d::Array<s3d::Vec2>> inners = j.at("inners").get<s3d::Array<s3d::Array<s3d::Vec2>>>();
	s3d::Array<s3d::Float2> vertices = j.at("vertices").get<s3d::Array<s3d::Float2>>();
	s3d::Array<s3d::TriangleIndex> indices = j.at("indices").get<s3d::Array<s3d::TriangleIndex>>();
	s3d::RectF boundingRect = j.at("boundingRect").get<s3d::RectF>();

	value = s3d::Polygon{ outer, std::move(inners), std::move(vertices), std::move(indices), boundingRect, s3d::SkipValidation::No };
}

////////////////////////////////////////////////////////////////
//
//	MultiPolygon
//
////////////////////////////////////////////////////////////////

//void JSONSerializer<s3d::MultiPolygon>::to_json(s3d::JSON::json_base& j, const s3d::MultiPolygon& value)
//{
//	j = value.asArray();
//}
//
//void JSONSerializer<s3d::MultiPolygon>::from_json(const s3d::JSON::json_base& j, s3d::MultiPolygon& value)
//{
//	value = s3d::MultiPolygon{ j.get<s3d::Array<s3d::Polygon>>() };
//}

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

void JSONSerializer<s3d::TriangleIndex>::to_json(s3d::JSON::json_base& j, const s3d::TriangleIndex& value)
{
	j = { { "i0", value.i0 },
		  { "i1", value.i1 },
		  { "i2", value.i2 } };
}

void JSONSerializer<s3d::TriangleIndex>::from_json(const s3d::JSON::json_base& j, s3d::TriangleIndex& value)
{
	j.at("i0").get_to(value.i0);
	j.at("i1").get_to(value.i1);
	j.at("i2").get_to(value.i2);
}

////////////////////////////////////////////////////////////////
//
//	TriangleIndex32
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::TriangleIndex32>::to_json(s3d::JSON::json_base& j, const s3d::TriangleIndex32& value)
{
	j = { { "i0", value.i0 },
		  { "i1", value.i1 },
		  { "i2", value.i2 } };
}

void JSONSerializer<s3d::TriangleIndex32>::from_json(const s3d::JSON::json_base& j, s3d::TriangleIndex32& value)
{
	j.at("i0").get_to(value.i0);
	j.at("i1").get_to(value.i1);
	j.at("i2").get_to(value.i2);
}

////////////////////////////////////////////////////////////////
//
//	Shape2D
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Shape2D>::to_json(s3d::JSON::json_base& j, const s3d::Shape2D& value)
{
	j = { { "vertices", value.vertices() },
		  { "indices", value.indices() } };
}

void JSONSerializer<s3d::Shape2D>::from_json(const s3d::JSON::json_base& j, s3d::Shape2D& value)
{
	s3d::Array<s3d::Float2> vertices = j.at("vertices").get<s3d::Array<s3d::Float2>>();
	s3d::Array<s3d::TriangleIndex> indices = j.at("indices").get<s3d::Array<s3d::TriangleIndex>>();
	value = s3d::Shape2D{ std::move(vertices), std::move(indices) };
}

////////////////////////////////////////////////////////////////
//
//	UUIDValue
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::UUIDValue>::to_json(s3d::JSON::json_base& j, const s3d::UUIDValue& value)
{
	j = value.to_string();
}

void JSONSerializer<s3d::UUIDValue>::from_json(const s3d::JSON::json_base& j, s3d::UUIDValue& value)
{
	if (const auto uuid = s3d::UUIDValue::Parse(j.get<std::string_view>()))
	{
		value = *uuid;
		return;
	}
	else
	{
		throw s3d::JSON::json_base::parse_error::create(101, 0, "Invalid UUID string", nullptr);
	}
}

////////////////////////////////////////////////////////////////
//
//	Input
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Input>::to_json(s3d::JSON::json_base& j, const s3d::Input& value)
{
	j = { { "deviceType", s3d::FromEnum(value.deviceType()) },
		  { "code", value.code() },
		  { "playerIndex", value.playerIndex() } };
}

void JSONSerializer<s3d::Input>::from_json(const s3d::JSON::json_base& j, s3d::Input& value)
{
	const s3d::InputDeviceType deviceType = s3d::ToEnum<s3d::InputDeviceType>(j.at("deviceType").get<s3d::uint8>());
	const s3d::uint8 code = j.at("code").get<s3d::uint8>();
	const s3d::uint8 playerIndex = j.at("playerIndex").get<s3d::uint8>();
	value = s3d::Input{ deviceType, code, playerIndex };
}

////////////////////////////////////////////////////////////////
//
//	InputCombination
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::InputCombination>::to_json(s3d::JSON::json_base& j, const s3d::InputCombination& value)
{
	j = { { "input1", value.input1() },
		  { "input2", value.input2() } };
}

void JSONSerializer<s3d::InputCombination>::from_json(const s3d::JSON::json_base& j, s3d::InputCombination& value)
{
	const s3d::Input input1 = j.at("input1").get<s3d::Input>();
	const s3d::Input input2 = j.at("input2").get<s3d::Input>();
	value = s3d::InputCombination{ input1, input2 };
}

////////////////////////////////////////////////////////////////
//
//	InputGroup
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::InputGroup>::to_json(s3d::JSON::json_base& j, const s3d::InputGroup& value)
{
	j = { { "inputs", value.inputs() },
		  { "inputCombinations", value.inputCombinations() } };
}

void JSONSerializer<s3d::InputGroup>::from_json(const s3d::JSON::json_base& j, s3d::InputGroup& value)
{
	s3d::Array<s3d::Input> inputs = j.at("inputs").get<s3d::Array<s3d::Input>>();
	s3d::Array<s3d::InputCombination> inputCombinations = j.at("inputCombinations").get<s3d::Array<s3d::InputCombination>>();
	value = s3d::InputGroup{ std::move(inputs), std::move(inputCombinations) };
}

////////////////////////////////////////////////////////////////
//
//	Point
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Point>::to_json(s3d::JSON::json_base& j, const s3d::Point& value)
{
	j = { { "x", value.x }, { "y", value.y } };
}

void JSONSerializer<s3d::Point>::from_json(const s3d::JSON::json_base& j, s3d::Point& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
}

////////////////////////////////////////////////////////////////
//
//	Float2
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Float2>::to_json(s3d::JSON::json_base& j, const s3d::Float2& value)
{
	j = { { "x", value.x }, { "y", value.y } };
}

void JSONSerializer<s3d::Float2>::from_json(const s3d::JSON::json_base& j, s3d::Float2& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
}

////////////////////////////////////////////////////////////////
//
//	Vec2
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Vec2>::to_json(s3d::JSON::json_base& j, const s3d::Vec2& value)
{
	j = { { "x", value.x }, { "y", value.y } };
}

void JSONSerializer<s3d::Vec2>::from_json(const s3d::JSON::json_base& j, s3d::Vec2& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
}

////////////////////////////////////////////////////////////////
//
//	Float3
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Float3>::to_json(s3d::JSON::json_base& j, const s3d::Float3& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "z", value.z } };
}

void JSONSerializer<s3d::Float3>::from_json(const s3d::JSON::json_base& j, s3d::Float3& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("z").get_to(value.z);
}

////////////////////////////////////////////////////////////////
//
//	Vec3
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Vec3>::to_json(s3d::JSON::json_base& j, const s3d::Vec3& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "z", value.z } };
}

void JSONSerializer<s3d::Vec3>::from_json(const s3d::JSON::json_base& j, s3d::Vec3& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("z").get_to(value.z);
}

////////////////////////////////////////////////////////////////
//
//	Float4
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Float4>::to_json(s3d::JSON::json_base& j, const s3d::Float4& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "z", value.z }, { "w", value.w } };
}

void JSONSerializer<s3d::Float4>::from_json(const s3d::JSON::json_base& j, s3d::Float4& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("z").get_to(value.z);
	j.at("w").get_to(value.w);
}

////////////////////////////////////////////////////////////////
//
//	Vec4
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Vec4>::to_json(s3d::JSON::json_base& j, const s3d::Vec4& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "z", value.z }, { "w", value.w } };
}

void JSONSerializer<s3d::Vec4>::from_json(const s3d::JSON::json_base& j, s3d::Vec4& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("z").get_to(value.z);
	j.at("w").get_to(value.w);
}

////////////////////////////////////////////////////////////////
//
//	Color
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Color>::to_json(s3d::JSON::json_base& j, const s3d::Color& value)
{
	j = { { "r", value.r }, { "g", value.g }, { "b", value.b }, { "a", value.a } };
}

void JSONSerializer<s3d::Color>::from_json(const s3d::JSON::json_base& j, s3d::Color& value)
{
	j.at("r").get_to(value.r);
	j.at("g").get_to(value.g);
	j.at("b").get_to(value.b);
	j.at("a").get_to(value.a);
}

////////////////////////////////////////////////////////////////
//
//	ColorF
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::ColorF>::to_json(s3d::JSON::json_base& j, const s3d::ColorF& value)
{
	j = { { "r", value.r }, { "g", value.g }, { "b", value.b }, { "a", value.a } };
}

void JSONSerializer<s3d::ColorF>::from_json(const s3d::JSON::json_base& j, s3d::ColorF& value)
{
	j.at("r").get_to(value.r);
	j.at("g").get_to(value.g);
	j.at("b").get_to(value.b);
	j.at("a").get_to(value.a);
}

////////////////////////////////////////////////////////////////
//
//	HSV
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::HSV>::to_json(s3d::JSON::json_base& j, const s3d::HSV& value)
{
	j = { { "h", value.h }, { "s", value.s }, { "v", value.v }, { "a", value.a } };
}

void JSONSerializer<s3d::HSV>::from_json(const s3d::JSON::json_base& j, s3d::HSV& value)
{
	j.at("h").get_to(value.h);
	j.at("s").get_to(value.s);
	j.at("v").get_to(value.v);
	j.at("a").get_to(value.a);
}

////////////////////////////////////////////////////////////////
//
//	Line
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Line>::to_json(s3d::JSON::json_base& j, const s3d::Line& value)
{
	j = { { "start", value.start }, { "end", value.end } };
}

void JSONSerializer<s3d::Line>::from_json(const s3d::JSON::json_base& j, s3d::Line& value)
{
	j.at("start").get_to(value.start);
	j.at("end").get_to(value.end);
}

////////////////////////////////////////////////////////////////
//
//	Rect
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Rect>::to_json(s3d::JSON::json_base& j, const s3d::Rect& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "w", value.w }, { "h", value.h } };
}

void JSONSerializer<s3d::Rect>::from_json(const s3d::JSON::json_base& j, s3d::Rect& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("w").get_to(value.w);
	j.at("h").get_to(value.h);
}

////////////////////////////////////////////////////////////////
//
//	RectF
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::RectF>::to_json(s3d::JSON::json_base& j, const s3d::RectF& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "w", value.w }, { "h", value.h } };
}

void JSONSerializer<s3d::RectF>::from_json(const s3d::JSON::json_base& j, s3d::RectF& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("w").get_to(value.w);
	j.at("h").get_to(value.h);
}

////////////////////////////////////////////////////////////////
//
//	Circle
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Circle>::to_json(s3d::JSON::json_base& j, const s3d::Circle& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "r", value.r } };
}

void JSONSerializer<s3d::Circle>::from_json(const s3d::JSON::json_base& j, s3d::Circle& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("r").get_to(value.r);
}

////////////////////////////////////////////////////////////////
//
//	Ellipse
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Ellipse>::to_json(s3d::JSON::json_base& j, const s3d::Ellipse& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "a", value.a }, { "b", value.b } };
}

void JSONSerializer<s3d::Ellipse>::from_json(const s3d::JSON::json_base& j, s3d::Ellipse& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("a").get_to(value.a);
	j.at("b").get_to(value.b);
}

////////////////////////////////////////////////////////////////
//
//	SuperEllipse
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::SuperEllipse>::to_json(s3d::JSON::json_base& j, const s3d::SuperEllipse& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "a", value.a }, { "b", value.b }, { "n", value.n } };
}

void JSONSerializer<s3d::SuperEllipse>::from_json(const s3d::JSON::json_base& j, s3d::SuperEllipse& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("a").get_to(value.a);
	j.at("b").get_to(value.b);
	j.at("n").get_to(value.n);
}

////////////////////////////////////////////////////////////////
//
//	Triangle
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Triangle>::to_json(s3d::JSON::json_base& j, const s3d::Triangle& value)
{
	j = { { "p0", value.p0 }, { "p1", value.p1 }, { "p2", value.p2 } };
}

void JSONSerializer<s3d::Triangle>::from_json(const s3d::JSON::json_base& j, s3d::Triangle& value)
{
	j.at("p0").get_to(value.p0);
	j.at("p1").get_to(value.p1);
	j.at("p2").get_to(value.p2);
}

////////////////////////////////////////////////////////////////
//
//	Quad
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Quad>::to_json(s3d::JSON::json_base& j, const s3d::Quad& value)
{
	j = { { "p0", value.p0 }, { "p1", value.p1 }, { "p2", value.p2 }, { "p3", value.p3 } };
}

void JSONSerializer<s3d::Quad>::from_json(const s3d::JSON::json_base& j, s3d::Quad& value)
{
	j.at("p0").get_to(value.p0);
	j.at("p1").get_to(value.p1);
	j.at("p2").get_to(value.p2);
	j.at("p3").get_to(value.p3);
}

////////////////////////////////////////////////////////////////
//
//	RoundRect
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::RoundRect>::to_json(s3d::JSON::json_base& j, const s3d::RoundRect& value)
{
	j = { { "x", value.x }, { "y", value.y }, { "w", value.w }, { "h", value.h }, { "r", value.r } };
}

void JSONSerializer<s3d::RoundRect>::from_json(const s3d::JSON::json_base& j, s3d::RoundRect& value)
{
	j.at("x").get_to(value.x);
	j.at("y").get_to(value.y);
	j.at("w").get_to(value.w);
	j.at("h").get_to(value.h);
	j.at("r").get_to(value.r);
}

////////////////////////////////////////////////////////////////
//
//	Polygon
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
//	MultiPolygon
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
//	LineString
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
//	Bezier2
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Bezier2>::to_json(s3d::JSON::json_base& j, const s3d::Bezier2& value)
{
	j = { { "p0", value.p0 }, { "p1", value.p1 }, { "p2", value.p2 } };
}

void JSONSerializer<s3d::Bezier2>::from_json(const s3d::JSON::json_base& j, s3d::Bezier2& value)
{
	j.at("p0").get_to(value.p0);
	j.at("p1").get_to(value.p1);
	j.at("p2").get_to(value.p2);
}

////////////////////////////////////////////////////////////////
//
//	Bezier3
//
////////////////////////////////////////////////////////////////

void JSONSerializer<s3d::Bezier3>::to_json(s3d::JSON::json_base& j, const s3d::Bezier3& value)
{
	j = { { "p0", value.p0 }, { "p1", value.p1 }, { "p2", value.p2 }, { "p3", value.p3 } };
}

void JSONSerializer<s3d::Bezier3>::from_json(const s3d::JSON::json_base& j, s3d::Bezier3& value)
{
	j.at("p0").get_to(value.p0);
	j.at("p1").get_to(value.p1);
	j.at("p2").get_to(value.p2);
	j.at("p3").get_to(value.p3);
}
