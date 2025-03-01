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

# include <Siv3D/DepthStencilState.hpp>
# include <Siv3D/FormatLiteral.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String DepthStencilState::format() const
	{
		String result = U"DepthStencilState\n{\n";
		result += U"\tdepthTestEnabled: {}\n"_fmt(depthTestEnabled);
		result += U"\tdepthWriteEnabled: {}\n"_fmt(depthWriteEnabled);
		result += U"\tdepthCompareFunction: {}\n"_fmt(depthCompareFunction);
		result += U"}";
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const DepthStencilState& value)
	{
		formatData.string.append(value.format());
	}
}
