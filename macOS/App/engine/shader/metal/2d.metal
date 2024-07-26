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
	float4 color;
	float2 uv;
};

struct VSConstants2D
{
	float2x4 g_transform;
};

struct PSConstants2D
{
	float4 g_colorMul;
	float4 g_colorAdd;
};

float4 s3d_positionTransform2D(float2 pos, float2x4 t)
{
	const float2 t_13_14 = float2(t[0][2], t[0][3]);
	const float2 t_11_12 = float2(t[0][0], t[0][1]);
	const float2 t_21_22 = float2(t[1][0], t[1][1]);
	return float4((t_13_14 + (pos.x * t_11_12) + (pos.y * t_21_22)), 0.0f, 1.0f);
}

float4 s3d_colorTransform(float4 color, constant PSConstants2D* c)
{
	color *= c->g_colorMul;
	color += c->g_colorAdd;
	color.rgb *= color.a;
	return color;
}

vertex
PSInput VS_Shape(uint vertexID [[vertex_id]], constant VSInput* vertices, constant VSConstants2D* c)
{
	PSInput out;
	out.position	= s3d_positionTransform2D(vertices[vertexID].position, c->g_transform);
	out.color		= vertices[vertexID].color;
	out.uv			= vertices[vertexID].uv;
	return out;
}

fragment
float4 PS_Shape(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
	float4 result = in.color;

	return s3d_colorTransform(result, c);
}

fragment
float4 PS_LineDot(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
	float4 result = in.color;

	const float u = (0.5 * (in.uv.x - 0.5));
	const float w = fwidth(u);
	const float distance = abs(2.0 * fract(u) - 1.0);
	const float alpha = smoothstep((0.5 - w), (0.5 + w), distance);
	result.a *= alpha;

	return s3d_colorTransform(result, c);
}

fragment
float4 PS_LineDash(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
    float4 result = in.color;

    const float u = (0.25 * (in.uv.x - 1.0));
    const float w = fwidth(u);
    const float distance = abs(2.0 * fract(u) - 1.0);
    const float alpha = smoothstep((0.4 - w), (0.4 + w), distance);
    result.a *= alpha;

	return s3d_colorTransform(result, c);
}

fragment
float4 PS_LineLongDash(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
    float4 result = in.color;

    const float u = (0.1 * (in.uv.x - 1.0));
    const float w = fwidth(u);
    const float distance = abs(2.0 * fract(u) - 1.0);
    const float alpha = smoothstep((0.3 - w), (0.3 + w), distance);
    result.a *= alpha;

	return s3d_colorTransform(result, c);
}

fragment
float4 PS_LineDashDot(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
    float4 result = in.color;

    const float u = (0.1 * (in.uv.x - 1.0));
    const float u2 = u + 0.5;
    const float w = fwidth(u);
    const float distance = abs(2.0 * fract(u) - 1.0);
    const float distance2 = abs(2.0 * fract(u2) - 1.0);
    const float alpha1 = smoothstep((0.4 - w), (0.4 + w), distance);
    const float alpha2 = smoothstep((0.9 - w), (0.9 + w), distance2);
    result.a *= max(alpha1, alpha2);

	return s3d_colorTransform(result, c);
}

fragment
float4 PS_LineRoundDot(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
    float4 result = in.color;

    const float2 uv = ((in.uv + float2(0.5, 0.0)) * float2(0.5, 1));
    const float w = fwidth(uv.y);
    const float distance = length(float2(4.0, 2.0) * fract(uv) - float2(2.0, 1.0));
    const float alpha = (1.0 - smoothstep((1.0 - w), (1.0 + w), distance));
    result.a *= alpha;

	return s3d_colorTransform(result, c);
}
