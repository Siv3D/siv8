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

	constexpr Triangle Quad::triangle(const size_t index) const
	{
		if (index == 0)
		{
			return{ p0, p1, p3 };
		}
		else if (index == 1)
		{
			return{ p3, p1, p2 };
		}
		else
		{
			throw std::out_of_range{ "Quad::triangle() index out of range" };
		}
	}
}
