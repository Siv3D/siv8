# Pattern coordinate design

Status: the basic Metal coordinate path is implemented. Complete Metal validation
before porting the behavior to D3D11. Public-header documentation describes the
adopted coordinate contract. Temporary backend differences belong in development
notes and test instructions, not the public API documentation.
Pending stages are tracked in [TODO](../../TODO.md).

## Coordinate model

A pattern is defined in the common drawing coordinates before the local and
camera transforms. Geometry and pattern undergo the same transforms. Translating
the drawing moves the pattern, rotating it rotates the pattern, and nonuniform
scaling deforms the pattern along with the geometry.

There is no implicit per-shape origin. Adjacent shapes drawn in the same
coordinate system can share a continuous pattern. Moving a circle by changing
its center changes geometry within that coordinate system; it does not move the
pattern's origin. Translating the coordinate system with `Transformer2D` moves
both. Likewise, coordinates already transformed by a CPU geometry operation are
the input coordinates for this model; the renderer does not reconstruct an
earlier object coordinate system.

Pattern angle, scale, and offset continue to define the pattern's own UV
transform. Apply that transform to the drawing coordinates without an additional
RMS factor. Scene presentation scaling remains a later operation affecting the
rendered scene as a whole.

## Carry coordinates forward

The [Metal pattern shaders](../../macOS/App/engine/shader/metal/2d.metal) use
interpolated drawing positions. The renderer packs their UV transform without
RMS scaling through
[PatternParameters](../../Siv3D/include/Siv3D/Pattern/PatternParameters.ipp).

For the adopted model, the vertex shader has both values needed by the GPU:

```text
vertex position in drawing coordinates
  +-- local/camera/projection transform --> rasterized position
  +-- copy to the existing float2 varying --> interpolated drawing position
                                               |
                                         pattern UV transform
                                               |
                                         pattern color
```

The existing 2D vertex shader output already has a `float2 uv` varying. A vertex
shader for this path puts the untransformed vertex position there, while the
six pattern fragment shaders read it instead of the fragment's target position.
`VS_Pattern` forwards drawing positions for patterns. `VS_QuadWarp` retains its
own entry point and renderer selection. Their forwarding code is currently the
same, but their interfaces remain independent so quad-warp calculations can be
moved between shader stages without changing the pattern path. Reconsider
integration only after quad-warp optimization establishes its final interface.
Normal texture UV forwarding stays on the normal shape/texture vertex shader path.

This needs no new vertex attribute, larger vertex buffer, additional interpolator,
inverse transform, or per-pixel inverse calculation. The existing affine UV
calculation and pattern filtering can remain in the fragment shader. Removing
RMS compensation also removes the pattern packing step's reciprocal-scale
dependency. Tessellation still uses RMS and is a separate responsibility.

Changing the vertex shader selected for pattern draws can change batching and
pipeline transitions when mixed with other draws. An unchanged vertex format
does not establish identical performance; validate those transitions explicitly.

## Integration boundaries

The Metal renderer has multiple pattern draw entry points for fills, frames,
arcs, polygons, and line strings. They select the same position-forwarding
vertex path and pack pattern parameters without RMS scaling. The command buffer
does not need a new pattern-coordinate state or a cache of inverse transforms.

During the Metal-first stage, the D3D11 RMS packing overload is retained so that
its behavior changes only when it is ported. Metal uses the parameterless packing
overload and a separate `Pattern2D` vertex shader slot. The existing `QuadWarp`
slot and D3D11 quad-warp shader selection remain unchanged. D3D11 does not yet
load or select the appended `Pattern2D` slot.

Custom shaders follow the interface documented on `PatternParameters`: a custom vertex shader paired
with a built-in pattern fragment shader must forward drawing coordinates in the
matching varying. Replacing only the fragment shader can also expose the new
varying values to that custom shader. The existing shape/texture shader interface
must not be changed globally to achieve the pattern behavior.

Validation should cover translation, rotation, uniform/nonuniform scale, shear,
reflection, local/camera composition, nonzero viewport origins, offset and angle,
and continuity across separate primitives. Also check changes between pattern,
solid, texture, and quad-warp draws, state restoration, and vertex batch splits.
Tests should compare known interior colors or corresponding image regions with
appropriate filtering tolerances, rather than require byte-identical edge
antialiasing. Store automated cases and complete visual programs under `Test`.

[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) checks parameter packing and the
basic Metal coordinate path for all six patterns: translation, quarter-turn
rotation, uniform/nonuniform scale, and local/camera composition. The image
comparison selects neighborhoods with stable interior colors in a reference
render, then samples the corresponding transformed positions.
