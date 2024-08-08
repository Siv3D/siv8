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

	////////////////////////////////////////////////////////////////
	//
	//	choice
	//
	////////////////////////////////////////////////////////////////

	inline String::value_type& String::choice()&
	{
		return choice(GetDefaultRNG());
	}

	inline const String::value_type& String::choice() const&
	{
		return choice(GetDefaultRNG());
	}

	inline String::value_type String::choice()&&
	{
		return choice(GetDefaultRNG());
	}

	String::value_type& String::choice(Concept::UniformRandomBitGenerator auto&& rbg)&
	{
		const size_t size = m_string.size();

		if (size == 0)
		{
			ThrowChoiceOutOfRange();
		}

		return m_string[RandomClosedOpen<size_t>(0, size, std::forward<decltype(rbg)>(rbg))];
	}

	const String::value_type& String::choice(Concept::UniformRandomBitGenerator auto&& rbg) const&
	{
		const size_t size = m_string.size();

		if (size == 0)
		{
			ThrowChoiceOutOfRange();
		}

		return m_string[RandomClosedOpen<size_t>(0, size, std::forward<decltype(rbg)>(rbg))];
	}

	String::value_type String::choice(Concept::UniformRandomBitGenerator auto&& rbg)&&
	{
		const size_t size = m_string.size();

		if (size == 0)
		{
			ThrowChoiceOutOfRange();
		}

		return m_string[RandomClosedOpen<size_t>(0, size, std::forward<decltype(rbg)>(rbg))];
	}

	////////////////////////////////////////////////////////////////
	//
	//	shuffle / shuffled
	//
	////////////////////////////////////////////////////////////////

	String& String::shuffle(Concept::UniformRandomBitGenerator auto&& rbg) & noexcept
	{
		Shuffle(m_string.begin(), m_string.end(), std::forward<decltype(rbg)>(rbg));
		return *this;
	}

	String String::shuffle(Concept::UniformRandomBitGenerator auto&& rbg) && noexcept
	{
		Shuffle(m_string.begin(), m_string.end(), std::forward<decltype(rbg)>(rbg));
		return std::move(*this);
	}

	String String::shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&
	{
		String result{ *this };
		result.shuffle(std::forward<decltype(rbg)>(rbg));
		return result;
	}

	String String::shuffled(Concept::UniformRandomBitGenerator auto&& rbg) && noexcept
	{
		return std::move(shuffle(std::forward<decltype(rbg)>(rbg)));
	}
}
