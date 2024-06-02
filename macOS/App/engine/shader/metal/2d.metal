#include <metal_stdlib>
using namespace metal;

struct Vertex2D
{
	simd::float2 pos;
	simd::float2 uv;
	simd::float4 color;
};

vertex
float4 VS_Shape(uint vertexID [[vertex_id]], constant Vertex2D* vertices)
{
	float4 vertexOutPositions(vertices[vertexID].pos, 0.0f, 1.0f);
	return vertexOutPositions;
}

fragment
float4 PS_Shape(float4 vertexOutPositions [[stage_in]])
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
