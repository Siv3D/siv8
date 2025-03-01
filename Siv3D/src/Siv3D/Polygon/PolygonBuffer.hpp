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
	Polygon CalculateLineRoundBuffer(const Line& line, double distance, const QualityFactor& qualityFactor);

	////////////////////////////////////////////////////////////////
	//
	//	CalculateRoundedQuad
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Polygon CalculateRoundedQuad(const Quad& quad, double distance, const QualityFactor& qualityFactor);

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
	Polygon CalculatePolygonRoundBuffer(std::initializer_list<Vec2> outer, double distance, const QualityFactor& qualityFactor);

	////////////////////////////////////////////////////////////////
	//
	//	CircleToPolygon
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Polygon CircleToPolygon(const Circle& circle, const QualityFactor& qualityFactor);
}
