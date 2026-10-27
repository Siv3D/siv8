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

The sum of the two widths gives the nominal dash scale. Placement follows the
line halfway between the inner and outer boundaries, including its offset when
the widths differ. Circle distances are analytic. Ellipse distances use an
approximate arc-length table; the final vertices use the ellipse and its normal
directly. Angles retain the ordinary arc API's meaning, including the ellipse's
radial start direction. Negative sweeps advance from the specified start in the
opposite direction. Distances precede local and camera transforms.

Every patterned arc fits its period to the **full circumference**, including
partial sweeps. For circumference C, total width w and preset period P, the
number of periods is max(1, round(C / (P w))); the fitted pitch is C divided by
that count. Dash/gap proportions stay the same, while RoundDot keeps diameter w
and adjusts only center spacing. Phase uses the fitted unit (pitch / P) and is
anchored to the specified start, independently of sweep and cap. Changing only
the sweep reveals or clips a fixed pattern; it does not rearrange existing pieces
at 360°. Resizing the shape or changing width can change the integer count.

Flat clips at the arc endpoints. Other patterned caps extend the ends by half
the width, shortened to half the remaining circumference as the gap closes.
Round-dot clips use fixed full-curve centers; two fragments of the same seam dot
share one center and form a whole dot at closure. The 4,096-piece limit counts
that dot once. Neighboring dots on tight bends can still overlap.

Solid styles delegate to ordinary arc drawing and retain its cap behavior.
Existing `drawDashedFrame()` APIs specify a requested number and ratio of dashes
around the whole perimeter and remain useful for that purpose.

Arc bands use analytic normals and flat dash cuts, so they do not have polyline
miter joins. A fixed full-revolution position/normal grid bounds curvature subdivision;
pattern boundaries add vertices to that grid. Tight inward ellipse offsets can
still fold over, and nearby round dots or tangent endpoint extensions can
overlap. These cases retain ordinary blending without boolean union or masks.
See [the public contract](../../Siv3D/include/Siv3D/Circle.hpp) for empty results,
finite-input requirements and the piece budget.

The canonical [comparison program](../../Test/Manual/DashedPathDrawing.md) has an
arc page selected with C or `--arcs`. It includes forward/reverse open arcs,
full circles/ellipses, a flat ellipse and asymmetric widths (I). Its Line column
keeps the original shader implementation visible alongside every preset. H or
`--closure` opens a sweep/full-reference/seam comparison. P animates through
360° while phase can remain paused; J/K step the angle, V reverses it, and O
alternates the enlarged seam with the full reference.

The shared distance-only [pattern cursor](../../Siv3D/src/Siv3D/Geometry2D/DashPattern.hpp)
serves both paths and arcs. Arc emission allocates one backend buffer per shape,
including RoundDot, and writes through the existing upload cursors. Ellipse
length tables reuse the existing four-entry bounded cache; offset tables include
the central offset in their key. Ordinary zero-offset frame drawing retains its
own specialization. No per-dash owning arrays are created.

[Arc tests](../../Test/Test_StyledArc.cpp) cover independent pattern equations,
dense double-precision offset-length integration, fixed vertices across sweeps,
359.9999°/360° image convergence, signed sweeps, seams, buffer
bounds, allocation failure, piece limits, public rendering and transforms.
Run the opt-in Release CPU submission benchmark separately:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=StyledArc.submission_benchmark'
```

It reports median/p10/p90 per call for public submission, the same builder using
preallocated heap storage, and ordinary solid drawing as different-output
context. Fixed geometry/phase, moving phase, changing axes, and a 359°–361° sweep
with fixed phase are separate rows;
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
| Circle / 20 / 2 | 0.699 [0.538–0.883] | 0.461 [0.385–0.742] | 0.708 [0.469–0.851] |
| Circle / 120 / 8 | 1.667 [1.417–2.009] | 1.348 [1.182–1.611] | 0.991 [0.981–1.229] |
| Ellipse / 120 × 60 / 8 | 4.596 [4.337–5.096] | 4.076 [4.004–4.694] | 2.804 [2.797–3.284] |
| Ellipse / 120 × 12 / 8 | 3.941 [2.701–4.260] | 3.559 [3.253–3.996] | 2.686 [2.432–2.870] |
| Ellipse / 400 × 200 / 2 | 28.753 [28.220–30.919] | 24.231 [22.982–24.967] | 24.438 [19.292–26.693] |

With a fixed phase and sweeps changing from 359° to 361°, DashedFlat medians
were 1.668 µs for Circle 120 and 5.242 µs for Ellipse 120 × 60; RoundDot was
1.250 µs and 3.059 µs respectively. With changing axes exceeding the cache,
DashedFlat medians were 6.150 µs for 120 × 60, 5.471 µs for 120 × 12, and
30.851 µs for 400 × 200. Thin, long arcs emit more pieces. Compatible commands
batch together; a batch crossing backend buffer capacity can require another draw.

The isolated `-Os` upload comparison used start parameter 0.3, sweep 4.5,
inner/outer widths 3/5 and phase 0.25. Output attributes and indices matched in
all storage modes. Median nanoseconds per builder call; the last column uses
the preceding fixed-pitch implementation with identical input parameters:

| Shape / pattern | RAM | Metal default cache | Metal write-combined | Before / write-combined |
|---|---:|---:|---:|---:|
| Circle 120 / DashedFlat | 592 | 589 | 591 | 578 |
| Circle 120 / RoundDot | 470 | 485 | 473 | 495 |
| Ellipse 120 × 60 / DashedFlat | 2,087 | 2,095 | 2,099 | 2,012 |
| Ellipse 120 × 60 / RoundDot | 1,041 | 1,043 | 1,041 | 1,194 |
| Ellipse 120 × 12 / DashedFlat | 1,967 | 1,967 | 1,963 | 1,859 |
| Ellipse 120 × 12 / RoundDot | 887 | 903 | 888 | 964 |

The fixed full-revolution grid added about 2–6% (0.01–0.10 µs) to these
DashedFlat cases. RoundDot took about 4–13% less time in these cases; it now
samples fixed centers directly, and fitting can also change emitted piece
counts. These are different-output comparisons, not a general speedup claim.
No material write-combined penalty appeared. `-O3` output checks also passed;
its arc write-combined medians ranged from 0.465 to 1.914 µs. Public submission
and the isolated tool use different angles, width offsets and timing contexts;
compare storage modes or before/after runs within the same benchmark. Windows
runtime performance and visual output still require measurement on Windows.

## Rounded frames

`RoundRect::drawFrame(LineStyle, ...)` accepts either a centered thickness or
separate inner/outer thicknesses, with a single color:

```cpp
roundRect.drawFrame(LineStyle::Dashed(phase), 6, Palette::Orange);
roundRect.drawFrame(LineStyle::RoundDot(phase), 2, 6, Palette::Skyblue);
```

The four straight sides and four quarter circles form one closed pattern domain.
Distances follow the midpoint of the normalized inner and outer outlines. The
preset period fits an integer number of repeats to this perimeter, using the same
rule as styled arcs. Phase starts at the midpoint contour's upper-left top
contact and moves clockwise, without restarting at component boundaries. Caps
have no effect on a closed frame. Radius normalization and a collapsed interior's
outer-fill fallback follow ordinary frame drawing. Round dots keep the requested
diameter; tight corners can still bring neighboring dots together.

The existing `drawDashedFrame(RectangularDashStyle, ...)` keeps its explicit
count/ratio/offset contract. The new overload uses existing LineStyle presets and
does not add fields or new public style types. Four-independent-radius Rect/RectF
frames and arbitrary Polygon outlines are outside this addition.

The shared builder emits paired inner/outer samples for each dash, including
exact straight/arc junctions and the closing point. It uses bounded local arrays,
one buffer reservation and the same fitted-pattern cursor as analytic arcs.
Arc and frame mesh emission stay specialized to their geometry. It does not
construct a Polygon or allocate a container for each dash. Ordinary solid
frames bypass these helpers.

[Frame tests](../../Test/Test_StyledRoundRect.cpp) cover an independent centerline
coverage oracle, caps/phase, circular and capsule limits, small/zero radii,
collapsed interiors, half-alpha blending, transforms, allocation failure and the
piece budget. Use F or `--frames` in the canonical
[comparison program](../../Test/Manual/DashedPathDrawing.md). Space animates phase;
I changes the inner/outer split. The narrow column exposes the fill fallback.

Run the opt-in CPU benchmark separately from correctness tests:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=StyledRoundRect.submission_benchmark'
```

It rotates public submission, preallocated heap-builder output and ordinary
solid frames as different-output context, with matching public/heap triangle
counts. The upload diagnostic also verifies the new builder in RAM and both
Metal cache modes. These timings exclude GPU execution.

### Rounded frame measurement example

Apple M2, macOS/Metal Release, symmetric widths, moving phase. Public-call
wall-time medians in microseconds (p10–p90 in brackets), including backend
buffer writes. The benchmark uses 4 warmups, 21 batches of 64 calls, a 5 ms CPU
warmup before each batch and interactive thread QoS on macOS.

| Rectangle / radius / width | DashedFlat | DashDot | RoundDot |
|---|---:|---:|---:|
| 200 × 100 / 20 / 8 | 1.277 [0.859–1.505] | 1.554 [1.345–1.572] | 1.152 [1.138–1.395] |
| 120 × 40 / 20 / 8 | 1.253 [0.930–1.354] | 1.278 [1.095–1.486] | 0.703 [0.695–0.898] |
| 400 × 240 / 48 / 2 | 7.133 [6.872–8.336] | 5.699 [3.824–6.220] | 6.847 [6.682–7.937] |

The thin, large frame has many more pieces: RoundDot submits 598 triangles,
versus 70 for the first shape. Ordinary solid frames took 0.148–0.213 µs in
these runs, but produce different geometry. Public and heap-builder triangle
counts matched; the heap-builder medians for the first shape were
1.258 / 1.478 / 1.145 µs in the same pattern order.

The isolated upload diagnostic uses a 200 × 100 reference rectangle,
inner/outer widths 3/5, phase -0.5 and scale 1. Median nanoseconds per `-Os`
builder call, with all vertex attributes and indices checked in each mode:

| Radius / pattern | RAM | Metal default cache | Metal write-combined |
|---|---:|---:|---:|
| 0 / DashedFlat | 565 | 568 | 583 |
| 0 / RoundDot | 462 | 446 | 448 |
| 2 / DashedFlat | 608 | 602 | 588 |
| 2 / RoundDot | 460 | 463 | 453 |
| 20 / DashedFlat | 685 | 668 | 670 |
| 20 / RoundDot | 488 | 507 | 479 |
| 50 / DashedFlat | 726 | 724 | 733 |
| 50 / RoundDot | 498 | 528 | 475 |

No material write-combined penalty appeared. `-O3` checks also passed, with
write-combined medians of 0.366–0.659 µs. Isolated and public measurements use
different phases, width offsets and timing contexts; compare storage modes
within the same benchmark. Windows runtime and visual results remain unverified.

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
