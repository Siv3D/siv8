﻿//-----------------------------------------------
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
	constexpr bool RectanglePack::isEmpty() const noexcept
	{
		return rects.isEmpty();
	}

	constexpr RectanglePack::operator bool() const noexcept
	{
		return (not rects.isEmpty());
	}
}
