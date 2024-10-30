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
	class PointsPerCircle
	{
	public:

		static constexpr uint32 MinPoints = 3u;

		static constexpr uint32 DefaultPoints = 24u;

		[[nodiscard]]
		PointsPerCircle() = default;

		[[nodiscard]]
		explicit constexpr PointsPerCircle(uint32 value) noexcept;

		[[nodiscard]]
		constexpr uint32 value() const noexcept;

		static constexpr PointsPerCircle Default() noexcept;

	private:

		uint32 m_value = 24;
	};
}

# include "detail/PointsPerCircle.ipp"
