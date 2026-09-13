# Ripple

`Pattern::Ripple` repeats a band along the distance from a common center. Its
contract is in [Ripple.hpp](../../Siv3D/include/Siv3D/Pattern/Ripple.hpp), and the
[manual sample](../../Test/Manual/Ripple.md) provides compact cards, off-card
centers, layered rings, radial animation, and object transforms. Metal and D3D11
use the same radial calculation and center filter.

## Placement and filtering

`center` is a drawing-coordinate point. `pitch`, `thickness`, and `radiusOffset`
use drawing-coordinate lengths. Increasing the offset moves the bands outward;
an offset change of one pitch returns to the same pattern. At zero offset the
center is inside the foreground band, producing a filled central disk rather
than a hollow ring. An offset of half a pitch can leave the center empty.

Moving the center outside a shape creates sweeping arcs. No rotation parameter
is needed for concentric circles. The existing local/camera transform carries
the pattern with the geometry, so nonuniform scaling produces ellipses.
Changing the shape's coordinates alone does not relocate the common center.
See the [coordinate model](pattern-coordinates.md).

The radial distance gives circular band boundaries without Wave's approximate
normal-width correction. At the origin, however, differentiating the distance
can yield a zero footprint for a 2x2 fragment quad with four equal radii. The
shader instead uses `length(fwidth(uv))`, as the Halftone shader does. This is a
conservative local footprint, including under nonuniform transforms. It is an
edge filter, not exact pixel-area integration; strong minification can alias.
Zero and full thickness preserve the normal foreground/background compositing.

## Payload

The [64-byte pattern payload](pattern-payload.md) is unchanged:

| Payload | Meaning |
| --- | --- |
| UV transform | `(drawingPosition - center) / pitch` |
| param0 | `thickness / pitch` |
| param1 | `radiusOffset / pitch` |
| extraParams | Zero; ignored by Ripple |

The fragment phase is `length(uv) - param1 + 0.5`. The half-period shift puts
radius zero at a band center when offset is zero. Wrapping this scalar with
`fract` supports both positive and negative offsets without a fragment branch.
The current Stripe edge-filter formula then evaluates the normalized width.
No command, buffer, vertex, or QuadWarp changes are required.

## D3D11 integration

`PS_PatternRipple` in
[2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl) uses continuous
UV derivatives and the existing premultiplied color helpers. The loader appends
`2d_pattern_ripple.ps` after Wave, and the renderer selects it for
`PatternType::Ripple`. See the [combined integration guide](d3d11-new-patterns.md)
for shader generation and verification.

## Validation

CPU tests reconstruct radii along multiple directions at positive and negative
positions, check colors, spacing, offsets, width endpoints, and unused extras.
Metal and D3D11 readbacks cover ring interiors, off-shape centers, positive/negative
phase, the symmetric center quad, alpha compositing, offset periodicity,
object/camera transforms, split shapes with a viewport, state restoration across
Wave, and repeated frames.
