# Loft: contours and frames

Loft connects corresponding points on successive positioned contours. The
[manual test](../../Test/Manual/Mesh3DLoftExamples.md) contains a complete changing
profile duct example and its validation steps. Exact preconditions belong to the
[Mesh3D header](../../Siv3D/include/Siv3D/Mesh3D.hpp).

## Migrating from height-based Loft

Replace parallel contour/height arrays with LoftSection entries. A former height h
becomes `frame = Vec3{0, h, 0}`. Keep the referenced contour arrays alive throughout
the call. LoftSection is a non-owning view and can share a contour between sections.
UV arguments move into LoftOptions. Builder placement precedes the options argument.

The new API keeps the local mapping `(x, y) -> (x, 0, -y)`. Frame origin distances
supply side V; they are not necessarily distances measured on the generated surface.
Frames must be finite affine transforms with positive determinants. Between adjacent
sections, the origin displacement must have positive projection on both section
plane normals. Add intermediate sections to follow a bend.

Whole-mesh reflection preserves facing through Mesh3D transforms, placed Builder
additions, and Assembly baking. Do not apply reverseWinding() as a correction
after a reflection; use invert() when intentionally turning a surface inside out.
The per-section frame orientation requirements above still apply.

The generator connects matching vertex indices without automatic resampling,
reversal or correspondence search. It rejects local degeneracy and locally folded
triangles but does not detect every contour or surface self-intersection. Requested
caps are triangulated in 2D; cap-free generation avoids that work. See the public
Doxygen for exact normal, tangent, UV and failure contracts.

## Historical generation-cost comparison

These are development measurements for the migration, not a current performance baseline.

A local macOS arm64 Release comparison used height-only circular contours, both
caps, smoothingAngle = 0, and reusable destination vertex/index capacity. Each
result is the median of five trials of 100 generations, alternating old/new order.
The old generator was compiled alongside the new one for this development check;
that temporary comparison code is not part of the library or automated suite.

| Sections × contour vertices | Previous generator | Frame-based generator |
| --- | ---: | ---: |
| 8 × 16 | 11.3 µs | 13.6 µs |
| 32 × 32 | 47.5 µs | 64.7 µs |
| 64 × 64 | 119.7 µs | 185.1 µs |

The new generator includes section placement and local triangle checks. It avoids
extra tangent normalization for hard contours and skips cap triangulation when
caps are disabled. These local measurements describe this workload; they do not
predict every model's cost. Generate static models once and reuse their MeshIDs.

