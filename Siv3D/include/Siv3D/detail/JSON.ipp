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

	JSON::JSON(const Concept::SignedIntegral auto value)
		: m_json(value) {}

	JSON::JSON(const Concept::UnsignedIntegral auto value)
		: m_json(value) {}

	JSON::JSON(const Concept::FloatingPoint auto value)
		: m_json(value) {}

	template <class Iterator>
	JSON::JSON(Iterator first, Iterator last)
		: m_json(first, last) {}

	template <JSONCompatibleType Type>
	JSON::JSON(const Array<Type>& arr)
		: m_json(arr.map([](auto&& v) { return json_base(v); })) {}

	template <JSONCompatibleType Type>
	JSON::JSON(const std::initializer_list<Type>& list)
		: m_json(list) {}

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
		if (const auto opt = getOpt<Type>())
		{
			return *opt;
		}
		else
		{
			detail::ThrowJSONGetError(typeid(Type).name(), formatUTF8Minimum());
		}
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
		return getOpt<Type>().value_or(std::forward<U>(defaultValue));
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
		if constexpr (std::is_same_v<Type, String>)
		{
			if (not isString())
			{
				return none;
			}

			return Unicode::FromUTF8(get().get<std::string>());
		}
		else if constexpr (std::is_same_v<Type, std::string>)
		{
			if (not isString())
			{
				return none;
			}

			return get().get<std::string>();
		}
		else if constexpr (std::is_arithmetic_v<Type>)
		{
			return get().get<Type>();
		}
		else
		{
			if (not isString())
			{
				return none;
			}

			return ParseOpt<Type>(get().get<std::string>());
		}
	}
}

