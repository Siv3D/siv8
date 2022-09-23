//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Error.hpp>

namespace s3d
{
	Error::Error(const char32* message)
		: Error{ StringView{ message } } {}

	Error::Error(const StringView message)
	{
		m_data.message.assign(message.begin(), message.end());
	}

	StringView Error::what() const noexcept
	{
		if (const size_t length = m_data.message.size())
		{
			return{ m_data.message.data(), length };
		}
		else
		{
			return{};
		}
	}
}
