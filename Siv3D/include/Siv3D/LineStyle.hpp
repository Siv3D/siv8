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

namespace s3d
{
	/// @brief 線のスタイル
	struct LineStyle
	{
		struct Parameters
		{
			double dotOffset;

			LineCap cap;

			bool isDotted;

			bool hasAlignedDot;

			[[nodiscard]]
			constexpr Parameters operator ()(double _dotOffset) const noexcept;

			[[nodiscard]]
			constexpr Parameters offset(double _dotOffset) const noexcept;
		};

		double dotOffset = 0.0;

		LineCap cap = LineCap::Square;

		bool isDotted = false;

		bool hasAlignedDot = true;

		[[nodiscard]]
		constexpr LineStyle() = default;

		[[nodiscard]]
		constexpr LineStyle(const Parameters& params) noexcept;

		[[nodiscard]]
		constexpr bool hasSquareCap() const noexcept;

		[[nodiscard]]
		constexpr bool hasRoundCap() const noexcept;

		[[nodiscard]]
		constexpr bool hasNoCap() const noexcept;

		[[nodiscard]]
		constexpr bool hasSquareDot() const noexcept;

		[[nodiscard]]
		constexpr bool hasRoundDot() const noexcept;

		/// @brief 始点と終点に四角いキャップを持つ線
		static constexpr Parameters SquareCap{ 0.0, LineCap::Square, false, false };

		/// @brief 始点と終点に丸いキャップを持つ線
		static constexpr Parameters RoundCap{ 0.0, LineCap::Round, false, false };

		/// @brief 始点と終点にキャップを持たない線
		static constexpr Parameters Uncapped{ 0.0, LineCap::Flat, false, false };

		/// @brief 四角いドットの線
		static constexpr Parameters SquareDot{ 0.0, LineCap::Square, true, false };

		/// @brief 丸いドットの線
		static constexpr Parameters RoundDot{ 0.0, LineCap::Round, true, true };

		/// @brief デフォルトの線 (SquareCap)
		static constexpr Parameters Default = SquareCap;
	};
}

# include "detail/LineStyle.ipp"
