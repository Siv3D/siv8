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
	float4 colorPMA;
	float2 uv;
};

struct VSConstants2D
{
	float2x4 g_transform;
	float4 g_colorMul;
};

struct PSConstants2D
{
	float4 g_patternBackgroundColorMul;
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColorPMA;
	float4 g_sdfShadowColorPMA;
};

struct PSEffectConstants2D
{
	float2x4 g_patternUVTransform;
	float4 g_patternBackgroundColor;
	float3x3 g_quadWarpInvHomography;
	float4 g_quadWarpUVTransform;
};

inline float4 s3d_positionTransform(float2 pos, float2x4 t)
{
	const float2 t_13_14 = float2(t[0][2], t[0][3]);
	const float2 t_11_12 = float2(t[0][0], t[0][1]);
	const float2 t_21_22 = float2(t[1][0], t[1][1]);
	return float4((t_13_14 + (pos.x * t_11_12) + (pos.y * t_21_22)), 0.0f, 1.0f);
}

inline float4 s3d_premultiplyAlpha(float4 color)
{
	return float4((color.rgb * color.a), color.a);
}

inline float4 s3d_shapeColor(float4 vertexColor, constant PSConstants2D* c)
{
	return (vertexColor + (c->g_colorAdd * vertexColor.a));
}

inline float4 s3d_textureColor(float4 vertexColor, float4 textureColor, constant PSConstants2D* c)
{
	vertexColor *= textureColor;
	return (vertexColor + (c->g_colorAdd * vertexColor.a));
}

////////////////////////////////////////////////////////////////
//
//	Basic VS / PS
//
////////////////////////////////////////////////////////////////

vertex
PSInput VS_Shape(	uint vertexID [[vertex_id]],
					constant VSInput* vertices,
					constant VSConstants2D* c0)
{
	PSInput result;
	result.position	= s3d_positionTransform(vertices[vertexID].position, c0->g_transform);
	result.colorPMA	= s3d_premultiplyAlpha(vertices[vertexID].color * c0->g_colorMul);
	result.uv		= vertices[vertexID].uv;
	return result;
}

vertex
PSInput VS_QuadWarp(	uint vertexID [[vertex_id]],
						constant VSInput* vertices,
						constant VSConstants2D* c0)
{
	PSInput result;
	result.position	= s3d_positionTransform(vertices[vertexID].position, c0->g_transform);
	result.colorPMA	= s3d_premultiplyAlpha(vertices[vertexID].color * c0->g_colorMul);
	result.uv		= vertices[vertexID].position;
	return result;
}

fragment
float4 PS_Shape(	PSInput input [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]])
{
	return s3d_shapeColor(input.colorPMA, c0);
}

fragment
float4 PS_Texture(	PSInput input [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]],
					texture2d<float> texture0 [[texture(0)]],
					sampler sampler0 [[sampler(0)]])
{
	return s3d_textureColor(input.colorPMA, texture0.sample(sampler0, input.uv), c0);
}

fragment
float4 PS_QuadWarp(	PSInput input [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]],
					constant PSEffectConstants2D* c1 [[buffer(1)]],
					texture2d<float> texture0 [[texture(0)]],
					sampler sampler0 [[sampler(0)]])
{
	const float3 t = (c1->g_quadWarpInvHomography * float3(input.uv, 1.0f));
	const float2 uv = ((t.xy / t.z) * c1->g_quadWarpUVTransform.xy + c1->g_quadWarpUVTransform.zw);
	return s3d_textureColor(input.colorPMA, texture0.sample(sampler0, uv), c0);
}

////////////////////////////////////////////////////////////////
//
//	Line Patterns
//
////////////////////////////////////////////////////////////////

fragment
float4 PS_LineDot(	PSInput input [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]])
{
	float4 result = input.colorPMA;

	const float u = (0.5f * (input.uv.x - 0.5f));
	const float w = fwidth(u);
	const float distance = abs(2.0f * fract(u) - 1.0f);
	const float alpha = smoothstep((0.5f - w), (0.5f + w), distance);
	result *= alpha;

	return s3d_shapeColor(result, c0);
}

fragment
float4 PS_LineDash(	PSInput input [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]])
{
	float4 result = input.colorPMA;

	const float u = (0.25f * (input.uv.x - 1.0f));
	const float w = fwidth(u);
	const float distance = abs(2.0f * fract(u) - 1.0f);
	const float alpha = smoothstep((0.4f - w), (0.4f + w), distance);
	result *= alpha;

	return s3d_shapeColor(result, c0);
}

fragment
float4 PS_LineLongDash(	PSInput input [[stage_in]],
						constant PSConstants2D* c0 [[buffer(0)]])
{
	float4 result = input.colorPMA;

	const float u = (0.1f * (input.uv.x - 1.0f));
	const float w = fwidth(u);
	const float distance = abs(2.0f * fract(u) - 1.0f);
	const float alpha = smoothstep((0.3f - w), (0.3f + w), distance);
	result *= alpha;

	return s3d_shapeColor(result, c0);
}

fragment
float4 PS_LineDashDot(	PSInput input [[stage_in]],
						constant PSConstants2D* c0 [[buffer(0)]])
{
	float4 result = input.colorPMA;

	const float u = (0.1f * (input.uv.x - 1.0f));
	const float u2 = u + 0.5f;
	const float w = fwidth(u);
	const float distance = abs(2.0f * fract(u) - 1.0f);
	const float distance2 = abs(2.0f * fract(u2) - 1.0f);
	const float alpha1 = smoothstep((0.4f - w), (0.4f + w), distance);
	const float alpha2 = smoothstep((0.9f - w), (0.9f + w), distance2);
	result *= max(alpha1, alpha2);

	return s3d_shapeColor(result, c0);
}

fragment
float4 PS_LineRoundDot(	PSInput input [[stage_in]],
						constant PSConstants2D* c0 [[buffer(0)]])
{
	float4 result = input.colorPMA;

	const float2 uv = ((input.uv + float2(0.5f, 0.0f)) * float2(0.5f, 1.0f));
	const float w = fwidth(uv.y);
	const float distance = length(float2(4.0f, 2.0f) * fract(uv) - float2(2.0f, 1.0f));
	const float alpha = (1.0f - smoothstep((1.0f - w), (1.0f + w), distance));
	result *= alpha;

	return s3d_shapeColor(result, c0);
}

////////////////////////////////////////////////////////////////
//
//	Patterns
//
////////////////////////////////////////////////////////////////
	
inline float4 Pattern_BackgroundColor(float4 backgroundColor, constant PSConstants2D* c)
{
	const float4 color = s3d_premultiplyAlpha(backgroundColor * c->g_patternBackgroundColorMul);
	return (color + (c->g_colorAdd * color.a));
}

inline float2 Pattern_UVTransform(float2 uv, float2x4 t)
{
	const float2 t_13_14 = float2(t[0][2], t[0][3]);
	const float2 t_11_12 = float2(t[0][0], t[0][1]);
	const float2 t_21_22 = float2(t[1][0], t[1][1]);
	return (t_13_14 + (uv.x * t_11_12) + (uv.y * t_21_22));
}

inline float2 Pattern_Integral(float2 v)
{
	v /= 2.0f;
	return (floor(v) + max((2.0f * fract(v) - 1.0f), 0.0f));
}

inline float Pattern_CheckersFiltered(float2 p, float2 hv)
{
	const float2 fw = fwidth(p);
	const float w = max(fw.x, fw.y);
	float2 i = (Pattern_Integral(p + 0.5f * w) - Pattern_Integral(p - 0.5f * w));
	i *= hv;
	i /= w;
	return (i.x + i.y - 2.0f * i.x * i.y);
}

inline float2 Pattern_Skew(float2 v)
{
	const float2x2 transform = float2x2(1.0f, (1.0f / tan(3.1415926535f / 3.0f)), 0.0f, (1.0f / sin(3.1415926535f / 3.0f)));
	return (v * transform);
}

inline float Pattern_Hex(float2 p)
{
	const float2 HEX = float2(1.0f, 1.73205081f);
	const float4 t = (floor(float4(p, p - float2(0.5f, 1.0f)) / HEX.xyxy) + float4(0.5f, 0.5f, 0.5f, 0.5f));
	const float4 h = float4((p - t.xy * HEX), (p - (t.zw + float2(0.5f, 0.5f)) * HEX));
	const float2 hex = abs((dot(h.xy, h.xy) < dot(h.zw, h.zw)) ? h.xy : h.zw);
	return max(dot(hex, (HEX * 0.5f)), hex.x);
}

fragment
float4 PS_PatternPolkaDot(	PSInput input [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = Pattern_UVTransform(input.position.xy, c1->g_patternUVTransform);
	const float2 repeat = (2.0f * fract(uv) - 1.0f);
	const float value = length(repeat);
	const float fw = (length(float2(dfdx(value), dfdy(value))) * 0.70710678118);
	
	const float radiusScale = c1->g_patternUVTransform[1].z;
	const float c_val = smoothstep((radiusScale - fw), (radiusScale + fw), value);

	const float4 primary = s3d_shapeColor(input.colorPMA, c0);
	const float4 background = Pattern_BackgroundColor(c1->g_patternBackgroundColor, c0);
	
	return mix(primary, background, c_val);
}

fragment
float4 PS_PatternStripe(	PSInput input [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float u = Pattern_UVTransform(input.position.xy, c1->g_patternUVTransform).x;
	const float fw = fwidth(u);
	const float repeat = (2.0f * fract(u) - 1.0f);
	const float value = abs(repeat);
	
	const float thicknessScale = (c1->g_patternUVTransform[1].z * (1.0f + 2.0f * fw) - fw);
	const float t = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);

	const float4 primary = s3d_shapeColor(input.colorPMA, c0);
	const float4 background = Pattern_BackgroundColor(c1->g_patternBackgroundColor, c0);

	return mix(primary, background, t);
}

fragment
float4 PS_PatternGrid(	PSInput input [[stage_in]],
						constant PSConstants2D* c0 [[buffer(0)]],
						constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = Pattern_UVTransform(input.position.xy, c1->g_patternUVTransform);
	const float2 fw = fwidth(uv);
	const float2 repeat = (2.0f * fract(uv) - 1.0f);
	const float2 value = abs(repeat);
	
	const float2 thicknessScale = (c1->g_patternUVTransform[1].zz * float2(1.0f + fw) - fw);
	const float2 t1 = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);
	const float t2 = min(t1.x, t1.y);

	const float4 primary = s3d_shapeColor(input.colorPMA, c0);
	const float4 background = Pattern_BackgroundColor(c1->g_patternBackgroundColor, c0);

	return mix(primary, background, t2);
}

fragment
float4 PS_PatternChecker(	PSInput input [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = Pattern_UVTransform(input.position.xy, c1->g_patternUVTransform);
	const float t = Pattern_CheckersFiltered(uv, c1->g_patternUVTransform[1].zw);

	const float4 primary = s3d_shapeColor(input.colorPMA, c0);
	const float4 background = Pattern_BackgroundColor(c1->g_patternBackgroundColor, c0);
	
	return mix(primary, background, t);
}

fragment
float4 PS_PatternTriangle(	PSInput input [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = Pattern_UVTransform(input.position.xy, c1->g_patternUVTransform);
	const float2 fw = (fwidth(uv) * 0.25f);

	const float2 s1 = Pattern_Skew(uv + float2(-fw.x, -fw.y));
	const float2 s2 = Pattern_Skew(uv + float2(fw.x, fw.y));
	const float2 s3 = Pattern_Skew(uv + float2(-fw.x, fw.y));
	const float2 s4 = Pattern_Skew(uv + float2(fw.x, -fw.y));

	const float4 f1 = fract(float4(s1, s2));
	const float4 f2 = fract(float4(s3, s4));
	const float4 ss = float4(step(f1.x, f1.y), step(f1.z, f1.w), step(f2.x, f2.y), step(f2.z, f2.w));
	const float t = dot(ss, 0.25f);

	const float4 primary = s3d_shapeColor(input.colorPMA, c0);
	const float4 background = Pattern_BackgroundColor(c1->g_patternBackgroundColor, c0);
	
	return mix(primary, background, t);
}

fragment
float4 PS_PatternHexGrid(	PSInput input [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = Pattern_UVTransform(input.position.xy, c1->g_patternUVTransform);
	const float2 fw = fwidth(uv);
	const float w = (max(fw.x, fw.y) * 0.5f);

	const float thicknessScale = (c1->g_patternUVTransform[1].z * (1 + 2 * w));
	const float h = Pattern_Hex(uv);
	const float t = smoothstep((thicknessScale - w), (thicknessScale + w), h);

	const float4 primary = s3d_shapeColor(input.colorPMA, c0);
	const float4 background = Pattern_BackgroundColor(c1->g_patternBackgroundColor, c0);
	
	return mix(background, primary, t);
}

////////////////////////////////////////////////////////////////
//
//	MSDF Font
//
////////////////////////////////////////////////////////////////

constant float MSDF_PixelRange = 16.0f;
constant float MSDF_TextThreshold = 0.5f;

struct MSDFState
{
	float2 textureSize;
	float2 invTextureSize;
	float scale;
};

inline MSDFState MSDF_Init(texture2d<float> texture, float2 uv)
{
	MSDFState st;
	st.textureSize = float2(texture.get_width(), texture.get_height());
	st.invTextureSize = (1.0f / (st.textureSize));
	
	const float2 msdfUnit = (MSDF_PixelRange * st.invTextureSize);
	const float2 screenPixelRange = (0.5f / fwidth(uv));
	st.scale = dot(msdfUnit, screenPixelRange);

	return st;
}

inline float MSDF_Median(const float3 rgb)
{
	return median3(rgb.r, rgb.g, rgb.b);
}

inline float MSDF_SampleDistance(const texture2d<float> texture, const sampler sampler, const float2 uv)
{
	return MSDF_Median(texture.sample(sampler, uv).rgb);
}

inline float MSDF_Coverage(const float d, const float threshold, const float scale)
{
	return saturate((d - threshold) * scale + 0.5);
}

inline float MSDF_AlphaAt(const texture2d<float> texture, const sampler sampler, const float2 uv, const float threshold, const MSDFState st)
{
	return MSDF_Coverage(MSDF_SampleDistance(texture, sampler, uv), threshold, st.scale);
}

inline float4 MSDF_PremulAdd(const float4 colorPMA, const float4 g_colorAdd)
{
	return (colorPMA + (g_colorAdd * colorPMA.a));
}

fragment
float4 PS_MSDFFont( PSInput input [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]],
					texture2d<float> texture0 [[texture(0)]],
					sampler sampler0 [[sampler(0)]])
{
	const MSDFState st = MSDF_Init(texture0, input.uv);
	
	const float textAlpha = MSDF_AlphaAt(texture0, sampler0, input.uv, MSDF_TextThreshold, st);
	
	const float4 textPMA = (input.colorPMA * textAlpha);
	return MSDF_PremulAdd(textPMA, c0->g_colorAdd);
}

fragment
float4 PS_MSDFFont_Outline( PSInput input [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							texture2d<float> texture0 [[texture(0)]],
							sampler sampler0 [[sampler(0)]])
{
	const MSDFState st = MSDF_Init(texture0, input.uv);
	const float d = MSDF_SampleDistance(texture0, sampler0, input.uv);
	
	const float outlineAlpha = MSDF_Coverage(d, c0->g_sdfParam.y, st.scale);
	const float textAlpha = MSDF_Coverage(d, c0->g_sdfParam.x, st.scale);

	const float blend = textAlpha;
	float4 colorPMA = mix(c0->g_sdfOutlineColorPMA, input.colorPMA, blend);
	colorPMA *= outlineAlpha;
	
	return MSDF_PremulAdd(colorPMA, c0->g_colorAdd);
}

fragment
float4 PS_MSDFFont_Shadow(	PSInput input [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							texture2d<float> texture0 [[texture(0)]],
							sampler sampler0 [[sampler(0)]])
{
	const MSDFState st = MSDF_Init(texture0, input.uv);

	const float textAlpha = MSDF_AlphaAt(texture0, sampler0, input.uv, MSDF_TextThreshold, st);

	const float2 shadowOffset = (c0->g_sdfParam.zw * st.invTextureSize);
	const float shadowAlpha = MSDF_AlphaAt(texture0, sampler0, (input.uv - shadowOffset), MSDF_TextThreshold, st);

	const float sBase = (shadowAlpha * (1.0 - textAlpha));
	const float4 textPMA = (input.colorPMA * textAlpha);
	const float4 shadowPMA = (c0->g_sdfShadowColorPMA * sBase);

	const float4 finalPMA = (textPMA + shadowPMA);
	return MSDF_PremulAdd(finalPMA, c0->g_colorAdd);
}

fragment
float4 PS_MSDFFont_OutlineShadow(	PSInput input [[stage_in]],
									constant PSConstants2D* c0 [[buffer(0)]],
									texture2d<float> texture0 [[texture(0)]],
									sampler sampler0 [[sampler(0)]])
{
	const MSDFState st = MSDF_Init(texture0, input.uv);
	const float d = MSDF_SampleDistance(texture0, sampler0, input.uv);

	const float outlineAlpha = MSDF_Coverage(d, c0->g_sdfParam.y, st.scale);
	const float textAlpha = MSDF_Coverage(d, c0->g_sdfParam.x, st.scale);

	const float2 shadowOffset = (c0->g_sdfParam.zw * st.invTextureSize);
	const float shadowAlpha = MSDF_AlphaAt(texture0, sampler0, (input.uv - shadowOffset), c0->g_sdfParam.y, st);

	const float4 textPMA = (input.colorPMA * textAlpha);

	const float outlineCoverage = saturate(outlineAlpha - textAlpha);
	const float4 outlinePMA = (c0->g_sdfOutlineColorPMA * outlineCoverage);

	const float shadowCoverage = saturate(shadowAlpha * (1.0 - outlineAlpha));
	const float4 shadowPMA = (c0->g_sdfShadowColorPMA * shadowCoverage);

	const float4 finalPMA = (textPMA + outlinePMA + shadowPMA);
	return MSDF_PremulAdd(finalPMA, c0->g_colorAdd);
}

fragment
float4 PS_MSDFFont_Glow( PSInput in [[stage_in]],
						 constant PSConstants2D* c0 [[buffer(0)]],
						 texture2d<float> texture0 [[texture(0)]],
						 sampler sampler0 [[sampler(0)]])
{
	const float d = saturate(texture0.sample(sampler0, in.uv).a * 2.0);
	const float pd = pow(d, c0->g_sdfParam.x);

	const float4 finalPMA = float4((in.colorPMA.rgb * pd), (in.colorPMA.a * pd));
	return MSDF_PremulAdd(finalPMA, c0->g_colorAdd);
}

fragment
float4 PS_MSDFFont_Print(	PSInput input [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							texture2d<float> texture0 [[texture(0)]],
							sampler sampler0 [[sampler(0)]])
{
	const MSDFState st = MSDF_Init(texture0, input.uv);
	const float d = MSDF_SampleDistance(texture0, sampler0, input.uv);

	const float outlineDistance = 0.04;
	const float outlineThreshold = (MSDF_TextThreshold - outlineDistance);

	const float textAlpha = sqrt(saturate((d - 0.5) * st.scale + 0.5));
	const float outlineAlpha = sqrt(saturate((d - outlineThreshold) * st.scale + 0.5));

	const float2 shadowOffset = (float2(0.625, 0.625) * st.invTextureSize);
	const float d2 = MSDF_SampleDistance(texture0, sampler0, (input.uv - shadowOffset));
	const float shadowAlpha = sqrt(saturate((d2 - outlineThreshold) * st.scale + 0.5));

	float3 color = mix(float3(0.0, 0.0, 0.0), float3(1.0, 1.0, 1.0), textAlpha);
	const float hollowShadowAlpha = saturate(shadowAlpha * (1.0 - outlineAlpha));
	color = mix(color, float3(0.0, 0.0, 0.0), hollowShadowAlpha);

	const float finalAlpha = saturate(outlineAlpha + hollowShadowAlpha);
	return float4(color, finalAlpha);
}
