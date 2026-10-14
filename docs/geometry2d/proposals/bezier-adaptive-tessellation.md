# Bezier adaptive tessellation comparison

Status: comparison complete; density placement is the preferred implementation
candidate, not an adopted public API. Release scope: Bezier2 and Bezier3 drawing
for v0.8. Spline2D and Image paint/overwrite remain separate follow-up work.

Prefer density-based placement with a target deviation of **0.5 render-target
pixels** for the next drawing implementation. It reduces ordinary stroke
vertices and avoids the large count changes observed with midpoint subdivision
and with Wang's formula near straight curves. The measured cubic CPU cost is
close to the current implementation, which fits a game-oriented quality target
that allows some extra calculation to save vertices. Wang remains useful as a
bounded internal fallback and as the faster comparison baseline.

The [study instructions](../../../Test/Manual/BezierTessellationStudy.md) generate
the canonical local gallery, animation controls, plots, and CSV measurements.
All candidates currently live in
[test support](../../../Test/BezierTessellationStudy.hpp); production behavior
and public declarations are unchanged.

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
| Current | Existing public adaptive conversion, midpoint recursion, depth 5 | At most 32 segments; multiple leaves can split together |
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

The density candidate uses the approximate parabola density integral and inverse
from [Raph Levien's quadratic flattening analysis](https://raphlinus.github.io/graphics/curves/2019/12/23/flatten-quadbez.html).
Cubics first receive quadratic approximations, reserving 10% of the target for
that approximation. A single rounded count spans their accumulated densities;
output points are evaluated on the original cubic. This is the family of
approaches also used by [Kurbo's path flattening](https://docs.rs/kurbo/0.13.1/src/kurbo/bezpath.rs.html),
not a benchmark of Kurbo itself.

Two geometric details matter for animated curves:

- Split cubic approximation intervals at inflections. Otherwise an intermediate
  quadratic can become spuriously straight and make inverse placement jump.
- Evaluate the collinear limit directly instead of switching reversing
  quadratics to a much larger Wang count. With `a = P1-P0`, `b = P2-P1`,
  `v = a-b`, `A = dot(a,v)`, and `B = dot(b,v)`, integral coordinates in this
  limit are proportional to `sign(A) sqrt(abs(A))` and
  `sign(B) sqrt(abs(B))`. The common scale cancels in inverse placement, whose
  limiting inverse is `x abs(x)`. This keeps the count and placement near their
  noncollinear limits. Focused tests compare both sides of the limit.

The cubic workspace holds at most 64 quadratic pieces; exceeding it falls back
to bounded Wang subdivision. Nearly constant quadratic derivatives also use
Wang's estimate. These are geometric work limits, not exceptional-input guards.

## Vertex count and runtime

Representative Release measurements use Apple M2, Apple Clang 21, and the macOS
Metal renderer. CPU rankings were checked in a second run. They are host-specific
results; Windows execution and GPU execution time are not established here.

At the same 0.5-pixel target, the six ordinary base curves of each degree give:

| Degree and metric | Current | Wang | Density |
| --- | ---: | ---: | ---: |
| Bezier2 mean segments | 11.00 | 10.67 | 8.33 |
| Bezier2 mean GPU vertices, 6px stroke | 38.00 | 37.33 | 32.67 |
| Bezier3 mean segments | 19.67 | 22.83 | 16.17 |
| Bezier3 mean GPU vertices, 6px stroke | 55.33 | 61.67 | 48.33 |

For CPU timing, each degree instead uses 96 ordinary deformations. Values below
are median nanoseconds per curve from 11 rounds, with two warmup rounds and
rotating method order. Every round repeats the corpus 64 times.

| Degree and workload | Current | Wang | Density |
| --- | ---: | ---: | ---: |
| Bezier2, reusable output | 156 | 44 | 95 |
| Bezier2, output plus stroke mesh | 376 | 248 | 289 |
| Bezier3, reusable output | 352 | 98 | 407 |
| Bezier3, output plus stroke mesh | 654 | 418 | 675 |

The current public conversion returns a newly allocated LineString even in the
reuse workload. Candidate timings therefore include the benefit of reusable
storage; they do not isolate mathematics alone. Fresh-output measurements are
also exported and retain Wang's speed advantage. The mesh workload includes the
production stroke builder, reusable mesh buffers, and flattening; it excludes
renderer command submission and GPU work.

Density saves about 14% of quadratic stroke vertices and 13% of cubic stroke
vertices at this target. Its mesh workload is about 23% faster for quadratics
and 3% slower for cubics than current. Wang is about 34–36% faster in that
workload, but generates more vertices for the ordinary cubic set.

A separate test submits 768 offscreen round-capped strokes per frame through
the renderer. It times flattening and draw submission, excluding frame update,
presentation, and GPU waits. Across two runs, median CPU time was 2.07–2.24 µs
per curve for current, 1.62–1.64 µs for Wang, and 2.22–2.25 µs for density.
Density was approximately equal to current in one run and about 7% slower in
the other. Triangle counts were 31,112, 32,776, and 26,256 respectively. The
timing spread is visible in the report; these results do not predict FPS or
establish a GPU speedup.

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
43 versus three. These are representative counterexamples to choosing Wang
solely from its CPU timings.

This is a sampled result, not a one-segment-per-frame guarantee. All methods
use integer counts. Density and Wang redistribute existing vertices when the
count changes; neither has temporal state or hysteresis. The residual-change
metric subtracts true curve motion, but nearest-branch changes near a cusp can
make it larger than apparent pixel motion. For example, the near-cusp maximum
was 0.46 pixels for current, 0.12 for Wang, and 0.71 for density. Smaller count
changes alone do not guarantee less visible flicker.

## Appearance and default quality

The native gallery supports **0.5 pixels as the starting default**, especially
for density placement. At 0.75–1 pixel, small arches and S curves show more
obvious facets, particularly with 6- and 24-pixel strokes. Those values remain
useful explicit performance choices. A 0.25-pixel target is available for more
demanding drawing; it is not needed as the game-oriented default.

For all 12 ordinary base curves, compared with the current shipping default
of 0.48 and depth 5:

| Setting | Mean GPU vertices, 6px | Change | Mean sampled maximum error |
| --- | ---: | ---: | ---: |
| Current default | 47.50 | — | 0.311px |
| Wang 0.5 | 49.50 | +4.2% | 0.339px |
| Wang 0.75 | 43.33 | −8.8% | 0.494px |
| Density 0.5 | 40.50 | −14.7% | 0.378px |

Equal tolerance does not imply identical appearance. On seven larger raster
cases at stroke width 6, mean normalized coverage difference from the dense
reference was 0.0695 for current, 0.0459 for Wang 0.5, 0.0699 for Wang 0.75,
and 0.0965 for density 0.5. The metric is summed absolute red-channel difference
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

## Implementation direction

Use density placement as the main Bezier drawing kernel, with bounded Wang
fallbacks internal to it. Do not expose an algorithm-selection enum merely
because the experiment compared several methods. Keep an explicit vertex-work
budget separate from the visual quality target; replacing `maxDepth` with
`maxSegments` makes that budget understandable independently of the algorithm.
A default budget of 256 is a starting proposal, not a settled public contract.

The implementation should have one destination-writing core so draw paths can
reuse storage. Any value-returning convenience wrapper should call that core.
Do not add hidden temporal caches or per-object history to these small value
types. Preconditions, quality units, and behavior at the output cap belong in
the final Doxygen once the implementation contract is established.

The next evaluation must carry local/camera transforms into drawing quality,
including nonuniform scale, shear, scene scaling, and quad warp. The current
experiment validates identity transforms; it does not justify promising final
screen-pixel accuracy under projective warps. Independent of this work, geometry
conversion uses curve-coordinate units and must not silently inherit renderer
state.

Keep the first production patch at `drawAdaptive` and its dedicated internal
kernel. Existing Bezier `toLineStringAdaptive()` is also used by paint/overwrite,
and `Bezier3Flatten.hpp` is shared with Spline2D: replacing either in place would
extend the change beyond the requested scope. Decide the public conversion API
and the paint/Spline transition after Bezier drawing behavior is settled, without
duplicating the new algorithm. Pending implementation and validation stages are
tracked in [TODO.md](../../../TODO.md#bezier2-and-bezier3-adaptive-drawing).
