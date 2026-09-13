# Truchet

`Pattern::Truchet` joins two quarter-circle arcs per square tile. The public
contract is in [Truchet.hpp](../../Siv3D/include/Siv3D/Pattern/Truchet.hpp); the
[manual sample](../../Test/Manual/Truchet.md) compares three layouts and seeds.
Metal rendering is implemented. The remaining backend work is tracked in
[TODO](../../TODO.md) and the [combined D3D11 handoff](d3d11-new-patterns.md).

## Geometry and layout

Use `uv = Rotate(-angle) * (drawingPosition - origin) / pitch`. `floor(uv)` is
the tile coordinate, and `q = uv - floor(uv)` is its local position. Uniform tiles
place radius-0.5 quarter circles at `(0,0)` and `(1,1)` in local coordinates.
The other orientation reflects q.x, giving corners `(1,0)` and `(0,1)`.
Both orientations meet each tile edge at its midpoint with the same tangent.

Uniform keeps the first orientation everywhere. Alternating reflects tiles whose
column-plus-row parity is odd, including negative coordinates. Random selects
the orientation from the tile coordinate and seed. Seed does not change either
regular layout. The shape and width of the arcs are independent of this choice.
Thick arcs may merge; width zero reveals the background and width equal to pitch
fills the shape.

Placement uses the [common coordinate model](pattern-coordinates.md). Origin is
a tile corner, not the center of an arc. Changing origin moves the tiling and its
layout together; changing only shape geometry does not reset the tiling.
Nonuniform local/camera transforms deform circles with the object.

## Deterministic random layout

Do not use a trigonometric floating-point hash for the D3D11 port. The shader
hashes the IEEE-754 binary32 bits of `floor(uv) + 0.5`, then uses unsigned 32-bit
arithmetic with modulo-2^32 multiplication and logical shifts:

```text
key = bitcast_uint2(cell + 0.5)
h = key.x * 0x9E3779B9 XOR key.y * 0x85EBCA6B XOR seed
h ^= h >> 16
h *= 0x7FEB352D
h ^= h >> 15
h *= 0x846CA68B
h ^= h >> 16
flip = (h & 1) != 0
```

Using half-integer cell centers gives positive and negative zero the same key
without converting arbitrary floating-point tile coordinates to signed integers.
As with other procedural patterns, floating-point coordinate resolution limits
how far from the origin individual tiles remain distinguishable. This is not a
promise of arbitrary-precision tiling.

All seed bits are retained by storing its low and high 16-bit halves as numeric
floats. Both halves are exactly representable and finite. Packing the uint32 by
bit-casting it into a float instead would produce NaNs for some seeds, interfering
with float-based render-state comparisons. Reconstruct the seed with numeric
casts and an unsigned shift, not a bit-cast of these two payload components.

Fixed hash vectors are in [Test_Pattern.cpp](../../Test/Test_Pattern.cpp). The
rendering tests independently construct the circles in drawing coordinates and
use the fixed hash specification to determine their tile orientation.

## Payload and filtering

The [64-byte Pattern payload](pattern-payload.md) remains unchanged:

| Payload | Meaning |
| --- | --- |
| UV transform | Drawing coordinates to uv |
| param0 | thickness / pitch |
| param1 | Reserved, zero |
| extraParams.x | Numeric low 16 bits of seed |
| extraParams.y | Numeric high 16 bits of seed |
| extraParams.z | Layout: Random=0, Uniform=1, Alternating=2 |
| extraParams.w | Reserved, zero |

The fragment shader computes the smaller distance to the two quarter circles.
It uses `length(fwidth(uv))` as the edge footprint, rather than differentiating
orientation-dependent distances or wrapped tile coordinates. The distance
agrees on either side of each tile edge. The filter is local; it does not
integrate many subpixel tiles, so strong minification can alias.

Layout branches are uniform for a draw. Regular layouts skip hashing. No vertex,
command, effect-buffer, or QuadWarp changes are required.

## D3D11 port details

Translate `PS_PatternTruchet` and `Pattern_TruchetHash` from
[2d.metal](../../macOS/App/engine/shader/metal/2d.metal) into HLSL. Use
`asuint(cell + 0.5f)` for coordinate keys and numeric `(uint)` conversions for
seed halves. Preserve the uint arithmetic, all constants, and layout IDs.
Register `2d_pattern_truchet.ps` after Weave in the engine shader loader and add
the renderer shader ID/selection. Before this port the existing default selector
draws a solid shape; public Doxygen does not describe that temporary state.

Enable the Truchet GPU cases on Windows, run the focused and full test suites,
and compare regular/random galleries. Retain exact solid-interior, zero/full-width,
seed-independence for regular layouts, and tile-midpoint checks. Do not treat
matching seed packing alone as verification of the GPU hash or rendered layout.
