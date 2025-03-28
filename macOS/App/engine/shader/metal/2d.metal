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
	float4 color;
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
};

struct PSEffectConstants2D
{
	float2x4 g_patternUVTransform;
	float4 g_patternBackgroundColor;
	float3x3 g_quadWarpInvHomography;
	float4 g_quadWarpUVTransform;
};

float4 s3d_positionTransform(float2 pos, float2x4 t)
{
	const float2 t_13_14 = float2(t[0][2], t[0][3]);
	const float2 t_11_12 = float2(t[0][0], t[0][1]);
	const float2 t_21_22 = float2(t[1][0], t[1][1]);
	return float4((t_13_14 + (pos.x * t_11_12) + (pos.y * t_21_22)), 0.0f, 1.0f);
}

float4 s3d_premultiplyAlpha(float4 color)
{
	return float4((color.rgb * color.a), color.a);
}

float4 s3d_shapeColor(float4 vertexColor, constant PSConstants2D* c)
{
	return (vertexColor + (c->g_colorAdd * vertexColor.a));
}

float4 s3d_textureColor(float4 vertexColor, float4 textureColor, constant PSConstants2D* c)
{
	vertexColor *= textureColor;
	return (vertexColor + (c->g_colorAdd * vertexColor.a));
}

float4 s3d_patternBackgroundColor(float4 backgroundColor, constant PSConstants2D* c)
{
	const float4 color = s3d_premultiplyAlpha(backgroundColor * c->g_patternBackgroundColorMul);
	return (color + (c->g_colorAdd * color.a));
}

float2 s3d_patternTransform(float2 uv, float2x4 t)
{
	const float2 t_13_14 = float2(t[0][2], t[0][3]);
	const float2 t_11_12 = float2(t[0][0], t[0][1]);
	const float2 t_21_22 = float2(t[1][0], t[1][1]);
	return (t_13_14 + (uv.x * t_11_12) + (uv.y * t_21_22));
}

vertex
PSInput VS_Shape(	uint vertexID [[vertex_id]],
					constant VSInput* vertices,
					constant VSConstants2D* c0)
{
	PSInput result;
	result.position	= s3d_positionTransform(vertices[vertexID].position, c0->g_transform);
	result.color	= s3d_premultiplyAlpha(vertices[vertexID].color * c0->g_colorMul);
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
	result.color	= s3d_premultiplyAlpha(vertices[vertexID].color * c0->g_colorMul);
	result.uv		= vertices[vertexID].position;
	return result;
}

fragment
float4 PS_Shape(	PSInput in [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]])
{
	return s3d_shapeColor(in.color, c0);
}

fragment
float4 PS_Texture(	PSInput in [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]],
					texture2d<float> texture0 [[texture(0)]],
					sampler sampler0 [[sampler(0)]])
{
	return s3d_textureColor(in.color, texture0.sample(sampler0, in.uv), c0);
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
	return s3d_textureColor(input.color, texture0.sample(sampler0, uv), c0);
}

fragment
float4 PS_LineDot(	PSInput in [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]])
{
	float4 result = in.color;

	const float u = (0.5 * (in.uv.x - 0.5));
	const float w = fwidth(u);
	const float distance = abs(2.0 * fract(u) - 1.0);
	const float alpha = smoothstep((0.5 - w), (0.5 + w), distance);
	result *= alpha;

	return s3d_shapeColor(result, c0);
}

fragment
float4 PS_LineDash(	PSInput in [[stage_in]],
					constant PSConstants2D* c0 [[buffer(0)]])
{
    float4 result = in.color;

    const float u = (0.25 * (in.uv.x - 1.0));
    const float w = fwidth(u);
    const float distance = abs(2.0 * fract(u) - 1.0);
    const float alpha = smoothstep((0.4 - w), (0.4 + w), distance);
    result *= alpha;

	return s3d_shapeColor(result, c0);
}

fragment
float4 PS_LineLongDash(	PSInput in [[stage_in]],
						constant PSConstants2D* c0 [[buffer(0)]])
{
    float4 result = in.color;

    const float u = (0.1 * (in.uv.x - 1.0));
    const float w = fwidth(u);
    const float distance = abs(2.0 * fract(u) - 1.0);
    const float alpha = smoothstep((0.3 - w), (0.3 + w), distance);
    result *= alpha;

	return s3d_shapeColor(result, c0);
}

fragment
float4 PS_LineDashDot(	PSInput in [[stage_in]],
						constant PSConstants2D* c0 [[buffer(0)]])
{
    float4 result = in.color;

    const float u = (0.1 * (in.uv.x - 1.0));
    const float u2 = u + 0.5;
    const float w = fwidth(u);
    const float distance = abs(2.0 * fract(u) - 1.0);
    const float distance2 = abs(2.0 * fract(u2) - 1.0);
    const float alpha1 = smoothstep((0.4 - w), (0.4 + w), distance);
    const float alpha2 = smoothstep((0.9 - w), (0.9 + w), distance2);
    result *= max(alpha1, alpha2);

	return s3d_shapeColor(result, c0);
}

fragment
float4 PS_LineRoundDot(	PSInput in [[stage_in]],
						constant PSConstants2D* c0 [[buffer(0)]])
{
    float4 result = in.color;

    const float2 uv = ((in.uv + float2(0.5, 0.0)) * float2(0.5, 1));
    const float w = fwidth(uv.y);
    const float distance = length(float2(4.0, 2.0) * fract(uv) - float2(2.0, 1.0));
    const float alpha = (1.0 - smoothstep((1.0 - w), (1.0 + w), distance));
    result *= alpha;

	return s3d_shapeColor(result, c0);
}

fragment
float4 PS_PatternPolkaDot(	PSInput in [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, c1->g_patternUVTransform);
	const float2 repeat = (2.0 * fract(uv) - 1.0);
	const float value = length(repeat);
	const float fw = (length(float2(dfdx(value), dfdy(value))) * 0.70710678118);
	
	const float radiusScale = c1->g_patternUVTransform[1].z;
	const float c_val = smoothstep((radiusScale - fw), (radiusScale + fw), value);

	const float4 primary = s3d_shapeColor(in.color, c0);
	const float4 background = s3d_patternBackgroundColor(c1->g_patternBackgroundColor, c0);
	
	return mix(primary, background, c_val);
}

fragment
float4 PS_PatternStripe(	PSInput in [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float u = s3d_patternTransform(in.position.xy, c1->g_patternUVTransform).x;
	const float fw = fwidth(u);
	const float repeat = (2.0 * fract(u) - 1.0);
	const float value = abs(repeat);
	
	const float thicknessScale = (c1->g_patternUVTransform[1].z * (1 + 2 * fw) - fw);
	const float t = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);

	const float4 primary = s3d_shapeColor(in.color, c0);
	const float4 background = s3d_patternBackgroundColor(c1->g_patternBackgroundColor, c0);

	return mix(primary, background, t);
}

fragment
float4 PS_PatternGrid(	PSInput in [[stage_in]],
						constant PSConstants2D* c0 [[buffer(0)]],
						constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, c1->g_patternUVTransform);
	const float2 fw = fwidth(uv);
	const float2 repeat = (2.0 * fract(uv) - 1.0);
	const float2 value = abs(repeat);
	
	const float2 thicknessScale = (c1->g_patternUVTransform[1].zz * float2(1 + fw) - fw);
	const float2 t1 = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);
	const float t2 = min(t1.x, t1.y);

	const float4 primary = s3d_shapeColor(in.color, c0);
	const float4 background = s3d_patternBackgroundColor(c1->g_patternBackgroundColor, c0);

	return mix(primary, background, t2);
}

float2 Integral(float2 v)
{
	v /= 2.0;
	return (floor(v) + max((2.0 * fract(v) - 1.0), 0.0));
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

fragment
float4 PS_PatternChecker(	PSInput in [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, c1->g_patternUVTransform);
	const float t = CheckersFiltered(uv, c1->g_patternUVTransform[1].zw);

	const float4 primary = s3d_shapeColor(in.color, c0);
	const float4 background = s3d_patternBackgroundColor(c1->g_patternBackgroundColor, c0);
	
	return mix(primary, background, t);
}

float2 Skew(float2 v)
{
	const float2x2 transform = float2x2(1.0, (1.0 / tan(3.1415926535 / 3.0)), 0.0, (1.0 / sin(3.1415926535 / 3.0)));
	return (v * transform);
}

fragment
float4 PS_PatternTriangle(	PSInput in [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, c1->g_patternUVTransform);
	const float2 fw = (fwidth(uv) * 0.25);

	const float2 s1 = Skew(uv + float2(-fw.x, -fw.y));
	const float2 s2 = Skew(uv + float2(fw.x, fw.y));
	const float2 s3 = Skew(uv + float2(-fw.x, fw.y));
	const float2 s4 = Skew(uv + float2(fw.x, -fw.y));

	const float4 f1 = fract(float4(s1, s2));
	const float4 f2 = fract(float4(s3, s4));
	const float4 ss = float4(step(f1.x, f1.y), step(f1.z, f1.w), step(f2.x, f2.y), step(f2.z, f2.w));
	const float t = dot(ss, 0.25);

	const float4 primary = s3d_shapeColor(in.color, c0);
	const float4 background = s3d_patternBackgroundColor(c1->g_patternBackgroundColor, c0);
	
	return mix(primary, background, t);
}

float Hex(float2 p)
{
	const float2 HEX = float2(1, 1.73205081);
	const float4 t = (floor(float4(p, p - float2(0.5, 1)) / HEX.xyxy) + float4(0.5, 0.5, 0.5, 0.5));
	const float4 h = float4((p - t.xy * HEX), (p - (t.zw + float2(0.5, 0.5)) * HEX));
	const float2 hex = abs((dot(h.xy, h.xy) < dot(h.zw, h.zw)) ? h.xy : h.zw);
	return max(dot(hex, (HEX * 0.5)), hex.x);
}

fragment
float4 PS_PatternHexGrid(	PSInput in [[stage_in]],
							constant PSConstants2D* c0 [[buffer(0)]],
							constant PSEffectConstants2D* c1 [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, c1->g_patternUVTransform);
	const float2 fw = fwidth(uv);
	const float w = (max(fw.x, fw.y) * 0.5);

	const float thicknessScale = (c1->g_patternUVTransform[1].z * (1 + 2 * w));
	const float h = Hex(uv);
	const float t = smoothstep((thicknessScale - w), (thicknessScale + w), h);

	const float4 primary = s3d_shapeColor(in.color, c0);
	const float4 background = s3d_patternBackgroundColor(c1->g_patternBackgroundColor, c0);
	
	return mix(background, primary, t);
}
