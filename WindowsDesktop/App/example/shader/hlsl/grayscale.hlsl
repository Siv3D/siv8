Texture2D		g_texture0 : register(t0);
SamplerState	g_sampler0 : register(s0);

struct PSInput
{
	float4 position	: SV_POSITION;
	float4 colorPMA	: COLOR0;
	float2 uv		: TEXCOORD0;
};

cbuffer PSConstants2D : register(b0)
{
	float4 g_patternBackgroundColorMul;
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColorPMA;
	float4 g_sdfShadowColorPMA;
}

inline float4 s3d_textureColor(float4 vertexColorPMA, const float4 textureColorPMA)
{
	vertexColorPMA *= textureColorPMA;
	return (vertexColorPMA + (g_colorAdd * vertexColorPMA.a));
}

float4 PS_Grayscale(PSInput input) : SV_TARGET
{
	const float4 colorPMA = s3d_textureColor(input.colorPMA, g_texture0.Sample(g_sampler0, input.uv));
	const float yPMA = dot(colorPMA.rgb, float3(0.299, 0.587, 0.114));
	return float4(yPMA, yPMA, yPMA, colorPMA.a);
}
