﻿//-----------------------------------------------
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
	namespace wyhash
	{
		inline uint64 Hash(const Concept::TriviallyCopyable auto& data) noexcept
		{
			return Hash(std::addressof(data), sizeof(data));
		}
	}

	namespace xxHash3
	{
		inline uint64 Hash(const Concept::TriviallyCopyable auto& data) noexcept
		{
			return Hash(std::addressof(data), sizeof(data));
		}
	}
}