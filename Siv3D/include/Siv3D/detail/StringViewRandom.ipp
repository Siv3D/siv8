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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	choice
	//
	////////////////////////////////////////////////////////////////

	inline StringView::value_type StringView::choice() const
	{
		return choice(GetDefaultRNG());
	}

	String::value_type StringView::choice(Concept::UniformRandomBitGenerator auto&& urbg) const
	{
		const size_t size = m_view.size();

		if (size == 0)
		{
			ThrowChoiceOutOfRange();
		}

		return m_view[RandomClosedOpen<size_t>(0, size, urbg)];
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	shuffled
	//
	////////////////////////////////////////////////////////////////

	inline String StringView::shuffled() const
	{
		return shuffled(GetDefaultRNG());
	}

	String StringView::shuffled(Concept::UniformRandomBitGenerator auto&& urbg) const
	{
		String result{ m_view };
		result.shuffle(urbg);
		return result;
	}
}
