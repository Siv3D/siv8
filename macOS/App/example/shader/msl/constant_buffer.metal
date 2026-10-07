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
	float3x4 g_transform;
	float4 g_colorMul;
};

struct PSConstants2D
{
	float4 g_patternBackgroundColorMul;
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColorPMA;
	float4 g_sdfShadowColorPMA;
};

inline float4 s3d_positionTransform(const float2 position, const float3x4 transform)
{
	const float4 clip = transform * float3(position, 1.0f);
	return float4(clip.xy, 0.0f, clip.w);
}

inline float4 s3d_premultiplyAlpha(float4 color)
{
	return float4((color.rgb * color.a), color.a);
}

inline float4 s3d_applyColorAdd(const float4 colorPMA, const float4 colorAdd)
{
	return (colorPMA + (colorAdd * colorPMA.a));
}

inline float4 s3d_textureColor(float4 vertexColorPMA, const float4 textureColorPMA, constant PSConstants2D* c)
{
	vertexColorPMA *= textureColorPMA;
	return s3d_applyColorAdd(vertexColorPMA, c->g_colorAdd);
}

// User constants: Float4 offset (xy in local drawing coordinates), Float4 tint
// (straight RGBA). Both stages use slot 2, independently.
struct VSOffset
{
	float4 g_offset;
};

struct PSTint
{
	float4 g_tint;
};

vertex
PSInput VS_Offset(	uint vertexID [[vertex_id]],
					constant VSInput* vertices [[buffer(0)]],
					constant VSConstants2D* c0 [[buffer(1)]],
					constant VSOffset* c2 [[buffer(2)]])
{
	PSInput result;
	result.position = s3d_positionTransform((vertices[vertexID].position + c2->g_offset.xy), c0->g_transform);
	result.colorPMA = s3d_premultiplyAlpha(vertices[vertexID].color * c0->g_colorMul);
	result.uv = vertices[vertexID].uv;
	return result;
}

fragment
float4 PS_TintShape(	PSInput input [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]],
					constant PSTint* c2 [[buffer(2)]])
{
	return (s3d_applyColorAdd(input.colorPMA, c0->g_colorAdd) * s3d_premultiplyAlpha(c2->g_tint));
}

fragment
float4 PS_TintTexture(	PSInput input [[stage_in]],
						constant PSConstants2D* c0 [[buffer(0)]],
						constant PSTint* c2 [[buffer(2)]],
						texture2d<float> texture0 [[texture(0)]],
						sampler sampler0 [[sampler(0)]])
{
	return (s3d_textureColor(input.colorPMA, texture0.sample(sampler0, input.uv), c0) * s3d_premultiplyAlpha(c2->g_tint));
}
