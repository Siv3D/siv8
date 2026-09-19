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

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)

TEST_CASE("MetalShader.default_library_stage")
{
	CHECK(VertexShader::MSL(U"VS_Shape"));
	CHECK(PixelShader::MSL(U"PS_Shape"));
	CHECK(VertexShader::MSL(U"PS_Shape").isEmpty());
	CHECK(PixelShader::MSL(U"VS_Shape").isEmpty());
	for (const StringView entry : { U"", U"MissingShaderEntryPoint" })
	{
		INFO("entry = " << Unicode::ToUTF8(entry));
		CHECK(VertexShader::MSL(entry).isEmpty());
		CHECK(PixelShader::MSL(entry).isEmpty());
	}
}

TEST_CASE("MetalShader.source_stage")
{
	const std::string source = R"(
#include <metal_stdlib>
using namespace metal;
vertex float4 TestVS(uint id [[vertex_id]]) { return float4(float(id), 0, 0, 1); }
fragment float4 TestPS() { return float4(1); }
kernel void TestCS() {}
)";
	CHECK(VertexShader::MSL(source, U"TestVS"));
	CHECK(PixelShader::MSL(source, U"TestPS"));
	CHECK(VertexShader::MSL(source, U"TestPS").isEmpty());
	CHECK(PixelShader::MSL(source, U"TestVS").isEmpty());
	for (const StringView entry : { U"TestCS", U"", U"MissingShaderEntryPoint" })
	{
		INFO("entry = " << Unicode::ToUTF8(entry));
		CHECK(VertexShader::MSL(source, entry).isEmpty());
		CHECK(PixelShader::MSL(source, entry).isEmpty());
	}
	const std::string invalidSource = "This is not valid MSL.";
	CHECK(VertexShader::MSL(invalidSource, U"TestVS").isEmpty());
	CHECK(PixelShader::MSL(invalidSource, U"TestPS").isEmpty());
}

# endif
