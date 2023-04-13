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

# include <bit>
# include <charconv>
# include <Siv3D/String.hpp>
# include <Siv3D/FloatToString.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/IntToString.hpp>

namespace s3d
{
	String ToString(const float value)
	{
		char buffer[50];

		if (const auto result = std::to_chars(buffer, (buffer + std::size(buffer)), value, std::chars_format::fixed); 
			result.ec == std::errc{})
		{
			return String{ buffer, result.ptr };
		}
		else
		{
			return{};
		}
	}

	String ToString(const double value)
	{
		char buffer[328];

		if (const auto result = std::to_chars(buffer, (buffer + std::size(buffer)), value, std::chars_format::fixed);
			result.ec == std::errc{})
		{
			return String{ buffer, result.ptr };
		}
		else
		{
			return{};
		}
	}

	String ToString(const float value, const int32 decimalPlace)
	{
		return ToString(static_cast<double>(value), decimalPlace);
	}

	String ToString(const double value, const int32 decimalPlace)
	{
		const FloatFormatter formatter{ value, decimalPlace, false };
		return String(formatter.data(), (formatter.data() + formatter.size()));
	}

	String ToFixed(const float value, const int32 decimalPlace)
	{
		return ToFixed(static_cast<double>(value), decimalPlace);
	}

	String ToFixed(const double value, const int32 decimalPlace)
	{
		const FloatFormatter formatter{ value, decimalPlace, true };
		return String(formatter.data(), (formatter.data() + formatter.size()));
	}

	String ToBinary(const float value)
	{
		return ToBinary(std::bit_cast<uint32>(value));
	}

	String ToBinary(const double value)
	{
		return ToBinary(std::bit_cast<uint64>(value));
	}

	String ToOctal(const float value)
	{
		return ToOctal(std::bit_cast<uint32>(value));
	}

	String ToOctal(const double value)
	{
		return ToOctal(std::bit_cast<uint64>(value));
	}

	String ToHex(const float value, const LetterCase letterCase)
	{
		return ToHex(std::bit_cast<uint32>(value), letterCase);
	}

	String ToHex(const double value, const LetterCase letterCase)
	{
		return ToHex(std::bit_cast<uint64>(value), letterCase);
	}
}
