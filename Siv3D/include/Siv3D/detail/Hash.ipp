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

namespace s3d
{
	template <Concept::TriviallyCopyable TriviallyCopyable>
	inline uint64 Hash(const TriviallyCopyable& data) noexcept
	{
		return Hash(std::addressof(data), sizeof(data));
	}
}
