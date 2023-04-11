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

# include <Siv3D/Hash.hpp>
# include <ThirdParty/wyhash/wyhash.h>
# define XXH_IMPLEMENTATION
# define XXH_STATIC_LINKING_ONLY
# define XXH_NO_STREAM
# define XXH_NAMESPACE SIV3D_
# include <ThirdParty/xxHash/xxhash.h>

namespace s3d
{
	namespace wyhash
	{
		uint64 Hash(const void* data, const size_t size) noexcept
		{
			return ::wyhash(data, size, 0, DefaultHashSecret);
		}

		uint64 Hash(const void* data, const size_t size, const uint64 seed) noexcept
		{
			return ::wyhash(data, size, seed, DefaultHashSecret);
		}

		uint64 Hash(const void* data, const size_t size, const uint64 seed, const uint64(&secret)[4]) noexcept
		{
			return ::wyhash(data, size, seed, secret);
		}
	}

	namespace xxHash3
	{
		uint64 Hash(const void* data, const size_t size) noexcept
		{
			return SIV3D_XXH3_64bits(data, size);
		}

		uint64 Hash(const void* data, const size_t size, const uint64 seed) noexcept
		{
			return SIV3D_XXH3_64bits_withSeed(data, size, seed);
		}

		uint64 Hash(const void* data, const size_t size, const uint64 seed, const uint64(&secret)[4]) noexcept
		{
			return SIV3D_XXH3_64bits_withSecretandSeed(data, size, secret, (sizeof(uint64) * 4), seed);
		}
	}

	uint64 MixHash(const uint64 a, const uint64 b) noexcept
	{
		return ::wyhash64(a, b);
	}
}
