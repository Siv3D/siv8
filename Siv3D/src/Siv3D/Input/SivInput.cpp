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

# include <array>
# include <Siv3D/IntToString.hpp>
# include <Siv3D/InputGroups.hpp>
# include <Siv3D/InputState.hpp>
# include <Siv3D/Keyboard/IKeyboard.hpp>
# include <Siv3D/Mouse/IMouse.hpp>
//# include <Siv3D/XInput/IXInput.hpp>
//# include <Siv3D/Gamepad/IGamepad.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array MouseButtonNames =
		{
			U"MouseL"_sv,
			U"MouseR"_sv,
			U"MouseM"_sv,
			U"MouseX1"_sv,
			U"MouseX2"_sv,
			U"MouseX3"_sv,
			U"MouseX4"_sv,
			U"MouseX5"_sv,
		};

		static constexpr std::array GamePadPODNames =
		{
			U"Up"_sv,
			U"Right"_sv,
			U"Down"_sv,
			U"Left"_sv,
		};

		static constexpr std::array XInputButtonNames =
		{
				U"DPad_Up"_sv,
				U"DPad_Down"_sv,
				U"DPad_Left"_sv,
				U"DPad_Right"_sv,
				U"Menu(Start)"_sv,
				U"View(Back)"_sv,
				U"LT"_sv,
				U"RT"_sv,
				U"LB"_sv,
				U"RB"_sv,
				U"Unknown"_sv,
				U"Unknown"_sv,
				U"A"_sv,
				U"B"_sv,
				U"X"_sv,
				U"Y"_sv,
		};

		[[nodiscard]]
		static InputState* GetInputState(const InputDeviceType device, const uint8 playerIndex, const uint8 code)
		{
			switch (device)
			{
			case InputDeviceType::Keyboard:
				return &SIV3D_ENGINE(Keyboard)->getInputState(code);
			case InputDeviceType::Mouse:
				return &SIV3D_ENGINE(Mouse)->getInputState(code);
			default:
				return nullptr;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	String Input::name() const
	{
		switch (m_device)
		{
		case InputDeviceType::Keyboard:
			return SIV3D_ENGINE(Keyboard)->getName(m_code);
		case InputDeviceType::Mouse:
			assert(m_code < MouseButtonNames.size());
			return String{ MouseButtonNames[m_code] };
		case InputDeviceType::Gamepad:
			{
				if (InRange<uint8>(m_code, 0x80, 0x83))
				{
					return (U"Gamepad-POV_" + GamePadPODNames[m_code - 0x80]);
				}
				else
				{
					return (U"Gamepad-Button" + ToString(m_code));
				}
			}
		case InputDeviceType::XInput:
			assert(m_code < XInputButtonNames.size());
			return (U"XInput-" + XInputButtonNames[m_code]);
		default:
			return{};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	down, pressed, up
	//
	////////////////////////////////////////////////////////////////

	bool Input::down() const
	{
		if (const auto pState = GetInputState(m_device, m_playerIndex, m_code))
		{
			return pState->down();
		}
		else
		{
			return false;
		}
	}

	bool Input::pressed() const
	{
		if (const auto pState = GetInputState(m_device, m_playerIndex, m_code))
		{
			return pState->pressed();
		}
		else
		{
			return false;
		}
	}

	bool Input::up() const
	{
		if (const auto pState = GetInputState(m_device, m_playerIndex, m_code))
		{
			return pState->up();
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearInput
	//
	////////////////////////////////////////////////////////////////

	void Input::clearInput() const
	{
		if (auto pState = GetInputState(m_device, m_playerIndex, m_code))
		{
			return pState->clearInput();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	isCleared
	//
	////////////////////////////////////////////////////////////////

	bool Input::isCleared() const
	{
		if (const auto pState = GetInputState(m_device, m_playerIndex, m_code))
		{
			return pState->isCleared();
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	pressedDuration
	//
	////////////////////////////////////////////////////////////////

	Duration Input::pressedDuration() const
	{
		if (const auto pState = GetInputState(m_device, m_playerIndex, m_code))
		{
			return pState->pressedDuration;
		}
		else
		{
			return Duration{ 0 };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator |
	//
	////////////////////////////////////////////////////////////////

	InputGroup Input::operator |(const Input other) const
	{
		return InputGroup{ *this, other };
	}

	InputGroup Input::operator |(const InputCombination& other) const
	{
		return InputGroup{ *this, other };
	}

	InputGroup Input::operator |(const InputGroup& other) const
	{
		return InputGroup{ *this, other };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Input& value)
	{
		Formatter(formatData, value.name());
	}

	////////////////////////////////////////////////////////////////
	//
	//	Serialize
	//
	////////////////////////////////////////////////////////////////

	String Input::Serialize(const Input& input)
	{
		if (input.playerIndex())
		{
			return U"({}, {}, {})"_fmt(FromEnum(input.deviceType()), input.code(), input.playerIndex());
		}
		else
		{
			return U"({}, {})"_fmt(FromEnum(input.deviceType()), input.code());
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	Deserialize
	//
	////////////////////////////////////////////////////////////////

	Input Input::Deserialize(const StringView s)
	{
		std::istringstream is{ s.toUTF8() };

		char unused;
		is >> unused;

		uint32 values[3];
		is
			>> values[0] >> unused
			>> values[1] >> unused;

		if (unused == ',')
		{
			is >> values[2] >> unused;
		}
		else
		{
			values[2] = 0;
		}

		return Input{ ToEnum<InputDeviceType>(static_cast<uint8>(values[0])),
			static_cast<uint8>(values[1]),
			static_cast<uint8>(values[2]) };
	}
}
