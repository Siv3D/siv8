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

// Standard 2D template: VS_Shape, PS_Shape, PS_Texture.
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
	row_major float2x4 g_transform;
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

inline float2 s3d_transformPoint2D(const float2 position, const float2x4 transform)
{
	const float2 translation = transform._13_14;
	const float2 basisX = transform._11_12;
	const float2 basisY = transform._21_22;
	return (translation + (position.x * basisX) + (position.y * basisY));
}

inline float4 s3d_positionTransform(const float2 position, const float2x4 transform)
{
	return float4(s3d_transformPoint2D(position, transform), transform._23_24);
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

////////////////////////////////////////////////////////////////
//
//	Basic VS / PS
//
////////////////////////////////////////////////////////////////

PSInput VS_Shape(VSInput input)
{
	PSInput result;
	result.position	= s3d_positionTransform(input.position, g_transform);
	result.colorPMA = s3d_premultiplyAlpha(input.color * g_colorMul);
	result.uv		= input.uv;
	return result;
}

float4 PS_Shape(PSInput input) : SV_TARGET
{
	return s3d_applyColorAdd(input.colorPMA, g_colorAdd);
}

float4 PS_Texture(PSInput input) : SV_TARGET
{
	return s3d_textureColor(input.colorPMA, g_texture0.Sample(g_sampler0, input.uv));
}
