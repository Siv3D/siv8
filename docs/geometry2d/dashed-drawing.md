# Styled curve and path drawing

`LineString`, `Bezier2`, `Bezier3`, and `Spline2D` accept the existing
[LineStyle](../../Siv3D/include/Siv3D/LineStyle.hpp) in `draw()`.
`LineString::drawClosed()` closes a polyline; `Spline2D::draw()` uses the spline's
stored closure. Style is the first argument, as it is for `Line`.

```cpp
const LineStyle style{ LineStyle::Dashed(Scene::Time()) };
curve.draw(style, 6, Palette::Orange);
path.drawClosed(LineStyle::DashDot, 6, ColorF{ 0.3, 0.6, 1.0, 0.5 });
```

The presets fix dash and gap proportions relative to the stroke width. Positive
`dotOffset` moves the pattern forward; signed offsets wrap by the preset's
period. Distances are measured in source coordinates before local/camera
transforms. `Solid` uses ordinary stroke rendering with the specified cap.

| Pattern | Visible lengths / gaps, in width units | Period |
|---|---|---:|
| Dotted | 1 / 1 | 2 |
| Dashed | 2.4 / 1.6 | 4 |
| LongDash | 7 / 3 | 10 |
| DashDot | 6 / 1.5 / 1 / 1.5 | 10 |
| RoundDot | Diameter 1, center spacing 2 | 2 |

The intervals follow the half-coverage boundaries of the existing Line shaders;
the table describes the repeating pattern, not necessarily the initial clipped
piece. For patterned open paths, Flat clips at the endpoints. Square and Round
both extend the whole path by half the width, with the phase origin at the
extended start. They do not round or extend each dash. RoundDot follows Line's
open non-Flat pitch fitting, which can compress dots slightly along the path.
Closed paths ignore the cap and keep the preset pitch.

Curves use the same adaptive centerline sampling and quality arguments as their
ordinary `draw()`. Distances are measured on that sampled polyline. Tightening
quality, changing transforms, or changing the sampling budget can move dash
boundaries slightly; this is not exact analytic arc-length placement. Convert to
a stored `LineString` when fixed sampling is useful across frames.

Public contracts, including empty results and the 4,096-piece limit before
closed-seam merging, are in
[LineString::draw()](../../Siv3D/include/Siv3D/LineString.hpp), with links from
the curve APIs. Exceeding the limit draws nothing rather than a truncated prefix.
For closed paths, visible stroked pieces at both ends join, so that run can have
a different length. RoundDot keeps whole dots whose centers are in one cycle;
spacing across the seam can differ from the regular pitch.

## Rendering limits

The v0.8 renderer draws each distance-cut dash as an ordinary GPU stroke with
flat cuts. It retains existing polyline joins and short-distance simplification.
RoundDot places a small shader quad at each sampled center, oriented along the
local tangent. A dot crossing a corner stays a round point instead of bending a
short stroke around the corner. Neither path performs overlap removal.

At a corner, a dash ending just after the vertex can leave an outgoing segment
shorter than the miter extension. The stroke can fold over itself, producing a
distorted outline and extra blending. This occurs even at a right angle; it is
not limited to near reversals. Changing the path cap does not change the join.
Opaque color hides the darkening but does not repair the outline.

Wide strokes on tight curves, self-intersections, and dots near a closed seam
can overlap. An open endpoint cuts a partial round dot in its local tangent
direction. Shader antialiasing on Line and geometry-cut dashes can differ near
the cut edges. There is no mask, boolean union, or expensive fallback.

The [manual program](../../Test/Manual/DashedPathDrawing.md) lets you vary the
preset, phase, path cap and opacity on angular paths, curves and a closed seam.

## Implementation and measurement

The shared [distance cutter](../../Siv3D/src/Siv3D/Geometry2D/DashedPath.hpp)
builds a cumulative-distance table and sweeps forward. Exact repeated points are
removed; other source vertices remain inside cut runs. Join auxiliary points
never participate in distance placement. One renderer-owned workspace retains
point arrays, compact run ranges and round-dot quads, avoiding an owning
allocation per dash. Work scales with input samples, emitted points and pieces.

The shared renderer submits strokes through `addLineString()` and round dots
through a four-vertex, six-index builder using the existing RoundDot shader.
Compatible commands can batch together. Solid paths bypass distance cutting.
The existing Line fast path and LineStyle type are unchanged. The round-dot
builder preserves the [write-only upload boundary](../renderer2d/README.md#measuring-cpu-writes-to-upload-buffers).

The [tests](../../Test/Test_DashedPath.cpp) compare fixed-pattern placement with
independent shader equations, and cover corner cuts, seams, duplicate points,
empty results, budget rejection, upload bounds, transforms and public rendering.
Straight-path images also compare pattern interiors with Line; analytic shader
fringes are excluded from that comparison. An opt-in submission benchmark runs
separately from correctness tests:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=DashedPath.submission_benchmark'
```

On Windows, use the corresponding Release invocation in the
[build guide](../development/README.md#build-and-test). Run on the current host
alone, with other builds and benchmarks stopped.

The benchmark rotates method order, uses 8 warmups and 21 timed samples, warms
the CPU for 5 ms before each sample, and reports median/p10/p90 for batches of
128 calls. Timing covers CPU submission, including distance cutting and actual
backend buffer writes, and excludes `System::Update()`, GPU execution and
presentation. The macOS polyline comparisons also report thread CPU time.
Triangle/draw counts are verified outside timing. The cached-piece baseline
stores cutter output before timing and submits matching geometry. Solid drawing
has different output and serves as context. Curve rows include adaptive sampling
and compare with a cached sampled polyline. Fixed and moving phases are measured
for Dashed, DashDot and RoundDot. These measurements do not directly compare with
arbitrary dash/gap settings, which produce different amounts of geometry.

## Example Release measurements

Apple M2, macOS/Metal, moving phase. Values are wall-time medians in microseconds
per path call; brackets show p10–p90. CPU submission time depends on clock
frequency and system load. Compare matching work within a row; solid output is
different. These are not GPU frame-time measurements.

| Path / pattern | Public [p10–p90] | Cached pieces | Solid |
|---|---:|---:|---:|
| right-angle / Dashed | 0.99 [0.83–1.22] | 0.67 | 0.37 |
| right-angle / DashDot | 0.70 [0.66–0.86] | 0.50 | 0.40 |
| right-angle / RoundDot | 0.41 [0.31–0.73] | 0.24 | 0.24 |
| cubic-s / Dashed | 1.90 [1.79–2.34] | 1.42 | 0.78 |
| cubic-s / DashDot | 1.88 [1.65–2.36] | 1.18 | 0.75 |
| cubic-s / RoundDot | 1.17 [0.89–1.43] | 0.59 | 0.81 |
| closed-seam / Dashed | 1.62 [1.39–2.04] | 1.23 | 0.21 |
| closed-seam / DashDot | 1.40 [1.27–1.75] | 1.00 | 0.21 |
| closed-seam / RoundDot | 1.09 [0.90–1.33] | 0.73 | 0.21 |
| straight / Dashed | 1.92 [1.67–2.35] | 1.41 | 0.31 |
| straight / DashDot | 1.71 [1.40–1.96] | 1.11 | 0.32 |
| straight / RoundDot | 1.35 [1.25–1.61] | 1.00 | 0.30 |
| dense-513 / Dashed | 47.50 [40.01–50.86] | 32.22 | 9.70 |
| dense-513 / DashDot | 43.53 [25.20–46.83] | 30.38 | 9.70 |
| dense-513 / RoundDot | 33.60 [30.38–36.24] | 19.37 | 12.05 |

The public and cached-piece cases have matching triangle and draw-call counts.
Dense paths still pay for every emitted piece; reusing workspace capacity does
not eliminate submission costs. Curve calls additionally include sampling:

| Curve / Dashed | Public [p10–p90] | Cached sampled line | Solid curve |
|---|---:|---:|---:|
| Bezier2 | 3.92 [3.72–4.90] | 3.39 | 1.27 |
| Bezier3 | 4.61 [4.42–6.03] | 3.29 | 2.39 |
| Spline2D, 3 curves | 6.56 [5.98–8.29] | 4.02 | 3.39 |

The isolated [upload tool](../../tools/renderer2d-index-upload.mm) also measures
full and clipped round-dot quads (4 vertices, 6 indices), validating all written
attributes and indices after timing. The following values are nanoseconds per
builder call with preallocated storage, excluding command submission and GPU
execution. Optimization variants are separate measurements; small differences
between them or cache modes are not speedup claims.

| Build / dot | RAM | Metal default cache | Metal write-combined |
|---|---:|---:|---:|
| -Os / full | 12.17 | 12.29 | 12.41 |
| -Os / clipped | 12.82 | 12.61 | 12.41 |
| -O3 / full | 16.28 | 15.99 | 15.87 |
| -O3 / clipped | 15.95 | 15.91 | 15.95 |

These runs show no material write-combined penalty in the round-dot builder.
No Windows timing is inferred from Metal measurements.
