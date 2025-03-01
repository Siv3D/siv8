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

# pragma once
# include <memory> // std::addressof
# include "Types.hpp"
# include "Concepts.hpp"

namespace s3d
{
	namespace rapidhash
	{
		/// @brief rapidhash のデフォルトのシード値 | Default seed value for rapidhash
		inline constexpr uint64 DefaultSeed = 0xbdd89aa982704029ull;

		/// @brief rapidhash のシークレット値 | Secret value for rapidhash
		using Secret = uint64[3];

		/// @brief rapidhash のデフォルトのシークレット値 | Default secret value for rapidhash
		inline constexpr Secret DefaultHashSecret{ 0x2d358dccaa6c78a5ull, 0x8bb84b93962eacc9ull, 0x4b33a62ed433d4a3ull };

		////////////////////////////////////////////////////////////////
		//
		//	Hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const void* data, size_t size) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const void* data, size_t size, uint64 seed) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @param secret シークレット値 | Secret value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const void* data, size_t size, uint64 seed, const Secret& secret) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const Concept::TriviallyCopyable auto& data) noexcept;
	}

	inline namespace xxHash3
	{
		/// @brief xxHash3 のデフォルトのシード値 | Default seed value for xxHash3
		inline constexpr uint64 DefaultSeed = 0;

		/// @brief xxHash3 のシークレット値 | Secret value for xxHash3
		using Secret = uint8[192];

		////////////////////////////////////////////////////////////////
		//
		//	Hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const void* data, size_t size) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const void* data, size_t size, uint64 seed) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @param secret シークレット値 | Secret value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const void* data, size_t size, uint64 seed, const Secret& secret) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const Concept::TriviallyCopyable auto& data) noexcept;
	}

	/// @brief 2 つのハッシュ値を混ぜます。 | Mixes two hash values.
	/// @param a 一方のハッシュ値 | One of the hash values
	/// @param b もう一方のハッシュ値 | The other hash value
	/// @return 混ぜたハッシュ値 | Mixed hash value
	/// @remark 複数のハッシュ値から新しいハッシュ値を生成するために使います。 | Used to generate a new hash value from multiple hash values.
	[[nodiscard]]
	uint64 MixHash(uint64 a, uint64 b) noexcept;
}

# include "detail/Hash.ipp"
