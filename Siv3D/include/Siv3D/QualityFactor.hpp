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
# include "Utility.hpp"
# include "PointsPerCircle.hpp"

namespace s3d
{
	/// @brief 品質係数
	class QualityFactor
	{
	public:

		static constexpr double DefaultQuality = 1.0;

		[[nodiscard]]
		QualityFactor() = default;

		[[nodiscard]]
		explicit constexpr QualityFactor(double value) noexcept;

		[[nodiscard]]
		constexpr QualityFactor operator *(double s) const noexcept;

		[[nodiscard]]
		friend constexpr QualityFactor operator *(double s, const QualityFactor& q) noexcept
		{
			return QualityFactor{ q.m_value * s };
		}

		[[nodiscard]]
		constexpr double value() const noexcept;

		[[nodiscard]]
		constexpr PointsPerCircle toPointsPerCircle(double r) const noexcept;

		[[nodiscard]]
		static constexpr QualityFactor Default() noexcept;

		[[nodiscard]]
		static QualityFactor FromSceneScaling(double s = 1.0) noexcept;

	private:

		double m_value = DefaultQuality;
	};
}

# include "detail/QualityFactor.ipp"
