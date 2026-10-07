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
# include <sstream>

TEST_CASE("JSON")
{
	{
		const JSON j_null(JSONValueType::Null);
		const JSON j_object(JSONValueType::Object);
		const JSON j_array(JSONValueType::Array);
		const JSON j_string(JSONValueType::String);
		const JSON j_boolean(JSONValueType::Bool);
		const JSON j_number_int(JSONValueType::NumberInt);
		const JSON j_number_uint(JSONValueType::NumberUint);
		const JSON j_number_float(JSONValueType::NumberFloat);
		const JSON j_binary(JSONValueType::Binary);

		CHECK((j_null.getType()) == (JSONValueType::Null));
		CHECK((j_object.getType()) == (JSONValueType::Object));
		CHECK((j_array.getType()) == (JSONValueType::Array));
		CHECK((j_string.getType()) == (JSONValueType::String));
		CHECK((j_boolean.getType()) == (JSONValueType::Bool));
		CHECK((j_number_int.getType()) == (JSONValueType::NumberInt));
		CHECK((j_number_uint.getType()) == (JSONValueType::NumberUint));
		CHECK((j_number_float.getType()) == (JSONValueType::NumberFloat));
		CHECK((j_binary.getType()) == (JSONValueType::Binary));

		CHECK(j_null.isNull());
		CHECK(j_object.isObject());
		CHECK(j_array.isArray());
		CHECK(j_string.isString());
		CHECK(j_boolean.isBool());
		CHECK(j_number_int.isInt());
		CHECK(j_number_int.isNumber());
		CHECK(j_number_uint.isNumber());
		CHECK(j_number_float.isFloat());
		CHECK(j_number_float.isNumber());
		CHECK(j_binary.isBinary());
	}

	{
		const JSON j_null(nullptr);
		const JSON j_object(JSON::EmptyObject());
		const JSON j_array(Array<int32>{ 1, 2, 3, 4, 5 });
		const JSON j_string("Hello, Siv3D!");
		const JSON j_boolean(true);
		const JSON j_number_int(42);
		const JSON j_number_uint(42u);
		const JSON j_number_float(42.0);

		CHECK((j_null.getType()) == (JSONValueType::Null));
		CHECK((j_object.getType()) == (JSONValueType::Object));
		CHECK((j_array.getType()) == (JSONValueType::Array));
		CHECK((j_string.getType()) == (JSONValueType::String));
		CHECK((j_boolean.getType()) == (JSONValueType::Bool));
		CHECK((j_number_int.getType()) == (JSONValueType::NumberInt));
		CHECK((j_number_uint.getType()) == (JSONValueType::NumberUint));
		CHECK((j_number_float.getType()) == (JSONValueType::NumberFloat));
	}

	{
		const JSON j_null = nullptr;
		const JSON j_object = JSON::EmptyObject();
		const JSON j_array = Array<int32>{ 1, 2, 3, 4, 5 };
		const JSON j_string = "Hello, Siv3D!";
		const JSON j_boolean = true;
		const JSON j_number_int = 42;
		const JSON j_number_uint = 42u;
		const JSON j_number_float = 42.0;

		CHECK((j_null.getType()) == (JSONValueType::Null));
		CHECK((j_object.getType()) == (JSONValueType::Object));
		CHECK((j_array.getType()) == (JSONValueType::Array));
		CHECK((j_string.getType()) == (JSONValueType::String));
		CHECK((j_boolean.getType()) == (JSONValueType::Bool));
		CHECK((j_number_int.getType()) == (JSONValueType::NumberInt));
		CHECK((j_number_uint.getType()) == (JSONValueType::NumberUint));
		CHECK((j_number_float.getType()) == (JSONValueType::NumberFloat));
	}

	{
		const JSON j_array = Array<String>{ U"a", U"b", U"c" };
		const JSON j_string = U"Hello, Siv3D!";

		CHECK(j_array.isArray());
		CHECK(j_string.isString());
	}

	{
		JSON j;
		CHECK(j.isNull());
		j = nullptr;
		CHECK(j.isNull());
		j = "Hello, Siv3D!";
		CHECK(j.isString());
		j = true;
		CHECK(j.isBool());
	}
}

TEST_CASE("JSON and String")
{
	const JSON json = JSON::Parse(U"{ \"a\": \"Siv3D\" }");
	CHECK(json.formatMinified() == U"{\"a\":\"Siv3D\"}");
	{
		JSON j;
		
		j["a"] = "Siv3D";
		CHECK((j) == (json));

		j["a"] = std::string_view{ "Siv3D" };
		CHECK((j) == (json));

		j["a"] = std::string{ "Siv3D" };
		CHECK((j) == (json));

		const char* str = "Siv3D";
		j["a"] = str;
		CHECK((j) == (json));

		j["a"] = U"Siv3D";
		CHECK((j) == (json));

		j["a"] = std::u32string_view{ U"Siv3D" };
		CHECK((j) == (json));

		j["a"] = StringView{ U"Siv3D" };
		CHECK((j) == (json));

		j["a"] = String{ U"Siv3D" };
		CHECK((j) == (json));

		const char32* str32 = U"Siv3D";
		j["a"] = str32;
		CHECK((j) == (json));
	}
}

TEST_CASE("JSON Invalid")
{
	{
		const JSON json = JSON::Parse(U"");
		CHECK(json.isInvalid());
	}

	{
		const JSON json = JSON::Parse(U"{ \"a\": \"Siv3D\"");
		CHECK(json.isInvalid());
	}
}

TEST_CASE("JSON.Parse.syntaxAndUnicode")
{
	const std::string_view valid[] = {
		"null", "true", "false", "0", "-0", "-12.5e+2", "1e-300", "\"\"", "[]", "{}",
		R"({"text":"日本語 😀","escaped":"\"\\\/\b\f\n\r\t","nul":"a\u0000b","pair":"\uD83D\uDE00"})",
		" \t\r\n[true,false,null,{},[],123] \r\n",
	};
	for (const auto source : valid)
	{
		INFO(source);
		const JSON json = JSON::Parse(source, AllowExceptions::Yes);
		REQUIRE_FALSE(json.isInvalid());
		CHECK(JSON::Parse(json.formatUTF8Minified()) == json);
		CHECK(JSON::Parse(json.format()) == json);
		CHECK(JSON::Parse(json.formatMinified(EnsureAscii::Yes)) == json);
	}

	const std::string_view invalid[] = {
		"", " ", "undefined", "NaN", "Infinity", "+1", "01", "-01", ".1", "1.", "1e", "1e+",
		"[1,]", "[,1]", "{\"a\":1,}", "{a:1}", "{'a':1}", "[1 2]", "{} true", "[", "{",
		"\"unterminated", R"("\x20")", R"("\uZZZZ")", R"("\uD800")", R"("\uDC00")",
		"\"raw\nnewline\"", "\"raw\rcarriage\"", "\"\xFF\"", "\"\xC0\xAF\"",
	};
	for (const auto source : invalid)
	{
		INFO(source);
		CHECK(JSON::Parse(source).isInvalid());
		CHECK_THROWS_AS(JSON::Parse(source, AllowExceptions::Yes), Error);
		JSON json = 42;
		CHECK_FALSE(json.parse(source));
		CHECK(json.isInvalid());
		json = 42;
		CHECK_THROWS_AS(json.parse(source, AllowExceptions::Yes), Error);
		CHECK(json.isInvalid());
	}

	const JSON text = JSON::Parse(valid[10]);
	CHECK(text["nul"].getUTF8() == std::string("a\0b", 3));
	CHECK(text["pair"].getString() == U"😀");
	CHECK(text["escaped"].getUTF8() == "\"\\/\b\f\n\r\t");
}

TEST_CASE("JSON.Parse.embeddedNulAndOverflow")
{
	const std::string sources[] = {
		std::string("{}\0trailing", 11), std::string("123\0", 4), std::string("\"a\0b\"", 5), "1e9999",
	};
	for (const auto& source : sources)
	{
		INFO(source);
		CHECK(JSON::Parse(source).isInvalid());
		CHECK_THROWS_AS(JSON::Parse(source, AllowExceptions::Yes), Error);
		JSON json = 42;
		CHECK_THROWS_AS(json.parse(source, AllowExceptions::Yes), Error);
		CHECK(json.isInvalid());
	}
}

TEST_CASE("JSON.Load.preservesSyntax")
{
	const std::string_view sources[] = { "[1\r2]", "\"a\rb\"", "{\"a\":tru\re}" };
	for (const auto source : sources)
	{
		INFO(source);
		REQUIRE(JSON::Parse(source).isInvalid());
		CHECK(JSON::Load(MemoryViewReader{ source.data(), source.size() }).isInvalid());
		CHECK_THROWS_AS(JSON::Load(MemoryViewReader{ source.data(), source.size() }, AllowExceptions::Yes), Error);
	}
}

TEST_CASE("JSON.construction.UTF32")
{
	const std::u32string_view view = U"日本語 😀";
	const std::u32string string{ view };
	CHECK(JSON(view).isString());
	CHECK(JSON(string).isString());
	CHECK(JSON(view) == JSON(U"日本語 😀"));
	JSON json;
	json = string;
	CHECK(json == JSON(U"日本語 😀"));
}

TEST_CASE("JSON.numbersAndConversions")
{
	const JSON json = JSON::Parse(R"([-9223372036854775808,9223372036854775807,18446744073709551615,9007199254740993,1.25,-0.0])");
	REQUIRE(json.isArray());
	CHECK(json[0].get<int64>() == std::numeric_limits<int64>::min());
	CHECK(json[1].get<int64>() == std::numeric_limits<int64>::max());
	CHECK(json[2].get<uint64>() == std::numeric_limits<uint64>::max());
	CHECK(json[3].get<int64>() == 9007199254740993LL);
	CHECK(json[4].get<double>() == 1.25);
	CHECK(std::signbit(json[5].get<double>()));
	CHECK(JSON::Parse(json.formatUTF8Minified()) == json);
	CHECK_FALSE(JSON("12").getOpt<int>());
	CHECK_FALSE(JSON(nullptr).getOpt<int>());
	CHECK_FALSE(JSON(12).getOpt<String>());
	CHECK_THROWS_AS(JSON("12").get<int>(), Error);
	CHECK(JSON("12").getOr<int>(7) == 7);
	CHECK(JSON(12).getOr<int>(7) == 12);
	CHECK(JSON::Parse("[1,2,3]").get<Array<int>>() == Array<int>{ 1, 2, 3 });
	CHECK_FALSE(JSON::Parse("[1,\"x\",3]").getOpt<Array<int>>());
}

TEST_CASE("JSON.mutationAndReferences")
{
	JSON json;
	json["name"] = U"日本語";
	json["values"] = JSON::EmptyArray();
	json["values"].push_back(1);
	json["values"].push_back(nullptr);
	json["values"].push_back(true);
	CHECK(json["values"].size() == 3);
	json["values"][0] = 7;
	json["values"].erase(size_t{ 1 });
	json["values"].pop_back();
	CHECK(json["values"].formatUTF8Minified() == "[7]");
	CHECK_THROWS_AS(json["values"][1], Error);
	CHECK_THROWS_AS(json["values"].erase(size_t{ 1 }), Error);
	CHECK_THROWS_AS(json[size_t{ 0 }], Error);
	CHECK(json.contains("name"));
	CHECK(json.hasElement(U"name"));
	json.erase("absent");
	json.erase(U"name");
	CHECK_FALSE(json.contains("name"));

	JSON alias = json["values"];
	JSON copy = alias.clone();
	alias[0] = 9;
	CHECK(json["values"][0].get<int>() == 9);
	CHECK(copy[0].get<int>() == 7);
	const JSON& constRoot = json;
	JSON constAlias = constRoot["values"];
	constAlias[0] = 10;
	CHECK(json["values"][0].get<int>() == 9);
	CHECK(constAlias[0].get<int>() == 10);
	JSON rootCopy = json;
	rootCopy["values"][0] = 11;
	CHECK(json["values"][0].get<int>() == 9);
	json["values"].clear();
	CHECK(json["values"].isArray());
	CHECK(json["values"].size() == 0);
}

TEST_CASE("JSON.iteration")
{
	JSON object = JSON::Parse(R"({"z":1,"a":2,"日本語":3})");
	Array<String> keys;
	for (auto [key, value] : object)
	{
		keys.push_back(key);
		value = (value.get<int>() * 2);
	}
	CHECK(keys == Array<String>{ U"z", U"a", U"日本語" });
	CHECK(object["a"].get<int>() == 4);
	const JSON array = JSON::Parse("[10,20,30]");
	auto it = array.begin();
	CHECK(it.key() == U"0");
	CHECK((it++).value().get<int>() == 10);
	CHECK(it.key() == U"1");
	CHECK((++it).value().get<int>() == 30);
	CHECK((it--).key() == U"2");
	CHECK((--it).key() == U"0");
	it = array.end();
	CHECK((--it).key() == U"2");
	const JSON empty = JSON::EmptyArray();
	CHECK(empty.begin() == empty.end());
}

TEST_CASE("JSON.binaryFormatsAndFlatten")
{
	const JSON json = JSON::Parse(R"({"name":"日本語 😀","items":[1,-2,3.5,true,null],"nested":{"a/b":{"~key":7}}})");
	CHECK(JSON::FromBSON(json.toBSON()) == json);
	CHECK(JSON::FromCBOR(json.toCBOR()) == json);
	CHECK(JSON::FromMessagePack(json.toMessagePack()) == json);
	CHECK(json.flatten().unflatten() == json);
	CHECK(json.flatten()["/nested/a~1b/~0key"].get<int>() == 7);
	for (const Blob& bytes : { Blob{}, Blob{ "\xC1", 1 } })
	{
		CHECK(JSON::FromBSON(bytes).isInvalid());
		CHECK(JSON::FromCBOR(bytes).isInvalid());
		CHECK(JSON::FromMessagePack(bytes).isInvalid());
		CHECK_THROWS_AS(JSON::FromBSON(bytes, AllowExceptions::Yes), Error);
		CHECK_THROWS_AS(JSON::FromCBOR(bytes, AllowExceptions::Yes), Error);
		CHECK_THROWS_AS(JSON::FromMessagePack(bytes, AllowExceptions::Yes), Error);
	}
}

TEST_CASE("JSON.templatesAndStringInputs")
{
	int fallback = 42;
	const int constFallback = 43;
	CHECK(JSON("x").getOr<int>(fallback) == 42);
	CHECK(JSON("x").getOr<int>(constFallback) == 43);
	CHECK(FromJSONOr(JSON("x"), fallback) == 42);
	CHECK(FromJSONOr<int>(JSON("x"), fallback) == 42);
	CHECK(FromJSONOr<int>(JSON("x"), constFallback) == 43);
	CHECK(FromJSONOr(JSON("x"), 44) == 44);
	CHECK(JSON(1).getOr<String>(U"fallback") == U"fallback");
	const String text = U"fallback";
	CHECK(JSON(1).getOr<String>(text) == text);
	CHECK(JSON(U"value").getOr<String>(text) == U"value");

	const Array<int> values{ 2, 3, 5 };
	CHECK(JSON(values.begin(), values.end()) == JSON(values));
	CHECK(JSON(values.end(), values.end()) == JSON::EmptyArray());
	std::istringstream stream{ "2 3 5" };
	CHECK(JSON(std::istream_iterator<int>{ stream }, std::istream_iterator<int>{}) == JSON(values));

	const char32* pointer = U"日本語 😀";
	char32 mutableString[] = U"日本語 😀";
	char32* mutablePointer = mutableString;
	const std::u32string_view view{ U"a\0b", 3 };
	CHECK(JSON(pointer) == JSON(U"日本語 😀"));
	CHECK(JSON(mutablePointer) == JSON(U"日本語 😀"));
	CHECK(JSON(view).getUTF8() == std::string("a\0b", 3));
	CHECK(JSON(view).get<std::u32string>() == std::u32string(view));
	CHECK(JSON(Array<std::u32string>{ U"a", U"日本語" }).get<Array<String>>() == Array<String>{ U"a", U"日本語" });
}

TEST_CASE("JSON.checkedAccessAndEmptyPop")
{
	const JSON object = JSON::Parse("{\"present\":null}");
	CHECK(object["present"].isNull());
	CHECK_THROWS_AS(object["absent"], Error);
	CHECK_THROWS_AS(object[U"absent"], Error);
	const JSON constArray = JSON::EmptyArray();
	CHECK_THROWS_AS(constArray["key"], Error);
	JSON empty = JSON::EmptyArray();
	CHECK_THROWS_AS(empty.pop_back(), Error);
	CHECK(empty.isArray());
	CHECK(empty.size() == 0);
	JSON scalar = 42;
	CHECK_THROWS_AS(scalar.pop_back(), Error);
	CHECK(scalar.get<int>() == 42);
	CHECK_THROWS_AS(scalar.getString(), Error);
	CHECK_THROWS_AS(scalar.getUTF8(), Error);
	CHECK_THROWS_AS(scalar.getBinary(), Error);
	const JSON malformedString = std::string("\xFF", 1);
	CHECK_THROWS_AS(malformedString.get<int>(), Error);
}

TEST_CASE("JSON.nullCommentsDuplicatesAndClear")
{
	const JSON null = JSON::Parse("null");
	CHECK(null.isNull());
	CHECK_FALSE(null.isInvalid());
	CHECK_FALSE(static_cast<bool>(null));
	JSON json = 1;
	CHECK(json.parse("null"));
	CHECK_FALSE(static_cast<bool>(json));
	CHECK_FALSE(JSON::Invalid());
	CHECK(JSON(false));
	CHECK(JSON(0));
	CHECK(JSON(""));
	CHECK(JSON::EmptyArray());
	CHECK(JSON::EmptyObject());
	CHECK(JSON::Parse("/* config */ {\"x\":1, // first\n\"x\":2}")["x"].get<int>() == 2);
	CHECK(JSON::Parse("/* unterminated").isInvalid());
	CHECK(JSON::Parse("[1,/* comment */]").isInvalid());
	for (const JSON& initial : { JSON(12), JSON(1.5), JSON(true), JSON("abc"), JSON::EmptyObject(), JSON::EmptyArray(), JSON::EmptyBinary() })
	{
		JSON value = initial.clone();
		value.clear();
		CHECK(value.getType() == initial.getType());
	}
	CHECK(JSON(nullptr).size() == 0);
	CHECK(JSON("abc").size() == 1);
	CHECK_FALSE(JSON(1).contains("x"));
}

namespace
{
	constexpr FilePathView JSONFixture = U"../../Test/data/json/";

	class JSONChunkReader : public MemoryViewReader
	{
	public:
		using MemoryViewReader::MemoryViewReader;
		using MemoryViewReader::read;

		int64 read(void* dst, int64 size) override
		{
			return MemoryViewReader::read(dst, Min(size, int64{ 3 }));
		}
	};

	class JSONIncompleteReader : public MemoryViewReader
	{
	public:
		using MemoryViewReader::MemoryViewReader;

		int64 size() const override
		{
			return (MemoryViewReader::size() + 1);
		}
	};
}

TEST_CASE("JSON.Load.filesAndReaders")
{
	const JSON json = JSON::Load(String{ JSONFixture } + U"sample.json", AllowExceptions::Yes);
	REQUIRE(json.isObject());
	CHECK(json["title"].getString() == U"設定 😀");
	CHECK(json["players"].size() == 2);
	CHECK(json["players"][1]["inventory"][0].getString() == U"鍵");
	CHECK(json["limits"]["maxUnsigned"].get<uint64>() == std::numeric_limits<uint64>::max());
	CHECK(json["special"]["a/b"]["~key"].getString() == U"value");
	CHECK(json["empty"].isObject());
	CHECK(json["optional"].isNull());

	const JSON expected = JSON::Parse(U"{\"text\":\"日本語 😀\",\"values\":[1,true,null]}\r\n");
	for (const StringView name : { U"utf8-bom.json", U"utf16le.json", U"utf16be.json" })
	{
		const FilePath path = (String{ JSONFixture } + name);
		INFO(path.toUTF8());
		CHECK(JSON::Load(path) == expected);
		JSON value;
		CHECK(value.load(path));
		CHECK(value == expected);
		CHECK(JSON::Load(BinaryFileReader{ path }) == expected);
	}

	for (const StringView name : { U"invalid-cr-token.json", U"invalid-cr-string.json", U"invalid-nul.json", U"invalid-utf8.json", U"invalid-truncated.json", U"invalid-utf16-odd.json", U"invalid-utf16-surrogate.json", U"nonexistent.json" })
	{
		const FilePath path = (String{ JSONFixture } + name);
		INFO(path.toUTF8());
		CHECK(JSON::Load(path).isInvalid());
		CHECK_THROWS_AS(JSON::Load(path, AllowExceptions::Yes), Error);
		JSON value = 42;
		CHECK_FALSE(value.load(path));
		CHECK(value.isInvalid());
		value = 42;
		CHECK_THROWS_AS(value.load(path, AllowExceptions::Yes), Error);
		CHECK(value.isInvalid());
	}

	const std::string source = expected.formatUTF8Minified();
	CHECK(JSON::Load(JSONChunkReader{ source.data(), source.size() }) == expected);
	CHECK(JSON::Load(JSONIncompleteReader{ source.data(), source.size() }).isInvalid());
	CHECK_THROWS_AS(JSON::Load(JSONIncompleteReader{ source.data(), source.size() }, AllowExceptions::Yes), Error);
	CHECK(JSON::Load(std::unique_ptr<IReader>{}).isInvalid());
	CHECK_THROWS_AS(JSON::Load(std::unique_ptr<IReader>{}, AllowExceptions::Yes), Error);
	JSON value;
	CHECK(value.load(MemoryViewReader{ source.data(), source.size() }));
	CHECK(value == expected);
	CHECK_FALSE(value.load(std::unique_ptr<IReader>{}));
	CHECK(value.isInvalid());
	CHECK(JSON::Load(MemoryViewReader{ "null", 4 }).isNull());
	MemoryViewReader positioned{ "prefix null", 11 };
	positioned.setPos(7);
	CHECK(JSON::Load(std::move(positioned)).isNull());
	CHECK(JSON::Load(MemoryViewReader{ "", 0 }).isInvalid());
}

TEST_CASE("JSON.saveAndFormatting")
{
	const JSON json = JSON::Load(String{ JSONFixture } + U"sample.json");
	REQUIRE_FALSE(json.isInvalid());
	const FilePath path = Test::OutputPath(U"json/日本語.json");
	REQUIRE(json.save(path));
	CHECK(JSON::Load(path) == json);
	REQUIRE(json.saveMinified(path, EnsureAscii::Yes));
	CHECK(JSON::Load(path) == json);
	CHECK(JSON::Parse(json.format(U'\t', 1)) == json);
	CHECK(JSON::Parse(json.formatUTF8(U' ', 0)) == json);
	const std::string ascii = json.formatUTF8Minified(EnsureAscii::Yes);
	CHECK(std::all_of(ascii.begin(), ascii.end(), [](const unsigned char c) { return (c < 128); }));
	CHECK_FALSE(json.save(Test::OutputPath(U"json/")));
	CHECK_FALSE(json.saveMinified(Test::OutputPath(U"json/")));

	const Blob before{ path };
	const JSON invalidString = std::string("\xFF", 1);
	CHECK_THROWS(invalidString.save(path));
	CHECK(Blob{ path } == before);
	CHECK_THROWS(invalidString.saveMinified(path));
	CHECK(Blob{ path } == before);
}

TEST_CASE("JSON.binaryDataAndFormatLimits")
{
	const JSON binary(JSON::json_base::binary({ 0, 1, 127, 128, 255 }));
	CHECK(binary.isBinary());
	CHECK(binary.getBinary() == Blob{ "\0\1\x7F\x80\xFF", 5 });
	CHECK(JSON::FromCBOR(binary.toCBOR()) == binary);
	CHECK(JSON::FromMessagePack(binary.toMessagePack()) == binary);
	JSON object;
	object["bytes"] = binary;
	CHECK(JSON::FromBSON(object.toBSON())["bytes"].getBinary() == binary.getBinary());
	CHECK_THROWS(JSON(42).toBSON());
	const JSON unsignedObject = JSON::Parse("{\"x\":18446744073709551615}");
	const Blob unsignedBSON = unsignedObject.toBSON();
	CHECK(unsignedBSON[4] == Byte{ 0x11 });
	CHECK(JSON::FromBSON(unsignedBSON) == unsignedObject);

	// flatten represents empty containers as null; this operation is intentionally lossy.
	const JSON emptyContainers = JSON::Parse("{\"a\":[],\"o\":{}}");
	CHECK(emptyContainers.flatten().unflatten() == JSON::Parse("{\"a\":null,\"o\":null}"));
	CHECK_THROWS(JSON::Parse("{\"not-a-pointer\":1}").unflatten());
	CHECK(JSON(std::numeric_limits<double>::infinity()).formatUTF8Minified() == "null");
	CHECK(JSON(std::numeric_limits<double>::quiet_NaN()).formatUTF8Minified() == "null");
	// Numeric get() follows the underlying conversion rules; it is not range validation.
	CHECK(JSON(256).get<uint8>() == 0);
	CHECK(JSON(1.75).get<int>() == 1);
	CHECK(JSON::Parse("{\"a\":1,\"b\":2}") != JSON::Parse("{\"b\":2,\"a\":1}"));
	CHECK(JSON::Parse("{\"0\":\"value\"}").flatten().unflatten() == JSON::Parse("[\"value\"]"));
}

TEST_CASE("JSON.generatedRoundTrips")
{
	JSON array = JSON::EmptyArray();
	for (int i = 0; i < 256; ++i)
	{
		JSON item;
		item["id"] = i;
		item["text"] = (U"項目 😀 " + ToString(i));
		item["number"] = (i * -0.125);
		item["enabled"] = ((i % 2) == 0);
		item["values"] = Array<int>{ i, -i, (i * i) };
		item["optional"] = nullptr;
		array.push_back(item);
	}
	CHECK(JSON::Parse(array.formatUTF8Minified()) == array);
	CHECK(JSON::Parse(array.formatUTF8(U' ', 4, EnsureAscii::Yes)) == array);
	CHECK(JSON::FromCBOR(array.toCBOR()) == array);
	CHECK(JSON::FromMessagePack(array.toMessagePack()) == array);
	std::string nested(128, '[');
	nested += "42";
	nested.append(128, ']');
	const JSON deep = JSON::Parse(nested);
	REQUIRE_FALSE(deep.isInvalid());
	CHECK(deep.formatUTF8Minified() == nested);
}

TEST_CASE("JSON.assignmentAndSwap")
{
	JSON original = JSON::Parse("{\"a\":1,\"b\":[2,3]}");
	JSON copied;
	copied = original;
	copied["a"] = 4;
	CHECK(original["a"].get<int>() == 1);
	JSON moved;
	moved = std::move(copied);
	CHECK(moved["a"].get<int>() == 4);
	CHECK(copied.isNull());
	JSON& self = moved;
	moved = std::move(self);
	CHECK(moved["a"].get<int>() == 4);
	JSON child = original["b"];
	JSON destination;
	destination = std::move(child);
	CHECK(destination == JSON::Parse("[2,3]"));
	CHECK(original["b"] == destination);
	original["a"] = std::move(destination);
	CHECK(original["a"] == JSON::Parse("[2,3]"));
	JSON first = original["a"];
	JSON second = original["b"];
	first.swap(second);
	first[0] = 8;
	CHECK(original["b"][0].get<int>() == 8);
	CHECK(original["a"][0].get<int>() == 2);
	JSON object = JSON::EmptyObject();
	for (int i = 0; i < 64; ++i)
	{
		object[ToString(i)] = i;
	}
	object["copy"] = object["0"].clone();
	CHECK(object["copy"].get<int>() == 0);
	JSON overlap = JSON::Parse("{\"child\":{\"value\":1}}");
	overlap = overlap["child"];
	CHECK(overlap["value"].get<int>() == 1);
	JSON parent = JSON::Parse("{\"child\":1}");
	parent["child"] = std::move(parent);
	CHECK(parent["child"]["child"].get<int>() == 1);
}

TEST_CASE("JSON.stringEscapesAndByteBoundaries")
{
	for (char c = 0; c < 32; ++c)
	{
		const std::string raw{ '\"', c, '\"' };
		CHECK(JSON::Parse(raw).isInvalid());
		const JSON escaped(std::string(1, c));
		CHECK(JSON::Parse(escaped.formatUTF8Minified()).getUTF8() == std::string(1, c));
	}
	for (const std::string_view bytes : { "\x80", "\xC2", "\xE0\x80\x80", "\xED\xA0\x80", "\xF4\x90\x80\x80" })
	{
		CHECK(JSON::Parse("\"" + std::string(bytes) + "\"").isInvalid());
	}
	const JSON unicode = JSON::Parse(R"("\u0000\u007f\u0080\u07ff\u0800\uffff\udbff\udfff")");
	REQUIRE(unicode.isString());
	CHECK(unicode.getString() == StringView{ U"\0\x7f\x80\x7ff\x800\xffff\U0010ffff", 7 });
	CHECK(JSON::Parse(unicode.formatUTF8Minified(EnsureAscii::Yes)) == unicode);
	CHECK(JSON::Parse("1.7976931348623157e308").get<double>() == std::numeric_limits<double>::max());
	CHECK(JSON::Parse("4.9406564584124654e-324").get<double>() == std::numeric_limits<double>::denorm_min());
	const JSON literal = U"{\"value\":42}"_json;
	CHECK(literal["value"].get<int>() == 42);
	CHECK("null"_json.isNull());
	CHECK(JSON{ 42 } == JSON::Parse("[42]"));
	CHECK(JSON{ { "x", 42 } } == JSON::Parse("{\"x\":42}"));
}
