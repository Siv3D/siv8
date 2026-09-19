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

// PS_Posterize: Quantize the final tinted/added texture color to five levels per RGB channel.
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

float4 PS_Posterize(PSInput input) : SV_TARGET
{
	const float4 colorPMA = s3d_textureColor(input.colorPMA, g_texture0.Sample(g_sampler0, input.uv));
	// Quantize straight RGB, then restore PMA. Alpha itself is not quantized.
	// Four intervals give levels 0, 0.25, 0.5, 0.75, and 1 for RGB in [0, 1].
	const float steps = 4.0f;
	if (colorPMA.a == 0.0f)
	{
		return colorPMA;
	}
	const float3 color = (colorPMA.rgb / colorPMA.a);
	const float3 quantized = (floor((color * steps) + 0.5f) / steps);
	return float4((quantized * colorPMA.a), colorPMA.a);
}
