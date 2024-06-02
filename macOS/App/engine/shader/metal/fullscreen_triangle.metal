#include <metal_stdlib>
using namespace metal;

struct VertexOut
{
	float4 position [[position]];
	float2 uv;
};

vertex
VertexOut VS_FullscreenTriangle(uint vertexID [[vertex_id]])
{
	VertexOut result;
	result.position = float4((vertexID == 2 ? 3.0 : -1.0),
							 (vertexID == 1 ? 3.0 : -1.0),
							 0.0,
							 1.0);
	result.uv = float2((vertexID == 2 ? 2.0 : 0.0),
					   (vertexID == 1 ? -1.0 : 1.0));
	
	return result;
}

fragment
float4 PS_FullscreenTriangle(VertexOut in [[stage_in]], texture2d<float> colorTexture [[texture(0)]])
{
	constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);

	return colorTexture.sample(textureSampler, in.uv);
}
