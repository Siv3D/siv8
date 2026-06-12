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

namespace
{
	constexpr FilePathView ValidBasic = U"../../Test/data/toml/valid_basic.toml";
	constexpr FilePathView ValidAllTypes = U"../../Test/data/toml/valid_all_types.toml";
	constexpr FilePathView ValidNested = U"../../Test/data/toml/valid_nested.toml";
	constexpr FilePathView ValidUnicode = U"../../Test/data/toml/valid_unicode.toml";
	constexpr FilePathView ValidUTF8BOM = U"../../Test/data/toml/valid_utf8_bom.toml";
	constexpr FilePathView ValidEmpty = U"../../Test/data/toml/valid_empty.toml";
	constexpr FilePathView ValidCommentsWhitespace = U"../../Test/data/toml/valid_comments_whitespace.toml";
	constexpr FilePathView ValidTypeMismatchFixture = U"../../Test/data/toml/valid_type_mismatch_fixture.toml";

	constexpr FilePathView InvalidUnclosedArray = U"../../Test/data/toml/invalid_unclosed_array.toml";
	constexpr FilePathView InvalidDuplicateKey = U"../../Test/data/toml/invalid_duplicate_key.toml";
	constexpr FilePathView InvalidDuplicateTable = U"../../Test/data/toml/invalid_duplicate_table.toml";
	constexpr FilePathView InvalidInvalidDate = U"../../Test/data/toml/invalid_invalid_date.toml";
	constexpr FilePathView InvalidInvalidNumber = U"../../Test/data/toml/invalid_invalid_number.toml";
	constexpr FilePathView InvalidNonUTF8Latin1 = U"../../Test/data/toml/invalid_non_utf8_latin1.toml";
	constexpr FilePathView Nonexistent = U"../../Test/data/toml/nonexistent.toml";
}

TEST_CASE("TOML.default")
{
	const TOML toml;

	CHECK(not toml);
	CHECK(toml.isInvalid());
	CHECK(not toml.isValid());
	CHECK(not toml.hasValue());
	CHECK_EQ(toml.getType(), TOMLValueType::Invalid);
	CHECK_EQ(toml.type(), TOMLValueType::Invalid);

	CHECK(not toml.isBool());
	CHECK(not toml.isInt());
	CHECK(not toml.isFloat());
	CHECK(not toml.isString());
	CHECK(not toml.isArray());
	CHECK(not toml.isTable());

	CHECK(toml.format().isEmpty());
	CHECK(toml.formatUTF8().empty());
}

TEST_CASE("TOML.Load.basic")
{
	const TOML toml = TOML::Load(ValidBasic);

	REQUIRE(toml);
	CHECK(toml.isTable());
	CHECK_EQ(toml.getType(), TOMLValueType::Table);
	CHECK_EQ(toml.size(), 5);

	CHECK(toml.contains(U"title"));
	CHECK(toml.contains(U"server"));
	CHECK(toml.contains(U"database"));
	CHECK(toml.contains(U"features"));
	CHECK(toml.contains(U"users"));
	CHECK(not toml.contains(U"missing"));

	CHECK_EQ(toml[U"title"].get<String>(), U"TOML test");

	const TOML server = toml[U"server"];
	REQUIRE(server);
	CHECK(server.isTable());
	CHECK_EQ(server[U"host"].get<String>(), U"localhost");
	CHECK_EQ(server[U"host"].getUTF8(), std::string{ "localhost" });
	CHECK_EQ(server[U"port"].get<int32>(), 8080);
	CHECK_EQ(server[U"enabled"].get<bool>(), true);

	const TOML database = toml[U"database"];
	REQUIRE(database);
	CHECK_EQ(database[U"user"].get<String>(), U"test_user");
	CHECK_EQ(database[U"password"].get<String>(), U"secret");
	CHECK_EQ(database[U"max_connections"].get<int32>(), 10);
	CHECK_EQ(database[U"timeout_seconds"].get<double>(), 30.5);

	const TOML features = toml[U"features"];
	REQUIRE(features);
	CHECK(features[U"items"].getArray<String>() == Array<String>{ U"login", U"search", U"export" });
	CHECK(features[U"retry_counts"].getArray<int32>() == Array<int32>{ 1, 2, 3 });

	const TOML users = toml[U"users"];
	REQUIRE(users);
	CHECK(users.isArray());
	CHECK_EQ(users.size(), 2);
	CHECK_EQ(users[0][U"name"].get<String>(), U"Alice");
	CHECK_EQ(users[0][U"admin"].get<bool>(), true);
	CHECK_EQ(users[1][U"name"].get<String>(), U"Bob");
	CHECK_EQ(users[1][U"admin"].get<bool>(), false);
}

TEST_CASE("TOML.Parse")
{
	{
		const TOML toml = TOML::Parse(U"title = \"Siv3D\"\nvalue = 42\n");

		REQUIRE(toml);
		CHECK(toml.isTable());
		CHECK_EQ(toml[U"title"].get<String>(), U"Siv3D");
		CHECK_EQ(toml[U"value"].get<int32>(), 42);
	}

	{
		const TOML toml = TOML::Parse(std::string{ "title = \"Siv3D\"\nvalue = 42\n" });

		REQUIRE(toml);
		CHECK(toml.isTable());
		CHECK_EQ(toml[U"title"].get<String>(), U"Siv3D");
		CHECK_EQ(toml[U"value"].get<int32>(), 42);
	}

	{
		const TOML toml = TOML::Parse(U"");

		REQUIRE(toml);
		CHECK(toml.isTable());
		CHECK(toml.isEmptyTable());
		CHECK_EQ(toml.size(), 0);
	}

	{
		const TOML toml = TOML::Parse(U"title = \"broken\"\narray = [1, 2, 3\n");

		CHECK(not toml);
		CHECK(toml.isInvalid());
	}
}

TEST_CASE("TOML.LoadResult.ParseResult")
{
	{
		const auto result = TOML::LoadResult(ValidBasic);
		REQUIRE(result);
	}

	{
		const auto result = TOML::ParseResult(U"title = \"Siv3D\"\nvalue = 42\n");
		REQUIRE(result);
	}

	{
		const auto result = TOML::LoadResult(Nonexistent);
		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK(not result.error().front().title.isEmpty());
	}

	{
		const auto result = TOML::ParseResult(U"title = \"broken\"\narray = [1, 2, 3\n");
		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK(not result.error().front().title.isEmpty());
	}
}

TEST_CASE("TOML.Load.invalid")
{
	constexpr std::array InvalidFiles
	{
		InvalidUnclosedArray,
		InvalidDuplicateKey,
		InvalidDuplicateTable,
		InvalidInvalidDate,
		InvalidInvalidNumber,
	};

	for (const auto& path : InvalidFiles)
	{
		{
			const TOML toml = TOML::Load(path);
			CHECK(not toml);
			CHECK(toml.isInvalid());
		}

		{
			const auto result = TOML::LoadResult(path);
			CHECK(not result);
			if (not result)
			{
				CHECK(not result.error().isEmpty());
			}
		}
	}

	{
		const TOML toml = TOML::Load(InvalidNonUTF8Latin1);
		CHECK(not toml);
		CHECK(toml.isInvalid());
	}

	{
		const auto result = TOML::LoadResult(InvalidNonUTF8Latin1);
		CHECK(not result);
	}
}

TEST_CASE("TOML.types")
{
	const TOML toml = TOML::Load(ValidAllTypes);
	REQUIRE(toml);

	CHECK(toml[U"bool_true"].isBool());
	CHECK(toml[U"bool_false"].isBool());
	CHECK_EQ(toml[U"bool_true"].get<bool>(), true);
	CHECK_EQ(toml[U"bool_false"].get<bool>(), false);

	CHECK(toml[U"int_zero"].isInt());
	CHECK(toml[U"int_positive"].isInt());
	CHECK(toml[U"int_negative"].isInt());
	CHECK(toml[U"int_with_underscores"].isInt());
	CHECK_EQ(toml[U"int_zero"].get<int32>(), 0);
	CHECK_EQ(toml[U"int_positive"].get<int32>(), 123456);
	CHECK_EQ(toml[U"int_negative"].get<int32>(), -98765);
	CHECK_EQ(toml[U"int_with_underscores"].get<int32>(), 1'000'000);

	CHECK(toml[U"float_plain"].isFloat());
	CHECK(toml[U"float_negative"].isFloat());
	CHECK(toml[U"float_exponent"].isFloat());
	CHECK(toml[U"float_inf"].isFloat());
	CHECK(toml[U"float_nan"].isFloat());
	CHECK(toml[U"float_plain"].get<double>() == doctest::Approx(3.14159));
	CHECK(toml[U"float_negative"].get<double>() == doctest::Approx(-0.25));
	CHECK(std::isinf(toml[U"float_inf"].get<double>()));
	CHECK(std::isnan(toml[U"float_nan"].get<double>()));

	CHECK(toml[U"string_basic"].isString());
	CHECK(toml[U"string_unicode"].isString());
	CHECK_EQ(toml[U"string_basic"].get<String>(), U"hello");
	CHECK_EQ(toml[U"string_unicode"].get<String>(), U"こんにちは Siv3D 🌏");

	CHECK(toml[U"local_date"].isLocalDate());
	CHECK_EQ(toml[U"local_date"].get<Date>(), Date{ 2026, 5, 4 });

	CHECK(toml[U"local_time"].isLocalTime());
	{
		const TOML::LocalTime time = toml[U"local_time"].getLocalTime();
		CHECK_EQ(time.hour, 12);
		CHECK_EQ(time.minute, 34);
		CHECK_EQ(time.second, 56);
		CHECK_EQ(time.millisecond, 789);
		CHECK_EQ(time.microsecond, 123);
		CHECK_EQ(time.nanosecond, 456);
	}

	CHECK(toml[U"local_datetime"].isLocalDateTime());
	{
		const TOML::LocalDateTime dateTime = toml[U"local_datetime"].getLocalDateTime();
		CHECK_EQ(dateTime.date, Date{ 2026, 5, 4 });
		CHECK_EQ(dateTime.time.hour, 12);
		CHECK_EQ(dateTime.time.minute, 34);
		CHECK_EQ(dateTime.time.second, 56);
		CHECK_EQ(dateTime.time.millisecond, 789);
	}

	CHECK(toml[U"offset_datetime_utc"].isOffsetDateTime());
	{
		const TOML::OffsetDateTime dateTime = toml[U"offset_datetime_utc"].getOffsetDateTime();
		CHECK_EQ(dateTime.localDateTime.date, Date{ 2026, 5, 4 });
		CHECK_EQ(dateTime.localDateTime.time.hour, 12);
		CHECK_EQ(dateTime.localDateTime.time.minute, 34);
		CHECK_EQ(dateTime.localDateTime.time.second, 56);
		CHECK_EQ(dateTime.offsetMinutes, 0);
	}

	CHECK(toml[U"offset_datetime_jst"].isOffsetDateTime());
	{
		const TOML::OffsetDateTime dateTime = toml[U"offset_datetime_jst"].getOffsetDateTime();
		CHECK_EQ(dateTime.localDateTime.date, Date{ 2026, 5, 4 });
		CHECK_EQ(dateTime.localDateTime.time.hour, 21);
		CHECK_EQ(dateTime.localDateTime.time.minute, 34);
		CHECK_EQ(dateTime.localDateTime.time.second, 56);
		CHECK_EQ(dateTime.offsetMinutes, 9 * 60);
	}
}

TEST_CASE("TOML.operator_at")
{
	const TOML toml = TOML::Load(ValidBasic);
	REQUIRE(toml);

	CHECK(toml[U"server"]);
	CHECK(toml[U"server"][U"host"]);
	CHECK_EQ(toml[U"server"][U"host"].get<String>(), U"localhost");

	CHECK(not toml[U"missing"]);
	CHECK(toml[U"missing"].isInvalid());
	CHECK(not toml[U"server"][U"missing"]);
	CHECK(not toml[U"server"][U"missing"][U"nested"]);
	CHECK(not toml[U"server"][0]);
	CHECK(not toml[U"users"][999]);

	CHECK_EQ(toml.at(U"server").at(U"host").get<String>(), U"localhost");
	CHECK_EQ(toml.at(U"users").at(0).at(U"name").get<String>(), U"Alice");

	CHECK_THROWS_AS((void)toml.at(U"missing"), Error);
	CHECK_THROWS_AS((void)toml[U"server"].at(0), Error);
	CHECK_THROWS_AS((void)toml[U"users"].at(999), Error);
	CHECK_THROWS_AS((void)toml[U"title"].at(U"missing"), Error);
}

TEST_CASE("TOML.getOpt_getOr_get")
{
	const TOML toml = TOML::Load(ValidTypeMismatchFixture);
	REQUIRE(toml);

	{
		const Optional<int32> opt = toml[U"as_int"].getOpt<int32>();
		REQUIRE(opt);
		CHECK_EQ(*opt, 123);
	}

	{
		const Optional<String> opt = toml[U"as_string_number"].getOpt<String>();
		REQUIRE(opt);
		CHECK_EQ(*opt, U"123");
	}

	CHECK(not toml[U"as_string_number"].getOpt<int32>());
	CHECK(not toml[U"as_float"].getOpt<int32>());
	CHECK(not toml[U"as_bool"].getOpt<int32>());
	CHECK(not toml[U"missing"].getOpt<int32>());

	CHECK_EQ(toml[U"as_string_number"].getOr<int32>(456), 456);
	CHECK_EQ(toml[U"missing"].getOr<String>(U"default"), U"default");

	CHECK_THROWS_AS((void)toml[U"as_string_number"].get<int32>(), Error);
	CHECK_THROWS_AS((void)toml[U"as_float"].get<int32>(), Error);
	CHECK_THROWS_AS((void)toml[U"missing"].get<int32>(), Error);
}

TEST_CASE("TOML.getParsed")
{
	const TOML toml = TOML::Load(ValidTypeMismatchFixture);
	REQUIRE(toml);

	{
		const Optional<int32> opt = toml[U"as_string_number"].getParsedOpt<int32>();
		REQUIRE(opt);
		CHECK_EQ(*opt, 123);
	}

	CHECK(not toml[U"as_int"].getParsedOpt<int32>());
	CHECK(not toml[U"missing"].getParsedOpt<int32>());

	CHECK_EQ(toml[U"as_string_number"].getParsed<int32>(), 123);
	CHECK_EQ(toml[U"as_int"].getParsedOr<int32>(456), 456);
	CHECK_THROWS_AS((void)toml[U"as_int"].getParsed<int32>(), Error);
}

TEST_CASE("TOML.getArray")
{
	const TOML toml = TOML::Load(ValidTypeMismatchFixture);
	REQUIRE(toml);

	CHECK(toml[U"as_array_ints"].getArray<int32>() == Array<int32>{ 1, 2, 3 });
	CHECK(not toml[U"as_array_mixed"].getArrayOpt<int32>());
	CHECK(not toml[U"as_int"].getArrayOpt<int32>());

	CHECK_THROWS_AS((void)toml[U"as_array_mixed"].getArray<int32>(), Error);
	CHECK_THROWS_AS((void)toml[U"as_int"].getArray<int32>(), Error);

	const TOML basic = TOML::Load(ValidBasic);
	REQUIRE(basic);

	const Array<TOML> users = basic[U"users"].getArray<TOML>();
	REQUIRE_EQ(users.size(), 2);
	CHECK_EQ(users[0][U"name"].get<String>(), U"Alice");
	CHECK_EQ(users[1][U"name"].get<String>(), U"Bob");
}

TEST_CASE("TOML.arrayView")
{
	const TOML toml = TOML::Load(ValidBasic);
	REQUIRE(toml);

	Array<String> items;
	for (const auto item : toml[U"features"][U"items"].arrayView())
	{
		items.push_back(item.get<String>());
	}
	CHECK(items == Array<String>{ U"login", U"search", U"export" });

	Array<String> userNames;
	for (const auto user : toml[U"users"].arrayView())
	{
		userNames.push_back(user[U"name"].get<String>());
	}
	CHECK(userNames == Array<String>{ U"Alice", U"Bob" });

	CHECK_THROWS_AS((void)toml[U"server"].arrayView(), Error);
	CHECK_THROWS_AS((void)toml[U"missing"].arrayView(), Error);
}

TEST_CASE("TOML.tableView")
{
	const TOML toml = TOML::Load(ValidBasic);
	REQUIRE(toml);

	Array<String> serverKeys;
	for (const auto member : toml[U"server"].tableView())
	{
		serverKeys.push_back(member.name);
	}
	CHECK(serverKeys == Array<String>{ U"host", U"port", U"enabled" });

	Array<String> rootKeys;
	for (const auto member : toml.tableView())
	{
		rootKeys.push_back(member.name);
	}
	CHECK(rootKeys == Array<String>{ U"title", U"server", U"database", U"features", U"users" });

	CHECK_THROWS_AS((void)toml[U"users"].tableView(), Error);
	CHECK_THROWS_AS((void)toml[U"missing"].tableView(), Error);
}

TEST_CASE("TOML.lifetime")
{
	const TOML server = TOML::Load(ValidBasic)[U"server"];

	REQUIRE(server);
	CHECK(server.isTable());
	CHECK_EQ(server[U"host"].get<String>(), U"localhost");
	CHECK_EQ(server[U"port"].get<int32>(), 8080);

	const TOML firstUser = TOML::Load(ValidBasic)[U"users"][0];

	REQUIRE(firstUser);
	CHECK(firstUser.isTable());
	CHECK_EQ(firstUser[U"name"].get<String>(), U"Alice");
	CHECK_EQ(firstUser[U"admin"].get<bool>(), true);
}

TEST_CASE("TOML.nested")
{
	const TOML toml = TOML::Load(ValidNested);
	REQUIRE(toml);

	CHECK_EQ(toml[U"window"][U"title"].get<String>(), U"Game");
	CHECK(toml[U"window"][U"size"].getArray<int32>() == Array<int32>{ 1280, 720 });
	CHECK_EQ(toml[U"window"][U"fullscreen"].get<bool>(), false);

	CHECK_EQ(toml[U"graphics"][U"vsync"].get<bool>(), true);
	CHECK(toml[U"graphics"][U"clear_color"].isTable());
	CHECK(toml[U"graphics"][U"clear_color"][U"r"].get<double>() == doctest::Approx(0.1));
	CHECK(toml[U"graphics"][U"clear_color"][U"g"].get<double>() == doctest::Approx(0.2));
	CHECK(toml[U"graphics"][U"clear_color"][U"b"].get<double>() == doctest::Approx(0.3));
	CHECK(toml[U"graphics"][U"clear_color"][U"a"].get<double>() == doctest::Approx(1.0));

	CHECK_EQ(toml[U"stages"].size(), 2);
	CHECK_EQ(toml[U"stages"][0][U"name"].get<String>(), U"forest");
	CHECK_EQ(toml[U"stages"][0][U"difficulty"].get<int32>(), 1);
	CHECK_EQ(toml[U"stages"][0][U"spawn_points"].size(), 3);
	CHECK(toml[U"stages"][0][U"spawn_points"][0].getArray<int32>() == Array<int32>{ 0, 0 });

	CHECK_EQ(toml[U"stages"][1][U"name"].get<String>(), U"desert");
	CHECK_EQ(toml[U"stages"][1][U"difficulty"].get<int32>(), 3);
	CHECK_EQ(toml[U"stages"][1][U"spawn_points"].size(), 2);
	CHECK(toml[U"stages"][1][U"spawn_points"][1].getArray<int32>() == Array<int32>{ 200, 160 });
}

TEST_CASE("TOML.unicode_bom_empty")
{
	{
		const TOML toml = TOML::Load(ValidUnicode);
		REQUIRE(toml);
		CHECK_EQ(toml[U"title"].get<String>(), U"日本語タイトル");
		CHECK_EQ(toml[U"description"].get<String>(), U"Unicode / 絵文字 / 記号のテスト: あいうえお、αβγ、🎮");
		CHECK_EQ(toml[U"names"][U"player"].get<String>(), U"太郎");
		CHECK_EQ(toml[U"messages"][U"game_over"].get<String>(), U"ゲームオーバー");
	}

	{
		const TOML toml = TOML::Load(ValidUTF8BOM);
		REQUIRE(toml);
		CHECK_EQ(toml[U"title"].get<String>(), U"UTF-8 BOM test");
		CHECK_EQ(toml[U"value"].get<int32>(), 42);
		CHECK_EQ(toml[U"message"].get<String>(), U"BOM 付き UTF-8");
	}

	{
		const TOML toml = TOML::Load(ValidEmpty);
		REQUIRE(toml);
		CHECK(toml.isTable());
		CHECK(toml.isEmptyTable());
		CHECK_EQ(toml.size(), 0);
	}

	{
		const TOML toml = TOML::Load(ValidCommentsWhitespace);
		REQUIRE(toml);
		CHECK(toml[U"empty_table"].isTable());
		CHECK(toml[U"empty_table"].isEmptyTable());
		CHECK_EQ(toml[U"config"][U"value"].get<int32>(), 42);
		CHECK(toml[U"config"][U"array"].getArray<int32>() == Array<int32>{ 1, 2, 3 });
	}
}

TEST_CASE("TOML.format")
{
	const TOML toml = TOML::Load(ValidBasic);
	REQUIRE(toml);

	const String formatted = toml.format();
	const std::string formattedUTF8 = toml.formatUTF8();

	CHECK(not formatted.isEmpty());
	CHECK(not formattedUTF8.empty());

	CHECK(formatted.contains(U"title"));
	CHECK(formatted.contains(U"server"));
	CHECK(formatted.contains(U"users"));
}
