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
	constexpr FilePathView ValidBasic = U"../../Test/data/ini/valid_basic.ini";
	constexpr FilePathView ValidCommentsWhitespace = U"../../Test/data/ini/valid_comments_whitespace.ini";
	constexpr FilePathView ValidDuplicate = U"../../Test/data/ini/valid_duplicate.ini";
	constexpr FilePathView ValidLoneCR = U"../../Test/data/ini/valid_lone_cr.ini";
	constexpr FilePathView ValidUnicode = U"../../Test/data/ini/valid_unicode.ini";
	constexpr FilePathView ValidUTF8BOM = U"../../Test/data/ini/valid_utf8_bom.ini";
	constexpr FilePathView ValidEmpty = U"../../Test/data/ini/valid_empty.ini";

	constexpr FilePathView InvalidMissingClosingBracket = U"../../Test/data/ini/invalid_missing_closing_bracket.ini";
	constexpr FilePathView InvalidEmptySectionName = U"../../Test/data/ini/invalid_empty_section_name.ini";
	constexpr FilePathView InvalidUnexpectedAfterSection = U"../../Test/data/ini/invalid_unexpected_after_section.ini";
	constexpr FilePathView InvalidMissingAssignment = U"../../Test/data/ini/invalid_missing_assignment.ini";
	constexpr FilePathView InvalidEmptyKey = U"../../Test/data/ini/invalid_empty_key.ini";
	constexpr FilePathView InvalidBareCR = U"../../Test/data/ini/invalid_bare_cr.ini";
	constexpr FilePathView InvalidNonUTF8Latin1 = U"../../Test/data/ini/invalid_non_utf8_latin1.ini";
	constexpr FilePathView Nonexistent = U"../../Test/data/ini/nonexistent.ini";
}

// 空の INI と invalid INI の基本状態を確認する。
TEST_CASE("INI.default")
{
	const INI ini;

	CHECK(ini);
	CHECK(ini.isValid());
	CHECK(not ini.isInvalid());
	CHECK(ini.isEmpty());
	CHECK(ini.sections().isEmpty());
	CHECK(not ini.hasGlobalSection());
	CHECK(not ini.hasSection(U"window"));
	CHECK(not ini.hasProperty(U"window", U"width"));
	CHECK(ini.format().isEmpty());
	CHECK(ini.formatUTF8().empty());

	INI invalid = INI::Invalid();

	CHECK(not invalid);
	CHECK(not invalid.isValid());
	CHECK(invalid.isInvalid());
	CHECK(invalid.isEmpty());
	CHECK(invalid.sections().isEmpty());
	CHECK(invalid.format().isEmpty());
	CHECK(invalid.formatUTF8().empty());

	invalid.clear();

	CHECK(invalid);
	CHECK(invalid.isValid());
	CHECK(not invalid.isInvalid());
	CHECK(invalid.isEmpty());
}

// 基本的な INI ファイルを読み込み、グローバルセクションと名前付きセクションを取得できることを確認する。
TEST_CASE("INI.Load.basic")
{
	const INI ini = INI::Load(ValidBasic);

	REQUIRE(ini);
	CHECK(not ini.isEmpty());
	CHECK(ini.hasGlobalSection());
	CHECK(ini.hasSection(U"window"));
	CHECK(ini.hasSection(U"player"));
	CHECK(not ini.hasSection(U"missing"));
	REQUIRE_EQ(ini.sections().size(), 3);

	CHECK(ini.hasGlobalProperty(U"title"));
	CHECK(ini.hasGlobalProperty(U"version"));
	CHECK(ini.hasGlobalProperty(U"active"));
	CHECK(not ini.hasGlobalProperty(U"missing"));
	CHECK_EQ(ini.getGlobal<String>(U"title"), U"Siv3D INI test");
	CHECK_EQ(ini.getGlobal<int32>(U"version"), 8);
	CHECK_EQ(ini.getGlobal<String>(U"active"), U"true");

	const INISection& window = ini[U"window"];
	CHECK_EQ(window.name, U"window");
	CHECK(window.hasProperty(U"width"));
	CHECK(window.hasProperty(U"height"));
	CHECK(window.hasProperty(U"fullscreen"));
	CHECK_EQ(window[U"width"].get<int32>(), 1280);
	CHECK_EQ(window[U"height"].get<int32>(), 720);
	CHECK_EQ(window[U"fullscreen"].getString(), U"false");

	CHECK_EQ(ini.get<int32>(U"window", U"width"), 1280);
	CHECK_EQ(ini.get<int32>(U"window", U"height"), 720);
	CHECK_EQ(ini.get<String>(U"window", U"fullscreen"), U"false");

	const INISection& player = ini.getSection(U"player");
	CHECK_EQ(player.get<String>(U"name"), U"Alice");
	CHECK_EQ(player.get<int32>(U"score"), 123);
	CHECK(player.get<double>(U"speed") == doctest::Approx(3.5));
}

// StringView と std::string からのパース、および空入力と失敗時の invalid 化を確認する。
TEST_CASE("INI.Parse")
{
	{
		const INI ini = INI::Parse(U"title = Siv3D\n\n[window]\nwidth = 1280\nheight = 720\n");

		REQUIRE(ini);
		CHECK(ini.hasGlobalSection());
		CHECK(ini.hasSection(U"window"));
		CHECK_EQ(ini.getGlobal<String>(U"title"), U"Siv3D");
		CHECK_EQ(ini.get<int32>(U"window", U"width"), 1280);
		CHECK_EQ(ini.get<int32>(U"window", U"height"), 720);
	}

	{
		const INI ini = INI::Parse(std::string{ "name = Alice\r\nscore = 123\r\n" });

		REQUIRE(ini);
		CHECK(ini.hasGlobalSection());
		CHECK_EQ(ini.getGlobal<String>(U"name"), U"Alice");
		CHECK_EQ(ini.getGlobal<int32>(U"score"), 123);
	}

	{
		const INI ini = INI::Parse(U"");

		REQUIRE(ini);
		CHECK(ini.isEmpty());
		CHECK(ini.sections().isEmpty());
	}

	{
		const INI ini = INI::Parse(U"[window\nwidth = 1280\n");

		CHECK(not ini);
		CHECK(ini.isInvalid());
	}

	{
		INI ini;

		CHECK(ini.parse(U"value = 42\n"));
		CHECK(ini);
		CHECK_EQ(ini.getGlobal<int32>(U"value"), 42);

		CHECK(not ini.parse(U"[broken\n"));
		CHECK(not ini);
		CHECK(ini.isInvalid());
	}
}

// Result API が成功時に INI、失敗時に INIParseErrorReason の配列を返すことを確認する。
TEST_CASE("INI.LoadResult.ParseResult")
{
	{
		const auto result = INI::LoadResult(ValidBasic);

		REQUIRE(result);
		CHECK(result.value());
		CHECK(result.value().hasSection(U"window"));
	}

	{
		const auto result = INI::ParseResult(U"x = 10\n[y]\nz = 20\n");

		REQUIRE(result);
		CHECK(result.value());
		CHECK_EQ(result.value().getGlobal<int32>(U"x"), 10);
		CHECK_EQ(result.value().get<int32>(U"y", U"z"), 20);
	}

	{
		const auto result = INI::LoadResult(Nonexistent);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::InvalidReader);
		CHECK(not result.error().front().title.isEmpty());
		CHECK(not result.error().front().format().isEmpty());
	}

	{
		const auto result = INI::ParseResult(U"[window\nwidth = 1280\n", {}, U"inline.ini");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::MissingClosingBracket);
		CHECK(not result.error().front().title.isEmpty());
		CHECK(not result.error().front().locations.isEmpty());
		CHECK(result.error().front().locations.front().contains(U"inline.ini"));
		CHECK(not result.error().front().hint.isEmpty());
		CHECK(not result.error().front().format().isEmpty());
	}
}

// 不正な INI ファイルが Load では invalid、LoadResult では詳細エラーになることを確認する。
TEST_CASE("INI.Load.invalid")
{
	constexpr std::array InvalidFiles
	{
		InvalidMissingClosingBracket,
		InvalidEmptySectionName,
		InvalidUnexpectedAfterSection,
		InvalidMissingAssignment,
		InvalidEmptyKey,
		InvalidBareCR,
		InvalidNonUTF8Latin1,
	};

	for (const auto& path : InvalidFiles)
	{
		{
			const INI ini = INI::Load(path);
			CHECK(not ini);
			CHECK(ini.isInvalid());
		}

		{
			const auto result = INI::LoadResult(path);
			CHECK(not result);
			if (not result)
			{
				CHECK(not result.error().isEmpty());
				CHECK(not result.error().front().title.isEmpty());
			}
		}
	}
}

// コメント行、空白の trim、セクション名・キー・値の trim が既定で機能することを確認する。
TEST_CASE("INI.comments_whitespace")
{
	const INI ini = INI::Load(ValidCommentsWhitespace);

	REQUIRE(ini);
	CHECK(ini.hasGlobalSection());
	CHECK(ini.hasSection(U"config"));
	CHECK_EQ(ini.getGlobal<String>(U"title"), U"Whitespace Test");
	CHECK_EQ(ini.get<int32>(U"config", U"value"), 42);
	CHECK_EQ(ini.get<String>(U"config", U"message"), U"hello world");
	CHECK(not ini.hasGlobalProperty(U"; semicolon comment"));
	CHECK(not ini.hasGlobalProperty(U"# hash comment"));

	{
		INIReadOptions options;
		options.allowHashComment = false;

		const auto result = INI::ParseResult(U"# comment\nvalue = 1\n", options);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::MissingAssignment);
	}

	{
		INIReadOptions options;
		options.allowSemicolonComment = false;

		const auto result = INI::ParseResult(U"; comment\nvalue = 1\n", options);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::MissingAssignment);
	}

	{
		INIReadOptions options;
		options.trimValue = false;

		const INI parsed = INI::Parse(U"key =  value  \n", options);

		REQUIRE(parsed);
		CHECK_EQ(parsed.getGlobal<String>(U"key"), U"  value");
	}
}

// 重複セクションと重複キーの既定動作、および各 policy の挙動を確認する。
TEST_CASE("INI.duplicate_policies")
{
	{
		const INI ini = INI::Load(ValidDuplicate);

		REQUIRE(ini);
		CHECK_EQ(ini.get<String>(U"config", U"value"), U"second");
		CHECK_EQ(ini.get<String>(U"config", U"keep"), U"one");
		CHECK_EQ(ini.get<String>(U"config", U"extra"), U"yes");
		CHECK_EQ(ini.get<String>(U"users", U"name"), U"Bob");
	}

	{
		INIReadOptions options;
		options.duplicateKeyPolicy = INIDuplicateKeyPolicy::KeepFirst;

		const INI ini = INI::Load(ValidDuplicate, options);

		REQUIRE(ini);
		CHECK_EQ(ini.get<String>(U"config", U"value"), U"first");
		CHECK_EQ(ini.get<String>(U"users", U"name"), U"Alice");
	}

	{
		INIReadOptions options;
		options.duplicateSectionPolicy = INIDuplicateSectionPolicy::Error;

		const auto result = INI::LoadResult(ValidDuplicate, options);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::DuplicateSection);
		CHECK_EQ(result.error().front().section, U"config");
	}

	{
		INIReadOptions options;
		options.duplicateKeyPolicy = INIDuplicateKeyPolicy::Error;

		const auto result = INI::ParseResult(U"[section]\na = 1\na = 2\nb = 1\nb = 2\n", options);

		REQUIRE(not result);
		REQUIRE_EQ(result.error().size(), 2);
		CHECK_EQ(result.error()[0].code, INIParseErrorCode::DuplicateKey);
		CHECK_EQ(result.error()[0].section, U"section");
		CHECK_EQ(result.error()[0].key, U"a");
		CHECK_EQ(result.error()[1].code, INIParseErrorCode::DuplicateKey);
		CHECK_EQ(result.error()[1].section, U"section");
		CHECK_EQ(result.error()[1].key, U"b");
	}
}

// 改行許容オプションが LF, CRLF, CR に対して機能することを確認する。
TEST_CASE("INI.newline_options")
{
	{
		const INI ini = INI::Parse(U"a = 1\nb = 2\n");

		REQUIRE(ini);
		CHECK_EQ(ini.getGlobal<int32>(U"a"), 1);
		CHECK_EQ(ini.getGlobal<int32>(U"b"), 2);
	}

	{
		const INI ini = INI::Parse(std::string{ "a = 1\r\nb = 2\r\n" });

		REQUIRE(ini);
		CHECK_EQ(ini.getGlobal<int32>(U"a"), 1);
		CHECK_EQ(ini.getGlobal<int32>(U"b"), 2);
	}

	{
		const auto result = INI::LoadResult(ValidLoneCR);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::BareCR);
	}

	{
		INIReadOptions options;
		options.allowLoneCR = true;

		const auto result = INI::LoadResult(ValidLoneCR, options);

		REQUIRE(result);
		CHECK_EQ(result.value().get<int32>(U"window", U"width"), 1280);
		CHECK_EQ(result.value().get<int32>(U"window", U"height"), 720);
	}
}

// Unicode, UTF-8 BOM, 空ファイルの読み込みを確認する。
TEST_CASE("INI.unicode_bom_empty")
{
	{
		const INI ini = INI::Load(ValidUnicode);

		REQUIRE(ini);
		CHECK_EQ(ini.get<String>(U"language", U"title"), U"日本語タイトル");
		CHECK_EQ(ini.get<String>(U"language", U"message"), U"こんにちは Siv3D 🌏");
		CHECK_EQ(ini.get<String>(U"language", U"symbol"), U"αβγ");
	}

	{
		const INI ini = INI::Load(ValidUTF8BOM);

		REQUIRE(ini);
		CHECK_EQ(ini.getGlobal<String>(U"title"), U"UTF-8 BOM test");
		CHECK_EQ(ini.getGlobal<int32>(U"value"), 42);
		CHECK_EQ(ini.get<String>(U"message", U"text"), U"BOM 付き UTF-8");
	}

	{
		const INI ini = INI::Load(ValidEmpty);

		REQUIRE(ini);
		CHECK(ini.isEmpty());
		CHECK(ini.sections().isEmpty());
	}
}

// セクション、INIItem, INI 全体での型変換取得、Optional 取得、デフォルト値取得を確認する。
TEST_CASE("INI.get_getOpt_getOr")
{
	const INI ini = INI::Parse(U"int = 123\nfloat = 3.5\ntext = Siv3D\nbad = abc\n\n[section]\nvalue = 456\n");

	REQUIRE(ini);

	CHECK_EQ(ini.getGlobal<String>(U"text"), U"Siv3D");
	CHECK_EQ(ini.getGlobal<int32>(U"int"), 123);
	CHECK(ini.getGlobal<double>(U"float") == doctest::Approx(3.5));
	CHECK_EQ(ini.get<int32>(U"section", U"value"), 456);

	{
		const Optional<int32> opt = ini.getGlobalOpt<int32>(U"int");
		REQUIRE(opt);
		CHECK_EQ(*opt, 123);
	}

	{
		const Optional<int32> opt = ini[U"section"].getOpt<int32>(U"value");
		REQUIRE(opt);
		CHECK_EQ(*opt, 456);
	}

	CHECK(not ini.getGlobalOpt<int32>(U"text"));
	CHECK(not ini.getGlobalOpt<int32>(U"bad"));
	CHECK(not ini.getGlobalOpt<int32>(U"missing"));
	CHECK(not ini.getOpt<int32>(U"missing", U"value"));
	CHECK(not ini[U"section"].getOpt<int32>(U"missing"));

	CHECK_EQ(ini.getGlobalOr<int32>(U"int", -1), 123);
	CHECK_EQ(ini.getGlobalOr<int32>(U"text", -1), -1);
	CHECK_EQ(ini.getOr<int32>(U"section", U"value", -1), 456);
	CHECK_EQ(ini.getOr<String>(U"missing", U"value", String{ U"default" }), U"default");
	CHECK_EQ(ini[U"section"].getOr<int32>(U"missing", -1), -1);

	CHECK_THROWS_AS((void)ini.getGlobal<int32>(U"text"), Error);
	CHECK_THROWS_AS((void)ini.get<int32>(U"missing", U"value"), Error);
	CHECK_THROWS_AS((void)ini[U"section"].get<int32>(U"missing"), Error);

	const INIItem& item = ini[U"section"][U"value"];
	CHECK_EQ(item.getString(), U"456");
	CHECK_EQ(item.getUTF8(), std::string{ "456" });
	CHECK_EQ(item.get<int32>(), 456);
	CHECK_EQ(item.getOr<int32>(-1), 456);
	CHECK(not ini.getGlobalOpt<int32>(U"bad"));
}

// operator[], getSection, propertyView, missing アクセス時の例外を確認する。
TEST_CASE("INI.section_access")
{
	const INI ini = INI::Load(ValidBasic);

	REQUIRE(ini);

	const INISection& global = ini[];
	CHECK_EQ(global.name, U"");
	CHECK_EQ(global[U"title"].getString(), U"Siv3D INI test");

	const INISection& window = ini.getSection(U"window");
	const Array<INISection::Property> properties = window.propertyView();

	REQUIRE_EQ(properties.size(), 3);
	CHECK_EQ(properties[0].key, U"width");
	CHECK_EQ(properties[0].value, U"1280");
	CHECK_EQ(properties[0].index, 0);
	CHECK_EQ(properties[1].key, U"height");
	CHECK_EQ(properties[1].value, U"720");
	CHECK_EQ(properties[1].index, 1);
	CHECK_EQ(properties[2].key, U"fullscreen");
	CHECK_EQ(properties[2].value, U"false");
	CHECK_EQ(properties[2].index, 2);

	CHECK_THROWS_AS((void)ini.getSection(U"missing"), Error);
	CHECK_THROWS_AS((void)ini[U"missing"], Error);
	CHECK_THROWS_AS((void)ini[U"window"][U"missing"], Error);
}

// addSection, addProperty, addGlobalProperty, remove 系 API と INIItem の代入を確認する。
TEST_CASE("INI.modify")
{
	INI ini;

	ini.addGlobalProperty(U"title", U"Game");
	ini.addSection(U"window");
	ini.addProperty(U"window", U"width", U"1280");
	ini.addProperty(U"window", U"height", U"720");
	ini.addProperty(U"player", U"name", U"Alice");

	REQUIRE(ini);
	CHECK(ini.hasGlobalSection());
	CHECK(ini.hasSection(U"window"));
	CHECK(ini.hasSection(U"player"));
	CHECK_EQ(ini.getGlobal<String>(U"title"), U"Game");
	CHECK_EQ(ini.get<int32>(U"window", U"width"), 1280);
	CHECK_EQ(ini.get<String>(U"player", U"name"), U"Alice");

	ini[U"window"][U"width"] = 1920;
	ini[U"player"].addProperty(U"score", 100);
	CHECK_EQ(ini.get<int32>(U"window", U"width"), 1920);
	CHECK_EQ(ini.get<int32>(U"player", U"score"), 100);

	ini.removeProperty(U"window", U"height");
	CHECK(not ini.hasProperty(U"window", U"height"));

	ini.removeGlobalProperty(U"title");
	CHECK(not ini.hasGlobalProperty(U"title"));

	ini.removeSection(U"player");
	CHECK(not ini.hasSection(U"player"));

	ini.removeGlobalSection();
	CHECK(not ini.hasGlobalSection());

	ini.clear();
	CHECK(ini);
	CHECK(ini.isEmpty());
}

// save を使わずに、INIWriteOptions と文字列化を確認する。
TEST_CASE("INI.format")
{
	INI ini;
	ini.addGlobalProperty(U"title", U"Siv3D");
	ini.addProperty(U"window", U"width", U"1280");
	ini.addProperty(U"window", U"height", U"720");

	CHECK_EQ(ini.format(), U"title = Siv3D\n\n[window]\nwidth = 1280\nheight = 720\n");
	CHECK_EQ(ini.formatUTF8(), std::string{ "title = Siv3D\n\n[window]\nwidth = 1280\nheight = 720\n" });

	{
		INIWriteOptions options;
		options.spaceAroundEquals = false;

		CHECK_EQ(ini.format(options), U"title=Siv3D\n\n[window]\nwidth=1280\nheight=720\n");
	}

	{
		INIWriteOptions options;
		options.trailingNewline = false;

		CHECK_EQ(ini.format(options), U"title = Siv3D\n\n[window]\nwidth = 1280\nheight = 720");
	}

	{
		INIWriteOptions options;
		options.newline = U"\r\n";

		CHECK_EQ(ini.format(options), U"title = Siv3D\r\n\r\n[window]\r\nwidth = 1280\r\nheight = 720\r\n");
	}

	{
		INIWriteOptions options;
		options.writeUTF8BOM = true;

		const std::string formatted = ini.formatUTF8(options);

		REQUIRE(3 <= formatted.size());
		CHECK_EQ(formatted.substr(0, 3), std::string{ "\xEF\xBB\xBF", 3 });
	}

	{
		const INI invalid = INI::Invalid();

		CHECK(invalid.format().isEmpty());
		CHECK(invalid.formatUTF8().empty());
	}
}

// 代表的な構文エラーが対応する INIParseErrorCode を返すことを確認する。
TEST_CASE("INI.error_codes")
{
	{
		const auto result = INI::ParseResult(U"[window\nwidth = 1280\n");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::MissingClosingBracket);
		CHECK_EQ(result.error().front().line, 1);
	}

	{
		const auto result = INI::ParseResult(U"[]\nvalue = 1\n");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::EmptySectionName);
	}

	{
		const auto result = INI::ParseResult(U"[window] trailing\nwidth = 1280\n");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::UnexpectedCharacterAfterSection);
		CHECK_EQ(result.error().front().section, U"window");
	}

	{
		const auto result = INI::ParseResult(U"width 1280\n");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::MissingAssignment);
	}

	{
		const auto result = INI::ParseResult(U"= value\n");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::EmptyKey);
	}

	{
		const auto result = INI::ParseResult(U"a = 1\rb = 2\n");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::BareCR);
	}

	{
		INIReadOptions options;
		options.duplicateSectionPolicy = INIDuplicateSectionPolicy::Error;

		const auto result = INI::ParseResult(U"[s]\na = 1\n[s]\nb = 2\n", options);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::DuplicateSection);
		CHECK_EQ(result.error().front().section, U"s");
	}

	{
		INIReadOptions options;
		options.duplicateKeyPolicy = INIDuplicateKeyPolicy::Error;

		const auto result = INI::ParseResult(U"[s]\na = 1\na = 2\n", options);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::DuplicateKey);
		CHECK_EQ(result.error().front().section, U"s");
		CHECK_EQ(result.error().front().key, U"a");
	}

	{
		const auto result = INI::ParseResult(std::string{ "[section]\nname = \xFF\n" });

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, INIParseErrorCode::InvalidEncoding);
	}
}
