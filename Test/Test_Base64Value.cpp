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

TEST_CASE("Base64Value.decodeToString.Success")
{
	const Array<String> inputs = {
		U"", U"f", U"foobar", U"こんにちは\U0001F600",
		String{ U"A\0B\0", 4 },
		U"\x7F\x80\x7FF\x800\xD7FF\xE000\xFDD0\xFEFF\xFFFF\U00010000\U0010FFFF",
	};

	String dst = U"previous contents";
	for (const auto& input : inputs)
	{
		const auto encoded = Base64Value::EncodeFromUTF8(Unicode::ToUTF8(input));
		const auto result = encoded.decodeToString(dst);
		REQUIRE(result);
		CHECK_EQ(*result, input.size());
		CHECK_EQ(dst, input);
		CHECK_EQ(encoded.decodeToString(), input);
	}

	for (const size_t length : { 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 129, 4096 })
	{
		CAPTURE(length);
		const String input(length, U'\U0001F600');
		const auto result = Base64Value::EncodeFromUTF8(Unicode::ToUTF8(input)).decodeToString(dst);
		REQUIRE(result);
		CHECK_EQ(*result, length);
		CHECK_EQ(dst, input);
	}

	const auto whitespace = Base64Value{ " \tZg==\r\n" }.decodeToString(dst);
	REQUIRE(whitespace);
	CHECK_EQ(*whitespace, 1u);
	CHECK_EQ(dst, U"f");

	const auto empty = Base64Value{ " \t\r\n" }.decodeToString(dst);
	REQUIRE(empty);
	CHECK_EQ(*empty, 0u);
	CHECK(dst.isEmpty());
}

TEST_CASE("Base64Value.decodeToString.InvalidBase64")
{
	const Array<std::pair<std::string, size_t>> inputs = {
		{ "!AAA", 0 }, { "AAAA!AAA", 4 }, { "AAAA!", 4 },
		{ "A", 1 }, { "AAAAA", 5 }, { " \tAAAA\n!AAA", 7 },
		// Base64 errors take precedence over errors in the decoded UTF-8.
		// Padding followed by non-whitespace is reported at the first '='.
		{ "/w==!", 2 },
	};
	for (const auto& [input, position] : inputs)
	{
		CAPTURE(input);
		String dst = U"previous contents";
		const Base64Value encoded{ input };
		const auto result = encoded.decodeToString(dst);
		REQUIRE_FALSE(result);
		CHECK(result.error().code == Base64Value::DecodeError::Code::InvalidBase64);
		CHECK_EQ(result.error().position, position);
		CHECK(dst.isEmpty());
		CHECK(encoded.decodeToString().isEmpty());
	}
}

TEST_CASE("Base64Value.decodeToString.InvalidUTF8")
{
	String dst = U"previous contents";
	const auto singleByte = Base64Value{ "/w==" }.decodeToString(dst);
	REQUIRE_FALSE(singleByte);
	CHECK(singleByte.error().code == Base64Value::DecodeError::Code::InvalidUTF8);
	CHECK_EQ(singleByte.error().position, 0u);
	CHECK(dst.isEmpty());

	const Array<std::string> invalidSequences = {
		"\xFF", "\x80", "\xC0\xAF", "\xE0\x80\x80", "\xED\xA0\x80",
		"\xF0\x80\x80\x80", "\xF4\x90\x80\x80", "\xF5\x80\x80\x80",
		"\xC2", "\xE2\x82", "\xF0\x9F\x98", "\xE2\x28\xA1", "\xC2" "A",
	};
	for (const auto& invalid : invalidSequences)
	{
		for (const size_t padding : { 0, 1, 12, 13, 14, 28, 29, 30, 60, 61, 62, 124, 125, 126, 255 })
		{
			// A multibyte prefix distinguishes byte offsets from character offsets.
			const std::string prefix = (std::string(padding, 'A') + "あ");
			for (const std::string_view suffix : { "", "B" })
			{
				CAPTURE(padding);
				CAPTURE(suffix.size());
				const std::string bytes = (prefix + invalid + std::string{ suffix });
				const auto encoded = Base64Value::EncodeFromUTF8(bytes);
				dst = U"previous contents";
				const auto result = encoded.decodeToString(dst);
				REQUIRE_FALSE(result);
				CHECK(result.error().code == Base64Value::DecodeError::Code::InvalidUTF8);
				CHECK_EQ(result.error().position, prefix.size());
				CHECK(dst.isEmpty());
				CHECK(encoded.decodeToString().isEmpty());
			}
		}
	}
}

TEST_CASE("Base64Value.decodeToString.ReuseAfterError")
{
	String dst(4096, U'X');
	for (const Base64Value& encoded : { Base64Value{ "!" }, Base64Value{ "/w==" } })
	{
		REQUIRE_FALSE(encoded.decodeToString(dst));
		CHECK(dst.isEmpty());

		const auto success = Base64Value{ "Zm9v" }.decodeToString(dst);
		REQUIRE(success);
		CHECK_EQ(*success, 3u);
		CHECK_EQ(dst, U"foo");

		const auto empty = Base64Value{}.decodeToString(dst);
		REQUIRE(empty);
		CHECK_EQ(*empty, 0u);
		CHECK(dst.isEmpty());
	}
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
