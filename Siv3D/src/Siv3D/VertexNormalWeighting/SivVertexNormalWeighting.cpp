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
# include <Siv3D/VertexNormalWeighting.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array VertexNormalWeightingStrings =
		{
			U"Angle"_sv,
			U"Area"_sv,
			U"Uniform"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const VertexNormalWeighting value)
	{
		formatData.string.append(VertexNormalWeightingStrings[FromEnum(value)]);
	}
}
