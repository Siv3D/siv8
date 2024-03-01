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
# include "HardwareRNG.hpp"
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4244)
# include <ThirdParty/pcg/pcg_random.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	namespace PRNG
	{
		class PCG64
		{
		public:

			using state_type	= pcg64;

			using result_type	= state_type::result_type;

			[[nodiscard]]
			PCG64() noexcept;

			[[nodiscard]]
			explicit PCG64(uint64 seed) noexcept;

			template <class SeedSequence>
			[[nodiscard]]
			explicit PCG64(SeedSequence&& seedSequence) noexcept;

			[[nodiscard]]
			explicit PCG64(state_type state) noexcept;

			void seed(uint64 seed) noexcept;

			template <class SeedSequence>
			void seed(SeedSequence&& seedSequence) noexcept;

			result_type operator()() noexcept;

			/// @brief [0, 1) の範囲の乱数を生成します。 | Generates a pseudo-random value in [0, 1)
			/// @return 生成された乱数 | A generated pseudo-random value
			[[nodiscard]]
			double generateReal() noexcept;

			[[nodiscard]]
			static constexpr result_type min() noexcept;

			[[nodiscard]]
			static constexpr result_type max() noexcept;

			[[nodiscard]]
			state_type serialize() const noexcept;

			void deserialize(state_type state) noexcept;

			[[nodiscard]]
			friend bool operator ==(const PCG64& lhs, const PCG64& rhs) noexcept
			{
				return lhs.m_state == rhs.m_state;
			}

		private:

			state_type m_state;
		};
	}
}

# include "detail/PCG64.ipp"
