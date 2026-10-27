# Geometry2D

API contracts live in the public headers, including
[Raycast](../../Siv3D/include/Siv3D/Geometry2D/Raycast.hpp),
[distance queries](../../Siv3D/include/Siv3D/Geometry2D/Distance.hpp), and
[boundary distance queries](../../Siv3D/include/Siv3D/Geometry2D/SignedDistance.hpp).

## Image writing

[Writing 2D shapes to Image](image-writing.md) covers compositing, the CPU/GPU
API inventory, validation, and reproducible performance measurements.

## Shape drawing proposals

[Shape drawing API proposals](proposals/shape-drawing-api.md) evaluates additions
to the 2D drawing API, with SVG galleries, existing API comparisons, external
examples, and staged implementation options. Remaining candidates are unadopted;
implemented rotated ellipses and elliptical arcs link to the Renderer2D guide,
Bezier and circle arc arrows link to the curve drawing guide, and independent-corner rectangles
and rounded speech balloons link to the rounded drawing guide.

## Dashed curves, paths and arcs

[Dashed drawing](dashed-drawing.md) covers `draw(LineStyle, ...)` and Circle/Ellipse
`drawArc(LineStyle, ...)`, `RoundRect::drawFrame(LineStyle, ...)`, fitted closed
contours, phase settings, and the
accepted corner/overlap limits. It links the
public-API manual program, tests, and submission benchmark.

## Spline paths

[Spline paths](splines.md) explains reusable curves, distance-based movement,
ownership, drawing buffers, and the interactive gallery for `Spline2D` and
`Spline2DMeasure`.

## Bezier drawing

[Curve drawing](bezier-drawing.md) explains single-ended and double-headed arrows for Bezier, circle arcs, LineString,
and Spline2D, quality units, segment
budgets, transform handling, and the density placement shared by Bezier2,
Bezier3, Spline2D, and Bezier Image writing. It
records the comparison with recursive subdivision and Wang's formula and links
to the reproducible renderer gallery and benchmarks.

## Rounded shape drawing

[Drawing tessellation budgets](rounded-drawing.md) compares circle, ellipse,
rounded-rectangle, cap, join, texture, and shadow paths against Circle's bounded
drawing cost. It distinguishes GPU subdivision from arc-length calculations,
records short-dash findings, and explains SuperEllipse's private tessellator.
It also covers [direct independent-corner fills and frames](rounded-drawing.md#independent-corner-rectangles)
and [rounded speech balloons](rounded-drawing.md#rounded-speech-balloons).

## Crescents, lenses and teardrops

[Circular shape drawing](circular-shapes.md) covers direct Circle fills, stored
Shape2D geometry including symmetric lenses, Polygon conversion, shared construction
and the visual gallery.

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
