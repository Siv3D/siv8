#include <metal_stdlib>
using namespace metal;

vertex
float4 vertexShader(uint vertexID [[vertex_id]], constant simd::float3* vertexPositions)
{
	float4 vertexOutPositions(vertexPositions[vertexID][0], vertexPositions[vertexID][1], vertexPositions[vertexID][2], 1.0f);
	return vertexOutPositions;
}

fragment
float4 fragmentShader(float4 vertexOutPositions [[stage_in]])
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
