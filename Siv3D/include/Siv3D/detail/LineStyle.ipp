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
	constexpr LineStyle::Parameters LineStyle::Parameters::operator ()(const double _dotOffset) const noexcept
	{
		return Parameters{ type, cap, _dotOffset };
	}

	constexpr LineStyle::LineStyle(const Parameters& params) noexcept
		: type{ params.type }
		, cap{ params.cap }
		, dotOffset{ params.dotOffset } {}
}
