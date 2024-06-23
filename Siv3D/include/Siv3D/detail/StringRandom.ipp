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
