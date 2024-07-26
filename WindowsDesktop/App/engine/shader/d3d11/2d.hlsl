//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

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
	float4 color	: COLOR0;
	float2 uv		: TEXCOORD0;
};

cbuffer VSConstants2D : register(b0)
{
	row_major float2x4 g_transform;
}

cbuffer PSConstants2D : register(b0)
{
	float4 g_colorMul;
	float4 g_colorAdd;
	//float4 g_sdfParam;
	//float4 g_sdfOutlineColor;
	//float4 g_sdfShadowColor;
	//float4 g_internal;
}

cbuffer PSPatternConstants2D : register(b1)
{
	row_major float2x4 g_uvTransform_params;
	float4 g_backgroundColor;
}

float4 s3d_positionTransform(float2 pos, float2x4 t)
{
	return float4((t._13_14 + (pos.x * t._11_12) + (pos.y * t._21_22)), t._23_24);
}

float4 s3d_colorTransform(float4 color)
{
	color *= g_colorMul;
	color += g_colorAdd;
	color.rgb *= color.a;
	return color;
}

PSInput VS(VSInput input)
{
	PSInput result;
	result.position	= s3d_positionTransform(input.position, g_transform);
	result.color	= input.color;
	result.uv		= input.uv;
	return result;
}

float4 PS_Shape(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	return s3d_colorTransform(result);
}

float4 PS_LineDotOld(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	const float u = (0.5 * (input.uv.x - 0.5));
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.5 - w), (0.5 + w), distance);
	result *= alpha;

	return s3d_colorTransform(result);
}

float4 PS_LineDot(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	const float u = (0.5 * (input.uv.x - 0.5));
	const float w = fwidth(u);
	const float value = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.5 - w), (0.5 + w), value);
	result *= alpha;

	return s3d_colorTransform(result);
}


float4 PS_LineDash(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	const float u = (0.25 * (input.uv.x - 1.0));
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.4 - w), (0.4 + w), distance);
	result *= alpha;

	return s3d_colorTransform(result);
}

float4 PS_LineLongDash(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	const float u = (0.1 * (input.uv.x - 1.0));
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.3 - w), (0.3 + w), distance);
	result *= alpha;

	return s3d_colorTransform(result);
}

float4 PS_LineDashDot(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	const float u = (0.1 * (input.uv.x - 1.0));
	const float u2 = (u + 0.5);
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float distance2 = abs(2.0 * frac(u2) - 1.0);
	const float alpha1 = smoothstep((0.4 - w), (0.4 + w), distance);
	const float alpha2 = smoothstep((0.9 - w), (0.9 + w), distance2);
	result *= max(alpha1, alpha2);

	return s3d_colorTransform(result);
}

float4 PS_LineRoundDot(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	const float2 uv = ((input.uv + float2(0.5, 0.0)) * float2(0.5, 1));
	const float w = fwidth(uv.y);
	const float distance = length(float2(4.0, 2.0) * frac(uv) - float2(2.0, 1.0));
	const float alpha = (1.0 - smoothstep((1.0 - w), (1.0 + w), distance));
	result *= alpha;

	return s3d_colorTransform(result);
}

float4 PS_PatternPolkaDot(PSInput input) : SV_TARGET
{
	const float2 uv = input.position.xy / 64;
	const float2 repeat = (2.0 * frac(uv) - 1.0);
	const float value = length(repeat);
	const float fw = (length(float2(ddx(value), ddy(value))) * 0.70710678118);
	const float c = smoothstep((0.5 - fw), (0.5 + fw), value);

	const float4 primary = input.color;
	const float4 background = float4(1, 1, 1, 0);
	const float4 result = lerp(primary, background, c);

	return s3d_colorTransform(result);
}
