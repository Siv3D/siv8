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
# include <Siv3D/Array.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>

namespace s3d
{
	class CEngineShader_D3D11 final : public ISiv3DEngineShader
	{
	public:

		void init() override;

		const VertexShader& getVS(EngineVS vs) const override;

		const PixelShader& getPS(EnginePS ps) const override;

	private:

		Array<VertexShader> m_vertexShaders;

		Array<PixelShader> m_pixelShaders;
	};
}
