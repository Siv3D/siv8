# Truchet

`Pattern::Truchet` joins two quarter-circle arcs per square tile. The public
contract is in [Truchet.hpp](../../Siv3D/include/Siv3D/Pattern/Truchet.hpp); the
[Pattern collection](../../Test/Manual/PatternCollection.md) includes all three layouts.
Metal and D3D11 use the same arc geometry and deterministic integer hash. See
the [shader maintenance notes](pattern-payload.md#backend-shader-maintenance) for backend wiring.

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

The shader hashes the IEEE-754 binary32 bits of `floor(uv) + 0.5`, then uses unsigned 32-bit
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

Layout selection is uniform for a draw. Regular layouts do not depend on the
hash result; a compiler may still evaluate it when lowering branches to selects.
No vertex, command, effect-buffer, or QuadWarp changes are required.

## D3D11 integration

`PS_PatternTruchet` and `Pattern_TruchetHash` in
[2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl) use
`asuint(cell + 0.5f)` for coordinate keys and numeric `uint` conversions for
seed halves. Unsigned arithmetic, constants, and layout IDs match Metal.
The loader appends `2d_pattern_truchet.ps` after Weave, and the renderer selects
it for `PatternType::Truchet`.

## Validation

CPU packing and Metal/D3D11 GPU cases cover all layouts, negative coordinates,
seeds with high bits set, transformations, split geometry, and state restoration.
Solid interiors, zero/full-width compositing, seed independence for regular
layouts, and tile-midpoint connectivity have exact checks. GPU rendering uses
an independent geometric reference, rather than seed packing alone.
