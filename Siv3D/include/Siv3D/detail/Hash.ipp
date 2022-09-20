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

# pragma once

namespace s3d
{
	template <Concept::TriviallyCopyable TriviallyCopyable>
	inline uint64 Hash(const TriviallyCopyable& data) noexcept
	{
		return ::wyhash(std::addressof(data), sizeof(data), 0, DefaultHashSecret);
	}
}
