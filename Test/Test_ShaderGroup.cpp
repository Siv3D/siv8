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

namespace
{
	constexpr StringView HLSLSource = UR"(
		float4 VS(uint id : SV_VertexID) : SV_POSITION
		{
			return float4(float(id), 0.0, 0.0, 1.0);
		}
		float4 PS() : SV_TARGET
		{
			return float4(1.0, 0.0, 0.0, 1.0);
		}
	)";

	constexpr StringView MSLSource = UR"(
		#include <metal_stdlib>
		using namespace metal;
		vertex float4 VS(uint id [[vertex_id]])
		{
			return float4(float(id), 0.0, 0.0, 1.0);
		}
		fragment float4 PS()
		{
			return float4(1.0, 0.0, 0.0, 1.0);
		}
	)";
}

TEMPLATE_TEST_CASE("ShaderGroup.platform_selection", "", VertexShader, PixelShader)
{
	constexpr StringView entryPoint = (std::is_same_v<TestType, VertexShader> ? U"VS" : U"PS");
	const HLSL hlsl = HLSL::FromSource(HLSLSource, entryPoint);
	const MSL msl = MSL::FromSource(MSLSource, entryPoint);

	SECTION("HLSL first")
	{
		CHECK(TestType{ hlsl | msl });
	}

	SECTION("MSL first")
	{
		CHECK(TestType{ msl | hlsl });
	}

	SECTION("Unused platform shader can be absent")
	{
	# if SIV3D_PLATFORM(WINDOWS)
		const ShaderGroup group{ hlsl, none };
	# elif SIV3D_PLATFORM(MACOS)
		const ShaderGroup group{ none, msl };
	# endif
		CHECK(TestType{ group });
	}

	SECTION("Unused platform shader is not compiled")
	{
	# if SIV3D_PLATFORM(WINDOWS)
		const ShaderGroup group{ hlsl, MSL::FromSource(U"invalid shader", entryPoint) };
	# elif SIV3D_PLATFORM(MACOS)
		const ShaderGroup group{ HLSL::FromSource(U"invalid shader", entryPoint), msl };
	# endif
		CHECK(TestType{ group });
	}

	SECTION("Selected shader creation failure returns an empty shader")
	{
	# if SIV3D_PLATFORM(WINDOWS)
		const ShaderGroup group{ HLSL::FromSource(HLSLSource, U"MissingEntryPoint"), msl };
	# elif SIV3D_PLATFORM(MACOS)
		const ShaderGroup group{ hlsl, MSL::FromSource(MSLSource, U"MissingEntryPoint") };
	# endif
		CHECK_FALSE(TestType{ group });
	}
}
