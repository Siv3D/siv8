//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

// DrawTriangles: generate three vertices per triangle from the vertex ID.
// User VS slot 2: origin/size, step/rotation (cos, sin), straight RGBA color.

#include <metal_stdlib>
using namespace metal;

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

struct Triangles
{
    float4 g_originSize;
    float4 g_stepRotation;
    float4 g_color;
};

vertex PSInput VS_Procedural(uint vertexID [[vertex_id]],
    constant VSConstants2D& c0 [[buffer(1)]], constant Triangles& c2 [[buffer(2)]])
{
    const float2 corners[3] = { float2(0, 0), float2(1, 0), float2(0, 1) };
    const float2 corner = corners[vertexID % 3] * c2.g_originSize.zw;
    const float cosine = c2.g_stepRotation.z, sine = c2.g_stepRotation.w;
    const float2 rotated = float2(corner.x * cosine - corner.y * sine, corner.x * sine + corner.y * cosine);
    const float2 position = c2.g_originSize.xy + rotated + float(vertexID / 3) * c2.g_stepRotation.xy;
    const float4 clip = c0.g_transform * float3(position, 1);
    const float4 color = c2.g_color * c0.g_colorMul;
    PSInput result;
    result.position = float4(clip.xy, 0, clip.w);
    result.colorPMA = float4(color.rgb * color.a, color.a);
    result.uv = corners[vertexID % 3];
    return result;
}

fragment float4 PS_Procedural(PSInput input [[stage_in]], constant PSConstants2D& c0 [[buffer(0)]])
{
    return input.colorPMA + c0.g_colorAdd * input.colorPMA.a;
}
