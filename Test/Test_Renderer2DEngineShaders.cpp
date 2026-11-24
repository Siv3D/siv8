//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Renderer2DEngineShaders.hpp"
# include <utility>

TEST_CASE("Renderer2D.engine_shader_initialization")
{
	const auto& engine = *SIV3D_ENGINE(EngineShader);
	Renderer2DEngineShaders shaders;
	shaders.init(engine);

	const std::pair<VertexShader::IDType Renderer2DEngineShaders::*, EngineVS> vertexShaders[] =
	{
		{ &Renderer2DEngineShaders::vsShape, EngineVS::Shape2D },
		{ &Renderer2DEngineShaders::vsPattern, EngineVS::Pattern2D },
	};
	for (const auto& [member, type] : vertexShaders)
	{
		CAPTURE(static_cast<int32>(type));
		REQUIRE(engine.getVS(type));
		CHECK((shaders.*member) == engine.getVS(type).id());
	}

	const std::pair<PixelShader::IDType Renderer2DEngineShaders::*, EnginePS> pixelShaders[] =
	{
		{ &Renderer2DEngineShaders::psShape, EnginePS::Shape2D },
		{ &Renderer2DEngineShaders::psTexture, EnginePS::Texture2D },
		{ &Renderer2DEngineShaders::psLineDot, EnginePS::LineDot },
		{ &Renderer2DEngineShaders::psLineDash, EnginePS::LineDash },
		{ &Renderer2DEngineShaders::psLineLongDash, EnginePS::LineLongDash },
		{ &Renderer2DEngineShaders::psLineDashDot, EnginePS::LineDashDot },
		{ &Renderer2DEngineShaders::psLineRoundDot, EnginePS::LineRoundDot },
		{ &Renderer2DEngineShaders::psPatternPolkaDot, EnginePS::PatternPolkaDot },
		{ &Renderer2DEngineShaders::psPatternStripe, EnginePS::PatternStripe },
		{ &Renderer2DEngineShaders::psPatternGrid, EnginePS::PatternGrid },
		{ &Renderer2DEngineShaders::psPatternChecker, EnginePS::PatternChecker },
		{ &Renderer2DEngineShaders::psPatternTriangle, EnginePS::PatternTriangle },
		{ &Renderer2DEngineShaders::psPatternHexGrid, EnginePS::PatternHexGrid },
		{ &Renderer2DEngineShaders::psPatternHalftone, EnginePS::PatternHalftone },
		{ &Renderer2DEngineShaders::psPatternWave, EnginePS::PatternWave },
		{ &Renderer2DEngineShaders::psPatternRipple, EnginePS::PatternRipple },
		{ &Renderer2DEngineShaders::psPatternWeave, EnginePS::PatternWeave },
		{ &Renderer2DEngineShaders::psPatternTruchet, EnginePS::PatternTruchet },
	};
	for (const auto& [member, type] : pixelShaders)
	{
		CAPTURE(static_cast<int32>(type));
		REQUIRE(engine.getPS(type));
		CHECK((shaders.*member) == engine.getPS(type).id());
	}
}

TEST_CASE("Renderer2D.pattern_shader_selection")
{
	Renderer2DEngineShaders shaders;
	shaders.psShape = PixelShader::IDType{ 100 };
	const std::pair<PatternType, PixelShader::IDType Renderer2DEngineShaders::*> patterns[] =
	{
		{ PatternType::PolkaDot, &Renderer2DEngineShaders::psPatternPolkaDot },
		{ PatternType::Stripe, &Renderer2DEngineShaders::psPatternStripe },
		{ PatternType::Grid, &Renderer2DEngineShaders::psPatternGrid },
		{ PatternType::Checker, &Renderer2DEngineShaders::psPatternChecker },
		{ PatternType::Triangle, &Renderer2DEngineShaders::psPatternTriangle },
		{ PatternType::HexGrid, &Renderer2DEngineShaders::psPatternHexGrid },
		{ PatternType::Halftone, &Renderer2DEngineShaders::psPatternHalftone },
		{ PatternType::Wave, &Renderer2DEngineShaders::psPatternWave },
		{ PatternType::Ripple, &Renderer2DEngineShaders::psPatternRipple },
		{ PatternType::Weave, &Renderer2DEngineShaders::psPatternWeave },
		{ PatternType::Truchet, &Renderer2DEngineShaders::psPatternTruchet },
	};
	uint32 id = 1;
	for (const auto& [pattern, member] : patterns)
	{
		shaders.*member = PixelShader::IDType{ id++ };
	}
	for (const auto& [pattern, member] : patterns)
	{
		CAPTURE(static_cast<int32>(pattern));
		CHECK(shaders.getPatternShader(pattern) == (shaders.*member));
	}
	CHECK(shaders.getPatternShader(static_cast<PatternType>(11)) == shaders.psShape);
	CHECK(shaders.getPatternShader(static_cast<PatternType>(255)) == shaders.psShape);
}
