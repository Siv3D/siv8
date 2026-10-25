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

# pragma once
# include <Siv3D/Pattern/PatternType.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>

namespace s3d
{
	struct Renderer2DEngineShaders
	{
		VertexShader::IDType vsShape;

		VertexShader::IDType vsPattern;

		PixelShader::IDType psShape;

		PixelShader::IDType psTexture;

		PixelShader::IDType psLineDot;

		PixelShader::IDType psLineDash;

		PixelShader::IDType psLineLongDash;

		PixelShader::IDType psLineDashDot;

		PixelShader::IDType psLineRoundDot;

		PixelShader::IDType psPatternPolkaDot;

		PixelShader::IDType psPatternStripe;

		PixelShader::IDType psPatternGrid;

		PixelShader::IDType psPatternChecker;

		PixelShader::IDType psPatternTriangle;

		PixelShader::IDType psPatternHexGrid;

		PixelShader::IDType psPatternHalftone;

		PixelShader::IDType psPatternWave;

		PixelShader::IDType psPatternRipple;

		PixelShader::IDType psPatternWeave;

		PixelShader::IDType psPatternTruchet;

		void init(const ISiv3DEngineShader& engineShader)
		{
			vsShape = engineShader.getVS(EngineVS::Shape2D).id();
			vsPattern = engineShader.getVS(EngineVS::Pattern2D).id();
			psShape = engineShader.getPS(EnginePS::Shape2D).id();
			psTexture = engineShader.getPS(EnginePS::Texture2D).id();
			psLineDot = engineShader.getPS(EnginePS::LineDot).id();
			psLineDash = engineShader.getPS(EnginePS::LineDash).id();
			psLineLongDash = engineShader.getPS(EnginePS::LineLongDash).id();
			psLineDashDot = engineShader.getPS(EnginePS::LineDashDot).id();
			psLineRoundDot = engineShader.getPS(EnginePS::LineRoundDot).id();
			psPatternPolkaDot = engineShader.getPS(EnginePS::PatternPolkaDot).id();
			psPatternStripe = engineShader.getPS(EnginePS::PatternStripe).id();
			psPatternGrid = engineShader.getPS(EnginePS::PatternGrid).id();
			psPatternChecker = engineShader.getPS(EnginePS::PatternChecker).id();
			psPatternTriangle = engineShader.getPS(EnginePS::PatternTriangle).id();
			psPatternHexGrid = engineShader.getPS(EnginePS::PatternHexGrid).id();
			psPatternHalftone = engineShader.getPS(EnginePS::PatternHalftone).id();
			psPatternWave = engineShader.getPS(EnginePS::PatternWave).id();
			psPatternRipple = engineShader.getPS(EnginePS::PatternRipple).id();
			psPatternWeave = engineShader.getPS(EnginePS::PatternWeave).id();
			psPatternTruchet = engineShader.getPS(EnginePS::PatternTruchet).id();
		}

		[[nodiscard]]
		PixelShader::IDType getPatternShader(const PatternType pattern) const noexcept
		{
			switch (pattern)
			{
			case PatternType::PolkaDot:
				return psPatternPolkaDot;
			case PatternType::Stripe:
				return psPatternStripe;
			case PatternType::Checker:
				return psPatternChecker;
			case PatternType::Grid:
				return psPatternGrid;
			case PatternType::Triangle:
				return psPatternTriangle;
			case PatternType::HexGrid:
				return psPatternHexGrid;
			case PatternType::Halftone:
				return psPatternHalftone;
			case PatternType::Wave:
				return psPatternWave;
			case PatternType::Ripple:
				return psPatternRipple;
			case PatternType::Weave:
				return psPatternWeave;
			case PatternType::Truchet:
				return psPatternTruchet;
			default:
				return psShape;
			}
		}
	};
}
