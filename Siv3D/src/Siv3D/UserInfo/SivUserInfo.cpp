//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/UserInfo.hpp>
# include <Siv3D/FormatLiteral.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String UserInfo::format() const
	{
		String output;
		output += U"computerName: {}\n"_fmt(computerName);
		output += U"userName: {}\n"_fmt(userName);
		output += U"fullUserName: {}\n"_fmt(fullUserName);
		output += U"defaultLocale: {}\n"_fmt(defaultLocale);
		output += U"defaultLanguage: {}"_fmt(defaultLanguage);

		return output;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const UserInfo& value)
	{
		formatData.string.append(value.format());
	}
}
