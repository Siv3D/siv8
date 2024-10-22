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
	[[nodiscard]]
	Polygon CalculateLineRoundBuffer(const Line& line, double distance, double qualityFactor);

	[[nodiscard]]
	Polygon CalculatePolygonBuffer(std::initializer_list<Vec2> outer, double distance);

	[[nodiscard]]
	Polygon CalculatePolygonRoundBuffer(std::initializer_list<Vec2> outer, double distance, double qualityFactor);
	
	[[nodiscard]]
	Polygon CircleToPolygon(const Circle& circle, double qualityFactor);
}
