# Adaptive Bezier and Spline drawing

`Bezier2`, `Bezier3`, and `Spline2D` share density-based vertex placement
for adaptive conversion and drawing. GPU drawing uses a default target deviation
of **0.5 render-target pixels**.
This balances a game-oriented appearance with vertex count. It avoids the large
subdivision jumps seen on the tested slow deformations, without per-curve history
or a temporal cache. Wang subdivision remains an internal bounded fallback.

## Choosing quality and work limits

```cpp
const Bezier3 curve{ { 40, 200 }, { 100, 20 }, { 400, 380 }, { 560, 200 } };
curve.drawAdaptive(6, Palette::White);             // 0.5 pixels, at most 256 segments
curve.drawAdaptive(6, Palette::White, 0.25, 256);   // Finer approximation
curve.drawAdaptive(6, Palette::White, 0.75, 128);   // Fewer vertices, visible facets possible
```

The authoritative contracts are in [Bezier2.hpp](../../Siv3D/include/Siv3D/Bezier2.hpp)
and [Bezier3.hpp](../../Siv3D/include/Siv3D/Bezier3.hpp). `targetError` is a finite
positive visual target, not a strict error bound. Quality uses the Local and
Camera transforms and the scene's render-resolution scale, including nonuniform
scaling and shear. It excludes QuadWarp and custom vertex-shader deformation,
consistent with [ScopedQuadWarp2D](../../Siv3D/include/Siv3D/ScopedQuadWarp2D.hpp).
It describes the centerline, not stroke joins, caps, or raster coverage.

The final argument is **`maxSegments`**, with a default of 256 and a precondition
of 1–4096. It replaces the old recursion-depth argument; an old call ending in
`5` now allows only five segments. This budget limits centerline segments, not
GPU vertices added for thickness, joins, and caps. The budget takes priority
over the quality target; one segment draws the endpoint chord. Finite,
representable geometry and intermediate calculations are caller preconditions.

The [manual gallery and study instructions](../../Test/Manual/BezierTessellationStudy.md)
cover moving curves, quality controls, native captures, plots, and measurements.
The study's **Current** label means the legacy recursive conversion, frozen in
[test support](../../Test/BezierTessellationLegacy.hpp) as a baseline. **Density** uses the adopted kernel, and native captures and
submission timings call the public drawing API.

## Implementation boundaries

A single [destination-writing kernel](../../Siv3D/src/Siv3D/Geometry2D/BezierTessellation.hpp)
accepts a source curve and an affine metric curve. It evaluates quality in the
metric and writes points on the source curve. Translation is removed before
metric calculations. The [shared renderer path](../../Siv3D/src/Siv3D/Renderer2D/SivRenderer2D_Bezier.cpp)
reuses a renderer-owned vertex array and passes it to the existing stroke builder.
D3D11 and Metal provide their scene scale; the tessellation logic is shared.
The fixed-size workspace is local to each call and does not allocate.

Geometry conversion (`toLineStringAdaptive`) uses curve-coordinate units and
is independent of renderer state. Both Bezier classes and Spline2D provide a
destination overload that retains allocated storage. Value-returning overloads
call the same implementation. Connected cubic intervals append directly to one
output array; capacity grows geometrically, without a temporary array per curve.
Closure removes the duplicate final endpoint, while retaining internal knots.
Spline drawing uses one stroke, preserving joins without caps at interval ends.

```cpp
LineString outline;
curve.toLineStringAdaptive(outline, 0.5, 256);
curve.paint(image, 6, Palette::White, EnableAntialiasing::Yes, 0.5, 256);
curve.overwrite(image, LineCap::Flat, 6, Palette::White,
                EnableAntialiasing::Yes, 0.25, 256);
```

`paint` / `overwrite` append quality and budget arguments after the existing
antialiasing argument. Quality is measured in **Image pixels**, with no Local,
Camera, scene-resolution, or QuadWarp state. Their default is also 0.5 pixels and
256 segments. They pass the generated polyline to the existing Image stroke
rasterizer, so compositing, antialiasing, and cap semantics remain those described
in [Image writing](image-writing.md). A caller drawing a static curve repeatedly
can retain the converted LineString to avoid generating it each time.

For `Spline2D`, the budget is named **`maxSegmentsPerCurve`** and applies to each
Bezier3 interval, not to the entire path, including `paint()` / `overwrite()`.
Image calls measure quality in Image pixels and follow the path's open/closed state.
Defaults are 0.5 and 256. With a budget
of one, only interval endpoints remain. Conversion keeps every distinct knot;
a closed result omits the repeated final point. See [Spline paths](splines.md)
for drawing and caching examples.

The v0.8.0 implementation scope is complete: adaptive Bezier and Spline
conversion, GPU drawing, and Bezier/Spline Image writing use the shared kernel.
Fixed-subdivision APIs, arc-length measurement, Walkers, and geometric queries
have separate algorithms and do not require this tessellation change.

Automated [drawing tests](../../Test/Test_BezierDrawAdaptive.cpp) cover affine
quality, singular transforms, storage reuse, output and workspace limits, near
collinear reversals, sampled count continuity, all cap combinations, degenerate
curves, default overloads, and the public render path at different scene
resolutions. The [integration tests](../../Test/Test_CurveAdaptive.cpp) add conversion storage,
Spline composition and drawing, Image quality and caps, and renderer-state
independence. [Spline Image tests](../../Test/Test_Spline2DImage.cpp) add direct
Image calls, open/closed cap handling, and compositing at knots and intersections.
The larger [study](../../Test/Test_BezierTessellation.cpp) is opt-in.

## What the comparison measures

The fixed corpus contains 23 quadratic and cubic curves, including 12 ordinary
curves, small curves, S shapes, loops, stationary endpoints, collinear reversals,
coincident endpoints, and a large arch. Seven deformation paths use 1,000 steps
each. A separate deterministic corpus has 512 random finite curves.

Geometry measurements use identity transforms, so one coordinate unit is one
render-target pixel. Screenshots use Metal, 4x MSAA, round caps, and stroke widths
of 1, 6, and 24 pixels. Gallery curves are fitted to their display cells **before**
flattening. Larger raster-difference captures are measured separately; their
segment counts need not match the gallery or unfitted geometry corpus.

The reference image is a 4,096-segment polyline rendered through the same stroke
builder. Curve-to-polyline distances are sampled at 4,097 parameter values;
reverse distances sample seven interior points on every line segment and use
the curve's closest-point query. Random cases use fewer forward samples. These
are sampled geometric errors, not certified Hausdorff bounds or pixel error
guarantees. The measured GPU vertex and triangle counts come from the actual
`Vertex2DBuilder`, rather than an estimate from polyline point counts.

## Algorithms

| Candidate | Count and placement | Important limitation |
| --- | --- | --- |
| Current | Legacy adaptive conversion, midpoint recursion, depth 5 | At most 32 segments; multiple leaves can split together |
| Current deep | Same implementation, depth 12 | Diagnostic control for the depth cap, not the shipping default |
| Wang | One arbitrary integer count, uniform parameter spacing | Tangential acceleration can cause excessive geometric subdivision |
| Density | Integrated curvature-derived density, inverse-density placement | Approximate error target; cubic setup costs more |

Both new candidates first test the control points against the **finite endpoint
segment**. If the whole control polygon lies within the tolerance, they emit
that chord. Both reserve output once the count is known, reuse destination
storage, suppress consecutive duplicate points, and cap output at 256 segments
in this experiment. An exhausted cap takes precedence over quality. The CSV
`limited` field describes this new-candidate cap, not the old recursion limit.

For degree `d`, the Wang estimate used here is:

```text
D = max length(P[i] - 2 P[i+1] + P[i+2])
N = ceil(sqrt(d (d - 1) D / (8 tolerance)))
```

Rounding is to any integer, not a power of two. This follows the formula
documented in [Skia's Wang implementation](https://skia.googlesource.com/skia/%2B/50f5e68b7cac24bfa83b3ce03dc55075f0c6db92/src/gpu/tessellate/GrWangsFormula.h).

Density placement uses the approximate parabola density integral and inverse
from [Raph Levien's quadratic flattening analysis](https://raphlinus.github.io/graphics/curves/2019/12/23/flatten-quadbez.html).
Cubics first receive quadratic approximations, reserving 10% of the target for
that approximation. A single rounded count spans their accumulated densities;
output points are evaluated on the original cubic. This is the family of
approaches also used by [Kurbo's path flattening](https://docs.rs/kurbo/0.13.1/src/kurbo/bezpath.rs.html),
not a benchmark of Kurbo itself.

Three geometric details matter for animated and transformed curves:

- Split cubic approximation intervals at inflections. Otherwise an intermediate
  quadratic can become spuriously straight and make inverse placement jump.
- Evaluate the collinear limit directly instead of switching reversing
  quadratics to a much larger Wang count. With `a = P1-P0`, `b = P2-P1`,
  `v = a-b`, `A = dot(a,v)`, and `B = dot(b,v)`, integral coordinates in this
  limit are proportional to `sign(A) sqrt(abs(A))` and
  `sign(B) sqrt(abs(B))`. The common scale cancels in inverse placement, whose
  limiting inverse is `x abs(x)`. This keeps the count and placement near their
  noncollinear limits. Focused tests compare both sides of the limit.
- Include cubic extrema projected onto the endpoint chord as vertices (at most
  two), using a control-point direction when the endpoints coincide. Density
  can vanish on a nearly straight interval leading into a reversal, leaving its
  turn underrepresented. Explicit extrema also retain turns after a rank-one
  affine transform. These vertices share the output budget with density samples.

The cubic workspace holds at most 64 quadratic pieces; exceeding it falls back
to bounded Wang subdivision. Nearly constant quadratic derivatives also use
Wang's estimate. These are geometric work limits, not exceptional-input guards.

## Vertex count and runtime

Representative Release measurements use Apple M2, Apple Clang 21, and the macOS
Metal renderer. CPU rankings were checked in a second run. These timings are
specific to that host and do not measure GPU execution time.

At the same 0.5-pixel target, the six ordinary base curves of each degree give:

| Degree and metric | Current | Wang | Density |
| --- | ---: | ---: | ---: |
| Bezier2 mean segments | 11.00 | 10.67 | 8.33 |
| Bezier2 mean GPU vertices, 6px stroke | 38.00 | 37.33 | 32.67 |
| Bezier3 mean segments | 19.67 | 22.83 | 16.33 |
| Bezier3 mean GPU vertices, 6px stroke | 55.33 | 61.67 | 48.67 |

For CPU timing, each degree instead uses 96 ordinary deformations. Values below
are median nanoseconds per curve from 11 rounds, with two warmup rounds and
rotating method order. Every round repeats the corpus 64 times.

| Degree and workload | Current | Wang | Density |
| --- | ---: | ---: | ---: |
| Bezier2, reusable output | 145 | 42 | 94 |
| Bezier2, output plus stroke mesh | 366 | 247 | 292 |
| Bezier3, reusable output | 343 | 98 | 344 |
| Bezier3, output plus stroke mesh | 644 | 421 | 616 |

The legacy Bezier baseline returns a newly allocated LineString even in the
reuse workload. Candidate timings therefore include the benefit of reusable
storage; they do not isolate mathematics alone. Fresh-output measurements are
also exported and retain Wang's speed advantage. The mesh workload includes the
production stroke builder, reusable mesh buffers, and flattening; it excludes
renderer command submission and GPU work.

Density saves about 14% of quadratic stroke vertices and 12% of cubic stroke
vertices at this target. Its mesh workload is about 20% faster for quadratics
and 4% faster for cubics than current. Wang is about 32–35% faster in that
workload, but generates more vertices for the ordinary cubic set.

A separate test submits 768 offscreen round-capped strokes per frame through
the renderer. Density calls the public `drawAdaptive`, including transform setup
and reusable output; other methods call adaptive conversion and `LineString::draw`.
It excludes frame update, presentation, and GPU waits. Across two runs, median
CPU time was 2.23–2.25 µs per curve for current, 1.62–1.66 µs for Wang, and
2.11–2.18 µs for density. Density was about 2–6% faster in these runs. Triangle counts were 31,112, 32,776, and 26,384 respectively. Submission
timing varies substantially with scheduling; these results do not predict FPS
or establish a GPU speedup.

## Spline and Image integration costs

The opt-in `CurveAdaptive.Benchmark` measures reusable Spline conversion and
Bezier Image writing; `CurveAdaptive.SubmissionBenchmark` measures public Spline
drawing. The [study instructions](../../Test/Manual/BezierTessellationStudy.md#integration-performance)
describe both and the report exports their raw measurements.

Short Catmull-Rom intervals show a different CPU tradeoff from the larger single
Beziers above. At target 0.5, with an amplitude-120 sinusoidal knot sequence and
30-unit horizontal spacing, density reduces point counts by about 23–26%:

| Intervals | Legacy points | Density points | Legacy / density triangles per draw | Legacy / density submission CPU, µs |
| --- | ---: | ---: | ---: | ---: |
| 4 | 19 | 14 | 40 / 30 | 2.96–3.99 / 3.53–3.83 |
| 32 | 134 | 103 | 270 / 208 | 15.21–16.54 / 25.58–25.82 |
| 128 | 519 | 400 | 1040 / 802 | 54.29–54.53 / 94.34–100.89 |

These are median timings from two Release runs on the same M2 host. Each frame
submits 64 offscreen paths. The timed region includes conversion, stroke mesh
construction, and command submission, but excludes frame update and GPU waits.
The legacy path uses depth 10; both use a 0.5 target. Reusable conversion alone
costs about 7.2 µs versus 23.9 µs for 128 intervals. The recursive baseline is
especially cheap for small, nearly straight intervals; reduced vertices do not
make density universally faster. Keep a converted LineString for unchanged
paths when its coordinate-space quality suits the intended zoom.

Cubic quadratic-approximation legs are calculated directly from endpoint
derivatives, sharing a derivative between adjacent intervals. This avoids
repeated position evaluation while retaining the same approximation and counts.
The comparison corpus verifies the geometric results separately from timing.

For Bezier Image writing, the 16 combinations per degree (64/256-pixel images,
thin/wide strokes, AA on/off, paint/overwrite) have median timing reductions of
about 8% for quadratics and 15% for cubics. Across the two runs, individual
quadratic cases range from about 15% faster to 1.4% slower; cubic cases are about
4–25% faster. Image allocation and curve construction are outside timing;
tessellation, rasterization, and compositing are included. Calls reuse and
repeatedly composite into the destination. These results apply to this corpus,
not all clipping, coverage, or image sizes.

The separate `Spline2DImage.Benchmark` compares direct Image calls with explicit
conversion plus LineString drawing and a cached outline, alongside reusable
conversion alone. See [Image validation and measurement](image-writing.md#validation-and-measurement)
for its workload and commands.

## Changes during deformation

At a 0.5-pixel target, the largest count changes between adjacent sampled
deformation steps were:

| Deformation | Current | Wang | Density |
| --- | ---: | ---: | ---: |
| Shallow quadratic arch | 4 | 1 | 1 |
| Same arch elevated to a cubic | 4 | 1 | 1 |
| Cubic S curve | 2 | 1 | 1 |
| Near a cubic cusp | 1 | 1 | 1 |
| Quadratic through collinear reversal | 0 | 0 | 1 |
| Quadratic near a straight chord | 0 | 16 | 1 |
| Cubic near a straight chord | 1 | 30 | 1 |

The last two Wang cases cross the chord shortcut's threshold. A curve that
needs one segment immediately switches to 17 or 31, because uniform parameter
spacing must accommodate large tangential acceleration. Removing the shortcut
would retain excessive subdivisions even for straight curves. Density remains
at one or two segments on these paths. The exact reversing quadratic needs 25
Wang segments versus five density segments; the collinear cubic reversal uses
43 versus five. These are representative counterexamples to choosing Wang
solely from its CPU timings.

This is a sampled result, not a one-segment-per-frame guarantee. All methods
use integer counts. Density and Wang redistribute existing vertices when the
count changes; neither has temporal state or hysteresis. The residual-change
metric subtracts true curve motion, but nearest-branch changes near a cusp can
make it larger than apparent pixel motion. For example, the near-cusp maximum
was 0.46 pixels for current, 0.12 for Wang, and 0.71 for density. Smaller count
changes alone do not guarantee less visible flicker.

## Appearance and default quality

The native gallery supports the adopted **0.5-pixel default**, especially
for density placement. At 0.75–1 pixel, small arches and S curves show more
obvious facets, particularly with 6- and 24-pixel strokes. Those values remain
useful explicit performance choices. A 0.25-pixel target is available for more
demanding drawing; it is not needed as the game-oriented default.

For all 12 ordinary base curves, compared with the legacy drawing default
of 0.48 and depth 5:

| Setting | Mean GPU vertices, 6px | Change | Mean sampled maximum error |
| --- | ---: | ---: | ---: |
| Current default | 47.50 | — | 0.311px |
| Wang 0.5 | 49.50 | +4.2% | 0.339px |
| Wang 0.75 | 43.33 | −8.8% | 0.494px |
| Density 0.5 | 40.67 | −14.4% | 0.378px |

Equal tolerance does not imply identical appearance. On seven larger raster
cases at stroke width 6, mean normalized coverage difference from the dense
reference was 0.0695 for current, 0.0459 for Wang 0.5, 0.0699 for Wang 0.75,
and 0.0958 for density 0.5. The metric is summed absolute red-channel difference
divided by reference coverage, not a perceptual score. Density spends the error
budget more broadly, so fewer vertices can produce more aggregate raster
difference even with a similar maximum centerline error. It is a deliberate
quality/performance tradeoff, not a claim that density looks best in every case.

In the random corpus at target 0.75, the largest sampled density error was
0.760 pixels. Treat the setting as a visual target rather than a strict maximum.
The large arch exposes a different problem with current defaults: its depth-5
output has about 4.39 pixels of error at target 0.5. Raising the depth produces
128 segments and 0.275 pixels, while Wang uses 105 and 0.408 pixels, and density
uses 92 and 0.454 pixels. Low vertex counts caused by an exhausted cap must not
be mistaken for better tessellation.

Sharp stroke joins near cusps are visible even in the dense reference. This
study does not change join/cap construction, and centerline refinement cannot
promise to remove those artifacts. The captured animation sequences and
representative frames complement the sampled metrics; they are not a complete
perceptual flicker test across display sizes and frame rates.
