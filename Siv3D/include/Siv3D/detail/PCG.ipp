//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	namespace PRNG
	{
		inline PCG64::PCG64() noexcept
			: m_rng{} {}

		inline PCG64::PCG64(const uint64 seed) noexcept
			: m_rng{ seed } {}

		inline PCG64::PCG64(const state_type& seeds) noexcept
			: m_rng{ PCG_128BIT_CONSTANT(seeds[0], seeds[1]) } {}

		inline void PCG64::seed(const uint64 seed) noexcept
		{
			m_rng.seed(seed);
		}

		inline void PCG64::seed(const state_type& seeds) noexcept
		{
			m_rng.seed(PCG_128BIT_CONSTANT(seeds[0], seeds[1]));
		}

		constexpr PCG64::result_type PCG64::min() noexcept
		{
			return engine_type::min();
		}

		constexpr PCG64::result_type PCG64::max() noexcept
		{
			return engine_type::max();
		}

		inline PCG64::result_type PCG64::operator ()() noexcept
		{
			return m_rng();
		}
	}
}
