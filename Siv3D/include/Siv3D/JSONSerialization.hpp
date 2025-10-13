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

# pragma once
# include "Common.hpp"
# include "JSON.hpp"
# include "2DShapesFwd.hpp"
# include "ColorHSV.hpp"

namespace nlohmann
{
	////////////////////////////////////////////////////////////////
	//
	//	Point
	//
	////////////////////////////////////////////////////////////////

	template <>
	struct adl_serializer<s3d::Point>
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
	struct adl_serializer<s3d::Float2>
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
	struct adl_serializer<s3d::Vec2>
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
	struct adl_serializer<s3d::Float3>
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
	struct adl_serializer<s3d::Vec3>
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
	struct adl_serializer<s3d::Float4>
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
	struct adl_serializer<s3d::Vec4>
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
	struct adl_serializer<s3d::Color>
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
	struct adl_serializer<s3d::ColorF>
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
	struct adl_serializer<s3d::HSV>
	{
		static void to_json(s3d::JSON::json_base& j, const s3d::HSV& value);
		static void from_json(const s3d::JSON::json_base& j, s3d::HSV& value);
	};

	////////////////////////////////////////////////////////////////
	//
	//	Line
	//
	////////////////////////////////////////////////////////////////

	template <>
	struct adl_serializer<s3d::Line>
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
	struct adl_serializer<s3d::Rect>
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
	struct adl_serializer<s3d::RectF>
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
	struct adl_serializer<s3d::Circle>
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
	struct adl_serializer<s3d::Ellipse>
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
	struct adl_serializer<s3d::SuperEllipse>
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
	struct adl_serializer<s3d::Triangle>
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
	struct adl_serializer<s3d::Quad>
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
	struct adl_serializer<s3d::RoundRect>
	{
		static void to_json(s3d::JSON::json_base& j, const s3d::RoundRect& value);
		static void from_json(const s3d::JSON::json_base& j, s3d::RoundRect& value);
	};
}
