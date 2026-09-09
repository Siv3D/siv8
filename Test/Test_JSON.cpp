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
