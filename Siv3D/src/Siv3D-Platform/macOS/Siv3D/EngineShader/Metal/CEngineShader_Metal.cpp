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

# include "CEngineShader_Metal.hpp"
# include <Siv3D/MSL.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
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
		
		{
			m_vertexShaders << MSL{ U"VS_FullscreenTriangle" };
			m_vertexShaders << MSL{ U"VS_Shape" };
			m_vertexShaders << MSL{ U"VS_QuadWarp" };
			
			if (not m_vertexShaders.all([](const auto& vs) { return static_cast<bool>(vs); })) // もしロードに失敗したシェーダがあれば
			{
				throw InternalEngineError{ U"Failed to load a engine shader" };
			}
		}
		
		{
			m_pixelShaders << MSL{ U"PS_FullscreenTriangle" };
			m_pixelShaders << MSL{ U"PS_Shape" };
			m_pixelShaders << MSL{ U"PS_Texture" };
			m_pixelShaders << MSL{ U"PS_QuadWarp" };
			m_pixelShaders << MSL{ U"PS_LineDot" };
			m_pixelShaders << MSL{ U"PS_LineDash" };
			m_pixelShaders << MSL{ U"PS_LineLongDash" };
			m_pixelShaders << MSL{ U"PS_LineDashDot" };
			m_pixelShaders << MSL{ U"PS_LineRoundDot" };
			m_pixelShaders << MSL{ U"PS_PatternPolkaDot" };
			m_pixelShaders << MSL{ U"PS_PatternStripe" };
			m_pixelShaders << MSL{ U"PS_PatternGrid" };
			m_pixelShaders << MSL{ U"PS_PatternChecker" };
			m_pixelShaders << MSL{ U"PS_PatternTriangle" };
			m_pixelShaders << MSL{ U"PS_PatternHexGrid" };
			
			if (not m_pixelShaders.all([](const auto& ps) { return static_cast<bool>(ps); })) // もしロードに失敗したシェーダがあれば
			{
				throw InternalEngineError{ U"Failed to load a engine shader" };
			}
		}
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
