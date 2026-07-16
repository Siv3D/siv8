//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <memory> // std::addressof
# include <span>
# include <array>
# include "Types.hpp"
# include "Concepts.hpp"
# include "Byte.hpp"

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
		uint64 BitwiseHash(const void* data, size_t size) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(const void* data, size_t size, uint64 seed) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @param secret シークレット値 | Secret value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(const void* data, size_t size, uint64 seed, const Secret& secret) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(std::span<const Byte> data) noexcept;
		
		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param seed シード値 | Seed value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(std::span<const Byte> data, uint64 seed) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param seed シード値 | Seed value
		/// @param secret シークレット値 | Secret value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(std::span<const Byte> data, uint64 seed, const Secret& secret) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(const Concept::TriviallyCopyable auto& data) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const Concept::TriviallyHashable auto& data) noexcept;
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
		uint64 BitwiseHash(const void* data, size_t size) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(const void* data, size_t size, uint64 seed) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param size 計算対象のデータのサイズ（バイト） | Size of data to be calculated (bytes)
		/// @param seed シード値 | Seed value
		/// @param secret シークレット値 | Secret value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(const void* data, size_t size, uint64 seed, const Secret& secret) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(std::span<const Byte> data) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param seed シード値 | Seed value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(std::span<const Byte> data, uint64 seed) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @param seed シード値 | Seed value
		/// @param secret シークレット値 | Secret value
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(std::span<const Byte> data, uint64 seed, const Secret& secret) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 BitwiseHash(const Concept::TriviallyCopyable auto& data) noexcept;

		/// @brief ハッシュ値を計算します。 | Calculates a hash value.
		/// @param data 計算対象のデータ | Data to be calculated
		/// @return ハッシュ値 | Hash value
		[[nodiscard]]
		uint64 Hash(const Concept::TriviallyHashable auto& data) noexcept;
	}

	////////////////////////////////////////////////////////////////
	//
	//	HashFloats
	//
	////////////////////////////////////////////////////////////////

	/// @brief 1 個以上の浮動小数点数からハッシュ値を計算します。
	/// @tparam T 浮動小数点数の型（float または double）
	/// @tparam ...Ts T と同じ型（すべての引数は同一の型である必要があります）
	/// @param x ハッシュ化する値
	/// @param ...xs ハッシュ化する追加の値
	/// @return 計算されたハッシュ値
	/// @remark -0.0 は +0.0 に正規化されるため、`a == b` である 2 つの値は常に同じハッシュ値になります。
	/// @remark NaN のハッシュ値はビットパターン（ペイロード）に依存するため、同じ NaN 同士でも異なる場合があります。
	template<class T, std::same_as<T>... Ts>
		requires std::same_as<T, float> || std::same_as<T, double>
	[[nodiscard]]
	uint64 HashFloats(T x, Ts... xs) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	MixHash
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2 つのハッシュ値を混ぜます。 | Mixes two hash values.
	/// @param a 一方のハッシュ値 | One of the hash values
	/// @param b もう一方のハッシュ値 | The other hash value
	/// @return 混ぜたハッシュ値 | Mixed hash value
	/// @remark 複数のハッシュ値から新しいハッシュ値を生成するために使います。 | Used to generate a new hash value from multiple hash values.
	[[nodiscard]]
	uint64 MixHash(uint64 a, uint64 b) noexcept;
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Byte>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Byte& value) const noexcept
	{
		return s3d::Hash(value);
	}
};

# include "detail/Hash.ipp"
