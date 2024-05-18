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

# include "CEngineShader_Metal.hpp"

namespace s3d
{
	void CEngineShader_Metal::init()
	{

	}

	const VertexShader& CEngineShader_Metal::getVS(const EngineVS vs) const
	{
		return m_vertexShaders[FromEnum(vs)];
	}

	const PixelShader& CEngineShader_Metal::getPS(const EnginePS ps) const
	{
		return m_pixelShaders[FromEnum(ps)];
	}
}
