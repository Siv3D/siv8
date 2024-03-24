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

[[nodiscard]]
static std::string CreateTestData()
{
	std::string s;

	for (auto ch : std::string{ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" })
	{
		s.append(98, ch);
		s.append("\r\n");
	}

	return s;
}

TEST_CASE("MemoryMappedFileView")
{
	const std::string testData = CreateTestData();
	{
		MemoryMappedFileView file{ U"../../Test/data/text/utf8_chars.txt" };
		CHECK(file.isOpen());
		CHECK(static_cast<bool>(file));
		CHECK_EQ(file.size(), 6200);

		{
			const auto mapped = file.mapAll();
			CHECK(mapped.data != nullptr);
			CHECK_EQ(mapped.size, 6200);

			std::string s(static_cast<const char*>(mapped.data), mapped.size);
			CHECK_EQ(s, testData);
		
			CHECK_FALSE(file.mapAll());
			CHECK_FALSE(file.map(0, 0));
			CHECK_FALSE(file.map(0, 100));
			file.unmap();
		}

		{
			const auto mapped = file.map(0, 0);
			CHECK(mapped.data != nullptr);
			CHECK_EQ(mapped.size, 0);

			CHECK_FALSE(file.mapAll());
			CHECK_FALSE(file.map(0, 0));
			CHECK_FALSE(file.map(0, 100));
			file.unmap();
		}

		{
			const auto mapped = file.map(0, 200);
			CHECK(mapped.data != nullptr);
			CHECK_EQ(mapped.size, 200);

			std::string s(static_cast<const char*>(mapped.data), mapped.size);
			CHECK_EQ(s, testData.substr(0, 200));

			CHECK_FALSE(file.mapAll());
			CHECK_FALSE(file.map(0, 0));
			CHECK_FALSE(file.map(0, 100));
			file.unmap();
		}

		{
			const auto mapped = file.map(100, 200);
			CHECK(mapped.data != nullptr);
			CHECK_EQ(mapped.size, 200);

			std::string s(static_cast<const char*>(mapped.data), mapped.size);
			CHECK_EQ(s, testData.substr(100, 200));

			CHECK_FALSE(file.mapAll());
			CHECK_FALSE(file.map(0, 0));
			CHECK_FALSE(file.map(0, 100));
			file.unmap();
		}
	}
}
