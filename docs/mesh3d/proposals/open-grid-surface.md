# Open grid surface study

Status: sample-only experiment; a public Surface API is deferred beyond v0.8.
This is a design record, not an adopted API contract or a committed release feature.

The complete prototype, fixtures, checks, and execution steps remain in the
[manual test](../../../Test/Manual/Mesh3DModelingStudies.md). Ordinary attachment
recipes are documented in the [modeling guide](../modeling.md).

## What the Surface prototype establishes

The same small triangulation function supports a two-row skirt and a doubly
curved cloth patch. Columns are u, rows are v, and the front side follows
`cross(dP/du, dP/dv)`. Every cell uses the same diagonal (b,c); positions and
normalized grid UVs are supplied before existing normal/tangent computation.

The input precondition is a small finite grid with nondegenerate, consistently
oriented triangles. Only the minimum dimensions are checked by the generator.
The separate checker verifies the known fixtures' counts, boundary incidence,
shared-edge directions, winding, and normal/tangent frames. A 2x2 plane checks
+Y orientation and the 1x2 case must fail. No exceptional-input guarantees are
being proposed for a public generator.

The experiment demonstrates a reusable open-grid core, but not enough evidence
to choose a public Surface contract. Reusable destination storage, diagonal
selection, UV parameterization, hard edges, and the relationship to HeightField
need a separate design review. Closed seams, poles, arbitrary topology, and
self-intersection repair remain outside this prototype.

