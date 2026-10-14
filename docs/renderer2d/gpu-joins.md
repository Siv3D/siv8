# Triangle and Quad GPU joins

`Triangle::drawFrame(thickness, fill, JoinStyle)` and the corresponding Quad
operation use a direct mesh for strictly convex rings whose inset edges all
remain forward-facing. Other topology uses the existing polygon buffer and
Earcut path. The inner/outer-thickness overloads, Image painting, polygon buffer
APIs, and ordinary LineString/Bezier/Spline drawing are separate operations.

The public [JoinStyle contract](../../Siv3D/include/Siv3D/JoinStyle.hpp) owns the
meaning of the styles. Bevel preserves the historical 60-degree clipped-miter
shape, including the radius-distance clipping plane. Miter retains unlimited
extension. Both input windings are supported. Round uses the shared Renderer2D
circle-pie quality calculation with raster scale, radius, and corner angle in
the direct path. Tessellation and boundary pixels can differ from the buffer
path. The fallback still uses its original scale-independent subdivision.

## Topology and ownership

[ClosedLineString.cpp](../../Siv3D/src/Siv3D/Polygon/ClosedLineString.cpp) selects
Default before trying the explicit-join builder. Triangle and Quad supply a
stack array, including the two repeated points that the original paths expect.
The explicit builder receives only the distinct ring vertices. Default uses
its original `BuildLineString` implementation and no new topology checks.

The shared
[Vertex2DBuilder](../../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder_Rounded.cpp)
performs these steps:

1. Require a positive width and a strictly convex three- or four-corner ring.
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

The existing polyline preparation and miter helpers simplify points, split sharp
corners, and work in Float2. Those operations change explicit unlimited-miter
geometry, so the small convex-ring calculation does not use them. Arc quality
and buffer allocation/writing conventions are shared instead.

## Validation

[Test_Renderer2DJoinStyle.cpp](../../Test/Test_Renderer2DJoinStyle.cpp) covers
index offsets, colors, winding, independent triangle-intersection area checks,
analytic miter area, deterministic varied topology, rejected geometry, buffer
failure, the Bevel threshold, Round radial/chord error under scaling, translucent
pixels, patterns, batch rollover, and exact fallback/Default comparisons.

Two triangulations of the same straight boundary may round differently at an
MSAA sample. The old/new solid comparison allows differences above 1/255 only
within a one-pixel boundary neighborhood in **both** images. Separate overlap
and alpha checks reject duplicated coverage; this tolerance does not permit
interior seams. Round quality is checked against the circle, rather than treating
old subdivision vertices as the ideal shape.

Run the host workflow from the [development guide](../development/README.md),
using `--test-case=Renderer2D.join_style*` for focused macOS iteration, followed
by the full host suite. The [manual gallery](../../Test/Manual/Renderer2DJoinStyle.md)
owns the interactive program and expected observations. The
[CPU benchmark](../development/renderer2d-join-benchmark.md) owns measurement
conditions and performance results.

## Image-side handoff and further work

Share geometric meaning and orientation conventions if Image gains public
JoinStyle support: round outer corners, unlimited miter intersection, and
Siv3D's clipped tip below 60 degrees. A simple chord bevel would change that
contract. Keep coverage/rasterization and GPU mesh generation independent.
The GPU work does not modify Image stroke code or its private join enum.
Nonpositive widths, duplicate points, and collinear corners continue through
the old GPU fallback. This preserves behavior rather than establishing a new
cross-backend normalization rule; Image's treatment of degenerate inputs should
be agreed separately before sharing a preprocessing helper.

Do not generalize the direct path based on convexity alone or handle thick and
concave rings by overlapping rectangles/fans. They need topology resolution.
The current fallback remains useful; replacement needs evidence that these
cases are frequent enough to justify it. A public LineString JoinStyle API is
a separate design stage. Pending platform validation and fallback quality work
are tracked in [TODO.md](../../TODO.md).
