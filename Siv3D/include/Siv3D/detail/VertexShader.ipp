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
	static VertexShader VertexShader::HLSL(Reader&& reader, const StringView entryPoint)
	{
		return VertexShader{ std::make_unique<Reader>(std::forward<Reader>(reader)), entryPoint };
	}

	////////////////////////////////////////////////////////////////
	//
	//	MSL
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	static VertexShader VertexShader::MSL(Reader&& reader, const StringView entryPoint)
	{
		return VertexShader{ std::make_unique<Reader>(std::forward<Reader>(reader)), entryPoint };
	}
}
