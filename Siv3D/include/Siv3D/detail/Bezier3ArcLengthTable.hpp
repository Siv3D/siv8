//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <array>
# include "../Common.hpp"

namespace s3d::detail
{
	// 32 regular intervals plus the at most four component-derivative roots.
	struct Bezier3ArcLengthTable
	{
		static constexpr size_t Subdivisions = 32;
		std::array<double, (Subdivisions + 5)> parameters{};
		std::array<double, (Subdivisions + 5)> distances{};
		size_t count = 0;

		[[nodiscard]]
		double length() const noexcept { return distances[count - 1]; }
	};
}
