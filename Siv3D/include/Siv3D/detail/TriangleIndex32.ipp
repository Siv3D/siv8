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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	flip
	//
	////////////////////////////////////////////////////////////////

	constexpr void TriangleIndex32::flip() noexcept
	{
		std::ranges::swap(i1, i2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	constexpr TriangleIndex32 TriangleIndex32::Zero() noexcept
	{
		return{ 0, 0, 0 };
	}
}
