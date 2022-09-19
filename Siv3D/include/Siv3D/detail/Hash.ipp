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
	inline uint64 Hash(const void* data, const size_t size) noexcept
	{
		return ::wyhash(data, size, 0, DefaultHashSecret);
	}

	inline uint64 Hash(const void* data, const size_t size, const uint64 seed) noexcept
	{
		return ::wyhash(data, size, seed, DefaultHashSecret);
	}

	inline uint64 Hash(const void* data, const size_t size, const uint64 seed, const uint64* secret) noexcept
	{
		return ::wyhash(data, size, seed, secret);
	}

	template <Concept::TriviallyCopyable TriviallyCopyable>
	inline uint64 Hash(const TriviallyCopyable& data) noexcept
	{
		return ::wyhash(std::addressof(data), sizeof(data), 0, DefaultHashSecret);
	}

	inline uint64 MixHash(const uint64 a, const uint64 b) noexcept
	{
		return ::wyhash64(a, b);
	}
}
