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

#include <metal_stdlib>
using namespace metal;

struct VSInput
{
	float2 position;
	float2 uv;
	float4 color;
};

struct PSInput
{
	float4 position [[position]];
	float4 colorPMA;
	float2 uv;
};

struct VSConstants2D
{
	float2x4 g_transform;
	float4 g_colorMul;
};

inline float2 s3d_transformPoint2D(const float2 position, const float2x4 transform)
{
	const float2 translation = transform[0].zw;
	const float2 basisX = transform[0].xy;
	const float2 basisY = transform[1].xy;
	return (translation + (position.x * basisX) + (position.y * basisY));
}

inline float4 s3d_positionTransform(const float2 position, const float2x4 transform)
{
	return float4(s3d_transformPoint2D(position, transform), 0.0f, 1.0f);
}

inline float4 s3d_premultiplyAlpha(float4 color)
{
	return float4((color.rgb * color.a), color.a);
}

vertex
PSInput VS_UVFlip(	uint vertexID [[vertex_id]],
					constant VSInput* vertices [[buffer(0)]],
					constant VSConstants2D* c0 [[buffer(1)]])
{
	PSInput result;
	result.position	= s3d_positionTransform(vertices[vertexID].position, c0->g_transform);
	result.colorPMA	= s3d_premultiplyAlpha(vertices[vertexID].color * c0->g_colorMul);
	result.uv		= float2((1.0f - vertices[vertexID].uv.x), vertices[vertexID].uv.y);
	return result;
}
