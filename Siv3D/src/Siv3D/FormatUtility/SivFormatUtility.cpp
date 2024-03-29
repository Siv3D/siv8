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

# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/FloatToString.hpp>

namespace s3d
{
	namespace
	{
		static constexpr StringView BytesUnits[9] =
		{
			U"B",
			U"KiB",
			U"MiB",
			U"GiB",
			U"TiB",
			U"PiB",
			U"EiB",
			U"ZiB",
			U"YiB",
		};
	}
	
	namespace detail
	{
		String PadLeft(String&& s, const std::pair<int32, char32>& padding)
		{
			const int32 targetLength = padding.first;

			if (static_cast<int32>(s.length()) < targetLength)
			{
				const int32 paddingCount = (targetLength - static_cast<int32>(s.length()));

				s.insert(s.begin(), paddingCount, padding.second);
			}

			return s;
		}
	}

	String FormatDataSize(const int64 bytes)
	{
		double s = static_cast<double>(bytes);

		for (uint32 i = 0; i < 9; ++i)
		{
			if (s < 10.0)
			{
				return (ToString(s, 2) + BytesUnits[i]);
			}
			else if (s < 100.0)
			{
				return (ToString(s, 1) + BytesUnits[i]);
			}
			else if (s < 1024.0)
			{
				return (ToString(s, 0) + BytesUnits[i]);
			}

			s /= 1024.0;
		}

		return{};
	}
}
