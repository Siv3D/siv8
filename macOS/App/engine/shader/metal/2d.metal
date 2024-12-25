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

struct PSPatternConstants2D
{
	float2x4 g_patternUVTransform_params;
	float4 g_patternBackgroundColor;
};

float4 s3d_positionTransform2D(float2 pos, float2x4 t)
{
	const float2 t_13_14 = float2(t[0][2], t[0][3]);
	const float2 t_11_12 = float2(t[0][0], t[0][1]);
	const float2 t_21_22 = float2(t[1][0], t[1][1]);
	return float4((t_13_14 + (pos.x * t_11_12) + (pos.y * t_21_22)), 0.0f, 1.0f);
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

float4 s3d_patternBackgroundColor(float4 color, constant PSConstants2D* c)
{
	color *= c->g_patternBackgroundColorMul;
	color.rgb *= color.a;
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
PSInput VS_Shape(uint vertexID [[vertex_id]], constant VSInput* vertices, constant VSConstants2D* c)
{
	PSInput out;
	out.position	= s3d_positionTransform2D(vertices[vertexID].position, c->g_transform);
	out.color		= vertices[vertexID].color;
	out.color.rgb *= out.color.a;
	out.uv			= vertices[vertexID].uv;
	return out;
}

fragment
float4 PS_Shape(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
	return s3d_shapeColor(in.color, c);
}

fragment
float4 PS_Texture(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]],
				  texture2d<float> texture0 [[texture(0)]], sampler sampler0 [[sampler(0)]])
{
	return s3d_textureColor(in.color, texture0.sample(sampler0, in.uv), c);
}

fragment
float4 PS_LineDot(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
	float4 result = in.color;

	const float u = (0.5 * (in.uv.x - 0.5));
	const float w = fwidth(u);
	const float distance = abs(2.0 * fract(u) - 1.0);
	const float alpha = smoothstep((0.5 - w), (0.5 + w), distance);
	result *= alpha;

	return s3d_shapeColor(result, c);
}

fragment
float4 PS_LineDash(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
    float4 result = in.color;

    const float u = (0.25 * (in.uv.x - 1.0));
    const float w = fwidth(u);
    const float distance = abs(2.0 * fract(u) - 1.0);
    const float alpha = smoothstep((0.4 - w), (0.4 + w), distance);
    result *= alpha;

	return s3d_shapeColor(result, c);
}

fragment
float4 PS_LineLongDash(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
    float4 result = in.color;

    const float u = (0.1 * (in.uv.x - 1.0));
    const float w = fwidth(u);
    const float distance = abs(2.0 * fract(u) - 1.0);
    const float alpha = smoothstep((0.3 - w), (0.3 + w), distance);
    result *= alpha;

	return s3d_shapeColor(result, c);
}

fragment
float4 PS_LineDashDot(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
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

	return s3d_shapeColor(result, c);
}

fragment
float4 PS_LineRoundDot(PSInput in [[stage_in]], constant PSConstants2D* c [[buffer(0)]])
{
    float4 result = in.color;

    const float2 uv = ((in.uv + float2(0.5, 0.0)) * float2(0.5, 1));
    const float w = fwidth(uv.y);
    const float distance = length(float2(4.0, 2.0) * fract(uv) - float2(2.0, 1.0));
    const float alpha = (1.0 - smoothstep((1.0 - w), (1.0 + w), distance));
    result *= alpha;

	return s3d_shapeColor(result, c);
}

fragment
float4 PS_PatternPolkaDot(PSInput in [[stage_in]],
	constant PSConstants2D* c [[buffer(0)]],
	constant PSPatternConstants2D* p [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, p->g_patternUVTransform_params);
	const float2 repeat = (2.0 * fract(uv) - 1.0);
	const float value = length(repeat);
	const float fw = (length(float2(dfdx(value), dfdy(value))) * 0.70710678118);
	
	const float radiusScale = p->g_patternUVTransform_params[1].z;
	const float c_val = smoothstep((radiusScale - fw), (radiusScale + fw), value);

	const float4 primary = s3d_shapeColor(in.color, c);
	const float4 background = s3d_patternBackgroundColor(p->g_patternBackgroundColor, c);
	
	return mix(primary, background, c_val);
}

fragment
float4 PS_PatternStripe(PSInput in [[stage_in]],
	constant PSConstants2D* c [[buffer(0)]],
	constant PSPatternConstants2D* p [[buffer(1)]])
{
	const float u = s3d_patternTransform(in.position.xy, p->g_patternUVTransform_params).x;
	const float fw = fwidth(u);
	const float repeat = (2.0 * fract(u) - 1.0);
	const float value = abs(repeat);
	
	const float thicknessScale = (p->g_patternUVTransform_params[1].z * (1 + 2 * fw) - fw);
	const float c1 = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);

	const float4 primary = s3d_shapeColor(in.color, c);
	const float4 background = s3d_patternBackgroundColor(p->g_patternBackgroundColor, c);

	return mix(primary, background, c1);
}

fragment
float4 PS_PatternGrid(PSInput in [[stage_in]],
	constant PSConstants2D* c [[buffer(0)]],
	constant PSPatternConstants2D* p [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, p->g_patternUVTransform_params);
	const float2 fw = fwidth(uv);
	const float2 repeat = (2.0 * fract(uv) - 1.0);
	const float2 value = abs(repeat);
	
	const float2 thicknessScale = (p->g_patternUVTransform_params[1].zz * float2(1 + fw) - fw);
	const float2 c1 = smoothstep((thicknessScale - fw), (thicknessScale + fw), value);
	const float c2 = min(c1.x, c1.y);

	const float4 primary = s3d_shapeColor(in.color, c);
	const float4 background = s3d_patternBackgroundColor(p->g_patternBackgroundColor, c);

	return mix(primary, background, c2);
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
float4 PS_PatternChecker(PSInput in [[stage_in]],
	constant PSConstants2D* c [[buffer(0)]],
	constant PSPatternConstants2D* p [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, p->g_patternUVTransform_params);
	const float c1 = CheckersFiltered(uv, p->g_patternUVTransform_params[1].zw);

	const float4 primary = s3d_shapeColor(in.color, c);
	const float4 background = s3d_patternBackgroundColor(p->g_patternBackgroundColor, c);
	
	return mix(primary, background, c1);
}

float2 Skew(float2 v)
{
	const float2x2 transform = float2x2(1.0, (1.0 / tan(3.1415926535 / 3.0)), 0.0, (1.0 / sin(3.1415926535 / 3.0)));
	return (v * transform);
}

fragment
float4 PS_PatternTriangle(PSInput in [[stage_in]],
	constant PSConstants2D* c [[buffer(0)]],
	constant PSPatternConstants2D* p [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, p->g_patternUVTransform_params);
	const float2 fw = (fwidth(uv) * 0.25);

	const float2 s1 = Skew(uv + float2(-fw.x, -fw.y));
	const float2 s2 = Skew(uv + float2(fw.x, fw.y));
	const float2 s3 = Skew(uv + float2(-fw.x, fw.y));
	const float2 s4 = Skew(uv + float2(fw.x, -fw.y));

	const float4 f1 = fract(float4(s1, s2));
	const float4 f2 = fract(float4(s3, s4));
	const float4 ss = float4(step(f1.x, f1.y), step(f1.z, f1.w), step(f2.x, f2.y), step(f2.z, f2.w));
	const float c1 = dot(ss, 0.25);

	const float4 primary = s3d_shapeColor(in.color, c);
	const float4 background = s3d_patternBackgroundColor(p->g_patternBackgroundColor, c);
	
	return mix(primary, background, c1);
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
float4 PS_PatternHexGrid(PSInput in [[stage_in]],
	constant PSConstants2D* c [[buffer(0)]],
	constant PSPatternConstants2D* p [[buffer(1)]])
{
	const float2 uv = s3d_patternTransform(in.position.xy, p->g_patternUVTransform_params);
	const float2 fw = fwidth(uv);
	const float w = (max(fw.x, fw.y) * 0.5);

	const float thicknessScale = (p->g_patternUVTransform_params[1].z * (1 + 2 * w));
	const float h = Hex(uv);
	const float c1 = smoothstep((thicknessScale - w), (thicknessScale + w), h);

	const float4 primary = s3d_shapeColor(in.color, c);
	const float4 background = s3d_patternBackgroundColor(p->g_patternBackgroundColor, c);
	
	return mix(background, primary, c1);
}
