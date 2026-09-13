# Pattern parameter payload

The pattern record contains four Float4 values (64 bytes). The fourth value is
additional pattern data; the existing six built-in pattern shaders ignore it.
The [size-gradient pattern](polka-dot-size-gradient.md) uses that additional
vector for an independent linear field. Its shader does not change the storage
layout or the adopted [coordinate model](pattern-coordinates.md).

D3D11 and Metal record and upload all four values using the same effect-buffer
layout. The [D3D11 validation guide](d3d11-pattern-payload-handoff.md) describes
shader-bytecode regeneration and Windows rendering checks. Unimplemented
expression candidates remain in [TODO](../../TODO.md).

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
Existing typed pattern constructors default the additional member to zero. The
packing return type is `std::array<Float4, 4>`.

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
batch. Returning to a default pattern restores zero, including across frames.

Each pattern record uses 64 bytes. Both backends submit the full 128-byte effect
buffer when it is dirty. Vertex size, vertex attributes, and the number of
interpolators are unchanged by the storage extension. Existing built-in pattern
fragment calculations are also unchanged. These are layout and operation facts,
not a measured GPU-time result.

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
