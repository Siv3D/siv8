//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
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
	# if 1

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/fullscreen_triangle.hlsl", U"engine/shader/d3d11/fullscreen_triangle.vs", ShaderStage::Vertex, U"VS"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/fullscreen_triangle.hlsl", U"engine/shader/d3d11/fullscreen_triangle.ps", ShaderStage::Pixel, U"PS"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d.vs", ShaderStage::Vertex, U"VS_Shape"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_quadwarp.vs", ShaderStage::Vertex, U"VS_QuadWarp"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_shape.ps", ShaderStage::Pixel, U"PS_Shape"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_texture.ps", ShaderStage::Pixel, U"PS_Texture"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_quadwarp.ps", ShaderStage::Pixel, U"PS_QuadWarp"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_line_dot.ps", ShaderStage::Pixel, U"PS_LineDot"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_line_dash.ps", ShaderStage::Pixel, U"PS_LineDash"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_line_long_dash.ps", ShaderStage::Pixel, U"PS_LineLongDash"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_line_dash_dot.ps", ShaderStage::Pixel, U"PS_LineDashDot"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_line_round_dot.ps", ShaderStage::Pixel, U"PS_LineRoundDot"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_pattern_polka_dot.ps", ShaderStage::Pixel, U"PS_PatternPolkaDot"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_pattern_stripe.ps", ShaderStage::Pixel, U"PS_PatternStripe"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_pattern_grid.ps", ShaderStage::Pixel, U"PS_PatternGrid"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_pattern_checker.ps", ShaderStage::Pixel, U"PS_PatternChecker"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_pattern_triangle.ps", ShaderStage::Pixel, U"PS_PatternTriangle"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/2d_pattern_hex_grid.ps", ShaderStage::Pixel, U"PS_PatternHexGrid"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/msdffont.ps", ShaderStage::Pixel, U"PS_MSDFFont"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/msdffont_outline.ps", ShaderStage::Pixel, U"PS_MSDFFont_Outline"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/msdffont_shadow.ps", ShaderStage::Pixel, U"PS_MSDFFont_Shadow"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/msdffont_outline_shadow.ps", ShaderStage::Pixel, U"PS_MSDFFont_OutlineShadow"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/msdffont_glow.ps", ShaderStage::Pixel, U"PS_MSDFFont_Glow"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

		if (not Platform::Windows::CompileHLSLFromFile(U"engine/shader/d3d11/2d.hlsl", U"engine/shader/d3d11/msdffont_print.ps", ShaderStage::Pixel, U"PS_MSDFFont_Print"))
		{
			throw InternalEngineError{ "Failed to compile a engine shader" };
		}

	# endif

		{
			m_vertexShaders << HLSL{ U"engine/shader/d3d11/fullscreen_triangle.vs" };
			m_vertexShaders << HLSL{ U"engine/shader/d3d11/2d.vs" };
			m_vertexShaders << HLSL{ U"engine/shader/d3d11/2d_quadwarp.vs" };

			if (not m_vertexShaders.all([](const auto& vs) { return static_cast<bool>(vs); })) // もしロードに失敗したシェーダがあれば
			{
				throw InternalEngineError{ "Failed to load a engine shader" };
			}
		}

		{
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/fullscreen_triangle.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_shape.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_texture.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_quadwarp.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_line_dot.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_line_dash.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_line_long_dash.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_line_dash_dot.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_line_round_dot.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_pattern_polka_dot.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_pattern_stripe.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_pattern_grid.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_pattern_checker.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_pattern_triangle.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/2d_pattern_hex_grid.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/msdffont.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/msdffont_outline.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/msdffont_shadow.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/msdffont_outline_shadow.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/msdffont_glow.ps" };
			m_pixelShaders << HLSL{ U"engine/shader/d3d11/msdffont_print.ps" };

			if (not m_pixelShaders.all([](const auto& ps) { return static_cast<bool>(ps); })) // もしロードに失敗したシェーダがあれば
			{
				throw InternalEngineError{ "Failed to load a engine shader" };
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
