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

# include "CEngineShader_D3D11.hpp"
# include <Siv3D/HLSL.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CEngineShader_D3D11::~CEngineShader_D3D11()
	{
		LOG_SCOPED_DEBUG("CEngineShader_D3D11::~CEngineShader_D3D11()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CEngineShader_D3D11::init()
	{
		LOG_SCOPED_DEBUG("CEngineShader_D3D11::init()");

		// エンジンシェーダを再コンパイルする場合、0 を 1 に変更する
	# if 0
	
		//Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/fullscreen_triangle.hlsl", U"engine/shader/d3d11/fullscreen_triangle.vs", ShaderStage::Vertex, U"VS");
		//Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/fullscreen_triangle.hlsl", U"engine/shader/d3d11/fullscreen_triangle.ps", ShaderStage::Pixel, U"PS");

		//if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d.vs", ShaderStage::Vertex, U"VS"))
		//{
		//	throw InternalEngineError{ U"Failed to compile a engine shader" };
		//}

		//if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_shape.ps", ShaderStage::Pixel, U"PS_Shape"))
		//{
		//	throw InternalEngineError{ U"Failed to compile a engine shader" };
		//}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_line_dot.ps", ShaderStage::Pixel, U"PS_LineDot"))
		{
			throw InternalEngineError{ U"Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_line_dash.ps", ShaderStage::Pixel, U"PS_LineDash"))
		{
			throw InternalEngineError{ U"Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_line_round_dot.ps", ShaderStage::Pixel, U"PS_LineRoundDot"))
		{
			throw InternalEngineError{ U"Failed to compile a engine shader" };
		}

	# endif

		{
			m_vertexShaders << HLSL{ U"engine/shader/d3d11/fullscreen_triangle.vs" };
			m_vertexShaders << HLSL{ U"engine/shader/d3d11/2d.vs" };

			if (not m_vertexShaders.all([](const auto& vs) { return static_cast<bool>(vs); })) // もしロードに失敗したシェーダがあれば
			{
				throw InternalEngineError{ U"Failed to load a engine shader" };
			}
		}

		{
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/fullscreen_triangle.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_shape.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_line_dot.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_line_dash.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_line_round_dot.ps" };

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

	const VertexShader& CEngineShader_D3D11::getVS(const EngineVS vs) const
	{
		return m_vertexShaders[FromEnum(vs)];
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPS
	//
	////////////////////////////////////////////////////////////////

	const PixelShader& CEngineShader_D3D11::getPS(const EnginePS ps) const
	{
		return m_pixelShaders[FromEnum(ps)];
	}
}
