//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "Concepts.hpp"

namespace s3d
{
	template <Concept::FloatingPoint Float>
	class KahanSummation
	{
	public:

		[[nodiscard]]
		KahanSummation() = default;

		[[nodiscard]] 
		constexpr KahanSummation(Float init) noexcept;

		constexpr KahanSummation& operator +=(Float value) noexcept;

		constexpr KahanSummation& operator -=(Float value) noexcept;

		[[nodiscard]]
		constexpr Float value() const noexcept;

	private:

		Float m_sum = 0;

		Float m_err = 0;
	};
}

# include "detail/KahanSummation.ipp"
