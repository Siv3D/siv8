#include <metal_stdlib>
using namespace metal;

struct VertexData
{
	float4 position;
	float2 textureCoordinate;
};

struct VertexOut
{
	float4 position [[position]];
	float2 textureCoordinate;
};

vertex
VertexOut sceneVertexShader(uint vertexID [[vertex_id]], constant VertexData* vertices)
{
	VertexOut out;
	out.position = vertices[vertexID].position;
	out.textureCoordinate = vertices[vertexID].textureCoordinate;
	return out;
}

fragment
float4 sceneFragmentShader(VertexOut in [[stage_in]], texture2d<float> colorTexture [[texture(0)]])
{
	constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);

	return colorTexture.sample(textureSampler, in.textureCoordinate);
}
