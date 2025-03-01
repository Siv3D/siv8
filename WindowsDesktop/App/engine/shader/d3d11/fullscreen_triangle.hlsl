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

Texture2D		g_texture0 : register(t0);
SamplerState	g_sampler0 : register(s0);

struct PSInput
{
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD0;
};

PSInput VS(uint id: SV_VERTEXID)
{
	PSInput result;

	result.position = float4((id == 2 ? 3.0 : -1.0),
							 (id == 1 ? 3.0 : -1.0),
							 0.0,
							 1.0);

	result.uv = float2((id == 2 ? 2.0 : 0.0),
					   (id == 1 ? -1.0 : 1.0));
	return result;
}

float4 PS(PSInput input) : SV_TARGET
{
	return float4(g_texture0.Sample(g_sampler0, input.uv).rgb, 1.0);
}
