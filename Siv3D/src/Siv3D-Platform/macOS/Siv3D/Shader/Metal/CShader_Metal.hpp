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
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>

namespace s3d
{
	class CShader_Metal final : public ISiv3DShader
	{
	public:

		~CShader_Metal();

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

	private:

	};
}