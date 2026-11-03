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

// Shared constant layout: Siv3D/RenderTextureOps/TextureProcessing.hpp

#include <metal_stdlib>
using namespace metal;

struct ProcessingConstants
{
	float4 sourceUV;
	float4 sourceBounds;
	float4 secondaryUV;
	float4 secondaryBounds;
	float4 parameters;
	float4 samples[49];
};

struct ProcessingVertex
{
	float4 position [[position]];
	float2 uv;
	float2 secondaryUV;
};

vertex
ProcessingVertex VS_TextureProcessing(const uint vertexID [[vertex_id]],
	constant ProcessingConstants& constants [[buffer(0)]])
{
	ProcessingVertex result;

	result.position = float4((vertexID == 2 ? 3.0 : -1.0),
							 (vertexID == 1 ? 3.0 : -1.0),
							 0.0,
							 1.0);
	const float2 uv = float2((vertexID == 2 ? 2.0 : 0.0),
						 (vertexID == 1 ? -1.0 : 1.0));
	result.uv = constants.sourceUV.xy + uv * constants.sourceUV.zw;
	result.secondaryUV = constants.secondaryUV.xy + uv * constants.secondaryUV.zw;

	return result;
}

inline float4 s3d_processingSample(texture2d<float> colorTexture, sampler textureSampler, const float2 uv,
	constant ProcessingConstants& constants, const bool bounded)
{
	return colorTexture.sample(textureSampler, bounded ? clamp(uv, constants.sourceBounds.xy, constants.sourceBounds.zw) : uv, level(0));
}

inline float4 s3d_processingResample(texture2d<float> colorTexture, sampler textureSampler, const float2 uv,
	constant ProcessingConstants& constants, const bool bounded)
{
	return s3d_processingSample(colorTexture, textureSampler, uv, constants, bounded);
}

inline float4 s3d_processingDownsample(texture2d<float> colorTexture, sampler textureSampler, const float2 uv,
	constant ProcessingConstants& constants, const bool bounded)
{
	const float2 offset = constants.parameters.xy;
	return (s3d_processingSample(colorTexture, textureSampler, uv + float2(-offset.x, -offset.y), constants, bounded)
		+ s3d_processingSample(colorTexture, textureSampler, uv + float2(offset.x, -offset.y), constants, bounded)
		+ s3d_processingSample(colorTexture, textureSampler, uv + float2(-offset.x, offset.y), constants, bounded)
		+ s3d_processingSample(colorTexture, textureSampler, uv + offset, constants, bounded)) * 0.25;
}

inline float4 s3d_processingGaussian(texture2d<float> colorTexture, sampler textureSampler, const float2 uv,
	constant ProcessingConstants& constants, const bool bounded)
{
	float4 result = s3d_processingSample(colorTexture, textureSampler, uv, constants, bounded) * constants.samples[0].x;
	for (uint i = 1; i < uint(constants.parameters.w); ++i)
	{
		const float2 offset = constants.parameters.xy * constants.samples[i].y;
		result += (s3d_processingSample(colorTexture, textureSampler, uv - offset, constants, bounded)
			+ s3d_processingSample(colorTexture, textureSampler, uv + offset, constants, bounded)) * constants.samples[i].x;
	}

	return result;
}

inline float4 s3d_processingDualUp(texture2d<float> colorTexture, sampler textureSampler, const float2 uv,
	constant ProcessingConstants& constants, const bool bounded)
{
	const float2 offset = constants.parameters.xy;
	return (s3d_processingSample(colorTexture, textureSampler, uv + float2(-2.0 * offset.x, 0.0), constants, bounded)
		+ s3d_processingSample(colorTexture, textureSampler, uv + float2(2.0 * offset.x, 0.0), constants, bounded)
		+ s3d_processingSample(colorTexture, textureSampler, uv + float2(0.0, -2.0 * offset.y), constants, bounded)
		+ s3d_processingSample(colorTexture, textureSampler, uv + float2(0.0, 2.0 * offset.y), constants, bounded)
		+ 2.0 * (s3d_processingSample(colorTexture, textureSampler, uv + float2(-offset.x, -offset.y), constants, bounded)
			+ s3d_processingSample(colorTexture, textureSampler, uv + float2(offset.x, -offset.y), constants, bounded)
			+ s3d_processingSample(colorTexture, textureSampler, uv + float2(-offset.x, offset.y), constants, bounded)
			+ s3d_processingSample(colorTexture, textureSampler, uv + offset, constants, bounded))) / 12.0;
}

fragment
float4 PS_ProcessingResample(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]])
{
	return s3d_processingResample(colorTexture, textureSampler, in.uv, constants, false);
}

fragment
float4 PS_ProcessingResampleRegion(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]])
{
	return s3d_processingResample(colorTexture, textureSampler, in.uv, constants, true);
}

fragment
float4 PS_ProcessingDownsample(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]])
{
	return s3d_processingDownsample(colorTexture, textureSampler, in.uv, constants, false);
}

fragment
float4 PS_ProcessingDownsampleRegion(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]])
{
	return s3d_processingDownsample(colorTexture, textureSampler, in.uv, constants, true);
}

fragment
float4 PS_ProcessingGaussian(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]])
{
	return s3d_processingGaussian(colorTexture, textureSampler, in.uv, constants, false);
}

fragment
float4 PS_ProcessingGaussianRegion(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]])
{
	return s3d_processingGaussian(colorTexture, textureSampler, in.uv, constants, true);
}

fragment
float4 PS_ProcessingDualUp(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]])
{
	return s3d_processingDualUp(colorTexture, textureSampler, in.uv, constants, false);
}

fragment
float4 PS_ProcessingDualUpRegion(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]])
{
	return s3d_processingDualUp(colorTexture, textureSampler, in.uv, constants, true);
}

fragment
float4 PS_ProcessingDualBlend(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]],
	texture2d<float> secondaryTexture [[texture(1)]])
{
	return mix(secondaryTexture.sample(textureSampler, in.secondaryUV, level(0)),
			   s3d_processingDualUp(colorTexture, textureSampler, in.uv, constants, false), constants.parameters.z);
}

fragment
float4 PS_ProcessingDualBlendRegion(ProcessingVertex in [[stage_in]],
	constant ProcessingConstants& constants [[buffer(0)]],
	texture2d<float> colorTexture [[texture(0)]], sampler textureSampler [[sampler(0)]],
	texture2d<float> secondaryTexture [[texture(1)]])
{
	return mix(secondaryTexture.sample(textureSampler,
					clamp(in.secondaryUV, constants.secondaryBounds.xy, constants.secondaryBounds.zw), level(0)),
			   s3d_processingDualUp(colorTexture, textureSampler, in.uv, constants, true), constants.parameters.z);
}
