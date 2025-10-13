//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/JSONSerialization.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/ColorHSV.hpp>
# include <Siv3D/2DShapes.hpp>

namespace nlohmann
{
	////////////////////////////////////////////////////////////////
	//
	//	Point
	//
	////////////////////////////////////////////////////////////////

	void adl_serializer<s3d::Point>::to_json(json& j, const s3d::Point& value)
	{
		j = { { "x", value.x }, { "y", value.y } };
	}

	void adl_serializer<s3d::Point>::from_json(const json& j, s3d::Point& value)
	{
		j.at("x").get_to(value.x);
		j.at("y").get_to(value.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Float2
	//
	////////////////////////////////////////////////////////////////

	void adl_serializer<s3d::Float2>::to_json(json& j, const s3d::Float2& value)
	{
		j = { { "x", value.x }, { "y", value.y } };
	}

	void adl_serializer<s3d::Float2>::from_json(const json& j, s3d::Float2& value)
	{
		j.at("x").get_to(value.x);
		j.at("y").get_to(value.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Vec2
	//
	////////////////////////////////////////////////////////////////

	void adl_serializer<s3d::Vec2>::to_json(json& j, const s3d::Vec2& value)
	{
		j = { { "x", value.x }, { "y", value.y } };
	}

	void adl_serializer<s3d::Vec2>::from_json(const json& j, s3d::Vec2& value)
	{
		j.at("x").get_to(value.x);
		j.at("y").get_to(value.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Float3
	//
	////////////////////////////////////////////////////////////////

	void adl_serializer<s3d::Float3>::to_json(json& j, const s3d::Float3& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "z", value.z } };
	}

	void adl_serializer<s3d::Float3>::from_json(const json& j, s3d::Float3& value)
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

	void adl_serializer<s3d::Vec3>::to_json(json& j, const s3d::Vec3& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "z", value.z } };
	}

	void adl_serializer<s3d::Vec3>::from_json(const json& j, s3d::Vec3& value)
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

	void adl_serializer<s3d::Float4>::to_json(json& j, const s3d::Float4& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "z", value.z }, { "w", value.w } };
	}

	void adl_serializer<s3d::Float4>::from_json(const json& j, s3d::Float4& value)
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

	void adl_serializer<s3d::Vec4>::to_json(json& j, const s3d::Vec4& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "z", value.z }, { "w", value.w } };
	}

	void adl_serializer<s3d::Vec4>::from_json(const json& j, s3d::Vec4& value)
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

	void adl_serializer<s3d::Color>::to_json(json& j, const s3d::Color& value)
	{
		j = { { "r", value.r }, { "g", value.g }, { "b", value.b }, { "a", value.a } };
	}

	void adl_serializer<s3d::Color>::from_json(const json& j, s3d::Color& value)
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

	void adl_serializer<s3d::ColorF>::to_json(json& j, const s3d::ColorF& value)
	{
		j = { { "r", value.r }, { "g", value.g }, { "b", value.b }, { "a", value.a } };
	}

	void adl_serializer<s3d::ColorF>::from_json(const json& j, s3d::ColorF& value)
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

	void adl_serializer<s3d::HSV>::to_json(json& j, const s3d::HSV& value)
	{
		j = { { "h", value.h }, { "s", value.s }, { "v", value.v }, { "a", value.a } };
	}

	void adl_serializer<s3d::HSV>::from_json(const json& j, s3d::HSV& value)
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

	void adl_serializer<s3d::Line>::to_json(json& j, const s3d::Line& value)
	{
		j = { { "start", value.start }, { "end", value.end } };
	}

	void adl_serializer<s3d::Line>::from_json(const json& j, s3d::Line& value)
	{
		j.at("start").get_to(value.start);
		j.at("end").get_to(value.end);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Rect
	//
	////////////////////////////////////////////////////////////////

	void adl_serializer<s3d::Rect>::to_json(json& j, const s3d::Rect& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "w", value.w }, { "h", value.h } };
	}

	void adl_serializer<s3d::Rect>::from_json(const json& j, s3d::Rect& value)
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

	void adl_serializer<s3d::RectF>::to_json(json& j, const s3d::RectF& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "w", value.w }, { "h", value.h } };
	}

	void adl_serializer<s3d::RectF>::from_json(const json& j, s3d::RectF& value)
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

	void adl_serializer<s3d::Circle>::to_json(json& j, const s3d::Circle& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "r", value.r } };
	}

	void adl_serializer<s3d::Circle>::from_json(const json& j, s3d::Circle& value)
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

	void adl_serializer<s3d::Ellipse>::to_json(json& j, const s3d::Ellipse& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "a", value.a }, { "b", value.b } };
	}

	void adl_serializer<s3d::Ellipse>::from_json(const json& j, s3d::Ellipse& value)
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

	void adl_serializer<s3d::SuperEllipse>::to_json(json& j, const s3d::SuperEllipse& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "a", value.a }, { "b", value.b }, { "n", value.n } };
	}

	void adl_serializer<s3d::SuperEllipse>::from_json(const json& j, s3d::SuperEllipse& value)
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

	void adl_serializer<s3d::Triangle>::to_json(json& j, const s3d::Triangle& value)
	{
		j = { { "p0", value.p0 }, { "p1", value.p1 }, { "p2", value.p2 } };
	}

	void adl_serializer<s3d::Triangle>::from_json(const json& j, s3d::Triangle& value)
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

	void adl_serializer<s3d::Quad>::to_json(json& j, const s3d::Quad& value)
	{
		j = { { "p0", value.p0 }, { "p1", value.p1 }, { "p2", value.p2 }, { "p3", value.p3 } };
	}

	void adl_serializer<s3d::Quad>::from_json(const json& j, s3d::Quad& value)
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

	void adl_serializer<s3d::RoundRect>::to_json(json& j, const s3d::RoundRect& value)
	{
		j = { { "x", value.x }, { "y", value.y }, { "w", value.w }, { "h", value.h }, { "r", value.r } };
	}

	void adl_serializer<s3d::RoundRect>::from_json(const json& j, s3d::RoundRect& value)
	{
		j.at("x").get_to(value.x);
		j.at("y").get_to(value.y);
		j.at("w").get_to(value.w);
		j.at("h").get_to(value.h);
		j.at("r").get_to(value.r);
	}
}
