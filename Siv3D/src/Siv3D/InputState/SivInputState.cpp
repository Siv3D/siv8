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

# include <Siv3D/InputState.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void InputState::update(const bool currentPressed)
	{
		const bool previousPressed = m_pressed;
		m_pressed	= currentPressed;
		m_down		= ((not previousPressed) && m_pressed);
		m_up		= (previousPressed && (not m_pressed));
		m_cleared	= false;

		if (m_down)
		{
			stopwatch.restart();
		}
		else if (m_up)
		{
			pressedDuration = stopwatch.elapsed();
			stopwatch.reset();
		}
		else if (m_pressed)
		{
			pressedDuration = stopwatch.elapsed();
		}
		else
		{
			pressedDuration = Duration{ 0 };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	void InputState::reset()
	{
		pressedDuration = Duration{ 0 };
		stopwatch.reset();
		m_up		= false;
		m_pressed	= false;
		m_down		= false;
		m_cleared	= false;
	}
}
