//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

Texture2D		g_texture0 : register(t0);
SamplerState	g_sampler0 : register(s0);

struct VSInput
{
	float2 position	: POSITION;
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0;
};

struct PSInput
{
	float4 position	: SV_POSITION;
	float4 colorPMA	: COLOR0;
	float2 uv		: TEXCOORD0;
};

cbuffer VSConstants2D : register(b0)
{
	row_major float2x4 g_transform;
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

cbuffer PSEffectConstants2D : register(b1)
{
	row_major float2x4 g_patternUVTransform;
	float4 g_patternBackgroundColor;
	row_major float3x3 g_quadWarpInvHomography;
	float4 g_quadWarpUVTransform;
}

inline float4 s3d_positionTransform(const float2 pos, const float2x4 t)
{
	return float4((t._13_14 + (pos.x * t._11_12) + (pos.y * t._21_22)), t._23_24);
}

inline float4 s3d_premultiplyAlpha(const float4 color)
{
	return float4((color.rgb * color.a), color.a);
}

inline float4 s3d_shapeColor(const float4 vertexColorPMA)
{
	return (vertexColorPMA + (g_colorAdd * vertexColorPMA.a));
}

inline float4 s3d_textureColor(float4 vertexColorPMA, const float4 textureColorPMA)
{
	vertexColorPMA *= textureColorPMA;
	return (vertexColorPMA + (g_colorAdd * vertexColorPMA.a));
}

////////////////////////////////////////////////////////////////
//
//	Basic VS / PS
//
////////////////////////////////////////////////////////////////

PSInput VS_Shape(VSInput input)
{
	PSInput result;
	result.position	= s3d_positionTransform(input.position, g_transform);
	result.colorPMA = s3d_premultiplyAlpha(input.color * g_colorMul);
	result.uv		= input.uv;
	return result;
}

PSInput VS_QuadWarp(VSInput input)
{
	PSInput result;
	result.position	= s3d_positionTransform(input.position, g_transform);
	result.colorPMA = s3d_premultiplyAlpha(input.color * g_colorMul);
	result.uv		= input.position;
	return result;
}

float4 PS_Shape(PSInput input) : SV_TARGET
{
	return s3d_shapeColor(input.colorPMA);
}

float4 PS_Texture(PSInput input) : SV_TARGET
{
	return s3d_textureColor(input.colorPMA, g_texture0.Sample(g_sampler0, input.uv));
}

float4 PS_QuadWarp(PSInput input) : SV_TARGET
{
	const float3 t = mul(float3(input.uv, 1.0f), g_quadWarpInvHomography);
	const float2 uv = ((t.xy / t.z) * g_quadWarpUVTransform.xy + g_quadWarpUVTransform.zw);
	return s3d_textureColor(input.colorPMA, g_texture0.Sample(g_sampler0, uv));
}

////////////////////////////////////////////////////////////////
//
//	Line Patterns
//
////////////////////////////////////////////////////////////////

float4 PS_LineDot(PSInput input) : SV_TARGET
{
	float4 result = input.colorPMA;

	const float u = (0.5 * (input.uv.x - 0.5));
	const float w = fwidth(u);
	const float value = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.5 - w), (0.5 + w), value);
	result *= alpha;

	return s3d_shapeColor(result);
}

float4 PS_LineDash(PSInput input) : SV_TARGET
{
	float4 result = input.colorPMA;

	const float u = (0.25 * (input.uv.x - 1.0));
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.4 - w), (0.4 + w), distance);
	result *= alpha;

	return s3d_shapeColor(result);
}

float4 PS_LineLongDash(PSInput input) : SV_TARGET
{
	float4 result = input.colorPMA;

	const float u = (0.1 * (input.uv.x - 1.0));
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.3 - w), (0.3 + w), distance);
	result *= alpha;

	return s3d_shapeColor(result);
}

float4 PS_LineDashDot(PSInput input) : SV_TARGET
{
	float4 result = input.colorPMA;

	const float u = (0.1 * (input.uv.x - 1.0));
	const float u2 = (u + 0.5);
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float distance2 = abs(2.0 * frac(u2) - 1.0);
	const float alpha1 = smoothstep((0.4 - w), (0.4 + w), distance);
	const float alpha2 = smoothstep((0.9 - w), (0.9 + w), distance2);
	result *= max(alpha1, alpha2);

	return s3d_shapeColor(result);
}

float4 PS_LineRoundDot(PSInput input) : SV_TARGET
{
	float4 result = input.colorPMA;

	const float2 uv = ((input.uv + float2(0.5, 0.0)) * float2(0.5, 1));
	const float w = fwidth(uv.y);
	const float distance = length(float2(4.0, 2.0) * frac(uv) - float2(2.0, 1.0));
	const float alpha = (1.0 - smoothstep((1.0 - w), (1.0 + w), distance));
	result *= alpha;

	return s3d_shapeColor(result);
}

////////////////////////////////////////////////////////////////
//
//	Patterns
//
////////////////////////////////////////////////////////////////
	
inline float4 Pattern_BackgroundColor()
{
	const float4 colorPMA = s3d_premultiplyAlpha(g_patternBackgroundColor * g_patternBackgroundColorMul);
	return (colorPMA + (g_colorAdd * colorPMA.a));
}

inline float2 Pattern_UVTransform(const float2 uv)
{
	return (g_patternUVTransform._13_14 + (uv.x * g_patternUVTransform._11_12) + (uv.y * g_patternUVTransform._21_22));
}

inline float2 Pattern_Integral(float2 v)
{
	v /= 2.0;
	return (floor(v) + max((2.0 * frac(v) - 1.0), 0.0));
}

inline float Pattern_CheckersFiltered(const float2 p, const float2 hv)
{
	const float2 fw = fwidth(p);
	const float w = max(fw.x, fw.y);
	float2 i = (Pattern_Integral(p + 0.5 * w) - Pattern_Integral(p - 0.5 * w));
	i *= hv;
	i /= w;
	return (i.x + i.y - 2.0 * i.x * i.y);
}

inline float2 Pattern_Skew(const float2 v)
{
	const float2x2 transform = float2x2(1.0, (1.0 / tan(3.1415926535 / 3.0)), 0.0, (1.0 / sin(3.1415926535 / 3.0)));
	return mul(transform, v);
}

inline float Pattern_Hex(const float2 p)
{
	const float2 HEX = float2(1, 1.73205081);
	const float4 t = (floor(float4(p, p - float2(0.5, 1)) / HEX.xyxy) + float4(0.5, 0.5, 0.5, 0.5));
	const float4 h = float4((p - t.xy * HEX), (p - (t.zw + float2(0.5, 0.5)) * HEX));
	const float2 hex = abs((dot(h.xy, h.xy) < dot(h.zw, h.zw)) ? h.xy : h.zw);
	return max(dot(hex, (HEX * 0.5)), hex.x);
}

float4 PS_PatternPolkaDot(PSInput input) : SV_TARGET
{
	const float2 uv = Pattern_UVTransform(input.position.xy);
	const float2 repeat = (2.0 * frac(uv) - 1.0);
	const float value = length(repeat);
	const float fw = (length(float2(ddx(value), ddy(value))) * 0.70710678118);

	const float radiusScale = g_patternUVTransform[1].z;
	const float c = smoothstep((radiusScale - fw), (radiusScale + fw), value);

	const float4 primary = s3d_shapeColor(input.colorPMA);
	const float4 background = Pattern_BackgroundColor();

	return lerp(primary, background, c);
}

float4 PS_PatternStripe(PSInput input) : SV_TARGET
{
	const float u = Pattern_UVTransform(input.position.xy).x;
	const float fw = fwidth(u);
	const float repeat = (2.0 * frac(u) - 1.0);
	const float value = abs(repeat);

	const float thicknessScale = (g_patternUVTransform[1].z * (1 + 2 * fw) - fw);
	const float c = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);

	const float4 primary = s3d_shapeColor(input.colorPMA);
	const float4 background = Pattern_BackgroundColor();

	return lerp(primary, background, c);
}

float4 PS_PatternGrid(PSInput input) : SV_TARGET
{
	const float2 uv = Pattern_UVTransform(input.position.xy);
	const float2 fw = fwidth(uv);
	const float2 repeat = (2.0 * frac(uv) - 1.0);
	const float2 value = abs(repeat);

	const float2 thicknessScale = (g_patternUVTransform[1].zz * float2(1 + fw) - fw);
	const float2 c = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);
	const float c2 = min(c.x, c.y);

	const float4 primary = s3d_shapeColor(input.colorPMA);
	const float4 background = Pattern_BackgroundColor();

	return lerp(primary, background, c2);
}

float4 PS_PatternChecker(PSInput input) : SV_TARGET
{
	const float2 uv = Pattern_UVTransform(input.position.xy);
	const float c = Pattern_CheckersFiltered(uv, g_patternUVTransform[1].zw);

	const float4 primary = s3d_shapeColor(input.colorPMA);
	const float4 background = Pattern_BackgroundColor();

	return lerp(primary, background, c);
}

float4 PS_PatternTriangle(PSInput input) : SV_TARGET
{
	const float2 uv = Pattern_UVTransform(input.position.xy);
	const float2 fw = (fwidth(uv) * 0.25);

	const float2 s1 = Pattern_Skew(uv + float2(-fw.x, -fw.y));
	const float2 s2 = Pattern_Skew(uv + float2(fw.x, fw.y));
	const float2 s3 = Pattern_Skew(uv + float2(-fw.x, fw.y));
	const float2 s4 = Pattern_Skew(uv + float2(fw.x, -fw.y));

	const float4 f1 = frac(float4(s1, s2));
	const float4 f2 = frac(float4(s3, s4));
	const float4 ss = float4(step(f1.x, f1.y), step(f1.z, f1.w), step(f2.x, f2.y), step(f2.z, f2.w));
	const float c = dot(ss, 0.25);

	const float4 primary = s3d_shapeColor(input.colorPMA);
	const float4 background = Pattern_BackgroundColor();

	return lerp(primary, background, c);
}

float4 PS_PatternHexGrid(PSInput input) : SV_TARGET
{
	const float2 uv = Pattern_UVTransform(input.position.xy);
	const float2 fw = fwidth(uv);
	const float w = (max(fw.x, fw.y) * 0.5);

	const float thicknessScale = (g_patternUVTransform[1].z * (1 + 2 * w));
	const float h = Pattern_Hex(uv);
	const float c = smoothstep((thicknessScale - w), (thicknessScale + w), h);

	const float4 primary = s3d_shapeColor(input.colorPMA);
	const float4 background = Pattern_BackgroundColor();

	return lerp(background, primary, c);
}

////////////////////////////////////////////////////////////////
//
//	MSDF Font
//
////////////////////////////////////////////////////////////////

static const float MSDF_PixelRange = 16.0;
static const float MSDF_TextThreshold = 0.5;

struct MSDFState
{
	float2 textureSize;
	float2 invTextureSize;
	float scale;
};

inline MSDFState MSDF_Init(const float2 uv)
{
	MSDFState st;
	g_texture0.GetDimensions(st.textureSize.x, st.textureSize.y);
	st.invTextureSize = rcp(st.textureSize);

	const float2 msdfUnit = (MSDF_PixelRange * st.invTextureSize);
	const float2 screenPixelRange = (0.5 / fwidth(uv));
	st.scale = dot(msdfUnit, screenPixelRange);

	return st;
}

inline float MSDF_Median(const float r, const float g, const float b)
{
	return max(min(r, g), min(max(r, g), b));
}

inline float MSDF_Median(const float3 rgb)
{
	return MSDF_Median(rgb.r, rgb.g, rgb.b);
}

inline float MSDF_SampleDistance(const float2 uv)
{
	return MSDF_Median(g_texture0.Sample(g_sampler0, uv).rgb);
}

inline float MSDF_Coverage(const float d, const float threshold, const float scale)
{
	return saturate((d - threshold) * scale + 0.5);
}

inline float MSDF_AlphaAt(const float2 uv, const float threshold, const MSDFState st)
{
	return MSDF_Coverage(MSDF_SampleDistance(uv), threshold, st.scale);
}

inline float4 MSDF_PremulAdd(const float4 colorPMA)
{
	return (colorPMA + (g_colorAdd * colorPMA.a));
}

float4 PS_MSDFFont(PSInput input) : SV_TARGET
{
	const MSDFState st = MSDF_Init(input.uv);
	
	const float textAlpha = MSDF_AlphaAt(input.uv, MSDF_TextThreshold, st);
	
	const float4 textPMA = (input.colorPMA * textAlpha);
	return MSDF_PremulAdd(textPMA);
}

float4 PS_MSDFFont_Outline(PSInput input) : SV_TARGET
{
	const MSDFState st = MSDF_Init(input.uv);
	const float d = MSDF_SampleDistance(input.uv);
	
	const float outlineAlpha = MSDF_Coverage(d, g_sdfParam.y, st.scale);
	const float textAlpha = MSDF_Coverage(d, g_sdfParam.x, st.scale);

	const float blend = textAlpha;
	float4 colorPMA = lerp(g_sdfOutlineColorPMA, input.colorPMA, blend);
	colorPMA *= outlineAlpha;
	
	return MSDF_PremulAdd(colorPMA);
}

float4 PS_MSDFFont_Shadow(PSInput input) : SV_TARGET
{
	const MSDFState st = MSDF_Init(input.uv);

	const float textAlpha = MSDF_AlphaAt(input.uv, MSDF_TextThreshold, st);
	
	const float2 shadowOffset = (g_sdfParam.zw * st.invTextureSize);
	const float shadowAlpha = MSDF_AlphaAt((input.uv - shadowOffset), MSDF_TextThreshold, st);

	const float sBase = (shadowAlpha * (1.0 - textAlpha));
	const float4 textPMA = (input.colorPMA * textAlpha);
	const float4 shadowPMA = (g_sdfShadowColorPMA * sBase);

	const float4 finalPMA = (textPMA + shadowPMA);
	return MSDF_PremulAdd(finalPMA);
}

float4 PS_MSDFFont_OutlineShadow(PSInput input) : SV_TARGET
{
	const MSDFState st = MSDF_Init(input.uv);
	const float d = MSDF_SampleDistance(input.uv);

	const float outlineAlpha = MSDF_Coverage(d, g_sdfParam.y, st.scale);
	const float textAlpha = MSDF_Coverage(d, g_sdfParam.x, st.scale);

	const float2 shadowOffset = (g_sdfParam.zw * st.invTextureSize);
	const float shadowAlpha = MSDF_AlphaAt((input.uv - shadowOffset), g_sdfParam.y, st);

	const float4 textPMA = (input.colorPMA * textAlpha);

	const float outlineCoverage = saturate(outlineAlpha - textAlpha);
	const float4 outlinePMA = g_sdfOutlineColorPMA * outlineCoverage;

	const float shadowCoverage = saturate(shadowAlpha * (1.0 - outlineAlpha));
	const float4 shadowPMA = g_sdfShadowColorPMA * shadowCoverage;

	const float4 finalPMA = (textPMA + outlinePMA + shadowPMA);
	return MSDF_PremulAdd(finalPMA);
}

float4 PS_MSDFFont_Glow(PSInput input) : SV_TARGET
{
	const float d = saturate(g_texture0.Sample(g_sampler0, input.uv).a * 2.0);
	const float pd = pow(abs(d), g_sdfParam.x);

	const float4 finalPMA = float4((input.colorPMA.rgb * pd), (input.colorPMA.a * pd));
	return MSDF_PremulAdd(finalPMA);
}

float4 PS_MSDFFont_Print(PSInput input) : SV_TARGET
{
	const MSDFState st = MSDF_Init(input.uv);
	const float d = MSDF_SampleDistance(input.uv);

	const float outlineDistance = 0.04;
	const float outlineThreshold = (MSDF_TextThreshold - outlineDistance);

	const float textAlpha = sqrt(saturate((d - 0.5) * st.scale + 0.5));
	const float outlineAlpha = sqrt(saturate((d - outlineThreshold) * st.scale + 0.5));

	const float2 shadowOffset = (float2(0.625, 0.625) * st.invTextureSize);
	const float d2 = MSDF_SampleDistance(input.uv - shadowOffset);
	const float shadowAlpha = sqrt(saturate((d2 - outlineThreshold) * st.scale + 0.5));

	float3 color = lerp(float3(0.0, 0.0, 0.0), float3(1.0, 1.0, 1.0), textAlpha);
	const float hollowShadowAlpha = saturate(shadowAlpha * (1.0 - outlineAlpha));
	color = lerp(color, float3(0.0, 0.0, 0.0), hollowShadowAlpha);

	const float finalAlpha = saturate(outlineAlpha + hollowShadowAlpha);
	return float4(color, finalAlpha);
}
