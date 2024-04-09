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

		CHECK_EQ(j_null.getType(), JSONValueType::Null);
		CHECK_EQ(j_object.getType(), JSONValueType::Object);
		CHECK_EQ(j_array.getType(), JSONValueType::Array);
		CHECK_EQ(j_string.getType(), JSONValueType::String);
		CHECK_EQ(j_boolean.getType(), JSONValueType::Bool);
		CHECK_EQ(j_number_int.getType(), JSONValueType::NumberInt);
		CHECK_EQ(j_number_uint.getType(), JSONValueType::NumberUint);
		CHECK_EQ(j_number_float.getType(), JSONValueType::NumberFloat);
		CHECK_EQ(j_binary.getType(), JSONValueType::Binary);

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
		const JSON j_object(JSON::MakeObject());
		const JSON j_array(Array<int32>{ 1, 2, 3, 4, 5 });
		const JSON j_string("Hello, Siv3D!");
		const JSON j_boolean(true);
		const JSON j_number_int(42);
		const JSON j_number_uint(42u);
		const JSON j_number_float(42.0);

		CHECK_EQ(j_null.getType(), JSONValueType::Null);
		CHECK_EQ(j_object.getType(), JSONValueType::Object);
		CHECK_EQ(j_array.getType(), JSONValueType::Array);
		CHECK_EQ(j_string.getType(), JSONValueType::String);
		CHECK_EQ(j_boolean.getType(), JSONValueType::Bool);
		CHECK_EQ(j_number_int.getType(), JSONValueType::NumberInt);
		CHECK_EQ(j_number_uint.getType(), JSONValueType::NumberUint);
		CHECK_EQ(j_number_float.getType(), JSONValueType::NumberFloat);
	}

	{
		const JSON j_null = nullptr;
		const JSON j_object = JSON::MakeObject();
		const JSON j_array = Array<int32>{ 1, 2, 3, 4, 5 };
		const JSON j_string = "Hello, Siv3D!";
		const JSON j_boolean = true;
		const JSON j_number_int = 42;
		const JSON j_number_uint = 42u;
		const JSON j_number_float = 42.0;

		CHECK_EQ(j_null.getType(), JSONValueType::Null);
		CHECK_EQ(j_object.getType(), JSONValueType::Object);
		CHECK_EQ(j_array.getType(), JSONValueType::Array);
		CHECK_EQ(j_string.getType(), JSONValueType::String);
		CHECK_EQ(j_boolean.getType(), JSONValueType::Bool);
		CHECK_EQ(j_number_int.getType(), JSONValueType::NumberInt);
		CHECK_EQ(j_number_uint.getType(), JSONValueType::NumberUint);
		CHECK_EQ(j_number_float.getType(), JSONValueType::NumberFloat);
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
		CHECK_EQ(j, json);

		j["a"] = std::string_view{ "Siv3D" };
		CHECK_EQ(j, json);

		j["a"] = std::string{ "Siv3D" };
		CHECK_EQ(j, json);

		const char* str = "Siv3D";
		j["a"] = str;
		CHECK_EQ(j, json);

		j["a"] = U"Siv3D";
		CHECK_EQ(j, json);

		j["a"] = std::u32string_view{ U"Siv3D" };
		CHECK_EQ(j, json);

		j["a"] = StringView{ U"Siv3D" };
		CHECK_EQ(j, json);

		j["a"] = String{ U"Siv3D" };
		CHECK_EQ(j, json);

		const char32* str32 = U"Siv3D";
		j["a"] = str32;
		CHECK_EQ(j, json);
	}
}
