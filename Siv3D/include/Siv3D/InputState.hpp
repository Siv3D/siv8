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
# include <Siv3D/Common.hpp>
# include <Siv3D/Stopwatch.hpp>

namespace s3d
{
	struct InputState
	{
		Duration pressedDuration{ 0 };

		Stopwatch stopwatch;

		void update(bool currentPressed);

		void reset();

		void clearInput();

		bool isCleared() const noexcept;

		bool up() const noexcept;

		bool pressed() const noexcept;

		bool down() const noexcept;

	private:

		bool m_up = false;

		bool m_pressed = false;

		bool m_down = false;

		bool m_cleared = false;
	};
}

# include "detail/InputState.ipp"
