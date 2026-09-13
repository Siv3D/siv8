# Pattern parameter payload

The pattern record contains four Float4 values (64 bytes). The fourth value is
additional pattern data; the existing six built-in pattern shaders ignore it.
This storage extension does not add a pattern type, gradient, profile, or change
the adopted [coordinate model](pattern-coordinates.md).

Metal records and uploads all four values. D3D11 completion is tracked in
[TODO](../../TODO.md); the [Windows handoff](d3d11-pattern-payload-handoff.md)
describes the remaining port. Backend staging information belongs here rather
than in public API documentation.

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
Existing typed pattern constructors default the new member to zero. The packing
return type changes from `std::array<Float4, 3>` to `std::array<Float4, 4>`;
callers explicitly depending on that type need updating.

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
`setQuadWarp` does not modify pattern fields. The Metal declaration uses
`g_patternExtraParams` at the matching position. No QuadWarp storage is aliased,
and its shader calculations and vertex interface are independent.

## State and cost

The Metal command manager records and compares all four vectors. Changing only
`extraParams` must create a pattern-state transition; identical values must still
batch. Transitioning back to a default pattern must restore zero rather than
retain data from an earlier draw or frame.

Pattern record storage grows from 48 to 64 bytes. The shared effect structure
grows from 112 to 128 bytes; Metal submits its full size when that buffer is
dirty. Vertex size, vertex attributes, and the number of interpolators do not
change. Existing built-in pattern fragment calculations do not change. These
are layout and operation facts, not a measured GPU-time result.

## D3D11 staging boundary

The shared packing API and `setPattern` use four vectors without compatibility
overloads. D3D11 still has its original three-vector command path and must be
updated before rebuilding. Its HLSL declaration and bytecode must also adopt the
new offsets: the old QuadWarp layout no longer matches the C++ effect structure.
There is no staging adapter or alternate layout to preserve the old path.

The Windows port should extend pattern input/record/current-state/getter types
to four vectors, place the added HLSL field immediately after the background,
regenerate shader bytecode, and validate the resulting build and rendering.

## Validation

[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) checks:

- Four-vector packing, zero defaults, preservation under singular/reflected UV
  mappings, and fixed shader field offsets.
- Pattern updates preserve QuadWarp fields, and QuadWarp updates preserve all
  pattern fields.
- A Metal diagnostic pixel shader reads byte offset 48 and uses all four
  components. The sequence zero/A/A/B/A/zero verifies restoration, state changes,
  and batching when only the additional vector differs. A QuadWarp draw and a
  subsequent diagnostic draw verify the independent state across shader changes.
- All six existing patterns render identically with zero and nonzero additional
  values. Existing coordinate, shape-path, viewport, custom-shader, and batch
  tests remain applicable.

The diagnostic shader is test-only. It is not a new built-in pattern feature.
