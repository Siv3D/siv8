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

# include <iostream>
# include <Siv3D/Error.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/FormatData.hpp>

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

	std::ostream& operator <<(std::ostream& os, const Error& value)
	{
		return (os << '[' << value.type() << "] " << value.what().narrow());
	}

	std::wostream& operator <<(std::wostream& os, const Error& value)
	{
		return (os << L'[' << value.type() << L"] " << value.what().toWstr());
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const Error& value)
	{
		return os << (U'[' + value.type() + U"] " + value.what());
	}

	void Formatter(FormatData& formatData, const Error& value)
	{
		formatData.string.append(U"["_sv);
		formatData.string.append(value.type());
		formatData.string.append(U"] "_sv);
		formatData.string.append(value.what());
	}
}
