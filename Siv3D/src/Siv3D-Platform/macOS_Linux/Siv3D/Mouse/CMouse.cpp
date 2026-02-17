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

# include "CMouse.hpp"
# include <Siv3D/UserAction.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	~CMouse
	//
	////////////////////////////////////////////////////////////////

	CMouse::~CMouse()
	{
		LOG_SCOPED_DEBUG("CMouse::~CMouse()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CMouse::init()
	{
		LOG_SCOPED_DEBUG("CMouse::init()");

		m_window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());

		m_internalMouseButtonStates.fill(MouseButtonState::Released);

		if (m_window)
		{
			::glfwSetMouseButtonCallback(m_window, OnMouseButtonUpdated);
			::glfwSetScrollCallback(m_window, OnScroll);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CMouse::update()
	{
		// マウスボタンの状態を更新
		{
			std::lock_guard lock{ m_mouseButtonMutex };

			for (uint32 i = 0; i < Mouse::NumButtons; ++i)
			{
				auto& internalState = m_internalMouseButtonStates[i];
				const bool pressed = (internalState == MouseButtonState::PressedFirst)
					|| (internalState == MouseButtonState::Pressed)
					|| (internalState == MouseButtonState::Tapped);

				m_mouseButton.states[i].update(pressed);
				
				if (internalState == MouseButtonState::Tapped)
				{
					internalState = MouseButtonState::Released;
				}
			}
		}

		{
			m_mouseButton.allInputs.clear();

			for (uint32 i = 0; i < Mouse::NumButtons; ++i)
			{
				const auto& state = m_mouseButton.states[i];

				if (state.pressed() || state.up())
				{
					m_mouseButton.allInputs.emplace_back(InputDeviceType::Mouse, static_cast<uint8>(i));
				}
			}

			if(m_mouseButton.allInputs.any([](const Input& input) { return input.down(); }))
			{
				SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::MouseButtonDown);
			}
		}

		// マウスホイールの状態を更新
		{
			std::lock_guard lock{ m_wheelMutex };
			m_wheel = std::exchange(m_wheelInternal, Vec2{ 0, 0 });
		}
	}
	////////////////////////////////////////////////////////////////
	//
	//	getInputState
	//
	////////////////////////////////////////////////////////////////

	InputState& CMouse::getInputState(const uint32 index) noexcept
	{
		assert(index < Mouse::NumButtons);
		return m_mouseButton.states[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAllInput
	//
	////////////////////////////////////////////////////////////////

	const Array<Input>& CMouse::getAllInput() const noexcept
	{
		return m_mouseButton.allInputs;
	}

	////////////////////////////////////////////////////////////////
	//
	//	wheel
	//
	////////////////////////////////////////////////////////////////

	Vec2 CMouse::wheel() const noexcept
	{
		return m_wheel;
	}

	////////////////////////////////////////////////////////////////
	//
	//	onMouseButtonUpdated
	//
	////////////////////////////////////////////////////////////////

	void CMouse::onMouseButtonUpdated(const int32 index, const bool pressed)
	{
		std::lock_guard lock{ m_mouseButtonMutex };
		
		auto& internalState = m_internalMouseButtonStates[index];
		
		if (internalState == MouseButtonState::Released)
		{
			if (pressed)
			{
				internalState = MouseButtonState::PressedFirst;
			}
		}
		else if (internalState == MouseButtonState::PressedFirst)
		{
			if (pressed)
			{
				internalState = MouseButtonState::Pressed;
			}
			else
			{
				internalState = MouseButtonState::Tapped;
			}
		}
		else if (internalState == MouseButtonState::Pressed)
		{
			if (not pressed)
			{
				internalState = MouseButtonState::Released;
			}
		}
		else // Tapped
		{
			if (pressed)
			{
				internalState = MouseButtonState::PressedFirst;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	onScroll
	//
	////////////////////////////////////////////////////////////////

	void CMouse::onScroll(const double x, const double y)
	{
		std::lock_guard lock{ m_wheelMutex };
		m_wheelInternal.moveBy(x, y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPrimaryTouchPos
	//
	////////////////////////////////////////////////////////////////

	Optional<Point> CMouse::getPrimaryTouchPos()
	{
		return none;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void CMouse::OnMouseButtonUpdated(GLFWwindow*, const int button, const int action, int)
	{
		SIV3D_ENGINE(Mouse)->onMouseButtonUpdated(button, (action == GLFW_PRESS));
	}

	void CMouse::OnScroll(GLFWwindow*, double xOffset, double yOffset)
	{
		// 浮動小数点数誤差による端数を除去する
		xOffset = (std::round(100 * xOffset) / 100.0);
		yOffset = (std::round(100 * yOffset) / 100.0);
		SIV3D_ENGINE(Mouse)->onScroll(-xOffset, -yOffset);
	}
}
