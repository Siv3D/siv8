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

# include <Siv3D/QuarterArcTable.hpp>

namespace s3d
{
	namespace QuarterArcTable
	{
		inline constexpr size_t TableSize = ((static_cast<size_t>(MaxQuality) * (MaxQuality + 1)) / 2); // 2016

		static const std::array<Float2, TableSize> SinCosTable = []()
		{
			std::array<Float2, TableSize> table;

			Float2* pDst = table.data();

			for (uint32 quality = MinQuality; quality <= MaxQuality; ++quality)
			{
				const float radDelta = (Math::HalfPiF / quality);

				for (uint32 i = 0; i < quality; ++i)
				{
					const float rad = (radDelta * i);
					*pDst++ = { std::sin(rad), -std::cos(rad) };
				}
			}

			return table;
		}();

		std::span<const Float2> GetUnitVectors(uint32 quality) noexcept
		{
			quality = Clamp(quality, MinQuality, MaxQuality);
			
			return{ SinCosTable.data() + ((quality * (quality - 1)) / 2), quality };
		}
	}
}
