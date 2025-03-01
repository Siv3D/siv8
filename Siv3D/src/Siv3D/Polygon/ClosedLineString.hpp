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
# include <Siv3D/PointVector.hpp>
# include <Siv3D/JoinStyle.hpp>

namespace s3d
{
	void DrawClosedLineString(const Array<Vec2>& points, JoinStyle joinStyle, double thickness, const ColorF& color);

	void DrawClosedLineString(const Array<Vec2>& points, JoinStyle joinStyle, double thickness, const PatternParameters& pattern);
}
