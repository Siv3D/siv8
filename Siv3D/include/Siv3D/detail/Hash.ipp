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
}
