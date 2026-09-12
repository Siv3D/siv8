# Pattern coordinate design

Status: adopted design, not yet implemented. Implement and validate Metal first,
then port the same behavior to D3D11. Public-header documentation will describe
the behavior available on each backend as the implementation progresses.
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

The current [Metal pattern shaders](../../macOS/App/engine/shader/metal/2d.metal)
use fragment positions in the render target. The renderer compensates their UV
scale with the reciprocal RMS of the combined local/camera transform, through
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
shader for this path can put the untransformed vertex position there, while the
six pattern fragment shaders read it instead of the fragment's target position.
The current `VS_QuadWarp` already forwards positions in this way. Reuse that
behavior with naming appropriate to its shared responsibility; keep normal
texture UV forwarding on the normal shape/texture vertex shader path.

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
arcs, polygons, and line strings. They should select the same position-forwarding
vertex path and pack pattern parameters without RMS scaling. The command buffer
does not need a new pattern-coordinate state or a cache of inverse transforms.

During the Metal-first stage, retain the D3D11 RMS packing path so that its
behavior changes only when it is ported. Make the packing responsibility explicit
instead of silently changing the meaning of a shared scalar argument.

Custom shaders require a documented interface: a custom vertex shader paired
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
