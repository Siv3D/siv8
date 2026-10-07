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

// PS_RGBToBGR: Exchange red and blue in the final tinted/added texture color.
// Ordinary textures only. Uses the standard PS buffer, texture 0, and sampler 0.
// Vertex color and standard color multiplication/addition precede the effect.

Texture2D		g_texture0 : register(t0);
SamplerState	g_sampler0 : register(s0);

struct PSInput
{
	float4 position	: SV_POSITION;
	float4 colorPMA	: COLOR0;
	float2 uv		: TEXCOORD0;
};

cbuffer PSConstants2D : register(b0)
{
	float4 g_patternBackgroundColorMul;
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColorPMA;
	float4 g_sdfShadowColorPMA;
}

inline float4 s3d_textureColor(float4 vertexColorPMA, const float4 textureColorPMA)
{
	vertexColorPMA *= textureColorPMA;
	return (vertexColorPMA + (g_colorAdd * vertexColorPMA.a));
}

float4 PS_RGBToBGR(PSInput input) : SV_TARGET
{
	const float4 colorPMA = s3d_textureColor(input.colorPMA, g_texture0.Sample(g_sampler0, input.uv));
	return colorPMA.bgra;
}
