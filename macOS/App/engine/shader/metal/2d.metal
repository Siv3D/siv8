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

float4 s3d_transform2D(float2 pos, float2x4 t)
{
	const float2 t_13_14 = float2(t[0][2], t[0][3]);
	const float2 t_11_12 = float2(t[0][0], t[0][1]);
	const float2 t_21_22 = float2(t[1][0], t[1][1]);
	return float4((t_13_14 + (pos.x * t_11_12) + (pos.y * t_21_22)), 0.0f, 1.0f);
}

vertex
PSInput VS_Shape(uint vertexID [[vertex_id]], constant VSInput* vertices, constant VSConstants2D* c)
{
	PSInput out;
	out.position	= s3d_transform2D(vertices[vertexID].position, c->g_transform);
	out.color		= vertices[vertexID].color;
	out.color.rgb	*= out.color.a;
	out.uv			= vertices[vertexID].uv;
	return out;
}

fragment
float4 PS_Shape(PSInput in [[stage_in]])
{
	return in.color;
}
