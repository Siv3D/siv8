# Pattern parameter payload

The pattern record contains four Float4 values (64 bytes). The fourth vector
stores type-specific data for [Halftone](halftone.md), [Wave](wave.md), and
[Truchet](truchet.md). PolkaDot, Stripe, Grid, Checker, Triangle, HexGrid, Ripple,
and Weave ignore it. The adopted [coordinate model](pattern-coordinates.md)
applies to every type.

D3D11 and Metal record and upload all four values using the same effect-buffer
layout.

## CPU record and shader constants

[PatternParameters](../../Siv3D/include/Siv3D/Pattern/PatternParameters.hpp)
defaults `extraParams` to zero. Its
[packing function](../../Siv3D/include/Siv3D/Pattern/PatternParameters.ipp) returns:

| Record index | Value | Record byte offset |
| --- | --- | ---: |
| 0 | m11, m12, m31, m32 | 0 |
| 1 | m21, m22, param0, param1 | 16 |
| 2 | backgroundColor | 32 |
| 3 | extraParams | 48 |

The first three vectors retain their existing meaning and order. Foreground
color travels through vertices, and pattern type selects the pixel shader.
Typed patterns leave unused additional components zero. The packing return type
is `std::array<Float4, 4>`.

The shader effect structure contains the 64-byte Pattern record:

| Effect constant field | Byte offset | Bytes |
| --- | ---: | ---: |
| patternUVTransform | 0 | 32 |
| patternBackgroundColor | 32 | 16 |
| patternExtraParams | 48 | 16 |
| Total | | 64 |

[PSEffectConstants2D::setPattern](../../Siv3D/src/Siv3D/Renderer2D/Renderer2DCommon.hpp)
maps all four packed vectors to the effect buffer. Both HLSL and Metal declarations
place `g_patternExtraParams` immediately after `g_patternBackgroundColor`.
D3D11 binds the effect buffer to pixel-shader slot b1.
[Scoped projective drawing](quad-warp.md) uses the vertex transform constants.

## Shader-side interpretation

Each pattern shader reads the existing `input.uv` varying as `drawingPosition`,
the common drawing coordinates before local and camera transforms.
`Pattern_UVTransform` produces `patternUV`; Stripe uses only its x component
as `stripeCoord`. The externally visible varying keeps its name, type, and
coordinate contract.

The packed fields retain their layout. Local aliases give each used component
its pattern-specific meaning before filtering. Declaration comments specify
the CPU normalization, which is not a conversion to screen pixels:

| Pattern | Shader aliases | Interpretation |
| --- | --- | --- |
| PolkaDot / Halftone | `normalizedRadius` | Radius in centered cell coordinates spanning [-1, 1]; Halftone packs its endpoints as `2 * radius / pitch` |
| Stripe / Grid | `normalizedThickness` | `thicknessScale / 2`, with Grid using the same value on both axes |
| Wave / Ripple / Truchet | `normalizedThickness` | `thickness / pitch` |
| Wave | `normalizedAmplitude`, `slopeAmplitude` | `amplitude / pitch` and `2 * pi * amplitude / wavelength` respectively |
| Ripple | `normalizedRadiusOffset` | `radiusOffset / pitch` |
| Weave | `normalizedBandWidth`, `normalizedClearanceWidth` | `thickness / pitch` and `(thickness + 2 * gap) / pitch` respectively |
| Checker | `axisIntensity` | x is vertical intensity; y is horizontal intensity |
| HexGrid | `cellEdgeThreshold` | `0.5 - thicknessScale * 0.25`, the threshold applied to the hex-cell metric |

Halftone's `radiusFieldGradient` and `radiusFieldBias` express its linear field
in pattern UV coordinates. Truchet's `Pattern_DecodeTruchetSeed` reconstructs
the seed from the numeric halves described in [Truchet](truchet.md#deterministic-random-layout);
the named Random and Alternating constants match `Pattern::Truchet::Layout`.
Uniform keeps the original tile orientation.

## Pattern XY transform

Within each backend, `s3d_transformPoint2D` supplies the affine XY calculation for
`Pattern_UVTransform`. The packed matrix components
have the following roles:

| Local name | CPU matrix components | HLSL access | Metal access |
| --- | --- | --- | --- |
| `translation` | `m31, m32` | `transform._13_14` | `transform[0].zw` |
| `basisX` | `m11, m12` | `transform._11_12` | `transform[0].xy` |
| `basisY` | `m21, m22` | `transform._21_22` | `transform[1].xy` |

The expression keeps the order `translation + position.x * basisX + position.y * basisY`.
Only these six components enter the XY result; the Pattern parameters in the
remaining two components stay available to their consuming pixel shaders.

Vertex positions use the separate homogeneous transform described in
[Scoped projective drawing](quad-warp.md#vertex-shader-interface). Pattern UV
conversion remains in the pixel shader using perspective-correct drawing positions.

## Color composition

The pattern vertex shader multiplies the foreground by ColorMul and converts it
to premultiplied alpha (PMA). `Pattern_BackgroundColorPMA` does the same for the
background using the pixel-shader copy of ColorMul. Each pattern interpolates
these PMA colors using its coverage, then calls `s3d_applyColorAdd` to apply ColorAdd
once to the result. The same helper applies ColorAdd after PMA composition in
shape, line, texture, and MSDF shaders; MSDF Print uses its own composition
without ColorAdd.

For a fixed additive color, `A(C) = C + ColorAdd * C.a` is linear, so
`mix(A(P), A(B), t) = A(mix(P, B, t))`. This lets the two colors share the
additive operation. PMA conversion must remain before interpolation: multiplying
interpolated straight RGB by interpolated alpha would mix the two colors'
alpha contributions incorrectly when their alpha values differ.

Floating-point operation order can affect RGBA8 rounding. The
`Pattern.color_composition` test compares scoped ColorMul/ColorAdd with the same
straight-color operations baked into both pattern colors before drawing. It
covers all pattern types, unequal foreground/background alpha, either or both
colors fully transparent, zero ColorMul alpha, negative and above-one color
components, and overlapping draws. This comparison permits one RGBA8 unit of
CPU/GPU rounding; fully transparent draws must preserve the canvas exactly.

## State and cost

Both command managers record, compare, retrieve, and restore all four vectors.
Their initial pattern records and current values are zero-initialized. Changing
only `extraParams` creates a pattern-state transition; identical values still
batch. Returning to a pattern with unused extras restores zero, including across
frames.

Each pattern record uses 64 bytes. Both backends submit the full 64-byte effect
buffer when it is dirty. Vertex size, vertex attributes, and the number of
interpolators are unchanged by the storage extension. Existing built-in pattern
fragment calculations are also unchanged. These are layout and operation facts,
not a measured GPU-time result.

## Backend shader maintenance

[2d.metal](../../macOS/App/engine/shader/metal/2d.metal) and
[2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl) must agree with
the shared effect layout. Both backends use `VS_Pattern` and select dedicated
pixel shaders from `PatternType`. Loader order must match
[EnginePS](../../Siv3D/src/Siv3D/EngineShader/IEngineShader.hpp): Halftone follows
FontPrint, then Wave, Ripple, Weave, and Truchet.

[CEngineShader_D3D11::init](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp)
contains the startup HLSL compilation block and loads the tracked `.vs`/`.ps`
assets by path. When changing HLSL, regenerate affected bytecode on Windows
through that block. After an effect-layout change, rebuild all
listed entry points so no binary retains an old layout. Preserve shader assets
as binary data. The Windows SDK's `fxc /dumpbin` can inspect reflected offsets.

Run the [Pattern tests and full suite](pattern-coordinates.md#validation) on each
affected host. The [gallery](../../Test/Manual/PatternCollection.md) checks visual
appearance and animation. Keep temporary sample sources and captures out of
tracked files and restore any platform entry point used for verification. For
incomplete Windows reports or illegal-instruction failures, follow the
[clean-build diagnosis](../development/README.md#windows-incremental-build-failures).

## Validation

[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) checks:

- Four-vector packing, zero defaults, preservation under singular/reflected UV
  mappings, and fixed shader field offsets.
- HLSL and Metal diagnostic pixel shaders read byte offset 48 and use all four
  components. The sequence zero/A/A/B/A/zero verifies restoration, state changes,
  and batching when only the additional vector differs. A QuadWarp draw and a
  subsequent diagnostic draw verify the independent state across shader changes.
- All six existing patterns render identically with zero and nonzero additional
  values. Existing coordinate, shape-path, viewport, custom-shader, and batch
  tests remain applicable.
- Color composition follows the PMA and ColorAdd ordering described above for
  all pattern types.

The diagnostic shader is test-only. It is not a new built-in pattern feature.
