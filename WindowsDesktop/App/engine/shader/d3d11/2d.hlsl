//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
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
	row_major float3x4 g_transform;
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
	// [0] = (m11, m12, m31, m32); [1] = (m21, m22, param0, param1).
	row_major float2x4 g_patternUVTransform;
	float4 g_patternBackgroundColor;
	// Type-specific payload, decoded by the selected pattern shader.
	float4 g_patternExtraParams;
}

inline float2 s3d_transformPoint2D(const float2 position, const float2x4 transform)
{
	const float2 translation = transform._13_14;
	const float2 basisX = transform._11_12;
	const float2 basisY = transform._21_22;
	return (translation + (position.x * basisX) + (position.y * basisY));
}

inline float4 s3d_positionTransform(const float2 position, const float3x4 transform)
{
	const float4 clip = mul(float3(position, 1.0f), transform);
	return float4(clip.xy, 0.0f, clip.w);
}

inline float4 s3d_premultiplyAlpha(const float4 color)
{
	return float4((color.rgb * color.a), color.a);
}

inline float4 s3d_applyColorAdd(const float4 colorPMA, const float4 colorAdd)
{
	return (colorPMA + (colorAdd * colorPMA.a));
}

inline float4 s3d_textureColor(float4 vertexColorPMA, const float4 textureColorPMA)
{
	vertexColorPMA *= textureColorPMA;
	return s3d_applyColorAdd(vertexColorPMA, g_colorAdd);
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

// Pass drawing coordinates for perspective-correct pattern interpolation.
PSInput VS_Pattern(VSInput input)
{
	PSInput result;
	result.position	= s3d_positionTransform(input.position, g_transform);
	result.colorPMA = s3d_premultiplyAlpha(input.color * g_colorMul);
	result.uv		= input.position;
	return result;
}

float4 PS_Shape(PSInput input) : SV_TARGET
{
	return s3d_applyColorAdd(input.colorPMA, g_colorAdd);
}

float4 PS_Texture(PSInput input) : SV_TARGET
{
	return s3d_textureColor(input.colorPMA, g_texture0.Sample(g_sampler0, input.uv));
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

	return s3d_applyColorAdd(result, g_colorAdd);
}

float4 PS_LineDash(PSInput input) : SV_TARGET
{
	float4 result = input.colorPMA;

	const float u = (0.25 * (input.uv.x - 1.0));
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.4 - w), (0.4 + w), distance);
	result *= alpha;

	return s3d_applyColorAdd(result, g_colorAdd);
}

float4 PS_LineLongDash(PSInput input) : SV_TARGET
{
	float4 result = input.colorPMA;

	const float u = (0.1 * (input.uv.x - 1.0));
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.3 - w), (0.3 + w), distance);
	result *= alpha;

	return s3d_applyColorAdd(result, g_colorAdd);
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

	return s3d_applyColorAdd(result, g_colorAdd);
}

float4 PS_LineRoundDot(PSInput input) : SV_TARGET
{
	float4 result = input.colorPMA;

	const float2 uv = ((input.uv + float2(0.5, 0.0)) * float2(0.5, 1));
	const float w = fwidth(uv.y);
	const float distance = length(float2(4.0, 2.0) * frac(uv) - float2(2.0, 1.0));
	const float alpha = (1.0 - smoothstep((1.0 - w), (1.0 + w), distance));
	result *= alpha;

	return s3d_applyColorAdd(result, g_colorAdd);
}

////////////////////////////////////////////////////////////////
//
//	Patterns
//
////////////////////////////////////////////////////////////////
	
inline float4 Pattern_BackgroundColorPMA()
{
	return s3d_premultiplyAlpha(g_patternBackgroundColor * g_patternBackgroundColorMul);
}

inline float2 Pattern_UVTransform(const float2 drawingPosition)
{
	return s3d_transformPoint2D(drawingPosition, g_patternUVTransform);
}

inline float2 Pattern_Integral(float2 v)
{
	v /= 2.0;
	return (floor(v) + max((2.0 * frac(v) - 1.0), 0.0));
}

inline float Pattern_CheckersFiltered(const float2 p, const float2 axisIntensity)
{
	const float2 fw = fwidth(p);
	const float w = max(fw.x, fw.y);
	float2 i = (Pattern_Integral(p + 0.5 * w) - Pattern_Integral(p - 0.5 * w));
	i *= axisIntensity;
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
	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float2 repeat = (2.0 * frac(patternUV) - 1.0);
	const float value = length(repeat);
	const float fw = (length(float2(ddx(value), ddy(value))) * 0.70710678118);

	// Radius in centered cell coordinates spanning [-1, 1].
	const float normalizedRadius = g_patternUVTransform[1].z;
	const float c = smoothstep((normalizedRadius - fw), (normalizedRadius + fw), value);

	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();

	return s3d_applyColorAdd(lerp(primary, background, c), g_colorAdd);
}

float4 PS_PatternHalftone(PSInput input) : SV_TARGET
{
	const float normalizedMinRadius = g_patternUVTransform[1].z; // 2 * minRadius / pitch
	const float normalizedMaxRadius = g_patternUVTransform[1].w; // 2 * maxRadius / pitch
	// Linear radius field expressed in pattern UV coordinates.
	const float2 radiusFieldGradient = g_patternExtraParams.xy;
	const float radiusFieldBias = g_patternExtraParams.z;

	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float2 cellCenter = (floor(patternUV) + 0.5f);
	const float2 repeat = (2.0f * (patternUV - cellCenter));
	const float radiusBlend = saturate(dot(cellCenter, radiusFieldGradient) + radiusFieldBias);
	const float normalizedRadius = lerp(normalizedMinRadius, normalizedMaxRadius,
		(radiusBlend * radiusBlend * (3.0f - 2.0f * radiusBlend)));
	// Differentiate continuous UVs, not the radius that changes between cells.
	const float fw = length(fwidth(patternUV));
	const float coverage = ((1.0f - smoothstep(normalizedRadius - fw, normalizedRadius + fw, length(repeat)))
		* saturate(normalizedRadius / fw));
	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();
	return s3d_applyColorAdd(lerp(background, primary, coverage), g_colorAdd);
}

float4 PS_PatternWave(PSInput input) : SV_TARGET
{
	const float normalizedThickness = g_patternUVTransform[1].z; // thickness / pitch
	const float normalizedAmplitude = g_patternUVTransform[1].w; // amplitude / pitch
	const float slopeAmplitude = g_patternExtraParams.x; // 2 * pi * amplitude / wavelength

	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float phase = (6.28318530718f * patternUV.x);
	const float waveCoord = (patternUV.y - normalizedAmplitude * sin(phase) + 0.5f);
	const float slope = (slopeAmplitude * cos(phase));
	// First-order normal-width correction, not an exact distance to the sine curve.
	const float width = saturate(normalizedThickness * sqrt(1.0f + slope * slope));
	// Differentiate before wrapping; zero amplitude uses the Stripe filter.
	const float fw = fwidth(waveCoord);
	const float value = abs(2.0f * frac(waveCoord) - 1.0f);
	const float thickness = (width * (1.0f + 2.0f * fw) - fw);
	const float t = smoothstep(thickness - fw, thickness + fw, value);
	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();
	return s3d_applyColorAdd(lerp(primary, background, t), g_colorAdd);
}

float4 PS_PatternRipple(PSInput input) : SV_TARGET
{
	const float normalizedThickness = g_patternUVTransform[1].z; // thickness / pitch
	const float normalizedRadiusOffset = g_patternUVTransform[1].w; // radiusOffset / pitch

	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float ringCoord = (length(patternUV) - normalizedRadiusOffset + 0.5f);
	// Continuous UV derivatives remain defined at the radial center, including
	// a 2x2 fragment quad whose four samples have equal distance to the center.
	const float fw = length(fwidth(patternUV));
	const float value = abs(2.0f * frac(ringCoord) - 1.0f);
	const float thickness = (normalizedThickness * (1.0f + 2.0f * fw) - fw);
	const float t = smoothstep(thickness - fw, thickness + fw, value);
	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();
	return s3d_applyColorAdd(lerp(primary, background, t), g_colorAdd);
}

float4 PS_PatternWeave(PSInput input) : SV_TARGET
{
	const float normalizedBandWidth = g_patternUVTransform[1].z; // thickness / pitch
	const float normalizedClearanceWidth = g_patternUVTransform[1].w; // (thickness + 2 * gap) / pitch

	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float2 fw = fwidth(patternUV);
	const float2 value = abs(2.0f * frac(patternUV + 0.5f) - 1.0f);
	const float2 width = (normalizedBandWidth * (1.0f + 2.0f * fw) - fw);
	const float2 clearance = (normalizedClearanceWidth * (1.0f + 2.0f * fw) - fw);
	const float2 band = (1.0f - smoothstep(width - fw, width + fw, value));
	const float2 expanded = (1.0f - smoothstep(clearance - fw, clearance + fw, value));
	// Filter the crossing parity too: at maximum gap, cuts reach cell boundaries.
	const float horizontalOver = Pattern_CheckersFiltered(patternUV + 0.5f, float2(1.0f, 1.0f));
	const float verticalCut = ((expanded.x - band.x) * band.y);
	const float horizontalCut = ((expanded.y - band.y) * band.x);
	const float coverage = (max(band.x, band.y) - lerp(verticalCut, horizontalCut, horizontalOver));
	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();
	return s3d_applyColorAdd(lerp(background, primary, coverage), g_colorAdd);
}

// Matches Pattern::Truchet::Layout.
static const uint Pattern_TruchetLayoutRandom = 0u;
static const uint Pattern_TruchetLayoutAlternating = 2u;

inline uint Pattern_DecodeTruchetSeed(float2 seedHalves)
{
	// The CPU stores finite numeric 16-bit halves, not bit-cast float payloads.
	const uint seedLow = uint(seedHalves.x);
	const uint seedHigh = uint(seedHalves.y);
	return (seedLow | (seedHigh << 16));
}

inline uint Pattern_TruchetHash(float2 cell, uint seed)
{
	// Hash exact IEEE-754 bits of half-integer cell centers. This avoids signed
	// float-to-int conversion limits and gives +0/-0 the same key at the origin.
	const uint2 key = asuint(cell + 0.5f);
	uint h = ((key.x * 0x9E3779B9u) ^ (key.y * 0x85EBCA6Bu) ^ seed);
	h ^= (h >> 16);
	h *= 0x7FEB352Du;
	h ^= (h >> 15);
	h *= 0x846CA68Bu;
	h ^= (h >> 16);
	return h;
}

float4 PS_PatternTruchet(PSInput input) : SV_TARGET
{
	const float normalizedThickness = g_patternUVTransform[1].z; // thickness / pitch

	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float2 cell = floor(patternUV);
	float2 tilePosition = (patternUV - cell);
	const uint layout = uint(g_patternExtraParams.z);
	bool flip = false; // Uniform (1) keeps the original orientation.
	if (layout == Pattern_TruchetLayoutRandom)
	{
		const uint seed = Pattern_DecodeTruchetSeed(g_patternExtraParams.xy);
		flip = ((Pattern_TruchetHash(cell, seed) & 1u) != 0u);
	}
	else if (layout == Pattern_TruchetLayoutAlternating)
	{
		flip = (frac(dot(cell, float2(0.5f, 0.5f))) > 0.25f);
	}
	tilePosition.x = (flip ? (1.0f - tilePosition.x) : tilePosition.x);
	// These equal-radius circles are disjoint, so the nearer center gives the nearer arc.
	const float2 oppositeCornerOffset = (tilePosition - 1.0f);
	const float distance = abs(sqrt(min(dot(tilePosition, tilePosition),
		dot(oppositeCornerOffset, oppositeCornerOffset))) - 0.5f);
	// Differentiate the continuous coordinates, not tile-dependent arc distances.
	const float fw = length(fwidth(patternUV));
	const float width = (normalizedThickness * (1.0f + 2.0f * fw) - fw);
	const float coverage = (1.0f - smoothstep(width - fw, width + fw, 2.0f * distance));
	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();
	return s3d_applyColorAdd(lerp(background, primary, coverage), g_colorAdd);
}

float4 PS_PatternStripe(PSInput input) : SV_TARGET
{
	const float normalizedThickness = g_patternUVTransform[1].z; // thicknessScale / 2

	const float2 drawingPosition = input.uv;
	const float stripeCoord = Pattern_UVTransform(drawingPosition).x;
	const float fw = fwidth(stripeCoord);
	const float repeat = (2.0 * frac(stripeCoord) - 1.0);
	const float value = abs(repeat);

	const float thicknessScale = (normalizedThickness * (1 + 2 * fw) - fw);
	const float c = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);

	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();

	return s3d_applyColorAdd(lerp(primary, background, c), g_colorAdd);
}

float4 PS_PatternGrid(PSInput input) : SV_TARGET
{
	const float2 normalizedThickness = g_patternUVTransform[1].zz; // thicknessScale / 2 on both axes

	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float2 fw = fwidth(patternUV);
	const float2 repeat = (2.0 * frac(patternUV) - 1.0);
	const float2 value = abs(repeat);

	const float2 thicknessScale = (normalizedThickness * float2(1 + fw) - fw);
	const float2 c = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);
	const float c2 = min(c.x, c.y);

	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();

	return s3d_applyColorAdd(lerp(primary, background, c2), g_colorAdd);
}

float4 PS_PatternChecker(PSInput input) : SV_TARGET
{
	const float2 axisIntensity = g_patternUVTransform[1].zw; // (verticalIntensity, horizontalIntensity)

	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float c = Pattern_CheckersFiltered(patternUV, axisIntensity);

	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();

	return s3d_applyColorAdd(lerp(primary, background, c), g_colorAdd);
}

float4 PS_PatternTriangle(PSInput input) : SV_TARGET
{
	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float2 fw = (fwidth(patternUV) * 0.25);

	const float2 s1 = Pattern_Skew(patternUV + float2(-fw.x, -fw.y));
	const float2 s2 = Pattern_Skew(patternUV + float2(fw.x, fw.y));
	const float2 s3 = Pattern_Skew(patternUV + float2(-fw.x, fw.y));
	const float2 s4 = Pattern_Skew(patternUV + float2(fw.x, -fw.y));

	const float4 f1 = frac(float4(s1, s2));
	const float4 f2 = frac(float4(s3, s4));
	const float4 ss = float4(step(f1.x, f1.y), step(f1.z, f1.w), step(f2.x, f2.y), step(f2.z, f2.w));
	const float c = dot(ss, 0.25);

	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();

	return s3d_applyColorAdd(lerp(primary, background, c), g_colorAdd);
}

float4 PS_PatternHexGrid(PSInput input) : SV_TARGET
{
	const float cellEdgeThreshold = g_patternUVTransform[1].z; // 0.5 - thicknessScale * 0.25

	const float2 drawingPosition = input.uv;
	const float2 patternUV = Pattern_UVTransform(drawingPosition);
	const float2 fw = fwidth(patternUV);
	const float w = (max(fw.x, fw.y) * 0.5);

	const float filteredEdgeThreshold = (cellEdgeThreshold * (1 + 2 * w));
	const float h = Pattern_Hex(patternUV);
	const float c = smoothstep((filteredEdgeThreshold - w), (filteredEdgeThreshold + w), h);

	const float4 primary = input.colorPMA;
	const float4 background = Pattern_BackgroundColorPMA();

	return s3d_applyColorAdd(lerp(background, primary, c), g_colorAdd);
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
	float2 invTextureSize;
	float coverageScale;
};

inline MSDFState MSDF_Init(const float2 uv)
{
	float2 textureSize;
	g_texture0.GetDimensions(textureSize.x, textureSize.y);

	MSDFState state;
	state.invTextureSize = rcp(textureSize);

	// Combine atlas dimensions and UV derivatives before taking the reciprocal.
	const float2 screenPixelRange = ((0.5f * MSDF_PixelRange) / (textureSize * fwidth(uv)));
	state.coverageScale = (screenPixelRange.x + screenPixelRange.y);

	return state;
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

inline float MSDF_Coverage(const float sampledDistance, const float threshold, const float coverageScale)
{
	return saturate((sampledDistance - threshold) * coverageScale + 0.5);
}

inline float MSDF_AlphaAt(const float2 uv, const float threshold, const MSDFState state)
{
	return MSDF_Coverage(MSDF_SampleDistance(uv), threshold, state.coverageScale);
}

float4 PS_MSDFFont(PSInput input) : SV_TARGET
{
	const MSDFState state = MSDF_Init(input.uv);
	
	const float textAlpha = MSDF_AlphaAt(input.uv, MSDF_TextThreshold, state);
	
	const float4 textPMA = (input.colorPMA * textAlpha);
	return s3d_applyColorAdd(textPMA, g_colorAdd);
}

float4 PS_MSDFFont_Outline(PSInput input) : SV_TARGET
{
	const float textThreshold = g_sdfParam.x;
	const float outlineThreshold = g_sdfParam.y;

	const MSDFState state = MSDF_Init(input.uv);
	const float sampledDistance = MSDF_SampleDistance(input.uv);
	
	const float outlineAlpha = MSDF_Coverage(sampledDistance, outlineThreshold, state.coverageScale);
	const float textAlpha = MSDF_Coverage(sampledDistance, textThreshold, state.coverageScale);

	float4 colorPMA = lerp(g_sdfOutlineColorPMA, input.colorPMA, textAlpha);
	colorPMA *= outlineAlpha;
	
	return s3d_applyColorAdd(colorPMA, g_colorAdd);
}

float4 PS_MSDFFont_Shadow(PSInput input) : SV_TARGET
{
	const float2 shadowOffsetTexels = g_sdfParam.zw;

	const MSDFState state = MSDF_Init(input.uv);

	const float textAlpha = MSDF_AlphaAt(input.uv, MSDF_TextThreshold, state);
	
	const float2 shadowOffsetUV = (shadowOffsetTexels * state.invTextureSize);
	const float shadowAlpha = MSDF_AlphaAt((input.uv - shadowOffsetUV), MSDF_TextThreshold, state);

	const float shadowOnlyAlpha = (shadowAlpha * (1.0 - textAlpha));
	const float4 textPMA = (input.colorPMA * textAlpha);
	const float4 shadowPMA = (g_sdfShadowColorPMA * shadowOnlyAlpha);

	const float4 finalPMA = (textPMA + shadowPMA);
	return s3d_applyColorAdd(finalPMA, g_colorAdd);
}

float4 PS_MSDFFont_OutlineShadow(PSInput input) : SV_TARGET
{
	const float textThreshold = g_sdfParam.x;
	const float outlineThreshold = g_sdfParam.y;
	const float2 shadowOffsetTexels = g_sdfParam.zw;

	const MSDFState state = MSDF_Init(input.uv);
	const float sampledDistance = MSDF_SampleDistance(input.uv);

	const float outlineAlpha = MSDF_Coverage(sampledDistance, outlineThreshold, state.coverageScale);
	const float textAlpha = MSDF_Coverage(sampledDistance, textThreshold, state.coverageScale);

	const float2 shadowOffsetUV = (shadowOffsetTexels * state.invTextureSize);
	const float shadowAlpha = MSDF_AlphaAt((input.uv - shadowOffsetUV), outlineThreshold, state);

	const float4 textPMA = (input.colorPMA * textAlpha);

	const float outlineCoverage = saturate(outlineAlpha - textAlpha);
	const float4 outlinePMA = g_sdfOutlineColorPMA * outlineCoverage;

	const float shadowCoverage = saturate(shadowAlpha * (1.0 - outlineAlpha));
	const float4 shadowPMA = g_sdfShadowColorPMA * shadowCoverage;

	const float4 finalPMA = (textPMA + outlinePMA + shadowPMA);
	return s3d_applyColorAdd(finalPMA, g_colorAdd);
}

float4 PS_MSDFFont_Glow(PSInput input) : SV_TARGET
{
	const float glowExponent = g_sdfParam.x;

	const float glowBase = saturate(g_texture0.Sample(g_sampler0, input.uv).a * 2.0);
	const float glowFactor = pow(abs(glowBase), glowExponent);

	const float4 finalPMA = float4((input.colorPMA.rgb * glowFactor), (input.colorPMA.a * glowFactor));
	return s3d_applyColorAdd(finalPMA, g_colorAdd);
}

float4 PS_MSDFFont_Print(PSInput input) : SV_TARGET
{
	const MSDFState state = MSDF_Init(input.uv);
	const float sampledDistance = MSDF_SampleDistance(input.uv);

	const float outlineDistanceOffset = 0.04;
	const float outlineThreshold = (MSDF_TextThreshold - outlineDistanceOffset);

	const float textAlpha = sqrt(saturate((sampledDistance - 0.5) * state.coverageScale + 0.5));
	const float outlineAlpha = sqrt(saturate((sampledDistance - outlineThreshold) * state.coverageScale + 0.5));

	const float2 shadowOffsetTexels = float2(0.625, 0.625);
	const float2 shadowOffsetUV = (shadowOffsetTexels * state.invTextureSize);
	const float shadowDistance = MSDF_SampleDistance(input.uv - shadowOffsetUV);
	const float shadowAlpha = sqrt(saturate((shadowDistance - outlineThreshold) * state.coverageScale + 0.5));

	float3 color = lerp(float3(0.0, 0.0, 0.0), float3(1.0, 1.0, 1.0), textAlpha);
	const float hollowShadowAlpha = saturate(shadowAlpha * (1.0 - outlineAlpha));
	color = lerp(color, float3(0.0, 0.0, 0.0), hollowShadowAlpha);

	const float finalAlpha = saturate(outlineAlpha + hollowShadowAlpha);
	return float4(color, finalAlpha);
}
