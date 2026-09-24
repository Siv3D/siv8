# Continuing a ray after contact

Status: investigated; the API extension below is **not adopted or implemented**.
Release scope: a candidate for a future Geometry2D change, outside this
investigation. Pending decisions are tracked in [TODO](../../../TODO.md).

The [Raycast contract](../../../Siv3D/include/Siv3D/Geometry2D/Raycast.hpp)
includes the origin. Consequently, restarting from a hit position is not a
complete reflection or movement algorithm. The
[diagnostic](../../../Test/Manual/RaycastContinuation.md) separates that limitation
from numerical effects and tests the workarounds below.

## Findings

| Situation | Observed result and consequence |
|---|---|
| Reflection at a smooth wall | The next call commonly returns zero or a tiny distance at the same wall. Repeated reflection can alternate directions without consuming the remaining travel distance. |
| Reflection inside a circle or inside a Polygon hole | Discarding the zero-distance result does not reveal the next exit or the opposite wall. Ignoring the entire shape also loses that boundary. |
| Concave Polygon or separated MultiPolygon members | A later hit can belong to the same query target. Excluding the previous target is insufficient, even when the original contact is moving away. |
| Sliding along a straight edge | The ray shares a positive-length interval with the boundary. There is no first strictly positive point of that interval; merely excluding distance zero cannot define the desired movement. |
| Corner or point-touching members | Reflecting against one returned normal can leave a container or enter another member. Resolving a simultaneous contact requires a movement policy beyond selecting one normal. |
| Numerically returned hit positions | Roundoff can produce zero, a tiny positive distance, or no immediate hit on restart. General SuperEllipse clipping can reject a returned point lying just outside its bounding box; oblique straight edges can cease to be exactly collinear. A numerical hit position is not a persistent contact identity. |

The scan covers every supported shape type, concave and holed polygons,
point-touching members, SuperEllipse exponents from 0.25 to 64, inside and outside
origins, oblique directions, and independent changes of size and world position.
It is a diagnostic sample, not an exhaustive proof. Large translations combined
with tiny shapes deliberately stress representable coordinate precision; those
results should not be conflated with ordinary scene failures.

## Workaround comparison

| Approach | Useful for | Limitation |
|---|---|---|
| Reject a returned zero-distance hit | Ending a query that only needs the first contact | Does not search the remaining boundaries of that target. A tiny positive re-hit also evades an exact-zero check. |
| Move along the outgoing ray | An application with a known minimum feature size | A fixed step can jump over a thin wall. Tangential steps can remain in a curved boundary's contact band. Subtract the step from the remaining travel budget and never step beyond that budget. |
| Move along the normal | Separating from a locally smooth surface | Choose the allowed side: a container interior needs the opposite sign from a solid obstacle's exterior. The displaced ray follows a different path. Corners and nearby geometry still need handling. |
| Move one representable coordinate step | Producing a different stored position | Often remains in the contact tolerance and returns zero again. |
| Ignore the previous shape | Separate convex obstacles after an outward reflection | Misses holes, concave re-entry, and other members of the same MultiPolygon. |

No single fixed or scale-adjusted displacement solves all these cases. An
iteration cap prevents a frame from hanging, but the application must also choose
what happens to unused movement; the cap does not make the path correct.

## Candidate: an explicit lower distance bound

Preserve the existing overloads and consider an additional form that searches
`[minDistance, maxDistance]`, with distances measured from the original origin.
A positive lower bound deliberately excludes a caller-chosen near-origin interval.
It can retrieve another boundary of the same target without relocating the ray
or collecting all intersections. It still misses geometry inside the excluded
interval and does not supply corner or sliding response.

A temporary prototype continued candidate enumeration after rejected near hits.
It found the opposite hole wall, the next circle/ellipse/SuperEllipse exit, and a
nearby separate member, while retaining the original maximum-distance endpoint.
Away from contact bands its hit presence and distance agreed with an equivalently
shifted existing query across straight and curved shapes. This supports feasibility;
it does not establish a final numerical contract or a speed guarantee.

The implementation boundary is
[SivGeometry2D_Raycast.cpp](../../../Siv3D/src/Siv3D/Geometry2D/SivGeometry2D_Raycast.cpp):
filter candidates in the common accumulator, continue circle/ellipse enumeration
when an origin hit is excluded, and intersect collinear overlap intervals with the
requested range. General SuperEllipse enumeration can continue through the same
visitor. No new triangulation, intersection array, or geometry cache is required.
The public header, existing Raycast tests, and the diagnostic would change with it.

Before adoption, decide lower-endpoint tolerance, invalid/empty ranges, and the
meaning of `startsInside` when the lower bound lies inside the target. Keeping it
about the original origin is consistent with its current name. Retain boundary
intervals as hits; treating them as passable belongs to an explicit movement
policy. Compare Release cost for ordinary queries and continuation queries,
including tangencies and nonconvex curves. Keep the existing call path free of
new lower-bound work where practical, and measure rather than assume this cost.

A full contact manifold, general collision response, or an all-hits API is not
needed to evaluate this smaller extension. Corner and sliding examples should
state their chosen response and travel-budget rules separately.
