﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/HLSL.hpp>
# include <Siv3D/TextReader.hpp>
# include <Siv3D/Shader/D3D11/CShader_D3D11.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Platform::Windows
	{
		////////////////////////////////////////////////////////////////
		//
		//	CompileHLSLFromSource
		//
		////////////////////////////////////////////////////////////////

		Result<Blob, String> CompileHLSLFromSource(const std::string_view source, const ShaderStage shaderStage, const StringView entryPoint, const HLSL::CompileOption option)
		{
			if (CShader_D3D11* const pShader = static_cast<CShader_D3D11*>(Siv3DEngine::Get<ISiv3DShader>()))
			{
				std::string message;

				Blob byteCode = pShader->compileHLSL(source, "", Unicode::ToUTF8(entryPoint), shaderStage, option, message);

				if (not byteCode)
				{
					return Err{ Unicode::FromUTF8(message) };
				}

				return byteCode;
			}
			else
			{
				// グラフィックスバックエンドが D3D11 ではない
				return Err{ U"Graphics backend is not D3D11" };
			}
		}

		Result<Blob, String> CompileHLSLFromSource(const StringView source, const ShaderStage shaderStage, const StringView entryPoint, const HLSL::CompileOption option)
		{
			return CompileHLSLFromSource(Unicode::ToUTF8(source), shaderStage, entryPoint, option);
		}

		bool CompileHLSLFromSource(const std::string_view source, const FilePathView outputPath, const ShaderStage shaderStage, const StringView entryPoint, const HLSL::CompileOption option)
		{
			if (const auto result = CompileHLSLFromSource(source, shaderStage, entryPoint, option))
			{
				return result->save(outputPath);
			}
			else
			{
				return false;
			}
		}

		bool CompileHLSLFromSource(const FilePathView source, const FilePathView outputPath, const ShaderStage shaderStage, const StringView entryPoint, const HLSL::CompileOption option)
		{
			if (const auto result = CompileHLSLFromSource(source, shaderStage, entryPoint, option))
			{
				return result->save(outputPath);
			}
			else
			{
				return false;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	CompileHLSLFromFile
		//
		////////////////////////////////////////////////////////////////

		Result<Blob, String> CompileHLSLFromFile(const FilePathView hlslFilePath, const ShaderStage shaderStage, const StringView entryPoint, const HLSL::CompileOption option)
		{
			if (CShader_D3D11* const pShader = static_cast<CShader_D3D11*>(Siv3DEngine::Get<ISiv3DShader>()))
			{
				TextReader reader{ hlslFilePath };

				if (not reader)
				{
					return Err{ U"Failed to open file `{}`"_fmt(hlslFilePath) };
				}

				const std::string source = reader.readAllUTF8();

				std::string message;

				Blob byteCode = pShader->compileHLSL(source, Unicode::ToUTF8(hlslFilePath), Unicode::ToUTF8(entryPoint), shaderStage, option, message);

				if (not byteCode)
				{
					return Err{ Unicode::FromUTF8(message) };
				}

				return byteCode;
			}
			else
			{
				// グラフィックスバックエンドが D3D11 ではない
				return Err{ U"Graphics backend is not D3D11" };
			}
		}

		bool CompileHLSLFromFile(const FilePathView hlslFilePath, const FilePathView outputPath, const ShaderStage shaderStage, const StringView entryPoint, const HLSL::CompileOption option)
		{
			if (const auto result = CompileHLSLFromFile(hlslFilePath, shaderStage, entryPoint, option))
			{
				return result->save(outputPath);
			}
			else
			{
				return false;
			}
		}
	}
}
