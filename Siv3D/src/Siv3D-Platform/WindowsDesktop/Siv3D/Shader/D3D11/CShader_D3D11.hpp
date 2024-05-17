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
# include <Siv3D/Shader/IShader.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>
# include "VertexShader/D3D11VertexShader.hpp"
# include "PixelShader/D3D11PixelShader.hpp"

namespace s3d
{
	class CShader_D3D11 final : public ISiv3DShader
	{
	public:

		~CShader_D3D11();

		void init() override;

		VertexShader::IDType createVSFromFile(FilePathView path, StringView entryPoint) override;

		VertexShader::IDType createVSFromSource(StringView source, StringView entryPoint) override;

		PixelShader::IDType createPSFromFile(FilePathView path, StringView entryPoint) override;

		PixelShader::IDType createPSFromSource(StringView source, StringView entryPoint) override;

		void releaseVS(VertexShader::IDType handleID) override;

		void releasePS(PixelShader::IDType handleID) override;

		void setVS(VertexShader::IDType handleID) override;

		void setPS(PixelShader::IDType handleID) override;

		const Blob& getBinaryVS(VertexShader::IDType handleID) override;

		const Blob& getBinaryPS(PixelShader::IDType handleID) override;

	private:

		CRenderer_D3D11* pRenderer = nullptr;

		// device のコピー
		ID3D11Device* m_device = nullptr;

		// context のコピー
		ID3D11DeviceContext* m_context = nullptr;

		// VS の管理
		AssetHandleManager<VertexShader::IDType, D3D11VertexShader> m_vertexShaders{ "VertexShader" };

		// PS の管理
		AssetHandleManager<PixelShader::IDType, D3D11PixelShader> m_pixelShaders{ "PixelShader" };

		VertexShader::IDType createVS(Blob&& binary);

		PixelShader::IDType createPS(Blob&& binary);
	};
}
