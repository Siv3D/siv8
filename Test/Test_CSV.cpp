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
	constexpr FilePathView ValidBasic = U"../../Test/data/csv/valid_basic.csv";
	constexpr FilePathView ValidQuoted = U"../../Test/data/csv/valid_quoted.csv";
	constexpr FilePathView ValidLF = U"../../Test/data/csv/valid_lf.csv";
	constexpr FilePathView ValidLoneCR = U"../../Test/data/csv/valid_lone_cr.csv";
	constexpr FilePathView ValidUnicode = U"../../Test/data/csv/valid_unicode.csv";
	constexpr FilePathView ValidUTF8BOM = U"../../Test/data/csv/valid_utf8_bom.csv";
	constexpr FilePathView ValidEmpty = U"../../Test/data/csv/valid_empty.csv";
	constexpr FilePathView ValidRagged = U"../../Test/data/csv/valid_ragged.csv";

	constexpr FilePathView InvalidUnexpectedQuote = U"../../Test/data/csv/invalid_unexpected_quote.csv";
	constexpr FilePathView InvalidAfterQuote = U"../../Test/data/csv/invalid_after_quote.csv";
	constexpr FilePathView InvalidUnterminatedQuote = U"../../Test/data/csv/invalid_unterminated_quote.csv";
	constexpr FilePathView InvalidBareCR = U"../../Test/data/csv/invalid_bare_cr.csv";
	constexpr FilePathView InvalidNonUTF8Latin1 = U"../../Test/data/csv/invalid_non_utf8_latin1.csv";
	constexpr FilePathView Nonexistent = U"../../Test/data/csv/nonexistent.csv";
}

// 空の CSV と invalid CSV の基本状態を確認する。
TEST_CASE("CSV.default")
{
	const CSV csv;

	CHECK(csv);
	CHECK(csv.isValid());
	CHECK(not csv.isInvalid());
	CHECK(csv.isEmpty());
	CHECK_EQ(csv.rows(), 0);
	CHECK_EQ(csv.columns(0), 0);
	CHECK(not csv.inBounds(0, 0));
	CHECK(csv.data().isEmpty());
	CHECK(csv.format().isEmpty());
	CHECK(csv.formatUTF8().empty());

	CSV invalid = CSV::Invalid();

	CHECK(not invalid);
	CHECK(not invalid.isValid());
	CHECK(invalid.isInvalid());
	CHECK(invalid.isEmpty());
	CHECK_EQ(invalid.rows(), 0);
	CHECK(invalid.format().isEmpty());
	CHECK(invalid.formatUTF8().empty());

	invalid.clear();

	CHECK(invalid);
	CHECK(invalid.isValid());
	CHECK(not invalid.isInvalid());
	CHECK(invalid.isEmpty());
}

// CRLF 区切りの基本的な CSV ファイルを読み込めることを確認する。
TEST_CASE("CSV.Load.basic")
{
	const CSV csv = CSV::Load(ValidBasic);

	REQUIRE(csv);
	CHECK(not csv.isEmpty());
	CHECK_EQ(csv.rows(), 3);
	CHECK_EQ(csv.columns(0), 3);
	CHECK_EQ(csv.columns(1), 3);
	CHECK_EQ(csv.columns(999), 0);

	CHECK(csv.inBounds(0, 0));
	CHECK(csv.inBounds(2, 2));
	CHECK(not csv.inBounds(3, 0));
	CHECK(not csv.inBounds(0, 3));

	CHECK_EQ(csv[0][0], U"id");
	CHECK_EQ(csv[0][1], U"name");
	CHECK_EQ(csv[0][2], U"score");
	CHECK_EQ(csv[1][0], U"1");
	CHECK_EQ(csv[1][1], U"Alice");
	CHECK_EQ(csv[1][2], U"95");
	CHECK_EQ(csv[2][0], U"2");
	CHECK_EQ(csv[2][1], U"Bob");
	CHECK_EQ(csv[2][2], U"88");
}

// StringView と std::string からのパース、および空入力と失敗時の invalid 化を確認する。
TEST_CASE("CSV.Parse")
{
	{
		const CSV csv = CSV::Parse(U"a,b,c\n1,2,3\n");

		REQUIRE(csv);
		CHECK_EQ(csv.rows(), 2);
		CHECK_EQ(csv[0][0], U"a");
		CHECK_EQ(csv[0][1], U"b");
		CHECK_EQ(csv[0][2], U"c");
		CHECK_EQ(csv[1][0], U"1");
		CHECK_EQ(csv[1][1], U"2");
		CHECK_EQ(csv[1][2], U"3");
	}

	{
		const CSV csv = CSV::Parse(std::string{ "a,b,c\r\n1,2,3\r\n" });

		REQUIRE(csv);
		CHECK_EQ(csv.rows(), 2);
		CHECK_EQ(csv[1][0], U"1");
		CHECK_EQ(csv[1][1], U"2");
		CHECK_EQ(csv[1][2], U"3");
	}

	{
		const CSV csv = CSV::Parse(U"");

		REQUIRE(csv);
		CHECK(csv.isEmpty());
		CHECK_EQ(csv.rows(), 0);
	}

	{
		const CSV csv = CSV::Parse(U"abc,\"def\n");

		CHECK(not csv);
		CHECK(csv.isInvalid());
	}
}

// Result API が成功時に CSV、失敗時に CSVParseErrorReason の配列を返すことを確認する。
TEST_CASE("CSV.LoadResult.ParseResult")
{
	{
		const auto result = CSV::LoadResult(ValidBasic);

		REQUIRE(result);
		CHECK(result.value());
		CHECK_EQ(result.value().rows(), 3);
	}

	{
		const auto result = CSV::ParseResult(U"x,y\n10,20\n");

		REQUIRE(result);
		CHECK(result.value());
		CHECK_EQ(result.value()[1][0], U"10");
		CHECK_EQ(result.value()[1][1], U"20");
	}

	{
		const auto result = CSV::LoadResult(Nonexistent);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::InvalidReader);
		CHECK(not result.error().front().title.isEmpty());
		CHECK(not result.error().front().format().isEmpty());
	}

	{
		const auto result = CSV::ParseResult(U"abc,\"def\n", {}, U"inline.csv");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::UnterminatedQuotedField);
		CHECK(not result.error().front().title.isEmpty());
		CHECK(not result.error().front().locations.isEmpty());
		CHECK(result.error().front().locations.front().contains(U"inline.csv"));
		CHECK(not result.error().front().hint.isEmpty());
		CHECK(not result.error().front().format().isEmpty());
	}
}

// 不正な CSV ファイルが Load では invalid、LoadResult では詳細エラーになることを確認する。
TEST_CASE("CSV.Load.invalid")
{
	constexpr std::array InvalidFiles
	{
		InvalidUnexpectedQuote,
		InvalidAfterQuote,
		InvalidUnterminatedQuote,
		InvalidBareCR,
		InvalidNonUTF8Latin1,
	};

	for (const auto& path : InvalidFiles)
	{
		{
			const CSV csv = CSV::Load(path);
			CHECK(not csv);
			CHECK(csv.isInvalid());
		}

		{
			const auto result = CSV::LoadResult(path);
			CHECK(not result);
			if (not result)
			{
				CHECK(not result.error().isEmpty());
				CHECK(not result.error().front().title.isEmpty());
			}
		}
	}
}

// comma, double quote, CRLF を含む quoted field を RFC 4180 形式で読み取れることを確認する。
TEST_CASE("CSV.RFC4180_quoted_fields")
{
	const CSV csv = CSV::Load(ValidQuoted);

	REQUIRE(csv);
	REQUIRE_EQ(csv.rows(), 3);
	REQUIRE_EQ(csv.columns(0), 3);
	REQUIRE_EQ(csv.columns(1), 3);
	REQUIRE_EQ(csv.columns(2), 3);

	CHECK_EQ(csv[0][0], U"name");
	CHECK_EQ(csv[0][1], U"note");
	CHECK_EQ(csv[0][2], U"text");

	CHECK_EQ(csv[1][0], U"Alice");
	CHECK_EQ(csv[1][1], U"Hello, CSV");
	CHECK_EQ(csv[1][2], U"He said \"Hi\"");

	CHECK_EQ(csv[2][0], U"Bob");
	CHECK_EQ(csv[2][1], U"Line1\r\nLine2");
	CHECK_EQ(csv[2][2], U"");

	{
		const CSV parsed = CSV::Parse(U"\"a,b\",\"c\"\"d\",\"e\r\nf\"\r\n");

		REQUIRE(parsed);
		REQUIRE_EQ(parsed.rows(), 1);
		CHECK_EQ(parsed[0][0], U"a,b");
		CHECK_EQ(parsed[0][1], U"c\"d");
		CHECK_EQ(parsed[0][2], U"e\r\nf");
	}
}

// 読み込み時の改行許容オプションが LF, CRLF, CR に対して機能することを確認する。
TEST_CASE("CSV.newlines_options")
{
	{
		const CSV csv = CSV::Load(ValidLF);

		REQUIRE(csv);
		CHECK_EQ(csv.rows(), 3);
		CHECK_EQ(csv[1][0], U"1");
		CHECK_EQ(csv[2][1], U"Bob");
	}

	{
		CSVReadOptions options;
		options.allowLoneLF = false;

		const auto result = CSV::LoadResult(ValidLF, options);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::BareLF);
	}

	{
		const auto result = CSV::LoadResult(ValidLoneCR);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::BareCR);
	}

	{
		CSVReadOptions options;
		options.allowLoneCR = true;

		const auto result = CSV::LoadResult(ValidLoneCR, options);

		REQUIRE(result);
		CHECK_EQ(result.value().rows(), 3);
		CHECK_EQ(result.value()[2][1], U"Bob");
	}

	{
		CSVReadOptions options;
		options.allowLoneLF = false;

		const auto result = CSV::ParseResult(U"\"a\nb\",c\r\n", options);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::BareLF);
	}
}

// Unicode, UTF-8 BOM, 空ファイルの読み込みを確認する。
TEST_CASE("CSV.unicode_bom_empty")
{
	{
		const CSV csv = CSV::Load(ValidUnicode);

		REQUIRE(csv);
		REQUIRE_EQ(csv.rows(), 3);
		CHECK_EQ(csv[0][0], U"name");
		CHECK_EQ(csv[1][0], U"太郎");
		CHECK_EQ(csv[1][1], U"こんにちは");
		CHECK_EQ(csv[1][2], U"🎮");
		CHECK_EQ(csv[2][0], U"Αλφα");
		CHECK_EQ(csv[2][1], U"βγ");
		CHECK_EQ(csv[2][2], U"🌏");
	}

	{
		const CSV csv = CSV::Load(ValidUTF8BOM);

		REQUIRE(csv);
		REQUIRE_EQ(csv.rows(), 3);
		CHECK_EQ(csv[0][0], U"title");
		CHECK_EQ(csv[0][1], U"value");
		CHECK_EQ(csv[1][0], U"UTF-8 BOM test");
		CHECK_EQ(csv[1][1], U"42");
		CHECK_EQ(csv[2][0], U"message");
		CHECK_EQ(csv[2][1], U"BOM 付き UTF-8");
	}

	{
		const CSV csv = CSV::Load(ValidEmpty);

		REQUIRE(csv);
		CHECK(csv.isEmpty());
		CHECK_EQ(csv.rows(), 0);
	}
}

// 列数不一致を許容するデフォルト挙動と、検証オプション有効時の複数エラーを確認する。
TEST_CASE("CSV.consistent_columns")
{
	{
		const CSV csv = CSV::Load(ValidRagged);

		REQUIRE(csv);
		REQUIRE_EQ(csv.rows(), 3);
		CHECK_EQ(csv.columns(0), 3);
		CHECK_EQ(csv.columns(1), 2);
		CHECK_EQ(csv.columns(2), 4);
	}

	{
		CSVReadOptions options;
		options.requireConsistentColumns = true;

		const auto result = CSV::LoadResult(ValidRagged, options);

		REQUIRE(not result);
		REQUIRE_EQ(result.error().size(), 2);
		CHECK_EQ(result.error()[0].code, CSVParseErrorCode::InconsistentColumns);
		CHECK_EQ(result.error()[0].row, 1);
		CHECK_EQ(result.error()[1].code, CSVParseErrorCode::InconsistentColumns);
		CHECK_EQ(result.error()[1].row, 2);
	}
}

// セル値の型変換取得、Optional 取得、デフォルト値取得を確認する。
TEST_CASE("CSV.get_getOpt_getOr")
{
	const CSV csv = CSV::Parse(U"int,float,text,bad\n123,3.5,Siv3D,abc\n");

	REQUIRE(csv);
	REQUIRE_EQ(csv.rows(), 2);

	CHECK_EQ(csv.get<String>(0, 0), U"int");
	CHECK_EQ(csv.get<int32>(1, 0), 123);
	CHECK(csv.get<double>(1, 1) == doctest::Approx(3.5));
	CHECK_EQ(csv.get<String>(1, 2), U"Siv3D");

	{
		const Optional<int32> opt = csv.getOpt<int32>(1, 0);
		REQUIRE(opt);
		CHECK_EQ(*opt, 123);
	}

	CHECK(not csv.getOpt<int32>(1, 2));
	CHECK(not csv.getOpt<int32>(1, 3));
	CHECK(not csv.getOpt<int32>(99, 99));

	CHECK_EQ(csv.getOr<int32>(1, 0, -1), 123);
	CHECK_EQ(csv.getOr<int32>(1, 2, -1), -1);
	CHECK_EQ(csv.getOr<String>(99, 99, String{ U"default" }), U"default");

	CHECK_EQ(csv.get<int32>(99, 99), 0);
	CHECK_EQ(csv.get<String>(99, 99), U"");
}

// data, getData, row, getRow, operator[] の参照アクセサを確認する。
TEST_CASE("CSV.data_access")
{
	CSV csv = CSV::Parse(U"a,b\n1,2\n");

	REQUIRE(csv);
	CHECK(csv.data() == CSV::Table{ CSV::Row{ U"a", U"b" }, CSV::Row{ U"1", U"2" } });
	CHECK(csv.row(0) == CSV::Row{ U"a", U"b" });
	CHECK(csv[1] == CSV::Row{ U"1", U"2" });

	csv.row(1)[1] = U"200";
	CHECK_EQ(csv[1][1], U"200");

	csv.data()[0][0] = U"header";
	CHECK_EQ(csv[0][0], U"header");
}

// addRow, write, writeRow, newLine によるインメモリ編集を確認する。
TEST_CASE("CSV.modify")
{
	CSV csv;

	csv.addRow(U"id", U"name");
	csv.write(1);
	csv.write(U"Alice");
	csv.newLine();
	csv.write(2, U"Bob");
	csv.newLine();
	csv.addRow(CSV::Row{ U"3", U"Charlie" });

	REQUIRE(csv);
	REQUIRE_EQ(csv.rows(), 4);
	CHECK(csv.data() == CSV::Table{
		CSV::Row{ U"id", U"name" },
		CSV::Row{ U"1", U"Alice" },
		CSV::Row{ U"2", U"Bob" },
		CSV::Row{ U"3", U"Charlie" },
	});

	csv.clear();

	CHECK(csv);
	CHECK(csv.isEmpty());
	CHECK_EQ(csv.rows(), 0);
}

// save を使わずに、CSVWriteOptions と RFC 4180 形式の文字列化を確認する。
TEST_CASE("CSV.format")
{
	CSV csv;
	csv.addRow(U"plain", U"comma,value", U"quote \" value", U"line\nbreak");

	CHECK_EQ(csv.format(), U"plain,\"comma,value\",\"quote \"\" value\",\"line\nbreak\"\r\n");
	CHECK_EQ(csv.formatUTF8(), std::string{ "plain,\"comma,value\",\"quote \"\" value\",\"line\nbreak\"\r\n" });

	{
		CSVWriteOptions options;
		options.alwaysQuote = true;

		CHECK_EQ(csv.format(options), U"\"plain\",\"comma,value\",\"quote \"\" value\",\"line\nbreak\"\r\n");
	}

	{
		CSVWriteOptions options;
		options.trailingNewline = false;

		CHECK_EQ(csv.format(options), U"plain,\"comma,value\",\"quote \"\" value\",\"line\nbreak\"");
	}

	{
		CSVWriteOptions options;
		options.writeUTF8BOM = true;

		const std::string formatted = csv.formatUTF8(options);

		REQUIRE(3 <= formatted.size());
		CHECK_EQ(formatted.substr(0, 3), std::string{ "\xEF\xBB\xBF", 3 });
	}
}

// 代表的な構文エラーが対応する CSVParseErrorCode を返すことを確認する。
TEST_CASE("CSV.error_codes")
{
	{
		const auto result = CSV::ParseResult(U"abc,d\"ef\r\n");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::UnexpectedQuoteInUnquotedField);
		CHECK_EQ(result.error().front().line, 1);
		CHECK_EQ(result.error().front().row, 0);
		CHECK_EQ(result.error().front().column, 1);
	}

	{
		const auto result = CSV::ParseResult(U"\"abc\"def\r\n");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::UnexpectedCharacterAfterClosingQuote);
	}

	{
		const auto result = CSV::ParseResult(U"\"abc\r\n");

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::UnterminatedQuotedField);
	}

	{
		CSVReadOptions options;
		options.allowLoneLF = false;

		const auto result = CSV::ParseResult(U"abc,def\n", options);

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::BareLF);
	}

	{
		const auto result = CSV::ParseResult(std::string{ "abc,\xFF\n" });

		REQUIRE(not result);
		REQUIRE(not result.error().isEmpty());
		CHECK_EQ(result.error().front().code, CSVParseErrorCode::InvalidEncoding);
	}
}
