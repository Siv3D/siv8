//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/ThousandSeparate.hpp>
# include <Siv3D/FloatToString.hpp>
# include <Siv3D/Int128.hpp>
# include <Siv3D/Format.hpp>

namespace s3d
{
	namespace detail
	{
		String ThousandSeparateInt(const String& value, const char32 separator)
		{
			const bool isNegative = (value.starts_with(U'-'));
			const size_t digits = (value.length() - (isNegative ? 1 : 0));

			if (digits <= 3)
			{
				return value;
			}

			const size_t count = ((digits - 1) / 3);
			const size_t resultLength = (value.length() + count);

			String result;
			result.reserve(resultLength);

			if (isNegative)
			{
				result.push_back(U'-');
			}

			const char32* pSrc = value.c_str();

			if (isNegative)
			{
				++pSrc;
			}

			for (size_t i = 0; i < digits; ++i)
			{
				result.push_back(*pSrc++);

				if (((digits - i - 1) % 3 == 0) && (i != (digits - 1)))
				{
					result.push_back(separator);
				}
			}

			return result;
		}

		String ThousandsSeparateFloat(String&& value, const char32 separator)
		{
			String result = std::move(value);
			const size_t zeroPos = result.rfind(U'.');
			const size_t startIndex = ((zeroPos == String::npos) ? result.size() : zeroPos);
			const size_t endIndex = (result.starts_with(U'-') ? 1 : 0);

			size_t count = 0;

			for (size_t i = startIndex; i != endIndex; --i)
			{
				if (++count == 4)
				{
					result.insert(i, 1, separator);
					count = 1;
				}
			}

			return result;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ThousandsSeparate
	//
	////////////////////////////////////////////////////////////////

	String ThousandSeparate(const float value, const int32 decimalPlace, const FixDecimalPlace fixDecimalPlace, const char32 separator)
	{
		return detail::ThousandsSeparateFloat(fixDecimalPlace ? ToFixed(value, decimalPlace) : ToString(value, decimalPlace), separator);
	}

	String ThousandSeparate(const double value, const int32 decimalPlace, const FixDecimalPlace fixDecimalPlace, const char32 separator)
	{
		return detail::ThousandsSeparateFloat(fixDecimalPlace ? ToFixed(value, decimalPlace) : ToString(value, decimalPlace), separator);
	}

	String ThousandSeparate(const int128& value, const char32 separator)
	{
		return detail::ThousandSeparateInt(Format(value), separator);
	}

	String ThousandSeparate(const uint128& value, const char32 separator)
	{
		return detail::ThousandSeparateInt(Format(value), separator);
	}
}
