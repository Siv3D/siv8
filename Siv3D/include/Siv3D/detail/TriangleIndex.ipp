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
	////////////////////////////////////////////////////////////////
	//
	//	flip
	//
	////////////////////////////////////////////////////////////////

	constexpr void TriangleIndex::flip() noexcept
	{
		std::ranges::swap(i1, i2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	constexpr TriangleIndex TriangleIndex::Zero() noexcept
	{
		return{ 0, 0, 0 };
	}
}
