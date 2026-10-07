# Scoped projective drawing

[ScopedQuadWarp2D](../../Siv3D/include/Siv3D/ScopedQuadWarp2D.hpp) applies a
homography to ordinary 2D draws. Its public header defines the scope contract;
[Mat3x3](../../Siv3D/include/Siv3D/Mat3x3.hpp) defines matrix construction and
checked homography generation. The [manual program](../../Test/Manual/ScopedQuadWarp2D.md)
combines textures, patterns, curves, and MSDF text in one scope.

```cpp
const RectF source{ 0, 0, 400, 240 };
const Quad target{ 80, 80, 560, 120, 480, 380, 120, 330 };
if (const auto h = Mat3x3::TryHomography(source, target))
{
    const ScopedQuadWarp2D warp{ *h };
    source.draw(ColorF{ 0.2 });
    Circle{ 200, 120, 80 }.draw(Palette::Orange);
    texture.resized(100, 100).draw(20, 20);
}
```

The transform order for row vectors is `Local * Camera * Warp * Screen`.
Local/Camera scopes and Warp scopes therefore have fixed responsibilities,
regardless of their construction order. Compute the homography using coordinates
after Local and Camera, relative to the current viewport origin. Nested Warp
scopes apply the inner transform before the outer one. A scope with Identity
and `Target::Set` temporarily disables an outer warp.

Warp does not choose a curve tessellation quality. Set the existing quality
controls for the expected magnification. Cursor conversion and scissor rectangles
keep their existing coordinate systems. A source rectangle used to construct a
homography is a correspondence, not a clipping region. Drawing beyond it is
allowed only while the scope's homogeneous-W precondition still holds.

The checked factory accepts either winding, including a reflection between the
source and target. Invalid, numerically degenerate, or unrepresentable inputs
return `none`; a caller can skip the draw or retain its previous valid matrix.
Normalized double-precision calculations use fixed-size storage. The unchecked
factory shares those calculations and retains general four-point mappings.
OpenCV remains available to other engine features but is not used by Mat3x3.

## Vertex shader interface

All 2D vertex shaders use a 64-byte standard constant block. This replaces the
48-byte layout even for draws with an identity warp. Update and recompile every
custom VS that reads this block.

| Member | Offset | Size |
| --- | ---: | ---: |
| transform, three Float4 values | 0 | 48 |
| colorMul | 48 | 16 |

For a row-vector Mat3x3 M, the three vectors are `(M11,M12,0,M13)`,
`(M21,M22,0,M23)`, and `(M31,M32,0,M33)`. HLSL declares a row-major float3x4;
Metal declares a float3x4 with three float4 columns. These share the same bytes
but use opposite matrix-multiply syntax.

A texture-compatible HLSL VS is:

```hlsl
cbuffer VSConstants2D : register(b0)
{
    row_major float3x4 g_transform;
    float4 g_colorMul;
};
struct VSInput { float2 position : POSITION; float2 uv : TEXCOORD0; float4 color : COLOR0; };
struct PSInput { float4 position : SV_POSITION; float4 colorPMA : COLOR0; float2 uv : TEXCOORD0; };
PSInput VS(VSInput input)
{
    PSInput output;
    const float4 clip = mul(float3(input.position, 1), g_transform);
    output.position = float4(clip.xy, 0, clip.w);
    const float4 color = input.color * g_colorMul;
    output.colorPMA = float4(color.rgb * color.a, color.a);
    output.uv = input.uv;
    return output;
}
```

The equivalent Metal VS is:

```metal
#include <metal_stdlib>
using namespace metal;
struct VSConstants2D { float3x4 g_transform; float4 g_colorMul; };
struct VSInput { float2 position; float2 uv; float4 color; };
struct PSInput { float4 position [[position]]; float4 colorPMA; float2 uv; };
vertex PSInput VS(uint id [[vertex_id]],
                  constant VSInput* vertices [[buffer(0)]],
                  constant VSConstants2D& c [[buffer(1)]])
{
    const VSInput input = vertices[id];
    PSInput output;
    const float4 clip = c.g_transform * float3(input.position, 1);
    output.position = float4(clip.xy, 0, clip.w);
    const float4 color = input.color * c.g_colorMul;
    output.colorPMA = float4(color.rgb * color.a, color.a);
    output.uv = input.uv;
    return output;
}
```

Keep the homogeneous W in the position output; do not divide XY in the VS and
replace W with 1. The rasterizer then interpolates UV and vertex colors with
perspective correction. A custom vertex displacement belongs before the matrix
multiply when it should be warped with the drawing. For a built-in pattern PS,
forward the drawing position instead of texture UV, as described in
[pattern coordinates](pattern-coordinates.md).

Existing texture/MSDF-compatible custom pixel shaders can keep their UV and
PMA color inputs. A shader based on fragment screen position still sees screen
coordinates. Explicit non-perspective interpolation or a custom VS that ignores
the standard transform requires its own handling; the scope cannot transform
those operations automatically.

## Renderer responsibilities and migration

Local and Camera remain Mat3x2 states; Warp is a separate Mat3x3. The existing
Transform command records their composed Mat3x3. Viewport changes rebuild Screen
and upload the composed clip-space matrix. Changes coalesce until a draw;
entering and leaving an empty scope does not split a batch. RMS scaling derives
only from Local * Camera.

The standard shape/texture VS forwards texture UV. The pattern VS forwards drawing
coordinates. Both use the same position helper. There is no warp-specific VS,
PS, effect constant, or command. Pattern effect constants occupy 64 bytes.

The old Texture/TextureRegion `drawQuadWarp` overloads are replaced by a matrix
scope around ordinary `draw`. Texture regions, mirroring, and vertex colors use
the ordinary texture path. `TextEffect::QuadWarp` constructs its glyph transform
in coordinates after Local/Camera, then draws with the ordinary texture path
inside a nested scope. A collapsed or invalid glyph mapping is skipped.

[HLSL](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl),
[MSL](../../macOS/App/engine/shader/metal/2d.metal), CPU constants, and custom VS
examples must be kept together when changing the layout. Regenerate the tracked
D3D11 bytecode using the startup compilation block on Windows after HLSL changes.
Outstanding backend verification belongs in [TODO](../../TODO.md).

## Verification

[Test_Mat3x3.cpp](../../Test/Test_Mat3x3.cpp) checks composition, corner mappings,
round trips, both windings, scale changes, zero M33, and rejected inputs.
[Test_QuadWarp.cpp](../../Test/Test_QuadWarp.cpp) checks scope lifetime, rendering,
perspective interpolation, state transitions, and batching. The existing pattern,
constant-buffer, and renderer tests exercise the updated custom shader layout.
Run focused tests with `./macOS/run-tests.sh '--test-case=Mat3x3.*,QuadWarp.*'`,
then the full `./macOS/run-tests.sh` suite. Use the corresponding Windows test
workflow on a Windows host; one backend does not validate the other.
