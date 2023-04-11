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
# include <memory> // std::addressof
# include "Types.hpp"
# include "Concepts.hpp"

namespace s3d
{
	namespace wyhash
	{
		/// @brief ハッシュ関数のデフォルトのシークレット値 | Default secret value of hash function
		inline constexpr uint64 DefaultHashSecret[4] = { 0xa0761d6478bd642full, 0xe7037ed1a0b428dbull, 0x8ebc6af09c88c6e3ull, 0x589965cc75374cc3ull };

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @return ハッシュ値 | Hash value
		uint64 Hash(const void* data, size_t size) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @return ハッシュ値 | Hash value
		uint64 Hash(const void* data, size_t size, uint64 seed) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @param secret シークレット値 | Secret value
		/// @remark secret は 4 つの uint64 値の配列でなければなりません。 | secret must be an array of 4 uint64 values.
		/// @return ハッシュ値 | Hash value
		uint64 Hash(const void* data, size_t size, uint64 seed, const uint64* secret) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @tparam TriviallyCopyable TriviallyCopyable 型 | TriviallyCopyable type
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		template <Concept::TriviallyCopyable TriviallyCopyable>
		uint64 Hash(const TriviallyCopyable& data) noexcept;
	}

	inline namespace xxHash3
	{
		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @return ハッシュ値 | Hash value
		uint64 Hash(const void* data, size_t size) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @return ハッシュ値 | Hash value
		uint64 Hash(const void* data, size_t size, uint64 seed) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @param secret シークレット値 | Secret value
		/// @remark secret は 4 つの uint64 値の配列でなければなりません。 | secret must be an array of 4 uint64 values.
		/// @return ハッシュ値 | Hash value
		uint64 Hash(const void* data, size_t size, uint64 seed, const uint64* secret) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @tparam TriviallyCopyable TriviallyCopyable 型 | TriviallyCopyable type
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		template <Concept::TriviallyCopyable TriviallyCopyable>
		uint64 Hash(const TriviallyCopyable& data) noexcept;
	}

	/// @brief 2 つのハッシュ値を混ぜます。 | Mixes two hash values.
	/// @param a 一方のハッシュ値 | One of the hash values
	/// @param b もう一方のハッシュ値 | The other hash value
	/// @return 混ぜたハッシュ値 | Mixed hash value
	/// @remark 複数のハッシュ値から新しいハッシュ値を生成するために使います。 | Used to generate a new hash value from multiple hash values.
	uint64 MixHash(uint64 a, uint64 b) noexcept;
}

# include "detail/Hash.ipp"
