//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	inline void InputState::clearInput()
	{
		m_cleared = true;
	}

	inline bool InputState::isCleared() const noexcept
	{
		return m_cleared;
	}

	inline bool InputState::up() const noexcept
	{
		return m_up;
	}

	inline bool InputState::pressed() const noexcept
	{
		return ((not m_cleared) && m_pressed);
	}

	inline bool InputState::down() const noexcept
	{
		return ((not m_cleared) && m_down);
	}
}
