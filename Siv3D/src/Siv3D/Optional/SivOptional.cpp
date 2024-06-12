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

# include <ostream>
# include <Siv3D/Optional.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& os, const None_t&)
	{
		return (os << "none");
	}

	std::wostream& operator <<(std::wostream& os, const None_t&)
	{
		return (os << L"none");
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const None_t&)
	{
		static const std::u32string_view s = U"none";
		return os.write(s.data(), s.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const None_t&)
	{
		formatData.string.append(U"none"_sv);
	}
}
