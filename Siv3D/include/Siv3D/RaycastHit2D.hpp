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
# include "PointVector.hpp"

namespace s3d
{
	struct RaycastHit2D
	{
		Vec2 position{ 0.0, 0.0 };
		
		Vec2 normal{ 0.0, 0.0 };
		
		double distance = 0.0;
		
		bool startsInside = false;
	};
}
