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

# include <Siv3D/Hash.hpp>
# include <ThirdParty/wyhash/wyhash.h>

namespace s3d
{
	uint64 Hash(const void* data, const size_t size) noexcept
	{
		return ::wyhash(data, size, 0, DefaultHashSecret);
	}

	uint64 Hash(const void* data, const size_t size, const uint64 seed) noexcept
	{
		return ::wyhash(data, size, seed, DefaultHashSecret);
	}

	uint64 Hash(const void* data, const size_t size, const uint64 seed, const uint64* secret) noexcept
	{
		return ::wyhash(data, size, seed, secret);
	}

	uint64 MixHash(const uint64 a, const uint64 b) noexcept
	{
		return ::wyhash64(a, b);
	}
}
