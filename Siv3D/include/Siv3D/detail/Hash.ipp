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

namespace s3d
{
	namespace rapidhash
	{
		////////////////////////////////////////////////////////////////
		//
		//	Hash
		//
		////////////////////////////////////////////////////////////////

		inline uint64 Hash(const Concept::TriviallyCopyable auto& data) noexcept
		{
			return Hash(std::addressof(data), sizeof(data));
		}
	}

	inline namespace xxHash3
	{		
		////////////////////////////////////////////////////////////////
		//
		//	Hash
		//
		////////////////////////////////////////////////////////////////

		inline uint64 Hash(const Concept::TriviallyCopyable auto& data) noexcept
		{
			return Hash(std::addressof(data), sizeof(data));
		}
	}
}
