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
	constexpr Input::Input(const InputDevice device, const uint8 code, const uint8 playerIndex) noexcept
		: m_device{ device }
		, m_code{ code }
		, m_playerIndex{ playerIndex } {}

	constexpr InputDevice Input::device() const noexcept
	{
		return m_device;
	}

	constexpr uint8 Input::code() const noexcept
	{
		return m_code;
	}

	constexpr uint8 Input::playerIndex() const noexcept
	{
		return m_playerIndex;
	}

	constexpr uint32 Input::asUint32() const noexcept
	{
		return std::bit_cast<uint32>(*this);
	}

	constexpr InputCombination Input::operator +(const Input other) const noexcept
	{
		return{ *this, other };
	}
}
