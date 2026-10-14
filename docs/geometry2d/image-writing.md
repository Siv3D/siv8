# Writing 2D shapes to Image

Shape methods delegate rasterization to [ImageDraw](../../Siv3D/include/Siv3D/ImageDraw.hpp)
and compositing to [ImagePixel](../../Siv3D/include/Siv3D/ImagePixel.hpp).
Keep geometry, coverage, and color compositing separate: a stroke must combine
its segments, joins, and caps before applying color, or translucent intersections
become darker. `Image::paint()` is a separate image-to-image path; its historical
alpha behavior is not the contract of shape painting.

## Compositing and coordinates

`paint()` uses straight-alpha Source Over and updates destination alpha.
`overwrite()` replaces RGBA at full coverage and interpolates each stored RGBA
channel at partial coverage. Transparent paint leaves the destination unchanged;
transparent overwrite can erase it. RGB arithmetic uses stored byte values.

Integer rectangles address half-open pixel ranges. `Point` addresses one pixel;
`Vec2` and `Float2` cover a one-pixel square centered at the given coordinate,
which can affect adjacent pixels. `RectF` always uses fractional edge coverage.
Other shape methods expose an AA switch. The non-AA Circle fill retains its
integer raster: center and radius are rounded to the nearest integer, with ties
toward positive infinity. Circle frames with an inner hole and partial sweeps
use pixel centers; a frame whose inner circle disappears reuses the fill path.
A full pie, segment, or arc also reuses the corresponding fill or frame. Round line caps
use pixel centers even for zero-length lines. The integer fill path is retained
for its lower cost on small non-AA circles; coordinate uniformity alone does not
justify replacing it with per-row square roots.

Distance coverage and sample-based coverage are different approximations.
Triangle and Quad image fills use even-odd contour coverage, including concave
and crossed Quads. CPU output and GPU `.draw()` output are not required to be
pixel-identical. Image-writing throughput takes priority over
matching GPU tessellation, sampling, or blending details. Use the GPU comparison
to find missing operations and visible defects, not to require identical pixels.

`ImageDraw::DstAlpha::AssumeOpaque` is an explicit optimization for images whose
alpha is already 255. Shape methods use the general destination-alpha path.

## API inventory and GPU differences

The table lists image-writing families; both `paint` and `overwrite` forms exist
unless indicated. GPU-only features are candidates for separate API work, not
implicit promises of this CPU path. Pending decisions belong in [TODO](../../TODO.md).

| Shape | Image families | Additional GPU drawing features / differences |
| --- | --- | --- |
| Point, Vec2, Float2 | Pixel / centered square | No direct `.draw()` family |
| Rect, RectF | Fill, inner/outer frame | Dashed frame, shadow, gradients, patterns; Rect image frame has integer and floating thickness forms |
| Circle | Fill, frame, pie, arc (Flat / Square / Round), segment (height / angles) | Dashed frame, shadow, gradients, patterns |
| Ellipse | Fill, frame, pie | Dashed frame, gradients, patterns; GPU pie uses absolute radii |
| SuperEllipse | Fill, centered frame | Gradients, patterns |
| Triangle, Quad | Fill, frame | Vertex colors, patterns, selectable joins and inner/outer frames; image frames use centered round joins |
| RoundRect | Fill, frame | Dashed frame, shadow, gradients, patterns |
| Line | Stroke with independent end caps, arrow with tail cap, double-headed arrow | Gradients and line styles |
| LineString | Open stroke with independent end caps, closed stroke | Vertex colors, gradients, patterns, vertex markers |
| Bezier2, Bezier3 | Adaptive stroke with independent end caps and quality/budget controls | Fixed-subdivision `draw()`; adaptive GPU quality includes renderer transforms and render-target resolution |
| Polygon, MultiPolygon | Fill, frame, offsets | Transformed drawing, wireframe, patterns |
| Shape2D | Fill, frame, offsets | Wireframe, patterns; CPU uses the vertex contour with even-odd filling; GPU fill uses triangle indices |
| Spline2D | Adaptive open / closed stroke, independent open end caps, per-curve quality/budget controls | GPU quality includes renderer transforms and render-target resolution; open GPU paths use Square caps |

MultiPolygon draws members in order, so overlapping members compose repeatedly.
Polygon frames combine the outer contour and all holes using maximum stroke
coverage, then composite once. This uses the same round-join approximation as
LineString. Contour construction reuses scratch storage across holes. AA strokes
calculate each primitive's span once per row and retain its association with that primitive.
Only primitives in the same merged interval contribute to its coverage buffer;
disjoint contours do not repeatedly scan one another's bounds.

Shape2D image methods interpret vertices in order as a closed contour and do not
use triangle indices. The same vertex contour is used by its frame and
`mouseOver()`. Fills use the same 4x4 samples with AA and pixel-center samples
without AA as the polygon contour path. Arbitrary indexed meshes can therefore
draw a different region
with `.draw()`. Triangle-union rasterization is separate API work; it is not added
to the common contour path solely for GPU parity. Shape2D passes Float2 vertices
to the common rasterizer directly; offset application does not require a mapped
Vec2 array. Internal stroke construction still owns reusable working storage.

## Open stroke caps

`Line`, `LineString`, `Bezier2`, `Bezier3`, and open `Spline2D` paths accept
independent caps after the image argument:

```cpp
line.paint(image, LineCap::Flat, LineCap::Round, 8.0, Color{ 220, 60, 130, 113 });
path.overwrite(image, LineCap::Square, LineCap::Flat, 8.0, Palette::White);
```

A single cap argument still applies to both ends, and omitting it retains Round
ends for Image writing. Square extends the endpoint by half the stroke width;
Round adds a rounded end. The caps follow point order, so reversing a path and
swapping its caps preserves the region. Adjacent duplicate points do not create
extra ends. Open LineString joins remain round, and coverage is combined before
compositing. Bezier Image writing uses the shared [density tessellation](bezier-drawing.md),
with optional `targetError` and `maxSegments` after the antialiasing argument.
Defaults are 0.5 Image pixels and 256 segments. The positive finite target is
approximate, and the segment cap takes priority. Renderer transforms do not
affect Image quality. GPU `drawAdaptive()` defaults to Square caps, while Image
writing defaults to Round; specify caps explicitly when comparing the two.

```cpp
curve.paint(image, 6, Palette::White, EnableAntialiasing::Yes, 0.25, 256);
curve.overwrite(image, 6, Palette::White, EnableAntialiasing::No, 0.75, 128);
```

A zero-length Line, or an open LineString of at least two coincident points,
uses the union of the two cap regions: Square yields a centered square, otherwise
Round yields a disk, and two Flat caps draw nothing. Empty and one-point
LineStrings draw nothing. A Bezier or Spline whose control points all coincide also draws
nothing because adaptive conversion produces one point. These collapsed cases
have no tangent direction.

The straight-line rasterizer extends the body separately at each Square end and
adds only the requested Round disks. On a short Round/Flat line, the disk is
clipped at the opposite Flat end so it cannot extend past that endpoint. This
clipping is restricted to mixed caps whose support can reach the opposite end.
After removing adjacent duplicate points, a two-point LineString with different
caps reuses the Line rasterizer. Longer paths retain the common stroke union;
the neighboring round join already covers the inward part of an endpoint disk.

## Reusing sampled curves

Bezier and Spline adaptive conversion, GPU drawing, and Image writing share
the [curve tessellation kernel](bezier-drawing.md). The Image methods pass the
sampled path to the common stroke rasterizer.
Fixed-subdivision `draw()` remains a separate GPU family. Image callers needing
uniform parameter subdivisions can use `toLineString(subdivisions)` and paint
that result.

Spline2D directly writes its whole path with automatic closure from `isClosed()`.
Open paths default to Round caps; a closed path joins its seam and ignores caps.
The quality arguments follow the AA switch, with defaults of 0.5 Image pixels and
256 segments **per Bezier3 interval**. They are independent of renderer state.

```cpp
path.paint(image, 6.0, color);
path.overwrite(image, LineCap::Round, LineCap::Flat, 6.0, color,
    EnableAntialiasing::Yes, 0.25, 128);
```

A budget of one keeps only interval endpoints, including internal knots.
After sampling, fewer than two points in an open path or three in a closed path
draw nothing. This includes an all-coincident path and a closed straight
out-and-back path reduced to two points.

For repeated drawing, sample explicitly and retain the result:

```cpp
LineString outline;
// Retain outline across updates to reuse its allocation.
path.toLineStringAdaptive(outline, 0.5, 256);
if (path.isClosed())
{
    outline.paintClosed(image, 6.0, color);
}
else
{
    outline.paint(image, LineCap::Round, LineCap::Flat, 6.0, color);
}
```

Convert and draw the connected path as a whole. Painting each cubic separately
adds caps at internal knots and composites overlaps repeatedly. The closed
conversion omits the repeated endpoint, so use `paintClosed()` / `overwriteClosed()`
to retain its closing segment.

For unchanged geometry, keep the sampled LineString and skip conversion on
subsequent draws. Reusing conversion output does not reuse all rasterizer storage:
ImageDraw still prepares effective points, stroke primitives, and coverage work.
Measure these stages separately before adding a cache or a public drawing context.
Fewer tessellation points alone do not guarantee a faster whole draw; the
[curve integration measurements](bezier-drawing.md#spline-and-image-integration-costs)
include the tradeoff for many short cubic intervals. Pending Image API and
performance work is ordered in [TODO](../../TODO.md#2d-図形の-image-書き込み).

## Arrow tail caps

`Line::paintArrow()` / `overwriteArrow()` accept a `LineCap` after the image to
select the tail shape. Calls without it retain Flat tails.

```cpp
line.paintArrow(image, LineCap::Round, 8.0, SizeF{ 12, 20 }, color);
```

`headSize` specifies the width added to the shaft and the length of the head.
When the head would be longer than the line, both components scale down by the
same ratio. Square extends the tail by half the shaft width. Round adds only the
outward semicircle, preserving the head even when the arrow is shorter than the
cap radius. Coincident endpoints draw nothing for every Image cap; GPU Round
arrows instead draw a disk in this case.

Flat retains the existing Shape2D rasterizer. The other caps use fixed-size
contour and interval storage: Square extends the two tail vertices, and Round
unions the contour spans with analytic semicircle spans before compositing.
No-AA uses pixel centers without temporary heap storage; AA uses one reusable
4x4 sample-count row. Only touched pixel spans are converted and cleared. The
new cap paths use double geometry while the legacy Flat contour uses Float2;
boundary ties can differ when switching caps.

## Circle frames

AA frames split each row into the untouched inner hole, the fully covered ring,
and the inner/outer edge bands. Solid spans use the regular bulk color writer;
only edge spans need pixel-center distance evaluation. One coverage buffer is
reused across the partial spans, without clearing or scanning the hole. Tiny
frames instead evaluate the whole outer span, avoiding subdivision overhead.
Inner and outer coverage are rounded to bytes independently before subtraction,
including frames thinner than one pixel whose AA bands overlap. Frames with no
inner hole retain the ordinary Circle fill path. Full arcs reuse this frame path.

## Circle arc caps

`paintArc()` / `overwriteArc()` accept `LineCap` after the destination image.
The overloads without a cap retain Flat ends. Partial arcs use one analytic
span rasterizer: it clips disk/annulus cross-sections to the angular sector,
then unions their sample-index intervals with both caps before compositing.
Round caps use disks; Square caps use rectangles oriented along the endpoint
tangents. Fixed storage holds at most six intervals per sample row. No Polygon
or sampled LineString is constructed. AA uses a reusable 4x4 sample-count row,
converting, writing, and clearing only the merged pixel intervals touched by
those samples. This skips the empty interior of thin arcs. No-AA writes merged
pixel-center spans directly.

Zero-angle Round and Square arcs draw the cap alone, as zero-length lines do;
Flat draws nothing. Full sweeps keep the existing Circle frame path, including
its distance AA and integer fill when the inner hole disappears. Cap width uses
the visible radial band: once the inner radius reaches zero, that width is the
outer radius. Overlapping caps, including on almost-full arcs, composite once.
Sample intervals include radial, circular, and cap boundaries, subject to
floating-point boundary ties.

GPU Circle arcs also default to Flat. Square caps share the arc's indexed strip:
only the Square specialization adds endpoint pairs and extends the gradient over
the caps. The buffer is reserved once for the whole shape. Zero-angle Square arcs
form a square; full Square sweeps use the Flat strip without caps. The shared
vertex builder serves both Metal and D3D11.

GPU comparisons have additional differences beyond AA. GPU Circle arcs retain the
specified thickness when the inner radius passes the center, and overlapping
GPU caps may composite repeatedly. Image writing uses the visible radial width
and union coverage above. GPU Round arcs retain their caps at full sweeps.

## Circle segments

`paintSegment()` / `overwriteSegment()` specify a circular segment by its arc
midpoint direction and height from the chord to the arc. Their `FromAngles`
forms specify the arc endpoints through a start angle and signed sweep. Both
forms use the same rasterizer. Angle conversion uses a squared sine for height
to avoid cancellation in small segments. Full segments delegate to the existing
Circle fill, retaining its integer no-AA raster and distance-based AA.

Partial segments intersect analytic disk cross-sections with the chord's
half-plane. The bounds include chord endpoints and the retained cardinal
extrema. They use pixel centers without AA and 4x4 samples with AA, including
boundary samples, and reuse one sample-count row. Geometry is resolved before
compositing, so a translucent segment writes each covered pixel once. No-AA
writes spans directly without allocating a coverage row. The AA switch selects
a specialization before the scan loop, removing sample-count arithmetic and AA
branches from the no-AA path. Floating-point ties
can differ at boundaries; partial and full shapes use different AA approximations.

Both Image forms require a positive radius, consistent with Image Circle pies.
GPU `drawSegmentFromAngles()` uses the radius magnitude, while GPU height-based
`drawSegment()` also requires a positive radius.

## Ellipse pies

`paintPie()` and `overwritePie()` use the direction of rays from the center,
with zero upward and positive angles clockwise. These are geometric angles,
not the parameter of `Ellipse::getPointByAngle()`. Negative sweeps reverse the
direction. As with existing Ellipse image fills, both radii must be positive;
`drawPie()` instead uses their magnitudes. A sweep of at least one revolution
reuses the regular Image fill, including its AA and compositing behavior.

Partial pies intersect each analytic ellipse cross-section with the two radial
half-planes. Minor sectors intersect the cuts; major sectors union their spans
before writing, so overlapping regions never repeat color compositing. No-AA
uses pixel centers, and AA uses 4x4 sample counts with one reusable row. The
sampled region includes the ellipse and radial boundaries. Boundary ties can
still differ through floating-point rounding. Full sweeps retain the regular
fill's distance AA, so partial and full sweeps use different edge approximations,
as Circle pies already do. No Polygon is built and no ellipse distance is
solved per pixel for partial sweeps.

## SuperEllipse

The fill uses analytic horizontal cross-sections, including concave exponents.
It evaluates powers per sample row and reuses one coverage row, without creating
a Polygon or evaluating powers at every pixel. Exponent 2 delegates to Ellipse;
other exponents use 4x4 samples with AA and pixel-center samples without AA.
For these cross-sections, sample intervals include the left boundary and exclude
the right boundary.

Frames approximate the boundary with a closed round-join stroke. They combine
coverage before compositing, including overlapping parts of concave or thick
frames. The implementation subdivides a quadrant at the point of maximum chord
deviation (a 1/8-pixel target, bounded to 16 levels), then reflects the points
across both axes. This keeps cardinal points exact and avoids repeated powers
in the other quadrants. Exponent 1 produces the four diamond vertices directly.
The existing ClosedLineString path owns clipping, AA, and color compositing.
Its AA combines rectangle/disk coverage at joins; the chord-error target alone
is not a bound on alpha error relative to exact boundary-distance coverage.
Unlike the fill, exponent 2 uses this stroke approximation too; it does not
invoke the more expensive per-pixel Ellipse boundary-distance solver.

## Validation and measurement

[Test_ImageDraw.cpp](../../Test/Test_ImageDraw.cpp) owns the automated shape
matrix, independent compositing and sampling references, and opt-in benchmarks.
[Curve integration tests](../../Test/Test_CurveAdaptive.cpp) cover Bezier Image
quality, segment budgets, caps, default overloads, and renderer-state independence.
[Spline Image tests](../../Test/Test_Spline2DImage.cpp) cover open/closed paths,
independent caps, per-curve budgets, collapsed intervals, reversal, clipping,
invalid arguments, and one-time compositing at knots, seams, and intersections.
Keep conversion-specific checks in the curve tests and common raster/compositing
checks in ImageDraw.
The [manual gallery](../../Test/Manual/ImageWriting.md) compares CPU and GPU
appearance and exposes alpha as grayscale. Automated coverage comparisons do not
replace this visual check.
[GPU Ellipse pie tests](../../Test/Test_Renderer2DEllipsePie.cpp) capture full
sweeps in both directions, including multiple revolutions and several start
angles, for solid, radial-gradient, and pattern drawing. Full sweeps reuse
regular Ellipse drawing before converting partial-pie endpoint directions.

Run from the repository root on macOS, outside the sandbox:

```sh
./macOS/run-tests.sh '--test-case=~[benchmark]ImageDraw.*'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=ImageDraw.benchmark*'
./macOS/run-tests.sh '--test-case=~[benchmark]Spline2DImage.*'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Spline2DImage.Benchmark'
./macOS/run-tests.sh
```

On Windows, use the [documented Windows runner](../development/README.md#build-and-test)
with the same test filters; do not infer Windows performance from macOS timings.
The implementation shares scanline intersections across each AA sample row for
polygons. Their shared sample accumulator handles partial endpoint pixels
separately from the contiguous interior, allowing the latter to be vectorized.
Ellipse pies, Circle segments, and SuperEllipse fills use the same accumulator.
Ellipse fill classifies pixels with a bound on normalized radius before
using the canonical geometry boundary-distance solver near the edge. These
choices avoid per-pixel edge traversal and interior distance solving.

`Spline2DImage.Benchmark` separates reusable conversion, the direct Image API,
explicit conversion followed by LineString drawing, and drawing a cached outline.
It covers few/many short intervals, open/closed paths, 64/256-pixel images,
thin/wide strokes, both AA modes, and paint/overwrite. Direct and explicit calls
include tessellation and its temporary output; cached drawing excludes it but
still includes rasterizer working storage. Path construction and destination
allocation are outside timing. Compare per-case timings; the point count alone
does not describe the cost of a long path.

Benchmarks reuse allocated destination images and prebuilt geometry. They include
public API rasterization, compositing, and internal temporary allocations, but
exclude image allocation, geometry construction, and validation. The smaller
circle benchmark separates inside-image and clipped calls. The stroke-cap benchmark
covers Line, LineString, Bezier2, and Bezier3 with each shared cap, thin/wide
strokes, both AA modes, and paint/overwrite. It measures existing single-cap
calls to check the cost of supporting independent ends. The arrow-cap benchmark
covers default calls and explicit Flat / Round / Square tails, image sizes,
thin/wide shafts, AA modes, and paint/overwrite. It includes internal arrow
construction, with the previous Shape2D call as a reference in the same run;
use the default cases for before/after comparisons. The AA Circle frame
benchmark covers subpixel, thin, and wide rings at small and large sizes, with
opaque/translucent source colors and paint/overwrite. It includes clipped rings
and images entirely inside the hole, where no pixel should change. Polygon frame
cases include single contours and overlapping outer/hole frames. A separate hole-count
benchmark covers thin and wide frames with 1, 16, and 64 holes; its sequential
contour calls are a cost reference and intentionally composite overlaps more than
once. Use both sparse and overlapping contours when checking stroke changes.
The SuperEllipse frame benchmark includes contour generation inside each timed
call. It compares adaptive quadrants with `outer(QualityFactor{ 1.0 })` followed
by the same stroke path, across exponents, thin/wide strokes, AA modes, and image
sizes. These contours have different approximation errors; compare correctness
as well as elapsed time.
The Ellipse pie benchmark compares analytic spans with prebuilt, tessellated
Polygon pies using geometric ray angles. Polygon construction is excluded and
its approximation is not pixel-identical to the analytic region. It covers
narrow and major sectors and both moderate and thin aspect ratios; the smallest
narrow pies may miss all pixel-center or AA samples.
The Circle segment benchmark compares the analytic path with prebuilt Polygon
segments across shallow and major caps, axis-aligned and oblique chords, AA
modes, and image sizes. Polygon construction is excluded; tessellated contours
have a different approximation error. Large no-AA overwrite cases can favor
the Polygon reference, so compare AA and no-AA results separately.
The Circle arc benchmark covers Flat, Round, and Square ends on short, minor,
and major arcs, with thin and wide asymmetric radial thicknesses. Compare the
Flat cases before and after changes to the shared rasterizer to check existing
callers as well as the new caps. The hidden
`Renderer2D.benchmark_circle_arc_vertices` benchmark separately measures GPU vertex
generation into reused storage, across radii, sweep angles, radial/longitudinal
gradients, and Flat/Square caps. Use its Flat cases to check that cap support does
not add per-vertex work to the default path; it does not measure GPU execution.
In the image benchmarks, repeated paint converges to an opaque destination; these
measurements do not measure fresh transparent destinations. Compare Release builds on the same host, configuration,
and workload, and rerun a candidate before adopting an optimization.

Correctness comes before timing: compare coverage against independent references,
exercise fractional coordinates and clipping, and preserve ImagePixel
compositing rules. Do not make speed assertions in automated tests. Exceptional-input
validation and reusable rasterization storage require separate cost and contract
decisions before adding overhead or a public context type.
