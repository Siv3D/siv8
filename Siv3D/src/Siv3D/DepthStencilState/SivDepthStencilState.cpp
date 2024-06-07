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

# include <Siv3D/DepthStencilState.hpp>
# include <Siv3D/FormatLiteral.hpp>

namespace s3d
{
	String DepthStencilState::format() const
	{
		String result = U"DepthStencilState\n{\n";
		result += U"\tdepthEnabled: {}\n"_fmt(depthEnabled);
		result += U"\tdepthWriteEnabled: {}\n"_fmt(depthWriteEnabled);
		result += U"\tdepthCompareFunction: {}\n"_fmt(depthCompareFunction);
		result += U"}";
		return result;
	}

	void Formatter(FormatData& formatData, const DepthStencilState& value)
	{
		formatData.string.append(value.format());
	}
}
