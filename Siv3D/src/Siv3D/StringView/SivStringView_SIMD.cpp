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

# include <Siv3D/StringView.hpp>

# if SIV3D_CPU(X86_64)
#	include <emmintrin.h>
# endif

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	StringEquals
	//
	////////////////////////////////////////////////////////////////

	bool StringView::StringEquals(const char32* s1, const char32* s2, size_t length) noexcept
	{
	# if SIV3D_CPU(X86_64)

		const uint32* p1 = reinterpret_cast<const uint32*>(s1);
		const uint32* p2 = reinterpret_cast<const uint32*>(s2);

		while (4 <= length)
		{
			__m128i v1 = ::_mm_loadu_si128(reinterpret_cast<const __m128i*>(p1));
			__m128i v2 = ::_mm_loadu_si128(reinterpret_cast<const __m128i*>(p2));
			__m128i result = ::_mm_cmpeq_epi32(v1, v2);

			if (::_mm_movemask_epi8(result) != 0xFFFF)
			{
				return false;
			}

			p1 += 4;
			p2 += 4;
			length -= 4;
		}

		for (size_t i = 0; i < length; ++i)
		{
			if (p1[i] != p2[i])
			{
				return false;
			}
		}

		return true;

	# else

		return (std::memcmp(s1, s2, (length * sizeof(char32))) == 0);

	# endif
	}
}
