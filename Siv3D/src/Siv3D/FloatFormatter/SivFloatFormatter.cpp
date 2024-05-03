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

# include <charconv>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/String.hpp>
# include <ThirdParty/double-conversion/double-to-string.h>

namespace s3d
{
	constexpr int FormatFlags = (double_conversion::DoubleToStringConverter::UNIQUE_ZERO |
		double_conversion::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN);

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	FloatFormatter::FloatFormatter(const double value, const int32 decimalPlace, const bool fixed)
		: m_view{ formatFloat(value, decimalPlace, fixed) } {}

	////////////////////////////////////////////////////////////////
	//
	//	formatFloat
	//
	////////////////////////////////////////////////////////////////

	std::string_view FloatFormatter::formatFloat(const double value, const int32 decimalPlace, const bool fixed)
	{
		double_conversion::DoubleToStringConverter conv{ FormatFlags, "inf", "nan", 'e', -324, 309, 0, 0 };

		double_conversion::StringBuilder builder{ m_buffer, FormatFloatBufferSize };

		if (conv.ToFixed(value, decimalPlace, &builder))
		{
			const int length = builder.position();
			const char* p = builder.Finalize();
			const char* end = (p + length);

			if ((not fixed) && (decimalPlace != 0))
			{
				while (*(end - 1) == '0')
				{
					--end;
				}

				if (*(end - 1) == '.')
				{
					--end;
				}
			}

			if (((end - p) == 2) && (p[0] == '-') && (p[1] == '0'))
			{
				++p;
			}

			return{ p, static_cast<size_t>(end - p) };
		}
		else
		{
			conv.ToShortest(value, &builder);
			const size_t length = builder.position();
			const char* p = builder.Finalize();
			return{ p, length };
		}
	}

	namespace detail
	{
		void AppendFloat(String& dst, const float value)
		{
			char buffer[50];

			if (const auto result = std::to_chars(buffer, (buffer + std::size(buffer)), value, std::chars_format::fixed);
				result.ec == std::errc{})
			{
				dst.append(buffer, result.ptr);
			}
		}

		void AppendFloat(String& dst, const double value)
		{
			char buffer[328];

			if (const auto result = std::to_chars(buffer, (buffer + std::size(buffer)), value, std::chars_format::fixed);
				result.ec == std::errc{})
			{
				dst.append(buffer, result.ptr);
			}
		}
	}
}
