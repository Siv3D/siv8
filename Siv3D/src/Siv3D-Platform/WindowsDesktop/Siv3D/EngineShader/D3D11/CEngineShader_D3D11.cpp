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

# include "CEngineShader_D3D11.hpp"

namespace s3d
{
	void CEngineShader_D3D11::init()
	{
		m_vertexShaders << VertexShader::HLSL(U"engine/shader/d3d11/fullscreen_triangle.vs");
		
		m_pixelShaders << PixelShader::HLSL(U"engine/shader/d3d11/fullscreen_triangle.ps");
	}

	const VertexShader& CEngineShader_D3D11::getVS(const EngineVS vs) const
	{
		return m_vertexShaders[FromEnum(vs)];
	}

	const PixelShader& CEngineShader_D3D11::getPS(const EnginePS ps) const
	{
		return m_pixelShaders[FromEnum(ps)];
	}
}