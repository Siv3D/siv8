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
# include <Siv3D/Blob.hpp>
# include <Siv3D/HLSL.hpp>
# include <Siv3D/Shader/IShader.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>
# include "VertexShader/D3D11VertexShader.hpp"
# include "PixelShader/D3D11PixelShader.hpp"
# include "HLSLCompiler.hpp"

namespace s3d
{
	class CShader_D3D11 final : public ISiv3DShader
	{
	public:

		~CShader_D3D11() override;

		void init() override;

		VertexShader::IDType createVSFromFile(FilePathView path, StringView entryPoint) override;

		VertexShader::IDType createVSFromSource(StringView source, StringView entryPoint) override;

		VertexShader::IDType createVSFromBytecode(const Blob& bytecode) override;

		PixelShader::IDType createPSFromFile(FilePathView path, StringView entryPoint) override;

		PixelShader::IDType createPSFromSource(StringView source, StringView entryPoint) override;

		PixelShader::IDType createPSFromBytecode(const Blob& bytecode) override;

		void releaseVS(VertexShader::IDType handleID) override;

		void releasePS(PixelShader::IDType handleID) override;

		void setVS(VertexShader::IDType handleID) override;

		void setPS(PixelShader::IDType handleID) override;

		const Blob& getBytecodeVS(VertexShader::IDType handleID) override;

		const Blob& getBytecodePS(PixelShader::IDType handleID) override;

		Blob compileHLSL(std::string_view source, const std::string& sourceName, const std::string& entryPoint, ShaderStage shaderStage, HLSL::CompileOption option, std::string& message);

	private:

		CRenderer_D3D11* pRenderer = nullptr;

		// device のコピー
		ID3D11Device* m_device = nullptr;

		// context のコピー
		ID3D11DeviceContext* m_context = nullptr;

		HLSLCompiler m_shaderCompiler;

		// VS の管理
		AssetHandleManager<VertexShader::IDType, D3D11VertexShader> m_vertexShaders{ "VertexShader" };

		// PS の管理
		AssetHandleManager<PixelShader::IDType, D3D11PixelShader> m_pixelShaders{ "PixelShader" };

		VertexShader::IDType createVS(Blob&& bytecode);

		PixelShader::IDType createPS(Blob&& bytecode);
	};
}
