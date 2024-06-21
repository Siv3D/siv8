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

# include <Siv3D/RasterizerState.hpp>
# include <Siv3D/FormatLiteral.hpp>

namespace s3d
{
	String RasterizerState::format() const
	{
		String result = U"RasterizerState\n{\n";
		result += U"\ttriangleFillMode: {}\n"_fmt(triangleFillMode);
		result += U"\tcullMode: {}\n"_fmt(cullMode);
		result += U"\tantialiasedLine3D: {}\n"_fmt(antialiasedLine3D);
		result += U"\tdepthBias: {}\n"_fmt(depthBias);
		result += U"}";
		return result;
	}

	void Formatter(FormatData& formatData, const RasterizerState& value)
	{
		formatData.string.append(value.format());
	}
}
