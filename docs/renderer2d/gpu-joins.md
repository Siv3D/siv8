# Triangle and Quad GPU joins

`Triangle::drawFrame(thickness, fill, JoinStyle)` and
`drawFrame(innerThickness, outerThickness, fill, JoinStyle)`, with corresponding
Quad operations, default to Bevel. They use a direct mesh for strictly convex
rings whose inset edges all remain forward-facing. Other topology uses polygon
buffer and Earcut. Image painting shares the geometric meaning and has its own
coverage calculation. Polygon buffer APIs and ordinary LineString/Bezier/Spline
drawing are separate operations.

The public [JoinStyle contract](../../Siv3D/include/Siv3D/JoinStyle.hpp) owns the
meaning of the styles. Omitting the style selects Bevel in both GPU and Image
frames. Bevel preserves the historical 60-degree clipped-miter
shape, including the radius-distance clipping plane. Miter retains unlimited
extension. Both input windings are supported. Round uses the shared Renderer2D
circle-pie quality calculation with raster scale, radius, and corner angle in
both the direct and fallback paths. Raster scale includes the scene-resolution
scale as well as local/camera transforms. The policy still changes subdivision
in discrete steps as radius or angle changes; it does not promise identical
boundary pixels or monotonic pixel coverage at every quality threshold.

## Topology and ownership

[ClosedLineString.cpp](../../Siv3D/src/Siv3D/Polygon/ClosedLineString.cpp) selects
the direct mesh or fallback for the requested style. Triangle and Quad
supply a stack array. The centered stroke buffer view includes the two repeated
points needed to join its closing vertex. The direct builder receives only the
distinct ring vertices and separate inner/outer widths; centered entry points
supply half the total width to each side.

The shared
[Vertex2DBuilder](../../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder_Rounded.cpp)
performs these steps:

1. Require nonnegative widths with a positive sum and a strictly convex
   three- or four-corner ring.
   Duplicate edges, collinear corners, concavity, and self-intersection reject
   the direct path.
2. Intersect adjacent offset lines in double precision. Require each inset
   edge to have strictly positive projection on its original edge direction.
   For a convex ring, these parallel, forward-running edges form the inset
   polygon. This rejects both a vanished hole and a short quad edge disappearing
   while some of the hole remains.
3. Allocate one destination region. Partition the frame into edge trapezoids
   and corner fans anchored at inset vertices, sharing their boundaries.
   Emit clockwise triangles directly into the renderer's vertices and indices.

The edge/corner partition has disjoint interiors, so alpha is applied once.
Three- and four-corner preparation fits fixed stack arrays; no variable-length
scratch allocation or intermediate Float2 mesh is necessary. Reusing the
existing `BufferCreatorFunc` keeps buffer batching and failure behavior with
Metal and D3D11. `none` means unsupported topology with no reservation or draw
command; an engaged zero means allocation failure already handled, and does not
attempt another draw through fallback.

The former default centered path sent the repeated closing points to the ordinary
closed-polyline builder, which closes the ring itself. That could blend parts
of even an ordinary frame repeatedly. Offset strips can also overlap when a
hole disappears or a ring crosses itself. The default centered path now uses Bevel's
non-overlapping mesh or resolved buffer topology. Regression captures require
omitted style and explicit Bevel to match for solid and patterned
fills, and reject any color brighter than one translucent application.

The existing polyline preparation and miter helpers simplify points, split sharp
corners, and work in Float2. Those operations change explicit unlimited-miter
geometry, so the small convex-ring calculation does not use them. Arc quality
and buffer allocation/writing conventions are shared instead.

## Common shapes and width limits

Rotation and winding do not by themselves trigger fallback. The common-shape
regression uses an equilateral triangle (side 200), a 240 by 120 rectangle,
a trapezoid (parallel bases 160/240, height 120), a parallelogram (base 200,
height 120, horizontal shear 40), and a rhombus (diagonals 200/120). For each,
24 rotations, both windings, and widths 0.5/1/2/4/8/12/20/40/80 use the direct
path. The exact coordinates are in the automated test.

| Shape | Width at inset collapse |
| --- | ---: |
| Equilateral triangle | 115.47 |
| Rectangle, trapezoid, parallelogram above | 120 |
| Rhombus above | 102.90 |

For separate widths, inset collapse depends on the **inner** width: half the
centered width in the table. A large outer width does not collapse the hole.
The asymmetric regression combines inner/outer widths 0/20, 20/0, 4/16, 16/4,
and 40/200 with the same rotations and windings and all three styles, without
fallback. Outer-only frames have no inset collapse.

Tests probe just below and above these analytic limits. These are properties of
the specified shapes, not a statistical fallback rate for applications. A much
shorter trapezoid base can vanish before the entire hole closes; its first
collapsed edge is the relevant limit. Thin shapes can therefore reach fallback
at smaller widths.

Bevel's hard 60-degree transition is sensitive to roundoff at equilateral
corners. Direct and Image dot-product thresholds allow 1e-12 on the miter side;
the fallback's angle threshold allows 1e-12 radians. These constant comparisons
stabilize equality after ordinary rotation without adding per-corner arithmetic.
The tolerance bands differ negligibly; exact GPU/Image pixel matching is not
part of the contract. Tests cover rotated equilateral corners and angles on
either side of the threshold.

## Fallback robustness

The buffer input uses a local span view whose Boost strategy selects adaptive
exact orientation predicates (`side_robust` with `fp_equals_policy`). The default
approximate collinearity relation is not transitive and can violate the angular
sort comparator's strict-weak-ordering requirement when overlapping pieces meet.
The regression quad `{{61,93},{22,119},{7,208},{221,152}}`, width 160, Bevel,
reproduces the assertion with uniformly compiled Debug sources and libc++'s
strict-order checks. Exact orientation resolves the assertion without disabling
checks or changing vendor code. The specialized input type confines the strategy
to this GPU fallback; polygon-returning geometry and Image are unaffected.

Before Earcut, an in-place pass removes roundoff-sized collinear kinks left by
buffer intersections. Its area bound is 32 double epsilons times the larger
adjacent squared edge length, with reversals preserved. This is independent of
the exact predicate used for sorting. Keeping those redundant points can produce
sliver triangles and a doubly blended MSAA boundary sample. The cleanup adds no
allocation and does not affect the direct mesh.

The independent regression oracle clips half-planes for the offset edges and
acute-corner bisectors. It checks the filled footprint after the inset disappears,
including cyclic starting points and reversed winding. The isolated Debug scan
also exercises varied, repeated, crossing, and thick three/four-point inputs.
This is bounded evidence, not a proof of robustness for every floating-point input.

## Validation

[Test_Renderer2DJoinStyle.cpp](../../Test/Test_Renderer2DJoinStyle.cpp) covers
index offsets, colors, winding, independent triangle-intersection area checks,
analytic miter area, deterministic varied topology, rejected geometry, buffer
failure, the Bevel threshold, Round radial/chord error under scaling, translucent
pixels, patterns, batch rollover, forced-fallback comparisons, and exact equality
between omitted style and explicit Bevel. Dedicated checks cover inset-collapse
transitions, a quad losing only one inset edge, fallback zoom invariance, and the
wide-Bevel sorting regression.

Two triangulations of the same straight boundary may round differently at an
MSAA sample. The direct/fallback solid comparison allows differences above 1/255 only
within a one-pixel boundary neighborhood in **both** images. Separate overlap
and alpha checks reject duplicated coverage; this tolerance does not permit
interior seams. Round quality is checked against the circle. Transition tests choose widths
away from ordinary arc-quality thresholds, so they isolate the drawing-path
change from an unrelated change in subdivision count.

Run the host workflow from the [development guide](../development/README.md),
using `--test-case=Renderer2D.join_style*` for focused macOS iteration, followed
by the full host suite. The [manual gallery](../../Test/Manual/Renderer2DJoinStyle.md)
owns the interactive program and expected observations. The
[CPU benchmark](../development/renderer2d-join-benchmark.md) owns measurement
conditions and performance results.

## Inner/outer frames and Image integration

[Image inner/outer frames](../geometry2d/image-writing.md#triangle-and-quad-innerouter-frames)
share the default Bevel and the original even-odd interior. Each width selects
its side of a joined stroke measured from the original contour. A zero width
omits that side; negative widths, both zero, or no filled region draw nothing.
Adjacent duplicates and collinear spurs are normalized consistently.

The direct mesh places inset vertices using the inner width and outer corners
using the outer width. The former `stretched()` plus ordinary strip construction
could reverse an inset and draw outside the original region; the regression
uses a 160-square with inner width 200 and outer width zero, which must fill only
the square with one translucent application.

Fallback obtains the original region through the existing polygon ring
correction helper. It intersects the inner joined stroke with that region,
subtracts the region from the outer joined stroke, and unions the pieces before
triangulation. Every resulting Polygon is drawn, including disconnected lobes
of an inward-only crossed Quad. Equal positive widths reuse the centered buffer
once the filled contour has been normalized. This path deliberately owns the
extra topology work; ordinary convex frames do not allocate temporary storage.

The centered frame's degenerate-line behavior remains separate from the
filled-region contract of the inner/outer overload. Image and GPU keep their
own AA and Round tessellation. GPU/Image comparisons require matching interiors
away from the boundary, not identical edge coverage. Direct/fallback GPU
comparisons and half-transparent maximum-color checks remain stricter.

Do not generalize the direct path based on convexity alone or handle thick and
concave rings by overlapping rectangles/fans. They need topology resolution.
Replacement of the fallback needs evidence that these cases are frequent enough
to justify it. Public LineString JoinStyle is a separate design stage. Pending
platform validation and broader workload measurements are tracked in [TODO.md](../../TODO.md).
