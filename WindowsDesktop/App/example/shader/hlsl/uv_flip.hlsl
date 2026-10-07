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

// VS_UVFlip: mirror the full texture horizontally (UV range 0..1).
// Pair with the standard texture PS or a texture-effect PS.
// Cropped regions, atlas glyphs, patterns, and quad-warp UVs are not supported.

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

inline float4 s3d_positionTransform(const float2 position, const float3x4 transform)
{
	const float4 clip = mul(float3(position, 1.0f), transform);
	return float4(clip.xy, 0.0f, clip.w);
}

inline float4 s3d_premultiplyAlpha(const float4 color)
{
	return float4((color.rgb * color.a), color.a);
}

PSInput VS_UVFlip(VSInput input)
{
	PSInput result;
	result.position	= s3d_positionTransform(input.position, g_transform);
	result.colorPMA = s3d_premultiplyAlpha(input.color * g_colorMul);
	result.uv		= float2((1.0f - input.uv.x), input.uv.y);
	return result;
}
