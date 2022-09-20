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
# include <memory>
# include "Types.hpp"
# include "Concepts.hpp"

namespace s3d
{
	inline constexpr uint64 DefaultHashSecret[4] = { 0xa0761d6478bd642full, 0xe7037ed1a0b428dbull, 0x8ebc6af09c88c6e3ull, 0x589965cc75374cc3ull };

	uint64 Hash(const void* data, size_t size) noexcept;

	uint64 Hash(const void* data, size_t size, uint64 seed) noexcept;

	uint64 Hash(const void* data, size_t size, uint64 seed, const uint64* secret) noexcept;

	template <Concept::TriviallyCopyable TriviallyCopyable>
	uint64 Hash(const TriviallyCopyable& data) noexcept;

	uint64 MixHash(uint64 a, uint64 b) noexcept;
}

# include "detail/Hash.ipp"
