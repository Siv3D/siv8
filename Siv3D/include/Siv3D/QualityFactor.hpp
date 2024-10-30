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

namespace s3d
{
	/// @brief 品質係数
	class QualityFactor
	{
	public:

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
		static QualityFactor FromSceneScaling(double s = 1.0) noexcept;

	private:

		double m_value = 1.0;
	};
}

# include "detail/QualityFactor.ipp"
