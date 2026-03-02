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

# pragma once

namespace s3d
{
	namespace detail
	{
		[[noreturn]]
		void ThrowJSONGetError(const char* type, std::string_view s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
		requires std::constructible_from<JSON::json_base, Type>
	JSON::JSON(Type&& value)
		: m_json(std::forward<Type>(value)) {}

	template <std::input_iterator Iterator>
	JSON::JSON(Iterator first, Iterator last)
		: m_json(first, last) {}

	////////////////////////////////////////////////////////////////
	//
	//	load
	//
	////////////////////////////////////////////////////////////////

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	bool JSON::load(Reader&& reader, const AllowExceptions allowExceptions)
	{
		return load(std::make_unique<Reader>(std::move(reader)), allowExceptions);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator = 
	//
	////////////////////////////////////////////////////////////////

	JSON& JSON::operator =(auto&& value)
	{
		getRef() = std::forward<decltype(value)>(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	get
	//
	////////////////////////////////////////////////////////////////

	/// @brief 値を Type 型として取得します。
	/// @tparam Type 値の型
	/// @return 値
	/// @throw Error JSON の値を Type 型に変換できない場合
	template <class Type>
	Type JSON::get() const
	{
		return FromJSON<Type>(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOr
	//
	////////////////////////////////////////////////////////////////

	/// @brief 値を Type 型として取得します。取得できなかった場合は defaultValue を返します。
	/// @tparam Type 値の型
	/// @tparam U 取得できなかった場合に返す値の型
	/// @param defaultValue 取得できなかった場合に返す値
	/// @return 値
	template <class Type, class U>
	Type JSON::getOr(U&& defaultValue) const
	{
		return FromJSONOr<Type>(*this, std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOpt
	//
	////////////////////////////////////////////////////////////////

	/// @brief 値を Type 型として取得します。
	/// @tparam Type 値の型
	/// @return 値。取得できなかった場合は none
	template <class Type>
	Optional<Type> JSON::getOpt() const
	{
		return FromJSONOpt<Type>(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	JSON JSON::Load(Reader&& reader, const AllowExceptions allowExceptions)
	{
		return Load(std::make_unique<Reader>(std::move(reader)), allowExceptions);
	}

	////////////////////////////////////////////////////////////////
	//
	//	ToJSON
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
		requires std::constructible_from<JSON::json_base, Type>
	JSON ToJSON(Type&& value)
	{
		return JSON(std::forward<Type>(value));
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromJSON
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Type FromJSON(const JSON& json)
	{
		try
		{
			return json.base().template get<Type>();
		}
		catch (const JSON::json_base::exception&)
		{
			detail::ThrowJSONGetError(typeid(Type).name(), json.formatUTF8Minified());
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromJSONOr
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Type FromJSONOr(const JSON& json, Type&& defaultValue)
	{
		return FromJSONOpt<Type>(json).value_or(std::forward<Type>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromJSONOpt
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Optional<Type> FromJSONOpt(const JSON& json)
	{
		try
		{
			return json.base().template get<Type>();
		}
		catch (const JSON::json_base::exception&)
		{
			return none;
		}
	}
}

