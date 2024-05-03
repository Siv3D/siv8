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
	template <class ...Args>
	InputGroup::InputGroup(const Args&... args)
	{
		append(args...);
	}

	inline const Array<Input>& InputGroup::inputs() const
	{
		return m_inputs;
	}

	inline const Array<InputCombination>& InputGroup::inputCombinations() const
	{
		return m_inputCombinations;
	}

	template <class Type, class ...Args>
	void InputGroup::append(const Type& input, const Args&... args)
	{
		append(input);
		append(args...);
	}
}
