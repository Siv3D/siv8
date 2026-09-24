# Geometry2D

API contracts live in the public headers, including
[Raycast](../../Siv3D/include/Siv3D/Geometry2D/Raycast.hpp),
[distance queries](../../Siv3D/include/Siv3D/Geometry2D/Distance.hpp), and
[boundary distance queries](../../Siv3D/include/Siv3D/Geometry2D/SignedDistance.hpp).

## Continuing a ray after contact

Use `Raycast(ray, shape, minDistance, maxDistance)` to continue searching the same
target beyond a previous contact. Distances remain relative to the original ray
origin. Choose the excluded distance from the application's coordinate scale and
smallest relevant feature; there is no universal safe offset.

The [continuation diagnostic](../../Test/Manual/RaycastContinuation.md) compares
this form with unchanged origins, displaced origins, and one-representable-step
moves. It covers reflections, holes, concave shapes, separated and point-touching
members, sliding, and changes of numerical scale. Automated boundary and range
cases are in [Test_Geometry2D_Raycast.cpp](../../Test/Test_Geometry2D_Raycast.cpp).

A positive lower bound lets a reflected ray reach another boundary of the same
Polygon or MultiPolygon, but also excludes other geometry in that interval.
Discarding the first result or ignoring the entire target cannot retrieve those
later boundaries. Unlike displacing the origin, a lower bound does not relocate
the ray or change the distance budget. Charge each returned distance to the
remaining travel budget, and bound the number of contacts processed per update.

Range selection is not a collision-response policy. Sliding along a straight edge
still intersects an interval of that edge. At corners or point-touching members,
reflecting against one returned normal can leave a container or enter another
member. Applications must choose their allowed movement region and response to
simultaneous contacts; an iteration cap alone does not resolve them.

Numerical hit positions can lie slightly off a boundary, so restarting may yield
zero, a tiny positive distance, or no immediate hit. Normal displacement requires
choosing the allowed side (container interior versus solid obstacle exterior),
and a tangent displacement can remain in a curve's contact band. See the diagnostic
for examples rather than treating one displacement size as a general solution.

## Reflection and sliding

The [reflection and sliding example](../../Test/Manual/RaycastResponse.md) moves a
point among stationary axis-aligned rectangular walls. It demonstrates corner
reflection, tangential motion, point-touching obstacles, thin walls, and a bounded
contact loop. It is separate from the numerical query diagnostic above.

For this movement policy, query from distance zero and use the known rectangle
faces to decide whether the point would enter a wall. Collect all simultaneous
blocking faces before changing velocity: reflect every blocked component, or
remove it for sliding. Departing and tangent contacts with a convex rectangle can
be skipped for the current straight segment. Recheck all walls after changing
direction so a different wall at the same position remains visible.

The sample accounts for elapsed time at every hit, keeps the last contact position
when the iteration budget is exhausted, and exposes unused time. Its rectangle-specific contact logic
and scene tolerance should not be copied unchanged to arbitrary polygons, curved
surfaces, or bodies with radius. The manual defines those limits and the chosen
behavior at a blocked touching vertex.
