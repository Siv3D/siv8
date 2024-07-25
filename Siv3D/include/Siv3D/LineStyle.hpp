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
			double dotOffset;

			LineCap cap;

			LineType type;

			[[nodiscard]]
			constexpr Parameters operator ()(double _dotOffset) const noexcept;
		};

		double dotOffset = 0.0;

		LineCap cap = LineCap::Square;

		LineType type = LineType::Solid;

		[[nodiscard]]
		constexpr LineStyle() = default;

		[[nodiscard]]
		constexpr LineStyle(const Parameters& params) noexcept;

		static constexpr Parameters Square{ 0.0, LineCap::Square, LineType::Solid };

		static constexpr Parameters Round{ 0.0, LineCap::Round, LineType::Solid };

		static constexpr Parameters Flat{ 0.0, LineCap::Flat, LineType::Solid };

		static constexpr Parameters Dotted{ 0.0, LineCap::Square, LineType::Dotted };

		static constexpr Parameters Dashed{ 0.0, LineCap::Square, LineType::Dashed };

		static constexpr Parameters RoundDot{ 0.0, LineCap::Square, LineType::RoundDot };
	};
}

# include "detail/LineStyle.ipp"
