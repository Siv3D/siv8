//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Array.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>

namespace s3d
{
	class CEngineShader_Metal final : public ISiv3DEngineShader
	{
	public:

		~CEngineShader_Metal() override;

		void init() override;

		const VertexShader& getVS(EngineVS vs) const override;

		const PixelShader& getPS(EnginePS ps) const override;

	private:

		Array<VertexShader> m_vertexShaders;

		Array<PixelShader> m_pixelShaders;
	};
}
