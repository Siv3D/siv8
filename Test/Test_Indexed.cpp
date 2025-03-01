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

# include "Siv3DTest.hpp"

TEST_CASE("Indexed")
{
	{
		std::vector<int32> v = { 0, 10, 20 };

		for (auto [i, elem] : Indexed(v))
		{
			static_assert(std::is_same_v<decltype(i), int32>);
			static_assert(std::is_same_v<decltype(elem), int32&>);
			CHECK(i == (elem / 10));
		}
	}

	{
		std::vector<std::string> v = { "a", "b", "c" };

		for (auto [i, elem] : Indexed(v))
		{
			static_assert(std::is_same_v<decltype(i), int32>);
			static_assert(std::is_same_v<decltype(elem), std::string&>);
			CHECK(i == (elem[0] - 'a'));
		}
	}

	{
		const std::vector<int32> v = { 0, 10, 20 };

		for (auto [i, elem] : Indexed(v))
		{
			static_assert(std::is_same_v<decltype(i), int32>);
			static_assert(std::is_same_v<decltype(elem), const int32&>);
			CHECK(i == (elem / 10));
		}
	}

	{
		const std::vector<std::string> v = { "a", "b", "c" };

		for (auto [i, elem] : Indexed(v))
		{
			static_assert(std::is_same_v<decltype(i), int32>);
			static_assert(std::is_same_v<decltype(elem), const std::string&>);
			CHECK(i == (elem[0] - 'a'));
		}
	}

	{
		const std::vector<int32> v = { 0, 10, 20 };

		for (auto [i, elem] : Indexed<uint16>(v))
		{
			static_assert(std::is_same_v<decltype(i), uint16>);
			static_assert(std::is_same_v<decltype(elem), const int32&>);
			CHECK(i == (elem / 10));
		}
	}

	{
		const std::vector<int32> v = { 0, 10, 20 };

		for (auto [i, elem] : Indexed<size_t>(v))
		{
			static_assert(std::is_same_v<decltype(i), size_t>);
			static_assert(std::is_same_v<decltype(elem), const int32&>);
			CHECK(i == (elem / 10));
		}
	}
}
