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

# include <array>
# include <Siv3D/Anchor.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array AnchorStrings =
		{
			U"BottomRight"_sv,
			U"BottomCenter"_sv,
			U"BottomLeft"_sv,
			U"MiddleRight"_sv,
			U"Center"_sv,
			U"MiddleLeft"_sv,
			U"TopRight"_sv,
			U"TopCenter"_sv,
			U"TopLeft"_sv,
		};

		[[nodiscard]]
		static constexpr uint8 Remap(const uint8 value) noexcept
		{
			const uint8 v = ((value >> 3) / 2);
			const uint8 h = ((value & 0b000'111) / 2);
			return (v * 3 + h);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Anchor value)
	{
		formatData.string.append(AnchorStrings[Remap(value.m_value)]);
	}
}
