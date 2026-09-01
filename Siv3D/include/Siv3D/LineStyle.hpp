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
# include "Common.hpp"
# include "LineCap.hpp"
# include "LinePattern.hpp"

namespace s3d
{
	/// @brief 線のスタイル
	struct LineStyle
	{
		struct Parameters
		{
			LinePattern type;

			LineCap cap;

			double dotOffset = 0.0;

			[[nodiscard]]
			constexpr Parameters operator ()(double _dotOffset) const noexcept;
		};

		LinePattern type = LinePattern::Solid;

		LineCap cap = LineCap::Square;

		double dotOffset = 0.0;

		[[nodiscard]]
		constexpr LineStyle() = default;

		[[nodiscard]]
		constexpr LineStyle(const Parameters& params) noexcept;

		static constexpr Parameters Square{ LinePattern::Solid, LineCap::Square, 0.0 };

		static constexpr Parameters Round{ LinePattern::Solid, LineCap::Round, 0.0 };

		static constexpr Parameters Flat{ LinePattern::Solid, LineCap::Flat, 0.0 };

		static constexpr Parameters Dotted{ LinePattern::Dotted, LineCap::Square, 0.0 };

		static constexpr Parameters Dashed{ LinePattern::Dashed, LineCap::Square, 0.0 };

		static constexpr Parameters LongDash{ LinePattern::LongDash, LineCap::Square, 0.0 };

		static constexpr Parameters DashDot{ LinePattern::DashDot, LineCap::Square, 0.0 };

		static constexpr Parameters RoundDot{ LinePattern::RoundDot, LineCap::Square, 0.0 };

		static constexpr Parameters DottedFlat{ LinePattern::Dotted, LineCap::Flat, 0.0 };

		static constexpr Parameters DashedFlat{ LinePattern::Dashed, LineCap::Flat, 0.0 };

		static constexpr Parameters LongDashFlat{ LinePattern::LongDash, LineCap::Flat, 0.0 };

		static constexpr Parameters DashDotFlat{ LinePattern::DashDot, LineCap::Flat, 0.0 };

		static constexpr Parameters RoundDotFlat{ LinePattern::RoundDot, LineCap::Flat, 0.0 };

	};
}

# include "detail/LineStyle.ipp"
