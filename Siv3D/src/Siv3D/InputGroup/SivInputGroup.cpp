﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/InputGroups.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	String InputGroup::name() const
	{
		return (m_inputs.join(U"|") + m_inputCombinations.join(U"|"));
	}

	////////////////////////////////////////////////////////////////
	//
	//	down, pressed, up
	//
	////////////////////////////////////////////////////////////////

	bool InputGroup::down() const
	{
		return ((m_inputs.any([](const Input& i) { return i.down(); })
			|| m_inputCombinations.any([](const InputCombination& c) { return c.down(); })));
	}

	bool InputGroup::pressed() const
	{
		return ((m_inputs.any([](const Input& i) { return i.pressed(); })
			|| m_inputCombinations.any([](const InputCombination& c) { return c.pressed(); })));
	}

	bool InputGroup::up() const
	{
		return ((m_inputs.any([](const Input& i) { return i.up(); })
			|| m_inputCombinations.any([](const InputCombination& c) { return c.up(); })));
	}

	////////////////////////////////////////////////////////////////
	//
	//	preseedDuration
	//
	////////////////////////////////////////////////////////////////

	Duration InputGroup::pressedDuration() const
	{
		Duration duration{ 0 };

		for (const auto& input : m_inputs)
		{
			duration = Max(input.pressedDuration(), duration);
		}

		for (const auto& inputCombination : m_inputCombinations)
		{
			duration = Max(inputCombination.pressedDuration(), duration);
		}

		return duration;
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_pressed
	//
	////////////////////////////////////////////////////////////////

	int32 InputGroup::num_pressed() const
	{
		int32 num = 0;

		for (const auto& input : m_inputs)
		{
			num += input.pressed();
		}

		for (const auto& inputCombination : m_inputCombinations)
		{
			num += inputCombination.pressed();
		}

		return num;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator |
	//
	////////////////////////////////////////////////////////////////

	InputGroup InputGroup::operator |(const Input other) const noexcept
	{
		return InputGroup{ *this, other };
	}

	InputGroup InputGroup::operator |(const InputCombination& other) const noexcept
	{
		return InputGroup{ *this, other };
	}

	InputGroup InputGroup::operator |(const InputGroup& other) const noexcept
	{
		return InputGroup{ *this, other };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ==
	//
	////////////////////////////////////////////////////////////////

	bool operator ==(const InputGroup& lhs, const InputGroup& rhs)
	{
		return ((lhs.m_inputs == rhs.m_inputs)
			 && (lhs.m_inputCombinations == rhs.m_inputCombinations));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const InputGroup& value)
	{
		Formatter(formatData, value.name());
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void InputGroup::append() {}

	void InputGroup::append(const Input other)
	{
		(m_inputs << other).sort_and_unique();
	}

	void InputGroup::append(const InputCombination& other)
	{
		(m_inputCombinations << other).sort_and_unique();
	}

	void InputGroup::append(const InputGroup& other)
	{
		m_inputs.append(other.m_inputs).sort_and_unique();
		m_inputCombinations.append(other.m_inputCombinations).sort_and_unique();
	}
}
