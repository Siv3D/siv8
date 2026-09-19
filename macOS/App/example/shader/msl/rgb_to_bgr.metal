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

#include <metal_stdlib>
using namespace metal;

struct PSInput
{
	float4 position [[position]];
	float4 colorPMA;
	float2 uv;
};

struct PSConstants2D
{
	float4 g_patternBackgroundColorMul;
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColorPMA;
	float4 g_sdfShadowColorPMA;
};

inline float4 s3d_textureColor(float4 vertexColorPMA, const float4 textureColorPMA, constant PSConstants2D* c)
{
	vertexColorPMA *= textureColorPMA;
	return (vertexColorPMA + (c->g_colorAdd * vertexColorPMA.a));
}

fragment
float4 PS_RGBToBGR(	PSInput input [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]],
					texture2d<float> texture0 [[texture(0)]],
					sampler sampler0 [[sampler(0)]])
{
	const float4 colorPMA = s3d_textureColor(input.colorPMA, texture0.sample(sampler0, input.uv), c0);
	return colorPMA.bgra;
}
