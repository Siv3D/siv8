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
		CHECK_EQ(Base64Value::EncodeFromUTF8(s).getBase64(), base64);

		std::string decoded(base64Value.getMaxBinarySize(), '\0');
		
		const auto result = base64Value.decodeToMemory(decoded.data());
		CHECK(result);

		if (result)
		{
			decoded.resize(result.value());
		}

		CHECK_EQ(s, decoded);
	}

	for (const auto& [s, base64] : testCases)
	{
		CHECK_EQ(s, Base64Value{ base64 }.decodeToUTF8());
	}

	for (const auto& [s, base64] : testCases)
	{
		CHECK_EQ(Unicode::FromUTF8(s), Base64Value{ Unicode::FromUTF8(base64) }.decodeToString());
	}
}

TEST_CASE("Base64Value.encodeSelfReference")
{
	for (const size_t length : { 1, 2, 3, 7, 15, 16, 22, 23, 24, 31, 32, 63, 64, 65, 127, 128, 129, 255, 256, 257, 1024 })
	{
		CAPTURE(length);
		std::string original(length, '\0');
		for (size_t i = 0; i < length; ++i)
		{
			original[i] = static_cast<char>('A' + (i % 26));
		}

		for (const bool spareCapacity : { false, true })
		{
			CAPTURE(spareCapacity);
			for (const auto [offset, count] : {
				std::pair<size_t, size_t>{ 0, length },
				std::pair<size_t, size_t>{ 0, length / 2 },
				std::pair<size_t, size_t>{ length / 3, length / 2 },
				std::pair<size_t, size_t>{ length - 1, 1 } })
			{
				CAPTURE(offset);
				CAPTURE(count);
				const std::string expectedInput = original.substr(offset, count);
				const std::string expected = Base64Value::EncodeFromUTF8(expectedInput).getBase64();

				for (int method = 0; method < 3; ++method)
				{
					CAPTURE(method);
					std::string storage = original;
					if (spareCapacity)
					{
						storage.reserve(4096);
					}
					Base64Value value{ std::move(storage) };
					const std::string_view input{ value.getBase64().data() + offset, count };

					if (method == 0)
					{
						value.encodeFromUTF8(input);
					}
					else if (method == 1)
					{
						value.encodeFromMemory(input.data(), input.size());
					}
					else
					{
						value.encodeFromMemory(std::as_bytes(std::span{ input.data(), input.size() }));
					}

					CHECK(value.getBase64() == expected);
					CHECK(value.decodeToUTF8() == expectedInput);
				}
			}
		}
	}
}

TEST_CASE("Base64Value.encodeTerminator")
{
	for (const std::string& original : { std::string{}, std::string{ "A" }, std::string(1024, 'B') })
	{
		CAPTURE(original.size());
		Base64Value value{ original };
		value.encodeFromMemory(value.getBase64().data() + value.getBase64().size(), 1);
		CHECK(value.getBase64() == "AA==");

		Base64Value includingTerminator{ original };
		std::string expectedInput = original;
		expectedInput.push_back('\0');
		const std::string expected = Base64Value::EncodeFromUTF8(expectedInput).getBase64();
		includingTerminator.encodeFromMemory(includingTerminator.getBase64().data(), includingTerminator.getBase64().size() + 1);
		CHECK(includingTerminator.getBase64() == expected);
		CHECK(includingTerminator.decodeToUTF8() == expectedInput);
	}
}

TEST_CASE("Base64Value.encodeEmptyAndNull")
{
	Base64Value value{ "AAAA" };
	value.encodeFromMemory(nullptr, 4);
	CHECK(value.isEmpty());

	value = Base64Value{ "AAAA" };
	value.encodeFromMemory(value.getBase64().data() + value.getBase64().size(), 0);
	CHECK(value.isEmpty());

	value = Base64Value{ "AAAA" };
	value.encodeFromMemory(std::span<const Byte>{});
	CHECK(value.isEmpty());

	value = Base64Value{ "AAAA" };
	value.encodeFromUTF8(std::string_view{});
	CHECK(value.isEmpty());
}

# if SIV3D_RUN_BENCHMARK

TEST_CASE("Base64Value.Benchmark")
{
	const ScopedLogSilencer logSilencer;
	
	{
		Bench{}.title("Base64 encode - windmill.png").run("Base64Value::EncodeFromFile", [&]() { doNotOptimizeAway(Base64Value::EncodeFromFile(U"example/windmill.png")); });
		Bench{}.title("Base64 encode - windmill.png").run("Base64Value::EncodeFromBlob", [&]() { doNotOptimizeAway(Base64Value::EncodeFromBlob(Blob{ U"example/windmill.png" })); });
	}
}

# endif
