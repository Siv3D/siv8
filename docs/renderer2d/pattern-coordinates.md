# Pattern coordinate design

Status: the Metal coordinate path and its validation stage are complete. The
D3D11 port is pending. Public-header documentation describes the
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

Pattern draws use a separate vertex shader without changing the vertex layout or
adding per-vertex data. The pattern fragment shaders still perform one affine UV
mapping and their existing filtering. No inverse transform or extra pattern
coordinate state is computed by the command manager.

Batching is checked with frame metrics: eight rectangles sharing one pattern and
transform produce one draw call. Eight repetitions of pattern, solid, texture,
and quad warp produce 32 draw calls, one for each required state. Pattern draws
with different local/camera transforms still need transform-state transitions;
removing RMS packing does not combine those draws. These checks establish draw
counts and correct output, not a GPU-time speedup. No GPU timing comparison is
claimed.

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

## Validation

[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) owns these automated checks:

| Check | Evidence |
| --- | --- |
| Parameter packing | Affine mapping, translation, payload preservation, and the legacy scaling overload |
| Drawing coordinates | All six patterns with translation, quarter-turn and arbitrary rotation, uniform/nonuniform scale, reflection, shear, and local/camera composition; nonzero pattern angle and offset |
| Viewport and continuity | Nonzero viewport origins, clipping, restoration, and a rectangle split into a rectangle and two triangles without restarting the pattern |
| Shape paths | All 20 Metal pattern entry points, including fills, frames, arcs, polygons, CPU-transformed polygon coordinates, and line strings |
| State restoration | Pattern background and primary color multiplication/addition, alpha, collapsed geometry followed by a valid draw, and multiple frames |
| Shader and vertex batch transitions | Interleaved patterns, solid shapes, a multicolor texture, and quad warp before/at/after vertex-range boundaries |
| Custom shaders | A custom VS forwarding drawing coordinates, a custom PS consuming them, both together, and restoration of the built-in shaders |

Corresponding-position tests sample neighborhoods with stable interior colors
and allow a small color tolerance for transformed filtering. Shape-path tests
use ordinary solid geometry as a coverage mask and compare against a rectangular
pattern reference in the same drawing coordinates. Thus both missing geometry
and wrong pattern coordinates are observable. Continuity and batch tests compare
complete images when rasterization is expected to remain identical. Clipping
tests require exact background outside the viewport and allow one 8-bit unit of
color rounding inside, where a different projection size can affect filtering.

[Pattern transforms](../../Test/Manual/PatternTransforms.md) provides a complete
interactive program for comparing a single rectangle, the same shape divided
into primitives, and geometry moving within a shared coordinate system. It also
switches between local and camera transforms. The
[gallery](../../Test/Manual/PatternGallery.md) covers editable pattern parameters
and the separate effect of scene presentation scaling.
