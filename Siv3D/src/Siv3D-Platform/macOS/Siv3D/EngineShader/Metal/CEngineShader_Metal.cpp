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
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CEngineShader_Metal::~CEngineShader_Metal()
	{
		LOG_SCOPED_DEBUG("CEngineShader_Metal::~CEngineShader_Metal()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CEngineShader_Metal::init()
	{
		LOG_SCOPED_DEBUG("CEngineShader_Metal::init()");

		m_vertexShaders << VertexShader::MSL(U"fullscreen_triangle_vs");
		m_vertexShaders << VertexShader::MSL(U"vertexShader");

		m_pixelShaders << PixelShader::MSL(U"fullscreen_triangle_ps");
		m_pixelShaders << PixelShader::MSL(U"fragmentShader");
	}

	////////////////////////////////////////////////////////////////
	//
	//	getVS
	//
	////////////////////////////////////////////////////////////////

	const VertexShader& CEngineShader_Metal::getVS(const EngineVS vs) const
	{
		return m_vertexShaders[FromEnum(vs)];
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPS
	//
	////////////////////////////////////////////////////////////////

	const PixelShader& CEngineShader_Metal::getPS(const EnginePS ps) const
	{
		return m_pixelShaders[FromEnum(ps)];
	}
}
