# Crescents and teardrops

Circle provides direct single-color fills, and Shape2D provides stored geometry:

```cpp
const Circle moon{ 320, 240, 64 };
moon.drawCrescent(90_deg, 36, Palette::Skyblue);

const Circle bulb{ 520, 240, 28 };
const Vec2 tip{ 520, 320 };
bulb.drawTeardrop(tip, Palette::Orange);

const Shape2D crescent = Shape2D::Crescent(moon, 90_deg, 36);
const Shape2D teardrop = Shape2D::Teardrop(bulb, tip);
crescent.draw();
teardrop.drawFrame(4);
const Polygon polygon{ teardrop };
```

The authoritative contracts are in [Circle.hpp](../../Siv3D/include/Siv3D/Circle.hpp)
and [Shape2D.hpp](../../Siv3D/include/Siv3D/Shape2D.hpp).
The crescent subtracts an equal-radius circle translated toward the opening by
`width`. That value measures remaining thickness along the opening axis through
the original center; it is not a lunar phase or an area fraction. A teardrop is
the convex hull of the circle and the tip, with straight tangent sides. It has no
hole, and is not the curved-sided drop of an arbitrary path.

Use direct fills for changing parameters. Store a Shape2D or Polygon when the
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
