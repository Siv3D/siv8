#include <metal_stdlib>
using namespace metal;

struct PSInput
{
	float4 position [[position]];
	float4 colorPMA;
	float2 uv;
};

struct PSConstants2D
{
	float4 g_patternBackgroundColorMul;
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColorPMA;
	float4 g_sdfShadowColorPMA;
};

inline float4 s3d_textureColor(float4 vertexColor, float4 textureColor, constant PSConstants2D* c)
{
	vertexColor *= textureColor;
	return (vertexColor + (c->g_colorAdd * vertexColor.a));
}

fragment
float4 Grayscale(	PSInput input [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]],
					texture2d<float> texture0 [[texture(0)]],
					sampler sampler0 [[sampler(0)]])
{
	const float4 colorPMA = s3d_textureColor(input.colorPMA, texture0.sample(sampler0, input.uv), c0);
	const float yPMA = dot(colorPMA.rgb, float3(0.299f, 0.587f, 0.114f));
	return float4(yPMA, yPMA, yPMA, colorPMA.a);
}
