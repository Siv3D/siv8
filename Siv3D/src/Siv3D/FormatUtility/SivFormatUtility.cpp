﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/FormatUtility.hpp>

namespace s3d
{
	namespace detail
	{
		String Lpad(String&& s, const std::pair<int32, char32>& padding)
		{
			const int32 targetLength = padding.first;

			if (static_cast<int32>(s.length()) < targetLength)
			{
				const int32 paddingCount = (targetLength - static_cast<int32>(s.length()));

				s.insert(s.begin(), paddingCount, padding.second);
			}

			return s;
		}

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
	}
}