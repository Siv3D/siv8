# Ripple

`Pattern::Ripple` repeats a band along the distance from a common center. Its
contract is in [Ripple.hpp](../../Siv3D/include/Siv3D/Pattern/Ripple.hpp), and the
[manual sample](../../Test/Manual/Ripple.md) provides compact cards, off-card
centers, layered rings, radial animation, and object transforms. Metal rendering
is implemented; D3D11 work is tracked in [TODO](../../TODO.md).

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

## D3D11 port checklist

1. Port `PS_PatternRipple` from
   [2d.metal](../../macOS/App/engine/shader/metal/2d.metal) into
   [2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl). Preserve the
   UV footprint instead of switching to `fwidth(length(uv))`. Use the existing
   pattern UV and premultiplied color helpers.
2. Add `2d_pattern_ripple.ps` compilation/loading, preserving the appended
   `EnginePS::PatternRipple` order after Wave, and regenerate DXBC on Windows.
3. Add the D3D11 renderer shader ID, initialization, and `PatternType::Ripple`
   selection. The current default selector draws a solid shape until this port
   is implemented; this temporary limitation does not belong in public Doxygen.
4. Enable the Ripple GPU cases in
   [Test_Pattern.cpp](../../Test/Test_Pattern.cpp) for Windows. CPU packing cases
   already run on both hosts. Retain the center-quad edge case and exact
   zero/full-width compositing checks.
5. Run focused Pattern tests and the complete Windows suite. Inspect the sample
   at rest and in motion, including offset wraparound and nonuniform transforms.

## Validation

CPU tests reconstruct radii along multiple directions at positive and negative
positions, check colors, spacing, offsets, width endpoints, and unused extras.
Metal readbacks cover ring interiors, off-shape centers, positive/negative
phase, the symmetric center quad, alpha compositing, offset periodicity,
object/camera transforms, split shapes with a viewport, state restoration across
Wave, and repeated frames.
