//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Formatter.hpp>
# include <Siv3D/IntFormatter.hpp>
# include <Siv3D/FloatFormatter.hpp>

namespace s3d
{
	constexpr StringView BoolS[2] = { U"false", U"true" };

	constexpr StringView NullS{ U"null" };

	constexpr StringView LtS{ U"LT" };

	constexpr StringView GtS{ U"GT" };

	constexpr StringView EqS{ U"EQ" };

	constexpr StringView UnS{ U"UN" };

	void Formatter(FormatData& formatData, const bool value)
	{
		formatData.string.append(BoolS[value]);
	}

	void Formatter(FormatData& formatData, const int8 value)
	{
		Formatter(formatData, static_cast<int32>(value));
	}

	void Formatter(FormatData& formatData, const uint8 value)
	{
		Formatter(formatData, static_cast<uint32>(value));
	}

	void Formatter(FormatData& formatData, const int16 value)
	{
		Formatter(formatData, static_cast<int32>(value));
	}

	void Formatter(FormatData& formatData, const uint16 value)
	{
		Formatter(formatData, static_cast<uint32>(value));
	}

	void Formatter(FormatData& formatData, const int32 value)
	{
		const IntFormatter formatter{ value };	
		formatData.string.append(formatter.data(), (formatter.data() + formatter.size()));
	}

	void Formatter(FormatData& formatData, const uint32 value)
	{
		const IntFormatter formatter{ value };	
		formatData.string.append(formatter.data(), (formatter.data() + formatter.size()));
	}

	void Formatter(FormatData& formatData, const long value)
	{
		if constexpr (sizeof(long) == sizeof(int32))
		{
			Formatter(formatData, static_cast<int32>(value));
		}
		else
		{
			Formatter(formatData, static_cast<long long>(value));
		}
	}

	void Formatter(FormatData& formatData, const unsigned long value)
	{
		if constexpr (sizeof(unsigned long) == sizeof(uint32))
		{
			Formatter(formatData, static_cast<uint32>(value));
		}
		else
		{
			Formatter(formatData, static_cast<unsigned long long>(value));
		}
	}

	void Formatter(FormatData& formatData, const long long value)
	{
		const IntFormatter formatter{ static_cast<int64>(value) };	
		formatData.string.append(formatter.data(), (formatter.data() + formatter.size()));
	}

	void Formatter(FormatData& formatData, const unsigned long long value)
	{
		const IntFormatter formatter{ static_cast<uint64>(value) };	
		formatData.string.append(formatter.data(), (formatter.data() + formatter.size()));
	}

	void Formatter(FormatData& formatData, const float value)
	{
		Formatter(formatData, static_cast<double>(value));
	}

	void Formatter(FormatData& formatData, const double value)
	{
		const FloatFormatter formatter{ value, formatData.decimalPlaces.value, false };
		formatData.string.append(formatter.data(), (formatter.data() + formatter.size()));
	}

	void Formatter(FormatData& formatData, const long double value)
	{
		Formatter(formatData, static_cast<double>(value));
	}

	void Formatter(FormatData& formatData, const char ch)
	{
		formatData.string.push_back(static_cast<char32>(ch));
	}

	void Formatter(FormatData& formatData, const char8_t ch)
	{
		formatData.string.push_back(static_cast<char32>(ch));
	}

	void Formatter(FormatData& formatData, const char16_t ch)
	{
		formatData.string.push_back(static_cast<char32>(ch));
	}

	void Formatter(FormatData& formatData, const wchar_t ch)
	{
		formatData.string.push_back(static_cast<char32>(ch));
	}

	void Formatter(FormatData& formatData, const char32_t ch)
	{
		formatData.string.push_back(static_cast<char32>(ch));
	}

	void Formatter(FormatData& formatData, std::nullptr_t)
	{
		formatData.string.append(NullS);
	}

	void Formatter(FormatData& formatData, const char32_t* s)
	{
		formatData.string.append(s);
	}

	void Formatter(FormatData& formatData, const std::u32string& s)
	{
		formatData.string.append(s);
	}

	void Formatter(FormatData& formatData, const std::strong_ordering value)
	{
		Formatter(formatData, static_cast<std::weak_ordering>(value));
	}

	void Formatter(FormatData& formatData, const std::weak_ordering value)
	{
		formatData.string.append(std::is_lt(value) ? LtS : std::is_gt(value) ? GtS : EqS);
	}

	void Formatter(FormatData& formatData, const std::partial_ordering value)
	{
		formatData.string.append(std::is_lt(value) ? LtS : std::is_gt(value) ? GtS : std::is_eq(value) ? EqS : UnS);
	}
}
