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
# include <Siv3D/BlendFactor.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array BlendFactorStrings =
		{
			U""_sv,
			U"Zero"_sv,
			U"One"_sv,
			U"SourceColor"_sv,
			U"OneMinusSourceColor"_sv,
			U"SourceAlpha"_sv,
			U"OneMinusSourceAlpha"_sv,
			U"DestinationAlpha"_sv,
			U"OneMinusDestinationAlpha"_sv,
			U"DestinationColor"_sv,
			U"OneMinusDestinationColor"_sv,
			U"SourceAlphaSaturated"_sv,
			U""_sv,
			U""_sv,
			U"BlendColor"_sv,
			U"OneMinusBlendColor"_sv,
			U"Source1Color"_sv,
			U"OneMinusSource1Color"_sv,
			U"Source1Alpha"_sv,
			U"OneMinusSource1Alpha"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const BlendFactor value)
	{
		formatData.string.append(BlendFactorStrings[FromEnum(value)]);
	}
}
