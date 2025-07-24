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
	float4 color	: COLOR0;
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
	float4 g_sdfOutlineColor;
	float4 g_sdfShadowColor;
	//float4 g_internal;
}

cbuffer PSEffectConstants2D : register(b1)
{
	row_major float2x4 g_patternUVTransform;
	float4 g_patternBackgroundColor;
	row_major float3x3 g_quadWarpInvHomography;
	float4 g_quadWarpUVTransform;
}

float4 s3d_positionTransform(float2 pos, float2x4 t)
{
	return float4((t._13_14 + (pos.x * t._11_12) + (pos.y * t._21_22)), t._23_24);
}

float4 s3d_premultiplyAlpha(float4 color)
{
	return float4((color.rgb * color.a), color.a);
}

float4 s3d_shapeColor(float4 vertexColor)
{
	return (vertexColor + (g_colorAdd * vertexColor.a));
}

float4 s3d_textureColor(float4 vertexColor, float4 textureColor)
{
	vertexColor *= textureColor;
	return (vertexColor + (g_colorAdd * vertexColor.a));
}
	
float4 s3d_patternBackgroundColor()
{
	const float4 color = s3d_premultiplyAlpha(g_patternBackgroundColor * g_patternBackgroundColorMul);
	return (color + (g_colorAdd * color.a));
}

float2 s3d_patternTransform(float2 uv)
{
	return (g_patternUVTransform._13_14 + (uv.x * g_patternUVTransform._11_12) + (uv.y * g_patternUVTransform._21_22));
}

float s3d_median(float r, float g, float b)
{
	return max(min(r, g), min(max(r, g), b));
}

PSInput VS_Shape(VSInput input)
{
	PSInput result;
	result.position	= s3d_positionTransform(input.position, g_transform);
	result.color	= s3d_premultiplyAlpha(input.color * g_colorMul);
	result.uv		= input.uv;
	return result;
}

PSInput VS_QuadWarp(VSInput input)
{
	PSInput result;
	result.position	= s3d_positionTransform(input.position, g_transform);
	result.color	= s3d_premultiplyAlpha(input.color * g_colorMul);
	result.uv		= input.position;
	return result;
}

float4 PS_Shape(PSInput input) : SV_TARGET
{
	return s3d_shapeColor(input.color);
}

float4 PS_Texture(PSInput input) : SV_TARGET
{
	return s3d_textureColor(input.color, g_texture0.Sample(g_sampler0, input.uv));
}

float4 PS_QuadWarp(PSInput input) : SV_TARGET
{
	const float3 t = mul(float3(input.uv, 1.0f), g_quadWarpInvHomography);
	const float2 uv = ((t.xy / t.z) * g_quadWarpUVTransform.xy + g_quadWarpUVTransform.zw);
	return s3d_textureColor(input.color, g_texture0.Sample(g_sampler0, uv));
}

float4 PS_LineDot(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	const float u = (0.5 * (input.uv.x - 0.5));
	const float w = fwidth(u);
	const float value = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.5 - w), (0.5 + w), value);
	result *= alpha;

	return s3d_shapeColor(result);
}

float4 PS_LineDash(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	const float u = (0.25 * (input.uv.x - 1.0));
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.4 - w), (0.4 + w), distance);
	result *= alpha;

	return s3d_shapeColor(result);
}

float4 PS_LineLongDash(PSInput input) : SV_TARGET
{
	float4 result = input.color;

	const float u = (0.1 * (input.uv.x - 1.0));
	const float w = fwidth(u);
	const float distance = abs(2.0 * frac(u) - 1.0);
	const float alpha = smoothstep((0.3 - w), (0.3 + w), distance);
	result *= alpha;

	return s3d_shapeColor(result);
}

float4 PS_LineDashDot(PSInput input) : SV_TARGET
{
	float4 result = input.color;

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
	float4 result = input.color;

	const float2 uv = ((input.uv + float2(0.5, 0.0)) * float2(0.5, 1));
	const float w = fwidth(uv.y);
	const float distance = length(float2(4.0, 2.0) * frac(uv) - float2(2.0, 1.0));
	const float alpha = (1.0 - smoothstep((1.0 - w), (1.0 + w), distance));
	result *= alpha;

	return s3d_shapeColor(result);
}

float4 PS_PatternPolkaDot(PSInput input) : SV_TARGET
{
	const float2 uv = s3d_patternTransform(input.position.xy);
	const float2 repeat = (2.0 * frac(uv) - 1.0);
	const float value = length(repeat);
	const float fw = (length(float2(ddx(value), ddy(value))) * 0.70710678118);

	const float radiusScale = g_patternUVTransform[1].z;
	const float c = smoothstep((radiusScale - fw), (radiusScale + fw), value);

	const float4 primary = s3d_shapeColor(input.color);
	const float4 background = s3d_patternBackgroundColor();

	return lerp(primary, background, c);
}

float4 PS_PatternStripe(PSInput input) : SV_TARGET
{
	const float u = s3d_patternTransform(input.position.xy).x;
	const float fw = fwidth(u);
	const float repeat = (2.0 * frac(u) - 1.0);
	const float value = abs(repeat);

	const float thicknessScale = (g_patternUVTransform[1].z * (1 + 2 * fw) - fw);
	const float c = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);

	const float4 primary = s3d_shapeColor(input.color);
	const float4 background = s3d_patternBackgroundColor();

	return lerp(primary, background, c);
}

float4 PS_PatternGrid(PSInput input) : SV_TARGET
{
	const float2 uv = s3d_patternTransform(input.position.xy);
	const float2 fw = fwidth(uv);
	const float2 repeat = (2.0 * frac(uv) - 1.0);
	const float2 value = abs(repeat);

	const float2 thicknessScale = (g_patternUVTransform[1].zz * float2(1 + fw) - fw);
	const float2 c = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);
	const float c2 = min(c.x, c.y);

	const float4 primary = s3d_shapeColor(input.color);
	const float4 background = s3d_patternBackgroundColor();

	return lerp(primary, background, c2);
}

float2 Integral(float2 v)
{
	v /= 2.0;
	return (floor(v) + max((2.0 * frac(v) - 1.0), 0.0));
}

float CheckersFiltered(float2 p, float2 hv)
{
	const float2 fw = fwidth(p);
	const float w = max(fw.x, fw.y);
	float2 i = (Integral(p + 0.5 * w) - Integral(p - 0.5 * w));
	i *= hv;
	i /= w;
	return (i.x + i.y - 2.0 * i.x * i.y);
}

float4 PS_PatternChecker(PSInput input) : SV_TARGET
{
	const float2 uv = s3d_patternTransform(input.position.xy);
	const float c = CheckersFiltered(uv, g_patternUVTransform[1].zw);

	const float4 primary = s3d_shapeColor(input.color);
	const float4 background = s3d_patternBackgroundColor();

	return lerp(primary, background, c);
}

float2 Skew(float2 v)
{
	const float2x2 transform = float2x2(1.0, (1.0 / tan(3.1415926535 / 3.0)), 0.0, (1.0 / sin(3.1415926535 / 3.0)));
	return mul(transform, v);
}

float4 PS_PatternTriangle(PSInput input) : SV_TARGET
{
	const float2 uv = s3d_patternTransform(input.position.xy);
	const float2 fw = (fwidth(uv) * 0.25);

	const float2 s1 = Skew(uv + float2(-fw.x, -fw.y));
	const float2 s2 = Skew(uv + float2(fw.x, fw.y));
	const float2 s3 = Skew(uv + float2(-fw.x, fw.y));
	const float2 s4 = Skew(uv + float2(fw.x, -fw.y));

	const float4 f1 = frac(float4(s1, s2));
	const float4 f2 = frac(float4(s3, s4));
	const float4 ss = float4(step(f1.x, f1.y), step(f1.z, f1.w), step(f2.x, f2.y), step(f2.z, f2.w));
	const float c = dot(ss, 0.25);

	const float4 primary = s3d_shapeColor(input.color);
	const float4 background = s3d_patternBackgroundColor();

	return lerp(primary, background, c);
}

float Hex(float2 p)
{
	const float2 HEX = float2(1, 1.73205081);
	const float4 t = (floor(float4(p, p - float2(0.5, 1)) / HEX.xyxy) + float4(0.5, 0.5, 0.5, 0.5));
	const float4 h = float4((p - t.xy * HEX), (p - (t.zw + float2(0.5, 0.5)) * HEX));
	const float2 hex = abs((dot(h.xy, h.xy) < dot(h.zw, h.zw)) ? h.xy : h.zw);
	return max(dot(hex, (HEX * 0.5)), hex.x);
}

float4 PS_PatternHexGrid(PSInput input) : SV_TARGET
{
	const float2 uv = s3d_patternTransform(input.position.xy);
	const float2 fw = fwidth(uv);
	const float w = (max(fw.x, fw.y) * 0.5);

	const float thicknessScale = (g_patternUVTransform[1].z * (1 + 2 * w));
	const float h = Hex(uv);
	const float c = smoothstep((thicknessScale - w), (thicknessScale + w), h);

	const float4 primary = s3d_shapeColor(input.color);
	const float4 background = s3d_patternBackgroundColor();

	return lerp(background, primary, c);
}

float4 PS_MSDFFont(PSInput input) : SV_TARGET
{
	float2 size;
	g_texture0.GetDimensions(size.x, size.y);
	const float pxRange = 16.0;
	const float2 msdfUnit = (pxRange / size);

	const float3 s = g_texture0.Sample(g_sampler0, input.uv).rgb;
	const float d = s3d_median(s.r, s.g, s.b);

	const float td = (d - 0.5);
	const float textAlpha = saturate(td * dot(msdfUnit, 0.5 / fwidth(input.uv)) + 0.5);

	const float4 textureColor = (input.color * textAlpha);
	return (textureColor + (g_colorAdd * textureColor.a));
}

float4 PS_MSDFFont_Outline(PSInput input) : SV_TARGET
{
	float2 size;
	g_texture0.GetDimensions(size.x, size.y);
	const float pxRange = 16.0;
	const float2 msdfUnit = (pxRange / size);

	const float3 s = g_texture0.Sample(g_sampler0, input.uv).rgb;
	const float d = s3d_median(s.r, s.g, s.b);

	const float od = (d - g_sdfParam.y);
	const float thickAlpha = saturate(od * dot(msdfUnit, 0.5 / fwidth(input.uv)) + 0.5);

	const float td = (d - g_sdfParam.x);
	const float textAlpha = saturate(td * dot(msdfUnit, 0.5 / fwidth(input.uv)) + 0.5);

	const float blend = textAlpha;
	float4 textureColor = lerp(g_sdfOutlineColor, input.color, blend);
	textureColor *= thickAlpha;
	
	return (textureColor + (g_colorAdd * textureColor.a));
}
