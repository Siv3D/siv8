//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Hash.hpp>
# include <ThirdParty/wyhash/wyhash.h>
# include <ThirdParty/rapidhash/rapidhash.h>
# define XXH_IMPLEMENTATION
# define XXH_STATIC_LINKING_ONLY
# define XXH_NO_STREAM
# define XXH_NAMESPACE SIV3D_
# include <ThirdParty/xxHash/xxhash.h>

namespace s3d
{
	namespace rapidhash
	{
		////////////////////////////////////////////////////////////////
		//
		//	Hash
		//
		////////////////////////////////////////////////////////////////

		uint64 Hash(const void* data, const size_t size) noexcept
		{
			return ::rapidhash_internal(data, size, DefaultSeed, rapid_secret);
		}

		uint64 Hash(const void* data, const size_t size, const uint64 seed) noexcept
		{
			return ::rapidhash_internal(data, size, seed, rapid_secret);
		}

		uint64 Hash(const void* data, const size_t size, const uint64 seed, const Secret& secret) noexcept
		{
			return ::rapidhash_internal(data, size, seed, secret);
		}
	}

	inline namespace xxHash3
	{
		////////////////////////////////////////////////////////////////
		//
		//	Hash
		//
		////////////////////////////////////////////////////////////////

		uint64 Hash(const void* data, const size_t size) noexcept
		{
			return SIV3D_XXH3_64bits(data, size);
		}

		uint64 Hash(const void* data, const size_t size, const uint64 seed) noexcept
		{
			return SIV3D_XXH3_64bits_withSeed(data, size, seed);
		}

		uint64 Hash(const void* data, const size_t size, const uint64 seed, const Secret& secret) noexcept
		{
			return SIV3D_XXH3_64bits_withSecretandSeed(data, size, secret, sizeof(secret), seed);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	MixHash
	//
	////////////////////////////////////////////////////////////////

	uint64 MixHash(const uint64 a, const uint64 b) noexcept
	{
		return ::wyhash64(a, b);
	}
}
