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
# include <Siv3D/StringView.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Hash.hpp>

namespace s3d
{
	std::string StringView::narrow() const
	{
		return Unicode::Narrow(*this);
	}

	std::wstring StringView::toWstr() const
	{
		return Unicode::ToWstring(*this);
	}

	std::string StringView::toUTF8() const
	{
		return Unicode::ToUTF8(*this);
	}

	uint64 StringView::hash() const noexcept
	{
		return Hash(m_view.data(), size_bytes());
	}

	std::ostream& operator <<(std::ostream& os, const StringView& value)
	{
		return (os << value.narrow());
	}

	std::wostream& operator <<(std::wostream& os, const StringView& value)
	{
		return (os << value.toWstr());
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const StringView& value)
	{
		return os.write(value.data(), value.size());
	}
}
