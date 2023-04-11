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
	inline const char* FloatFormatter::data() const noexcept
	{
		return m_view.data();
	}

	inline size_t FloatFormatter::size() const noexcept
	{
		return m_view.size();
	}
}
