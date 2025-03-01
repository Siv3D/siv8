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
	//	IsNaN
	//
	////////////////////////////////////////////////////////////////

	bool IsNaN(const Concept::FloatingPoint auto value) noexcept
	{
		return std::isnan(value);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	IsFinite
	//
	////////////////////////////////////////////////////////////////

	bool IsFinite(const Concept::FloatingPoint auto value) noexcept
	{
		return std::isfinite(value);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	IsInfinity
	//
	////////////////////////////////////////////////////////////////

	bool IsInfinity(const Concept::FloatingPoint auto value) noexcept
	{
		return std::isinf(value);
	}
}
