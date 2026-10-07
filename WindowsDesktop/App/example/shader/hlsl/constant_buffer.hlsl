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

// Custom constant buffers: VS_Offset, PS_TintShape, PS_TintTexture.
// For solid shapes and ordinary textures; patterns, dashed lines, fonts, and
// quad-warp drawing require their own shader interfaces.

Texture2D		g_texture0 : register(t0);
SamplerState	g_sampler0 : register(s0);

struct VSInput
{
	float2 position	: POSITION;
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0;
};

struct PSInput
{
	float4 position	: SV_POSITION;
	float4 colorPMA	: COLOR0;
	float2 uv		: TEXCOORD0;
};

cbuffer VSConstants2D : register(b0)
{
	row_major float3x4 g_transform;
	float4 g_colorMul;
}

cbuffer PSConstants2D : register(b0)
{
	float4 g_patternBackgroundColorMul;
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColorPMA;
	float4 g_sdfShadowColorPMA;
}

inline float4 s3d_positionTransform(const float2 position, const float3x4 transform)
{
	const float4 clip = mul(float3(position, 1.0f), transform);
	return float4(clip.xy, 0.0f, clip.w);
}

inline float4 s3d_premultiplyAlpha(const float4 color)
{
	return float4((color.rgb * color.a), color.a);
}

inline float4 s3d_applyColorAdd(const float4 colorPMA, const float4 colorAdd)
{
	return (colorPMA + (colorAdd * colorPMA.a));
}

inline float4 s3d_textureColor(float4 vertexColorPMA, const float4 textureColorPMA)
{
	vertexColorPMA *= textureColorPMA;
	return s3d_applyColorAdd(vertexColorPMA, g_colorAdd);
}

// User constants: Float4 offset (xy in local drawing coordinates), Float4 tint
// (straight RGBA). Both stages use slot 2, independently.
cbuffer VSOffset : register(b2)
{
	float4 g_offset;
}

cbuffer PSTint : register(b2)
{
	float4 g_tint;
}

PSInput VS_Offset(VSInput input)
{
	PSInput result;
	result.position = s3d_positionTransform((input.position + g_offset.xy), g_transform);
	result.colorPMA = s3d_premultiplyAlpha(input.color * g_colorMul);
	result.uv = input.uv;
	return result;
}

float4 PS_TintShape(PSInput input) : SV_TARGET
{
	return (s3d_applyColorAdd(input.colorPMA, g_colorAdd) * s3d_premultiplyAlpha(g_tint));
}

float4 PS_TintTexture(PSInput input) : SV_TARGET
{
	return (s3d_textureColor(input.colorPMA, g_texture0.Sample(g_sampler0, input.uv)) * s3d_premultiplyAlpha(g_tint));
}
