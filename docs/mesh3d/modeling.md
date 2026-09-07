# Mesh3D modeling guide

Use Mesh3D factories for individual shapes, Mesh3DBuilder for direct composition,
and Mesh3DAssembly for named parts sharing shapes and materials. Finish a Builder
with `obtainMesh()`; it transfers storage and leaves an empty reusable Builder.
`getMesh()` borrows the current mesh, while `clear()` discards content and keeps
capacity. API contracts live in the [public headers](../../Siv3D/include/Siv3D/Mesh3D.hpp).

Assembly and OBJ usage is described in [assembly and export](assembly.md).
Changing sections is described in [Loft](loft.md). Use the [preview tool](preview.md)
for geometric inspection; it is not a final renderer.

## Transform argument types

Translation takes Vec3; uniform scale takes double and per-axis scale takes Vec3,
matching shape dimensions and Mesh3DPlacement. Arguments are converted once to
float/Float3 in the mutating implementation. Vertex storage and calculations
remain float; accepting Vec3 does not increase geometric precision. Zero/sign
behavior for scale is determined after conversion. Quaternion and Mat4x4 inputs
remain unchanged. See the public header for the transform contracts.

```cpp
const Vec3 offset{ 1.5, 0.25, -2.0 };
Mesh3D mesh = Mesh3D::Box().scaled(1.25).translated(offset);
mesh.scale(Vec3{ 2.0, 1.0, 0.5 }).translate(Vec3{ 0.0, 0.1, 0.0 });
```

## Existing API inventory

- `Circle::outer()` already produces a circular point array.
- `Circle::pieAsPolygon()` and `arcAsPolygon()` already produce sectors and thick
  arcs. Full turns produce a disk or a ring with a hole. These are Polygon
  outputs; no new sector generator is required for Extrude.
- Circle angle zero is screen-up; positive angles are screen-clockwise. For a
  contour written as `(r*cos(t), r*sin(t))`, the corresponding Circle start
  angle is `t + Pi/2`. Extrude/Loft then map `(x,y)` to local `(x,0,-y)`.
- `PointsPerCircle` describes the full circle, not the number of segments in a
  partial arc. `QualityFactor::toPointsPerCircle(radius)` is a radius-based
  quality heuristic, not a maximum geometric-error guarantee.
- The current Circle generators use FastMath::SinCos. Replacing a recipe's
  std::sin/std::cos sampling may change point positions, ordering, or count.
  Use them where their contract is sufficient; do not promise byte-identical
  replacement of an independently sampled contour.
- `Quaternion::FromUnitVectorPairs()` already expresses frame orientation;
  `SweepOptions::initialXAxis` already controls the initial section axis.

## Attachment frames returned by recipes

Frames are returned alongside the geometry and derived from the same inputs.
Column/beam mating logic contains no duplicate height/depth arithmetic. A
transformed parent exercises the local/world distinction and nonuniform scaling.
Four probe points test each frame's origin and axes, not only its translation.

The pipe recipe is deliberately a planar XY sweep. Its specified section X axis
is +Z, which stays constant under transport along that plane. Endpoint directions
come from the sampled endpoint segments, not the ideal arc tangents. The extension
uses the same 13-sided contour; explicit section orientation avoids relying on
automatically chosen Tube frames or a polygon's rotational symmetry. The test
compares both caps' transformed vertex sets, selected by cap plane and normal.
For these matching convex contours this checks the actual polygonal join as well
as frame alignment. It is not a general intersection or watertight-union test.

These examples support keeping attachment data in ordinary recipe structs.
They do not justify a universal AnchorID, frame inference from bounds, automatic
constraint evaluation, or a new axis-frame factory. Spatial curves would need
an explicit transport policy or returned frames; do not generalize the planar
recipe's constant +Z axis to them.

For complete executable recipes and their expected results, see the
[attachment and surface manual test](../../Test/Manual/Mesh3DModelingStudies.md).
The open-grid prototype's design status is recorded [separately](proposals/open-grid-surface.md).
