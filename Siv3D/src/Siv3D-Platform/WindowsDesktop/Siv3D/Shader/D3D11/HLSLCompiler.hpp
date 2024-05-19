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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/DLL.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	struct HLSLCompiler
	{
		LibraryHandle d3dCompiler = nullptr;

		decltype(D3DCompile)* p_D3DCompile = nullptr;

		Blob compile(std::string_view source, const std::string& sourceName, const std::string& entryPoint, const char* target, uint32 flags, std::string& message);
	};
}
