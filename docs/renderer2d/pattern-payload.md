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

The shader effect structure groups all Pattern fields in its first 64 bytes and
all QuadWarp fields in its second 64 bytes:

| Effect constant field | Byte offset | Bytes |
| --- | ---: | ---: |
| patternUVTransform | 0 | 32 |
| patternBackgroundColor | 32 | 16 |
| patternExtraParams | 48 | 16 |
| quadWarpInvHomography | 64 | 48 |
| quadWarpUVTransform | 112 | 16 |
| Total | | 128 |

[PSEffectConstants2D::setPattern](../../Siv3D/src/Siv3D/Renderer2D/Renderer2DCommon.hpp)
maps all four packed vectors to the first 64 bytes of the effect buffer.
`setQuadWarp` does not modify pattern fields. Both HLSL and Metal declarations
place `g_patternExtraParams` immediately after `g_patternBackgroundColor`.
D3D11 binds the effect buffer to pixel-shader slot b1. No QuadWarp storage is
aliased, and its shader calculations and vertex interface are independent.

## State and cost

Both command managers record, compare, retrieve, and restore all four vectors.
Their initial pattern records and current values are zero-initialized. Changing
only `extraParams` creates a pattern-state transition; identical values still
batch. Returning to a pattern with unused extras restores zero, including across
frames.

Each pattern record uses 64 bytes. Both backends submit the full 128-byte effect
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
through that block. An effect-layout change also affects QuadWarp; rebuild all
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
- Pattern updates preserve QuadWarp fields, and QuadWarp updates preserve all
  pattern fields.
- HLSL and Metal diagnostic pixel shaders read byte offset 48 and use all four
  components. The sequence zero/A/A/B/A/zero verifies restoration, state changes,
  and batching when only the additional vector differs. A QuadWarp draw and a
  subsequent diagnostic draw verify the independent state across shader changes.
- All six existing patterns render identically with zero and nonzero additional
  values. Existing coordinate, shape-path, viewport, custom-shader, and batch
  tests remain applicable.

The diagnostic shader is test-only. It is not a new built-in pattern feature.
