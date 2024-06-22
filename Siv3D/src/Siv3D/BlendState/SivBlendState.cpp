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

# include <Siv3D/BlendState.hpp>
# include <Siv3D/FormatLiteral.hpp>

namespace s3d
{
	String BlendState::format() const
	{
		String result = U"BlendState\n{\n";
		result += U"\tenabled: {}\n"_fmt(enabled);
		result += U"\tsourceRGB: {}\n"_fmt(sourceRGB);
		result += U"\tdestinationRGB: {}\n"_fmt(destinationRGB);
		result += U"\trgbOperation: {}\n"_fmt(rgbOperation);
		result += U"\tsourceAlpha: {}\n"_fmt(sourceAlpha);
		result += U"\tdestinationAlpha: {}\n"_fmt(destinationAlpha);
		result += U"\talphaOperation: {}\n"_fmt(alphaOperation);
		result += U"\talphaToCoverageEnabled: {}\n"_fmt(alphaToCoverageEnabled);
		result += U"\tcolorWriteMask: ";
		result += Format(getColorWriteMask());
		result += U"\n}";
		return result;
	}

	void Formatter(FormatData& formatData, const BlendState& value)
	{
		formatData.string.append(value.format());
	}
}
