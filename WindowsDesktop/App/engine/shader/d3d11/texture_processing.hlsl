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

struct ProcessingConstants
{
	float4 sourceUV;
	float4 sourceBounds;
	float4 secondaryUV;
	float4 secondaryBounds;
	float4 parameters;
};

cbuffer TextureProcessingConstants : register(b0)
{
	ProcessingConstants g_processingConstants;
}

cbuffer TextureProcessingGaussianSamples : register(b1)
{
	float4 g_gaussianSamples[49];
}

Texture2D<float4> g_texture0 : register(t0);
Texture2D<float4> g_texture1 : register(t1);
SamplerState g_sampler0 : register(s0);

struct ProcessingVertex
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float2 secondaryUV : TEXCOORD1;
};

ProcessingVertex VS_TextureProcessing(const uint vertexID : SV_VertexID)
{
	ProcessingVertex result;

	result.position = float4((vertexID == 2 ? 3.0 : -1.0),
							 (vertexID == 1 ? 3.0 : -1.0),
							 0.0,
							 1.0);
	const float2 uv = float2((vertexID == 2 ? 2.0 : 0.0),
						 (vertexID == 1 ? -1.0 : 1.0));
	result.uv = g_processingConstants.sourceUV.xy + uv * g_processingConstants.sourceUV.zw;
	result.secondaryUV = g_processingConstants.secondaryUV.xy + uv * g_processingConstants.secondaryUV.zw;

	return result;
}

inline float4 s3d_processingSample(const float2 uv, const bool bounded)
{
	return g_texture0.SampleLevel(g_sampler0,
		bounded ? clamp(uv, g_processingConstants.sourceBounds.xy, g_processingConstants.sourceBounds.zw) : uv, 0);
}

inline float4 s3d_processingResample(const float2 uv, const bool bounded)
{
	return s3d_processingSample(uv, bounded);
}

inline float4 s3d_processingDownsample(const float2 uv, const bool bounded)
{
	const float2 offset = g_processingConstants.parameters.xy;
	return (s3d_processingSample(uv + float2(-offset.x, -offset.y), bounded)
		+ s3d_processingSample(uv + float2(offset.x, -offset.y), bounded)
		+ s3d_processingSample(uv + float2(-offset.x, offset.y), bounded)
		+ s3d_processingSample(uv + offset, bounded)) * 0.25;
}

inline float4 s3d_processingGaussian(const float2 uv, const bool bounded)
{
	float4 result = s3d_processingSample(uv, bounded) * g_gaussianSamples[0].x;
	for (uint i = 1; i < uint(g_processingConstants.parameters.w); ++i)
	{
		const float2 offset = g_processingConstants.parameters.xy * g_gaussianSamples[i].y;
		result += (s3d_processingSample(uv - offset, bounded)
			+ s3d_processingSample(uv + offset, bounded)) * g_gaussianSamples[i].x;
	}

	return result;
}

inline float4 s3d_processingDualUp(const float2 uv, const bool bounded)
{
	const float2 offset = g_processingConstants.parameters.xy;
	return (s3d_processingSample(uv + float2(-2.0 * offset.x, 0.0), bounded)
		+ s3d_processingSample(uv + float2(2.0 * offset.x, 0.0), bounded)
		+ s3d_processingSample(uv + float2(0.0, -2.0 * offset.y), bounded)
		+ s3d_processingSample(uv + float2(0.0, 2.0 * offset.y), bounded)
		+ 2.0 * (s3d_processingSample(uv + float2(-offset.x, -offset.y), bounded)
			+ s3d_processingSample(uv + float2(offset.x, -offset.y), bounded)
			+ s3d_processingSample(uv + float2(-offset.x, offset.y), bounded)
			+ s3d_processingSample(uv + offset, bounded))) / 12.0;
}

float4 PS_ProcessingResample(const ProcessingVertex input) : SV_TARGET
{
	return s3d_processingResample(input.uv, false);
}

float4 PS_ProcessingResampleRegion(const ProcessingVertex input) : SV_TARGET
{
	return s3d_processingResample(input.uv, true);
}

float4 PS_ProcessingDownsample(const ProcessingVertex input) : SV_TARGET
{
	return s3d_processingDownsample(input.uv, false);
}

float4 PS_ProcessingDownsampleRegion(const ProcessingVertex input) : SV_TARGET
{
	return s3d_processingDownsample(input.uv, true);
}

float4 PS_ProcessingGaussian(const ProcessingVertex input) : SV_TARGET
{
	return s3d_processingGaussian(input.uv, false);
}

float4 PS_ProcessingGaussianRegion(const ProcessingVertex input) : SV_TARGET
{
	return s3d_processingGaussian(input.uv, true);
}

float4 PS_ProcessingDualUp(const ProcessingVertex input) : SV_TARGET
{
	return s3d_processingDualUp(input.uv, false);
}

float4 PS_ProcessingDualUpRegion(const ProcessingVertex input) : SV_TARGET
{
	return s3d_processingDualUp(input.uv, true);
}

float4 PS_ProcessingDualBlend(const ProcessingVertex input) : SV_TARGET
{
	return lerp(g_texture1.SampleLevel(g_sampler0, input.secondaryUV, 0),
				s3d_processingDualUp(input.uv, false), g_processingConstants.parameters.z);
}

float4 PS_ProcessingDualBlendRegion(const ProcessingVertex input) : SV_TARGET
{
	return lerp(g_texture1.SampleLevel(g_sampler0,
					clamp(input.secondaryUV, g_processingConstants.secondaryBounds.xy, g_processingConstants.secondaryBounds.zw), 0),
				s3d_processingDualUp(input.uv, true), g_processingConstants.parameters.z);
}
