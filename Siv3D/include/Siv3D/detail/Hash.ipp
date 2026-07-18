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

namespace s3d
{
	namespace rapidhash
	{
		////////////////////////////////////////////////////////////////
		//
		//	BitwiseHash
		//
		////////////////////////////////////////////////////////////////

		inline uint64 BitwiseHash(const Concept::TriviallyCopyable auto& data) noexcept
		{
			return BitwiseHash(std::addressof(data), sizeof(data));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Hash
		//
		////////////////////////////////////////////////////////////////

		inline uint64 Hash(const Concept::TriviallyHashable auto& data) noexcept
		{
			return BitwiseHash(std::addressof(data), sizeof(data));
		}
	}

	inline namespace xxHash3
	{
		////////////////////////////////////////////////////////////////
		//
		//	BitwiseHash
		//
		////////////////////////////////////////////////////////////////

		inline uint64 BitwiseHash(const Concept::TriviallyCopyable auto& data) noexcept
		{
			return BitwiseHash(std::addressof(data), sizeof(data));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Hash
		//
		////////////////////////////////////////////////////////////////

		inline uint64 Hash(const Concept::TriviallyHashable auto& data) noexcept
		{
			return BitwiseHash(std::addressof(data), sizeof(data));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	HashFloats
	//
	////////////////////////////////////////////////////////////////

	template<class T, std::same_as<T>... Ts>
		requires std::same_as<T, float> || std::same_as<T, double>
	uint64 HashFloats(T x, Ts... xs) noexcept
	{
		using U = std::conditional_t<std::same_as<T, float>, uint32, uint64>;
		// -0.0 を +0.0 に正規化
		const U bits[1 + sizeof...(Ts)] = {
			std::bit_cast<U>(x ? x : T{}),
			std::bit_cast<U>(xs ? xs : T{})...
		};
		return BitwiseHash(bits);
	}
}
