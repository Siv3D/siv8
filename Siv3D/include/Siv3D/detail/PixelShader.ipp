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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	HLSL
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	static PixelShader PixelShader::HLSL(Reader&& reader, const StringView entryPoint)
	{
		return PixelShader{ std::make_unique<Reader>(std::forward<Reader>(reader)), entryPoint };
	}

	////////////////////////////////////////////////////////////////
	//
	//	MSL
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	static PixelShader PixelShader::MSL(Reader&& reader, const StringView entryPoint)
	{
		return PixelShader{ std::make_unique<Reader>(std::forward<Reader>(reader)), entryPoint };
	}
}
