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

# include "CKeyboard.hpp"
# include <Siv3D/WindowState.hpp>
# include <Siv3D/UserAction.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Keyboard/FallbackNameList.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static String GetKeyName(const uint32 vk)
		{
			uint32 scanCode = ::MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);

			if (scanCode)
			{
				switch (vk)
				{
				case VK_PRIOR:
				case VK_NEXT:
				case VK_END:
				case VK_HOME:
				case VK_LEFT:
				case VK_UP:
				case VK_RIGHT:
				case VK_DOWN:
				case VK_INSERT:
				case VK_DELETE:
				case VK_DIVIDE:
				case VK_NUMLOCK:
					scanCode |= 0x100;
					break;
				}
			}

			wchar_t buffer[128]{};

			if (const int length = ::GetKeyNameTextW(static_cast<LONG>(scanCode << 16), buffer, _countof(buffer)))
			{
				return Unicode::FromWstring(std::wstring_view{ buffer, static_cast<size_t>(length) });
			}
			else if (FallbackKeyNames[vk])
			{
				return String{ FallbackKeyNames[vk] };
			}
			else
			{
				return U"{:#04x}"_fmt(vk);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CKeyboard::~CKeyboard()
	{
		LOG_SCOPED_DEBUG("CKeyboard::~CKeyboard()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CKeyboard::init()
	{
		LOG_SCOPED_DEBUG("CKeyboard::init()");

		m_hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());

		for (uint32 vk = 0; vk < m_names.size(); ++vk)
		{
			m_names[vk] = GetKeyName(vk);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CKeyboard::update()
	{
		if (not m_inputAttached)
		{
			if (not ::AttachThreadInput(::GetWindowThreadProcessId(m_hWnd, nullptr), ::GetCurrentThreadId(), TRUE))
			{
				LOG_ERROR("✖ AttachThreadInput() failed");
			}

			m_inputAttached = true;
		}

		m_allInputs.clear();

		const bool windowFocused = SIV3D_ENGINE(Window)->getState().focused;

		if (uint8 buf[Keyboard::NumKeys] = {}; ::GetKeyboardState(buf))
		{
			for (size_t i = 0; i < Keyboard::NumKeys; ++i)
			{
				const bool pressed = (windowFocused && ((buf[i] >> 7) & 0x1));

				auto& state = m_states[i];
				
				state.update(pressed);

				if ((state.pressed() || state.up())
					&& InRange<size_t>(i, VK_BACK, 0xEF))
				{
					m_allInputs.emplace_back(InputDeviceType::Keyboard, static_cast<uint8>(i));
				}
			}
		}

		if (windowFocused)
		{
			if (m_states[VK_ESCAPE].down())
			{
				SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::AnyKeyDown | UserAction::EscapeKeyDown);
			}
			else if(m_allInputs.any([](const Input& input) { return input.down(); }))
			{
				SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::AnyKeyDown);
			}

			for (uint32 mouseButtonIndex = VK_LBUTTON; mouseButtonIndex <= VK_XBUTTON2; ++mouseButtonIndex)
			{
				if (m_states[mouseButtonIndex].down())
				{
					SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::MouseButtonDown);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getInputState
	//
	////////////////////////////////////////////////////////////////

	InputState& CKeyboard::getInputState(const uint32 index) noexcept
	{
		assert(index < Keyboard::NumKeys);
		return m_states[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	getName
	//
	////////////////////////////////////////////////////////////////

	const String& CKeyboard::getName(const uint32 index) const noexcept
	{
		assert(index < Keyboard::NumKeys);
		return m_names[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAllInput
	//
	////////////////////////////////////////////////////////////////

	const Array<Input>& CKeyboard::getAllInput() const noexcept
	{
		return m_allInputs;
	}
}
