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
# include "Common.hpp"
# include "LineCap.hpp"
# include "LineType.hpp"

namespace s3d
{
	/// @brief 線のスタイル
	struct LineStyle
	{
		struct Parameters
		{
			LineType type;

			LineCap cap;

			double dotOffset = 0.0;

			[[nodiscard]]
			constexpr Parameters operator ()(double _dotOffset) const noexcept;
		};

		LineType type = LineType::Solid;

		LineCap cap = LineCap::Square;

		double dotOffset = 0.0;

		[[nodiscard]]
		constexpr LineStyle() = default;

		[[nodiscard]]
		constexpr LineStyle(const Parameters& params) noexcept;

		static constexpr Parameters Square{ LineType::Solid, LineCap::Square, 0.0 };

		static constexpr Parameters Round{ LineType::Solid, LineCap::Round, 0.0 };

		static constexpr Parameters Flat{ LineType::Solid, LineCap::Flat, 0.0 };

		static constexpr Parameters Dotted{ LineType::Dotted, LineCap::Square, 0.0 };

		static constexpr Parameters Dashed{ LineType::Dashed, LineCap::Square, 0.0 };

		static constexpr Parameters LongDash{ LineType::LongDash, LineCap::Square, 0.0 };

		static constexpr Parameters DashDot{ LineType::DashDot, LineCap::Square, 0.0 };

		static constexpr Parameters RoundDot{ LineType::RoundDot, LineCap::Square, 0.0 };

		static constexpr Parameters DottedFlat{ LineType::Dotted, LineCap::Flat, 0.0 };

		static constexpr Parameters DashedFlat{ LineType::Dashed, LineCap::Flat, 0.0 };

		static constexpr Parameters LongDashFlat{ LineType::LongDash, LineCap::Flat, 0.0 };

		static constexpr Parameters DashDotFlat{ LineType::DashDot, LineCap::Flat, 0.0 };

		static constexpr Parameters RoundDotFlat{ LineType::RoundDot, LineCap::Flat, 0.0 };

	};
}

# include "detail/LineStyle.ipp"
