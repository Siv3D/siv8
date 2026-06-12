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
	template <class Type>
	Optional<Type> TOML::getOpt() const
	{
		if (not m_value)
		{
			return none;
		}

		if constexpr (std::is_same_v<Type, TOML>)
		{
			return *this;
		}
		else if constexpr (std::is_same_v<Type, String>)
		{
			if (not isString())
			{
				return none;
			}
			
			return Unicode::FromUTF8(toml::get<std::string>(*m_value));
		}
		else if constexpr (std::is_same_v<Type, std::string>)
		{
			if (not isString())
			{
				return none;
			}
			
			return toml::get<std::string>(*m_value);
		}
		else if constexpr (std::is_same_v<Type, bool>)
		{
			if (not isBool())
			{
				return none;
			}
			
			return toml::get<bool>(*m_value);
		}
		else if constexpr (std::is_integral_v<Type>)
		{
			if (not isInt())
			{
				return none;
			}
			
			const int64 value = toml::get<int64>(*m_value);

			if constexpr (std::is_signed_v<Type>)
			{
				if ((value < static_cast<int64>((std::numeric_limits<Type>::min)()))
					|| (static_cast<int64>((std::numeric_limits<Type>::max)()) < value))
				{
					return none;
				}
			}
			else
			{
				if (value < 0)
				{
					return none;
				}
				
				if (static_cast<uint64>((std::numeric_limits<Type>::max)()) < static_cast<uint64>(value))
				{
					return none;
				}
			}

			return static_cast<Type>(value);
		}
		else if constexpr (std::is_floating_point_v<Type>)
		{
			if (isFloat())
			{
				return static_cast<Type>(toml::get<double>(*m_value));
			}
			
			if (isInt())
			{
				return static_cast<Type>(toml::get<int64>(*m_value));
			}
			
			return none;
		}
		else if constexpr (std::is_same_v<Type, LocalTime>)
		{
			if (not isLocalTime())
			{
				return none;
			}
			
			return LocalTime::FromTOML(toml::get<toml::local_time>(*m_value));
		}
		else if constexpr (std::is_same_v<Type, Date>)
		{
			if (not isLocalDate())
			{
				return none;
			}
			
			return ToDate(toml::get<toml::local_date>(*m_value));
		}
		else if constexpr (std::is_same_v<Type, LocalDateTime>)
		{
			if (not isLocalDateTime())
			{
				return none;
			}
			
			return LocalDateTime::FromTOML(toml::get<toml::local_datetime>(*m_value));
		}
		else if constexpr (std::is_same_v<Type, OffsetDateTime>)
		{
			if (not isOffsetDateTime())
			{
				return none;
			}
			
			return OffsetDateTime::FromTOML(toml::get<toml::offset_datetime>(*m_value));
		}
		else
		{
			static_assert(AlwaysFalse<Type>, "Unsupported TOML::getOpt<Type>(). Use getParsedOpt<Type>() for string-based parsing.");
		}
	}

	template <class Type>
	Type TOML::get() const
	{
		if (auto opt = getOpt<Type>())
		{
			return *opt;
		}
		
		ThrowTOMLGetError(typeid(Type).name(), path(), type(), format());
	}

	template <class Type, class U>
	Type TOML::getOr(U&& defaultValue) const
	{
		if (auto opt = getOpt<Type>())
		{
			return *opt;
		}
		
		return Type{ std::forward<U>(defaultValue) };
	}

	template <class Type>
	Optional<Type> TOML::getParsedOpt() const
	{
		if (not isString())
		{
			return none;
		}
		
		return ParseOpt<Type>(getString());
	}

	template <class Type>
	Type TOML::getParsed() const
	{
		if (auto opt = getParsedOpt<Type>())
		{
			return *opt;
		}
		
		ThrowTOMLParsedGetError(typeid(Type).name(), path(), type(), format());
	}

	template <class Type, class U>
	Type TOML::getParsedOr(U&& defaultValue) const
	{
		if (auto opt = getParsedOpt<Type>())
		{
			return *opt;
		}
		
		return Type{ std::forward<U>(defaultValue) };
	}

	template <class Type>
	Optional<Array<Type>> TOML::getArrayOpt() const
	{
		if (not isArray())
		{
			return none;
		}

		const auto& array = m_value->as_array();
		Array<Type> result;
		result.reserve(array.size());

		for (size_t i = 0; i < array.size(); ++i)
		{
			TOML element{ m_root, &(array[i]), MakeIndexPath(m_path, i) };
			
			if constexpr (std::is_same_v<Type, TOML>)
			{
				result.push_back(element);
			}
			else
			{
				if (auto opt = element.getOpt<Type>())
				{
					result.push_back(*opt);
				}
				else
				{
					return none;
				}
			}
		}

		return result;
	}

	template <class Type>
	Array<Type> TOML::getArray() const
	{
		if (not isArray())
		{
			ThrowNotArray(typeid(Type).name(), path(), type());
		}

		const auto& array = m_value->as_array();
		Array<Type> result;
		result.reserve(array.size());

		for (size_t i = 0; i < array.size(); ++i)
		{
			TOML element{ m_root, &(array[i]), MakeIndexPath(m_path, i) };
		
			if constexpr (std::is_same_v<Type, TOML>)
			{
				result.push_back(element);
			}
			else
			{
				if (auto opt = element.getOpt<Type>())
				{
					result.push_back(*opt);
				}
				else
				{
					ThrowArrayElementGetError(typeid(Type).name(), element.path(), element.type(), element.format());
				}
			}
		}

		return result;
	}

	template <ReaderObject Reader>
	TOML TOML::Load(Reader&& reader)
	{
		return Load(std::make_unique<Reader>(std::move(reader)));
	}

	template <ReaderObject Reader>
	Result<TOML, Array<TOMLParseErrorReason>> TOML::LoadResult(Reader&& reader)
	{
		return LoadResult(std::make_unique<Reader>(std::move(reader)));
	}
}
