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

# include <Siv3D/Blob.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include "HLSLCompiler.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	compile
	//
	////////////////////////////////////////////////////////////////

	Blob HLSLCompiler::compile(const std::string_view source, const std::string& sourceName, const std::string& entryPoint, const char* target, const uint32 flags, std::string& message)
	{
		message.clear();

		if (not p_D3DCompile)
		{
			d3dCompiler = DLL::LoadSystemLibrary(L"d3dcompiler_47.dll");
			p_D3DCompile = DLL::GetFunction(d3dCompiler, "D3DCompile");
		}

		ComPtr<ID3DBlob> binary, error;
		const HRESULT hr = p_D3DCompile(source.data(), source.size(),
			sourceName.c_str(), nullptr, nullptr,
			entryPoint.c_str(),
			target,
			flags, 0, &binary, &error);

		if (error)
		{
			message.assign(static_cast<const char*>(error->GetBufferPointer()));
		}

		if (FAILED(hr)) // 失敗したら
		{
			LOG_FAIL("CShader_D3D11::ShaderCompiler::compileShader(): D3DCompile():\n" + message);

			return{};
		}
		else if (error)
		{
			LOG_WARN("CShader_D3D11::ShaderCompiler::compileShader(): D3DCompile():\n" + message);
		}

		return Blob{ binary->GetBufferPointer(), binary->GetBufferSize() };
	}
}
