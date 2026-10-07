# Spline paths

[Spline2D](../../Siv3D/include/Siv3D/Spline2D.hpp) owns a connected sequence of
cubic Bezier segments. Use it when an application needs a reusable curve, such as
an enemy route, a camera rail, or a path along which objects are placed.
[Spline2DMeasure](../../Siv3D/include/Siv3D/Spline2DMeasure.hpp) adds reusable arc
length measurements for moving along that curve. The public headers define the
input, boundary, and ownership contracts.

## Constructing and querying a path

```cpp
const Array<Vec2> points{
    { 80, 300 }, { 140, 100 }, { 450, 120 }, { 600, 360 }
};
const auto path = Spline2D::FromCatmullRom(points);
const Spline2DMeasure measured{ path };
const Vec2 position = measured.pointAtDistance(120.0);
```

The default centripetal interpolation accounts for uneven distances between
knots. Uniform interpolation is also available through
[CatmullRomParameterization](../../Siv3D/include/Siv3D/CatmullRomParameterization.hpp).
The endpoint tangents of an open path use the first and last edges; switching
interpolation modes does not change this endpoint policy.

For directly controlled handles, build `Bezier3` segments and pass them to
`FromBezierSegments()`. Shared endpoints must match. Each segment remains
accessible through `segment(i)` for tangent, curvature, subcurve, and other Bezier
operations. Connected segments may have corners. Even Catmull-Rom paths do not
in general have continuous curvature, so constant travel speed alone does not
produce continuous acceleration for a camera or vehicle.

A [SplineLocation](../../Siv3D/include/Siv3D/SplineLocation.hpp) identifies a
segment and its local parameter, not a distance. `computeClosestPoint()` returns
a [SplineClosestPoint](../../Siv3D/include/Siv3D/SplineClosestPoint.hpp) containing
this location, its position, and the squared query distance. Pass its location
to `Spline2DMeasure::distanceAt()` to find the corresponding progress along the
path. Self-crossing paths can have multiple closest positions.

## Movement and ownership

Accumulate `speed * Scene::DeltaTime()` in a distance variable and evaluate it
through the same measure object each frame. Different moving objects can keep
their own distances while sharing the measurement. Select
[DistanceMode](../../Siv3D/include/Siv3D/DistanceMode.hpp) explicitly when wrapping
past the endpoints. Closing a path changes its geometry; wrapping changes how
out-of-range distances are interpreted.

The measurement owns a snapshot. Editing or clearing the source path does not
change it. Replace the measurement when the geometry changes:

```cpp
measuredPath = Spline2DMeasure{ editedPath }; // Copy the new geometry and measure it
```

When the original geometry is no longer needed, construction with
`std::move(path)` transfers its segment storage. Copying a measure copies its
geometry and tables; it does not run integration again.

The implementation shares arc length construction and inversion with `Bezier3`.
Each segment starts with 32 measurement intervals, split additionally at extrema
of its coordinate components so that a speed reversal cannot be hidden near a
quadrature interval endpoint. Speed is integrated with adaptively refined
five-point quadrature, and distance inversion uses a bracketed Newton
iteration. Refinement and iteration have fixed limits; the results are numerical
approximations, not certified error bounds. A distance query searches the
precomputed segment lengths and evaluates only the selected segment. It does not
allocate memory or rebuild the tables.

## Drawing and reuse

`path.draw()` is convenient for occasional drawing. For unchanged paths, save
the result of `toLineStringAdaptive()` and draw that instead. Use `drawClosed()`
for a closed path. The destination overload preserves the buffer's capacity:

```cpp
LineString outline;
path.toLineStringAdaptive(outline, 0.5);
// After editing the path, write into the same outline again.
```

The approximation threshold is measured in path coordinates. A large display
scale may require a smaller threshold. At the maximum subdivision depth, a
segment can retain more error than requested.

Rendering subdivisions are separate from arc length measurements. A geometrically
straight cubic can still have a nonlinear parameter-to-distance relationship.
For example, control points `(0,0), (0,0), (0,0), (100,0)` trace a straight segment
with position `x = 100 t^3`. Rendering it needs only two vertices, while constant
speed movement requires distance inversion.

## Validation

[Test_Spline2D.cpp](../../Test/Test_Spline2D.cpp) covers construction, nonuniform
interpolation against an independent evaluator, closure, duplicate knots,
closest points, storage reuse, distance boundaries, ownership, zero-length
segments, and round trips. The Bezier regression tests cover the shared numeric
and flattening operations in [Test_Bezier.cpp](../../Test/Test_Bezier.cpp).

The [interactive path gallery](../../Test/Manual/Spline2D.md) demonstrates
interpolation, nearest-point editing, constant-speed travel, and equal-distance
placement. It requires a graphics application; automated tests validate geometry
in memory.

The opt-in `Spline2DMeasure.Benchmark` test compares 10,000 one-shot Bezier
distance queries with cached path queries producing the same positions. It
excludes measurement construction from the query timings and asserts numerical
agreement without imposing timing thresholds. For example, run
`./macOS/run-tests.sh '--test-case=Spline2DMeasure.Benchmark'` on macOS. Follow the
[development guide](../development/README.md) for the host-specific runner. Debug
timings are useful for spotting repeated work, not for predicting shipped game
performance.
