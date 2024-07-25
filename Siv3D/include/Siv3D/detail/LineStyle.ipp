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
	constexpr LineStyle::Parameters LineStyle::Parameters::operator ()(const double _dotOffset) const noexcept
	{
		return Parameters{ _dotOffset, cap, type };
	}

	constexpr LineStyle::LineStyle(const Parameters& params) noexcept
		: dotOffset{ params.dotOffset }
		, cap{ params.cap }
		, type{ params.type } {}
}
