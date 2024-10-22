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
# include <Siv3D/Common.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Polygon.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	CalculateLineRoundBuffer
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Polygon CalculateLineRoundBuffer(const Line& line, double distance, double qualityFactor);

	////////////////////////////////////////////////////////////////
	//
	//	CalculateRoundedQuad
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Polygon CalculateRoundedQuad(const Quad& quad, double distance, double qualityFactor);

	////////////////////////////////////////////////////////////////
	//
	//	CalculatePolygonBuffer
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Polygon CalculatePolygonBuffer(std::initializer_list<Vec2> outer, double distance);

	////////////////////////////////////////////////////////////////
	//
	//	CalculatePolygonRoundBuffer
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Polygon CalculatePolygonRoundBuffer(std::initializer_list<Vec2> outer, double distance, double qualityFactor);

	////////////////////////////////////////////////////////////////
	//
	//	CircleToPolygon
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Polygon CircleToPolygon(const Circle& circle, double qualityFactor);
}
