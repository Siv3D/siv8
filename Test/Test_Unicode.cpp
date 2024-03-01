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

# include "Siv3DTest.hpp"

TEST_CASE("Unicode.ValidateAscii")
{
	{
		const std::string s = "abcdef";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), true);
	}

	{
		const std::string s = "abcあdef";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateAscii(s).error(), 3);
	}

	{
		std::string s = "\x80" "abcdef";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateAscii(s).error(), 0);
	}

	{
		std::string s = "abc" "\x80" "def";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateAscii(s).error(), 3);
	}

	{
		const std::string s = "abcdef\x80";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateAscii(s).error(), 6);
	}

}

TEST_CASE("Unicode.ValidateUTF8")
{
	{
		const std::string s = "abcdef";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), true);
	}

	{
		const std::string s = "abcあdef";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), true);
	}

	{
		std::string s = "\x80" "abcdef";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateUTF8(s).error(), 0);
	}

	{
		std::string s = "abc" "\x80" "def";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateUTF8(s).error(), 3);
	}

	{
		const std::string s = "abcあdef\x80";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateUTF8(s).error(), 9);
	}

	{
		const std::string s = "あいうえお";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), true);
	}
}

