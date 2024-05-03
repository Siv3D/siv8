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

namespace s3d
{
	namespace PRNG
	{
		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		inline PCG64::PCG64() noexcept
			: m_state{ pcg_extras::seed_seq_from<HardwareRNG>{} } {}

		inline PCG64::PCG64(const uint64 seed) noexcept
			: m_state{ seed } {}

		template <class SeedSequence>
		PCG64::PCG64(SeedSequence&& seedSequence) noexcept
			: m_state{ std::forward<SeedSequence>(seedSequence) } {}

		inline PCG64::PCG64(const state_type state) noexcept
			: m_state{ state } {}

		////////////////////////////////////////////////////////////////
		//
		//	seed
		//
		////////////////////////////////////////////////////////////////

		inline void PCG64::seed(const uint64 seed) noexcept
		{
			m_state.seed(seed);
		}

		template <class SeedSequence>
		void PCG64::seed(SeedSequence&& seedSequence) noexcept
		{
			m_state.seed(std::forward<SeedSequence>(seedSequence));
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		inline PCG64::result_type PCG64::operator ()() noexcept
		{
			return m_state();
		}

		////////////////////////////////////////////////////////////////
		//
		//	generateReal
		//
		////////////////////////////////////////////////////////////////

		inline double PCG64::generateReal() noexcept
		{
			const uint64 v = m_state();
			return ((v >> 11) * (1.0 / 9007199254740992.0));
		}

		////////////////////////////////////////////////////////////////
		//
		//	min, max
		//
		////////////////////////////////////////////////////////////////

		constexpr PCG64::result_type PCG64::min() noexcept
		{
			return state_type::min();
		}

		constexpr PCG64::result_type PCG64::max() noexcept
		{
			return state_type::max();
		}

		////////////////////////////////////////////////////////////////
		//
		//	serialize
		//
		////////////////////////////////////////////////////////////////

		inline PCG64::state_type PCG64::serialize() const noexcept
		{
			return m_state;
		}

		////////////////////////////////////////////////////////////////
		//
		//	deserialize
		//
		////////////////////////////////////////////////////////////////

		inline void PCG64::deserialize(const state_type state) noexcept
		{
			m_state = state;
		}
	}
}
