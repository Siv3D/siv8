//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	bool IsNaN(const Concept::FloatingPoint auto value) noexcept
	{
		return std::isnan(value);
	}

	bool IsFinite(const Concept::FloatingPoint auto value) noexcept
	{
		return std::isfinite(value);
	}

	bool IsInfinity(const Concept::FloatingPoint auto value) noexcept
	{
		return std::isinf(value);
	}
}
