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

# include <array>
# include <Siv3D/DriveType.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array DriveTypeStrings =
		{
			U"Unknown"_sv,
			U"Removable"_sv,
			U"Fixed"_sv,
			U"Network"_sv,
			U"CDROM"_sv,
			U"RAMDisk"_sv,
			U"Virtual"_sv,
		};
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const DriveType value)
	{
		formatData.string.append(DriveTypeStrings[FromEnum(value)]);
	}
}
