//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

// DrawTriangles: generate three vertices per triangle from the vertex ID.
// User VS slot 2: origin/size, step/rotation (cos, sin), straight RGBA color.

struct PSInput
{
    float4 position : SV_POSITION;
    float4 colorPMA : COLOR0;
    float2 uv : TEXCOORD0;
};

cbuffer VSConstants2D : register(b0)
{
    row_major float3x4 g_transform;
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

cbuffer Triangles : register(b2)
{
    float4 g_originSize;
    float4 g_stepRotation;
    float4 g_color;
}

PSInput VS_Procedural(uint vertexID : SV_VertexID)
{
    const float2 corners[3] = { float2(0, 0), float2(1, 0), float2(0, 1) };
    const float2 corner = corners[vertexID % 3] * g_originSize.zw;
    const float cosine = g_stepRotation.z, sine = g_stepRotation.w;
    const float2 rotated = float2(corner.x * cosine - corner.y * sine, corner.x * sine + corner.y * cosine);
    const float2 position = g_originSize.xy + rotated + float(vertexID / 3) * g_stepRotation.xy;
    const float4 clip = mul(float3(position, 1), g_transform);
    const float4 color = g_color * g_colorMul;
    PSInput result;
    result.position = float4(clip.xy, 0, clip.w);
    result.colorPMA = float4(color.rgb * color.a, color.a);
    result.uv = corners[vertexID % 3];
    return result;
}

float4 PS_Procedural(PSInput input) : SV_TARGET
{
    return input.colorPMA + g_colorAdd * input.colorPMA.a;
}
