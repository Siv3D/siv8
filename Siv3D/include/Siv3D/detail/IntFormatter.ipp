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
	inline const char* IntFormatter::data() const noexcept
	{
		return m_str;
	}

	inline size_t IntFormatter::size() const noexcept
	{
		return static_cast<size_t>(m_buffer - m_str + BufferSize - 1);
	}
}
