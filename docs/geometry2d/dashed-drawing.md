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

## Circle and ellipse arcs

`Circle::drawArc()` and `Ellipse::drawArc()` accept `LineStyle` as their first
argument, followed by the ordinary start angle, signed sweep, inner width, outer
width and color. The initial styled-arc API supports a single color.

```cpp
circle.drawArc(LineStyle::Dashed(phase), 0_deg, 270_deg, 3, 3, Palette::Orange);
ellipse.drawArc(LineStyle::RoundDot(phase), 30_deg, -240_deg, 4, 4, Palette::Skyblue);
```

Dash lengths and phase use the sum of the two widths. Placement follows the
line halfway between the inner and outer boundaries, including its offset when
the widths differ. Circle distances are analytic. Ellipse distances use an
approximate arc-length table; the final vertices use the ellipse and its normal
directly. Angles retain the ordinary arc API's meaning, including the ellipse's
radial start direction. Negative sweeps advance from the specified start in the
opposite direction. Distances precede local and camera transforms.

Solid styles delegate to ordinary arc drawing. Patterned full revolutions are
closed: they omit endpoint extensions, retain the preset pitch and share the
exact seam position. A dash may continue across the seam with a different total
length; round-dot spacing across the seam can differ. Existing
`drawDashedFrame()` APIs instead place a requested number and ratio of dashes
around the whole perimeter and remain useful for that purpose.

Arc bands use analytic normals and flat dash cuts, so they do not have polyline
miter joins. The position/normal sampling grid bounds curvature subdivision;
pattern boundaries add vertices to that grid. Tight inward ellipse offsets can
still fold over, and nearby round dots or tangent endpoint extensions can
overlap. These cases retain ordinary blending without boolean union or masks.
See [the public contract](../../Siv3D/include/Siv3D/Circle.hpp) for empty results,
finite-input requirements and the piece budget.

The canonical [comparison program](../../Test/Manual/DashedPathDrawing.md) has an
arc page selected with C or `--arcs`. It includes forward/reverse open arcs,
full circles/ellipses, a flat ellipse and asymmetric widths (I). Its Line column
keeps the original shader implementation visible alongside every preset.

The shared distance-only [pattern cursor](../../Siv3D/src/Siv3D/Geometry2D/DashPattern.hpp)
serves both paths and arcs. Arc emission allocates one backend buffer per shape,
including RoundDot, and writes through the existing upload cursors. Ellipse
length tables reuse the existing four-entry bounded cache; offset tables include
the central offset in their key. Ordinary zero-offset frame drawing retains its
own specialization. No per-dash owning arrays are created.

[Arc tests](../../Test/Test_StyledArc.cpp) cover independent pattern equations,
dense double-precision offset-length integration, signed sweeps, seams, buffer
bounds, allocation failure, piece limits, public rendering and transforms.
Run the opt-in Release CPU submission benchmark separately:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=StyledArc.submission_benchmark'
```

It reports median/p10/p90 per call for public submission, the same builder using
preallocated heap storage, and ordinary solid drawing as different-output
context. Fixed geometry/phase, moving phase, and changing axes are separate rows;
the changing-axis sequence exceeds the table cache. The public and heap rows
must produce matching triangle counts. Warmups and rotated method order reduce
startup/order effects. This measures CPU work, not GPU execution or FPS.
The [upload tool](../../tools/renderer2d-index-upload.mm) also compares identical
arc output in RAM and both Metal cache modes using the
[documented workflow](../renderer2d/README.md#measuring-cpu-writes-to-upload-buffers).

### Arc measurement example

Apple M2, macOS/Metal Release, 270-degree arcs, symmetric widths, moving phase.
Values are wall-time medians in microseconds per public call (p10–p90 in
brackets), including backend buffer writes. The benchmark temporarily requests
interactive thread QoS on macOS, uses 4 warmups and 21 timed batches of 64 calls,
and warms the CPU for 5 ms before each batch. These are CPU submission timings.

| Shape / semi-axes / width | DashedFlat | DashDot | RoundDot |
|---|---:|---:|---:|
| Circle / 20 / 2 | 0.387 [0.385–0.479] | 0.364 [0.362–0.424] | 0.376 [0.371–0.385] |
| Circle / 120 / 8 | 0.736 [0.730–0.758] | 0.693 [0.668–0.725] | 0.566 [0.561–0.586] |
| Ellipse / 120 × 60 / 8 | 2.173 [2.094–2.191] | 2.044 [2.040–2.051] | 1.562 [1.506–1.563] |
| Ellipse / 120 × 12 / 8 | 1.913 [1.910–1.985] | 1.747 [1.744–1.811] | 1.318 [1.317–1.319] |
| Ellipse / 400 × 200 / 2 | 16.579 [16.533–16.583] | 14.106 [14.054–14.127] | 16.723 [16.678–16.864] |

With changing axes exceeding the cache, DashedFlat medians were 3.096 µs for
120 × 60, 2.943 µs for 120 × 12, and 20.367 µs for 400 × 200. Thin, long arcs
emit more pieces: the last row's RoundDot averaged 730 triangles per call,
compared with 58 for 120 × 60. Compatible commands batched together; a batch
crossing backend buffer capacity can require another draw.

The isolated `-Os` upload comparison used start parameter 0.3, sweep 4.5,
inner/outer widths 3/5 and phase 0.25. Output attributes and indices matched in
all storage modes. Median nanoseconds per builder call:

| Shape / pattern | RAM | Metal default cache | Metal write-combined |
|---|---:|---:|---:|
| Circle 120 / DashedFlat | 581 | 576 | 574 |
| Circle 120 / RoundDot | 507 | 500 | 493 |
| Ellipse 120 × 60 / DashedFlat | 2,004 | 2,007 | 2,003 |
| Ellipse 120 × 60 / RoundDot | 1,188 | 1,187 | 1,187 |
| Ellipse 120 × 12 / DashedFlat | 1,838 | 1,836 | 1,835 |
| Ellipse 120 × 12 / RoundDot | 973 | 970 | 971 |

There was no material write-combined penalty in these cases. Repeating the
output checks with `-O3` also passed; its arc write-combined medians ranged from
0.498 to 1.832 µs. The small timing differences do not establish a cache-mode
speedup. Windows runtime performance
and visual output require measurement on Windows; Metal results do not cover it.

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

The [manual program](../../Test/Manual/DashedPathDrawing.md) compares all thirteen
LineStyle presets across Line, straight and angular LineStrings, Bezier2/3,
Spline2D and a closed path. It provides a shared phase, width and opacity, endpoint
guides, and an enlarged view of the selected preset.

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
