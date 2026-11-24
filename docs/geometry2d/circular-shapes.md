# Crescents, lenses and teardrops

Circle provides direct single-color fills for crescents and teardrops. Shape2D
provides stored geometry for those shapes and for symmetric lenses:

```cpp
const Circle moon{ 320, 240, 64 };
moon.drawCrescent(90_deg, 36, Palette::Skyblue);

const Circle bulb{ 520, 240, 28 };
const Vec2 tip{ 520, 320 };
bulb.drawTeardrop(tip, Palette::Orange);

const Shape2D crescent = Shape2D::Crescent(moon, 90_deg, 36);
const Shape2D teardrop = Shape2D::Teardrop(bulb, tip);
const Shape2D petal = Shape2D::Lens(Vec2{ 100, 400 }, Vec2{ 260, 400 }, 24);
crescent.draw();
teardrop.drawFrame(4);
petal.draw(Palette::Skyblue);
petal.drawFrame(3, Palette::Orange);
const Polygon polygon{ teardrop };
```

The authoritative contracts are in [Circle.hpp](../../Siv3D/include/Siv3D/Circle.hpp)
and [Shape2D.hpp](../../Siv3D/include/Siv3D/Shape2D.hpp).
The crescent subtracts an equal-radius circle translated toward the opening by
`width`. That value measures remaining thickness along the opening axis through
the original center; it is not a lunar phase or an area fraction. A teardrop is
the convex hull of the circle and the tip, with straight tangent sides. It has no
hole, and is not the curved-sided drop of an arbitrary path.

A lens uses two equal circular arcs through `from` and `to`. Its `halfWidth`
is the perpendicular distance from the axis to the boundary at the midpoint;
the maximum total width is twice that value. The shape is symmetric about the
axis and its perpendicular bisector. At half the endpoint distance it becomes
a circle. Wider, zero or negative widths and coincident endpoints return an
empty Shape2D. Reversing endpoints gives the same region. Use
`from.distanceFrom(to) * 0.5` for the circle boundary; independent floating-point
distance expressions can round differently right at that boundary.
Thin lenses make leaves and petals without adding another shape class.

Use the available direct fills for changing crescent/teardrop parameters. Store
a Shape2D or Polygon when the
shape stays fixed; drawing the stored mesh avoids regenerating vertices. The
Shape2D factories default to `QualityFactor{ 1.0 }`, independently of rendering
state. `QualityFactor::FromSceneScaling()` is available when generating for a
particular display transform. Direct drawing uses the renderer's raster RMS
scale, including scene resolution. Geometry quality is bounded like other
rounded drawing; it is not a strict pixel-error guarantee.

Direct frame methods are not provided. A stored Shape2D can use its existing
frame, pattern, Image, and Mesh2D conversion APIs. `Polygon{ shape }` copies the
geometry and retains its supplied triangulation; its coordinates still originate
from Shape2D's float vertices. Conversion does not recover double-precision
boundary information. Generic frames use the existing polyline joins, so thick
strokes and sharp tips can overlap.
Lens has no separate direct-fill API; repeatedly drawing a saved lens uses the
ordinary Shape2D or Polygon upload path without regenerating the arcs.
The gallery includes a thin lens with a 12-pixel frame: GPU and Image joins
can differ visibly at its tips. This uses each existing frame contract, without
a lens-specific overlap correction.

## Geometry and upload construction

The [shared geometry helper](../../Siv3D/src/Siv3D/Geometry2D/CircularShapes.hpp)
normalizes the radius, classifies empty/circle cases, and computes intersections
or tangent points. It emits a clockwise boundary and known triangle indices to
callbacks. Shape2D writes its two owned arrays; Metal and D3D11 write positions,
colors and offset indices into one buffer reservation, through the existing
write-only cursors. No Boolean operation or general triangulation is needed.

For a crescent, rays from the shared chord midpoint intersect both circles.
Sampling both boundaries at the same ray directions creates ordered strips even
when the crescent is thin. A rationalized inner distance avoids cancellation near
the full-circle limit. Both intersection tips are shared vertices. The outer
circle's angular rate determines ray density. At most 252 ray intervals produce
504 boundary vertices and 502 triangles; the inner arc keeps paired samples to
avoid crossing a thin outer chord.

A teardrop samples the exposed circular arc between the two tangent points and
fans it from the external tip. At most 252 arc intervals produce 254 vertices and
252 triangles. Every generated vertex belongs to the boundary, so Shape2D frame
and Image operations see the outline without internal fan spokes. Full-circle
direct cases delegate to ordinary Circle fill; stored circles use a boundary fan.
Their edge rasterization need not be pixel-identical.

A lens computes a quarter of its outline and reflects those samples, explicitly
including both tips and both maximum-width points. With half-length `a` and
half-width `h`, its circular half-angle is `alpha = 2 * atan(h / a)`. Coordinates
use ratios of half-angle sines, avoiding subtraction of a huge circle center and
radius as `h` becomes small. The circle's quality at radius `a`, scaled by
`alpha / sqrt(sin(alpha))`, sets angular density; four equal groups give
4–252 boundary vertices. This approximates the existing circle chord-error scale
with bounded work, rather than a strict pixel-error guarantee. A fan from a tip
uses `vertexCount - 2` triangles. No center vertex, Boolean intersection or
general triangulation is needed. The factory sizes its two owned arrays once
and writes directly into them; the existing factory helper receives the chosen
quality, without requiring a lens to store a Circle.

## Validation and measurement

The [manual gallery](../../Test/Manual/CircularShapes.md) is the canonical visual
program and contains focused test and benchmark commands. The
[automated tests](../../Test/Test_CircularShapes.cpp) compare geometry and uploaded
meshes, check analytic areas and sampled single coverage, and capture public
rendering in memory. The submission benchmark separates direct drawing,
construction plus drawing, and cached Shape2D/Polygon drawing with matching
geometry. Compare CPU submission and triangle counts together; it does not
measure GPU time.

The [upload diagnostic](../../tools/renderer2d-index-upload.mm) covers ordinary,
thin, near-circle, fallback and maximum-detail cases in ordinary RAM and actual
Metal upload buffers. See the shared
[measurement rules](../renderer2d/README.md#measuring-cpu-writes-to-upload-buffers).

Representative Apple M2 / Apple Clang measurements below are median nanoseconds
per builder invocation. The ordinary diagnostic uses radius 64, crescent width
36, and teardrop tip distance 110. Buffers are reused; allocation, renderer
submission, presentation and GPU work are excluded.

| Builder | Optimization | RAM | Metal default cache | Metal write-combined |
|---|---|---:|---:|---:|
| Crescent (62 vertices / 60 triangles) | `-Os` | 328 | 332 | 342 |
| Teardrop (36 vertices / 34 triangles) | `-Os` | 330 | 330 | 342 |
| Crescent (62 vertices / 60 triangles) | `-O3` | 298 | 299 | 299 |
| Teardrop (36 vertices / 34 triangles) | `-O3` | 260 | 260 | 260 |

These paths showed no large upload-memory penalty. The public submission
benchmark includes additional renderer work and can have a wide timing spread;
use its percentiles and thread CPU time instead of treating these isolated
builder numbers as the cost of a whole draw call. Cached geometry avoids the
trigonometry and construction measured here. Timing thresholds do not belong in
correctness tests.

### Lens measurements

Apple M2 / macOS Metal Release, `QualityFactor{ 1.0 }`. Values below are wall-time
medians in microseconds per call. The opt-in lens benchmark in the manual gallery
uses four warmups and 21 rotating batches of 128 calls, 5 ms CPU warmups, and
temporary interactive thread QoS. Construction includes the two owned arrays and
their destruction; scene updates and GPU execution are excluded.

| Half-length / half-width | Vertices | Construct | Construct + fill | Saved Shape2D fill | Saved Polygon fill |
|---|---:|---:|---:|---:|---:|
| 80 / 24 | 32 | 0.595 | 0.731 | 0.135 | 0.146 |
| 80 / 0.1 | 4 | 0.277 | 0.420 | 0.066 | 0.062 |
| 80 / 79.999 | 56 | 0.974 | 1.389 | 0.225 | 0.262 |
| 500 / 500 | 252 | 3.289 | 4.260 | 0.882 | 0.906 |

| Half-length / half-width | Saved Shape2D frame | Saved Polygon frame |
|---|---:|---:|
| 80 / 24 | 0.792 | 0.859 |
| 80 / 0.1 | 0.295 | 0.310 |
| 80 / 79.999 | 1.454 | 1.532 |
| 500 / 500 | 6.247 | 6.132 |

Frames use thickness 2. Matching Shape2D/Polygon methods submitted equal triangle
counts: fills use 2–250 triangles and these frames use 12–504. The public timing
spread is substantial: the 80/24 construction p10–p90 was 0.264–0.774 µs, and its
saved Shape2D fill was 0.070–0.187 µs. These results support storing static shapes,
but are not GPU timings or a general speedup ratio.

The isolated `-Os` diagnostic reuses those contours and preallocated buffers;
median nanoseconds per existing builder call, excluding shape generation:

| Half-length / half-width / operation | RAM | Metal default cache | Metal write-combined |
|---|---:|---:|---:|
| 80 / 24 / fill | 32 | 32 | 34 |
| 80 / 24 / frame | 241 | 244 | 265 |
| 80 / 0.1 / fill | 6 | 6 | 7 |
| 80 / 0.1 / frame | 73 | 73 | 85 |
| 80 / 79.999 / fill | 52 | 52 | 59 |
| 80 / 79.999 / frame | 411 | 396 | 447 |
| 500 / 500 / fill | 278 | 271 | 253 |
| 500 / 500 / frame | 1,853 | 1,833 | 1,956 |

Vertex attributes and indices matched across storage modes at both `-Os` and
`-O3`. The latter's write-combined medians were 10–165 ns for fills and
76–1,135 ns for frames. Compare memory modes within this diagnostic rather than
comparing these isolated timings directly with the public benchmark. Windows
build, runtime performance and visual output remain unverified on this host.
