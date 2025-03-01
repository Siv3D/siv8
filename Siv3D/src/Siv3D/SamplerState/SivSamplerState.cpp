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

# include <Siv3D/SamplerState.hpp>
# include <Siv3D/FormatLiteral.hpp>

namespace s3d
{
	String SamplerState::format() const
	{
		String result = U"SamplerState\n{\n";
		result += U"\tuAddressMode: {}\n"_fmt(uAddressMode);
		result += U"\tvAddressMode: {}\n"_fmt(vAddressMode);
		result += U"\twAddressMode: {}\n"_fmt(wAddressMode);
		result += U"\tminFilter: {}\n"_fmt(minFilter);
		result += U"\tmagFilter: {}\n"_fmt(magFilter);
		result += U"\tmipFilter: {}\n"_fmt(mipFilter);
		result += U"\tmaxAnisotropy: {}\n"_fmt(maxAnisotropy);
		result += U"\tcompareFunction: {}\n"_fmt(compareFunction);
		result += U"\tborderColor: {}\n"_fmt(borderColor);
		result += U"\tminLOD: {}\n"_fmt(minLOD);
		result += U"}";
		return result;
	}

	void Formatter(FormatData& formatData, const SamplerState& value)
	{
		formatData.string.append(value.format());
	}
}
