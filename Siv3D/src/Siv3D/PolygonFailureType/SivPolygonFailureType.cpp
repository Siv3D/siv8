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

# include <array>
# include <Siv3D/PolygonFailureType.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array PolygonFailureTypeStrings =
		{
			U"OK"_sv,
			U"FewPoints"_sv,
			U"WrongTopologicalDimension"_sv,
			U"Spikes"_sv,
			U"DuplicatePoints"_sv,
			U"NotClosed"_sv,
			U"SelfIntersections"_sv,
			U"WrongOrientation"_sv,
			U"InteriorRingsOutside"_sv,
			U"NestedInteriorRings"_sv,
			U"DisconnectedInterior"_sv,
			U"IntersectingInteriors"_sv,
			U"WrongCornerOrder"_sv,
			U"InvalidCoordinate"_sv,
			U"Unknown"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const PolygonFailureType value)
	{
		formatData.string.append(PolygonFailureTypeStrings[FromEnum(value)]);
	}
}
