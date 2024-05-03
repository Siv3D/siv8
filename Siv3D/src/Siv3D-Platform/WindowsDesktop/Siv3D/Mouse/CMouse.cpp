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

# include "CMouse.hpp"
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Keyboard/IKeyboard.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array<uint8, 6> ButtonVKs{ 1, 2, 4, 5, 6 };
	}

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

		m_hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());

		if (::GetSystemMetrics(SM_DIGITIZER) & NID_MULTI_INPUT)
		{
			m_touchAvailable = true;
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
			const auto keyboard = SIV3D_ENGINE(Keyboard);

			for (size_t i = 0; i < ButtonVKs.size(); ++i)
			{
				bool pressed = keyboard->getInputState(ButtonVKs[i]).pressed();

				if (i == 0)
				{
					std::lock_guard lock{ m_touch.mutex };
					pressed |= m_touch.primaryTouchScreenPos.has_value();
				}

				m_mouseButton.states[i].update(pressed);
			}
		}

		{
			m_mouseButton.allInputs.clear();

			for (uint32 i = 0; i < Mouse::NumButtons; ++i)
			{
				const auto& state = m_mouseButton.states[i];

				if (state.pressed() || state.up())
				{
					m_mouseButton.allInputs.emplace_back(InputDevice::Mouse, static_cast<uint8>(i));
				}
			}
		}

		// マウスホイールの状態を更新
		{
			std::lock_guard lock{ m_wheel.mutex };
			m_wheel.wheel = std::exchange(m_wheel.wheelInternal, Vec2{ 0, 0 });
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
		return m_wheel.wheel;
	}

	////////////////////////////////////////////////////////////////
	//
	//	onMouseButtonUpdated
	//
	////////////////////////////////////////////////////////////////

	void CMouse::onMouseButtonUpdated(const int32, const bool) {}

	////////////////////////////////////////////////////////////////
	//
	//	onScroll
	//
	////////////////////////////////////////////////////////////////

	void CMouse::onScroll(const double x, const double y)
	{
		std::lock_guard lock{ m_wheel.mutex };
		m_wheel.wheelInternal.moveBy(x, y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPrimaryTouchPos
	//
	////////////////////////////////////////////////////////////////

	Optional<Point> CMouse::getPrimaryTouchPos()
	{
		std::lock_guard lock{ m_touch.mutex };
		return m_touch.primaryTouchScreenPos;
	}

	////////////////////////////////////////////////////////////////
	//
	//	onTouchInput
	//
	////////////////////////////////////////////////////////////////

	void CMouse::onTouchInput(const Array<TOUCHINPUT>& touchInputs)
	{
		std::lock_guard lock{ m_touch.mutex };

		for (size_t i = 0; i < touchInputs.size(); ++i)
		{
			if ((not m_touch.currentPrimaryTouchID)
				&& (not (touchInputs[i].dwFlags & TOUCHEVENTF_UP)))
			{
				m_touch.currentPrimaryTouchID = touchInputs[i].dwID;
				break;
			}

			if (touchInputs[0].dwFlags & TOUCHEVENTF_UP)
			{
				m_touch.currentPrimaryTouchID.reset();
			}
		}

		m_touch.primaryTouchScreenPos.reset();

		if (m_touch.currentPrimaryTouchID)
		{
			for (const auto& touchInput : touchInputs)
			{
				if (touchInput.dwID == m_touch.currentPrimaryTouchID)
				{
					const POINT screenPos{ (touchInput.x / 100), (touchInput.y / 100) };
					m_touch.primaryTouchScreenPos.emplace(screenPos.x, screenPos.y);
				}
			}
		}
	}
}
