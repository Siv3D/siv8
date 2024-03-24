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

TEST_CASE("Base64Value")
{
	const Array<std::pair<std::string, std::string>> testCases = {
		{ "", "" },
		{ "f", "Zg==" },
		{ "fo", "Zm8=" },
		{ "foo", "Zm9v" },
		{ "foob", "Zm9vYg==" },
		{ "fooba", "Zm9vYmE=" },
		{ "foobar", "Zm9vYmFy" },
		{ "Hello, Siv3D!", "SGVsbG8sIFNpdjNEIQ==" },
		{ "Hello, Siv3D! こんにちは、Siv3D！", "SGVsbG8sIFNpdjNEISDjgZPjgpPjgavjgaHjga/jgIFTaXYzRO+8gQ==" },
	};

	for (const auto& [s, base64] : testCases)
	{
		const Base64Value base64Value{ base64 };
		CHECK_EQ(base64Value.getBase64(), base64);

		std::string decoded(base64Value.getBinarySize(), '\0');
		base64Value.decodeToMemory(decoded.data());
		CHECK_EQ(s, decoded);
	}
}
