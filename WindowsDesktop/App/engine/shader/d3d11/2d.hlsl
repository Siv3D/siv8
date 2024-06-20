﻿//-----------------------------------------------
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
	float4 g_colorMul;
}

cbuffer PSConstants2D : register(b0)
{
	float4 g_colorAdd;
	//float4 g_sdfParam;
	//float4 g_sdfOutlineColor;
	//float4 g_sdfShadowColor;
	//float4 g_internal;
}

float4 s3d_transform2D(float2 pos, float2x4 t)
{
	return float4((t._13_14 + (pos.x * t._11_12) + (pos.y * t._21_22)), t._23_24);
}

PSInput VS(VSInput input)
{
	PSInput result;
	result.position	= s3d_transform2D(input.position, g_transform);
	result.color	= (input.color * g_colorMul);
	result.color.rgb *= result.color.a;
	result.uv		= input.uv;
	return result;
}

float4 PS_Shape(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	result.rgb += (g_colorAdd.rgb * result.a);

	return result;
}
