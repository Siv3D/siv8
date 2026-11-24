# Renderer2D JoinStyle CPU benchmark

The [implementation guide](../renderer2d/gpu-joins.md) describes the adopted
mesh and fallback boundaries. This experiment measures the CPU preparation for
Triangle and Quad frames, separately from command recording and GPU execution.

## Reproduction

On macOS with Xcode command-line tools and the repository's Boost dependency:

```sh
./tools/renderer2d-join-profile/run.sh timing /tmp/siv8-join-timing
./tools/renderer2d-join-profile/run.sh allocations /tmp/siv8-join-allocations
./tools/renderer2d-join-profile/run.sh debug /tmp/siv8-join-debug
./tools/renderer2d-join-profile/run.sh frames-timing /tmp/siv8-frame-timing
./tools/renderer2d-join-profile/run.sh frames-allocations /tmp/siv8-frame-allocations
./tools/renderer2d-join-profile/run.sh frames-debug /tmp/siv8-frame-debug
/tmp/siv8-join-timing/join-profile inspect
/tmp/siv8-join-timing/join-profile scan 500
```

[run.sh](../../tools/renderer2d-join-profile/run.sh) builds the actual buffer,
triangulation, vertex builders, and their required geometry sources together with
`clang++ -std=c++23 -O3 -DNDEBUG`. It does **not** link the engine library: mixed
Debug/Release Boost template instantiations would invalidate the comparison.
The drawing entry points are dead-stripped. No window or GPU context is created.

The output directory retains the executable, `timing.csv`, `allocations.csv`, or `debug.csv`,
and `*-counts.csv`. Omitting the directory creates a temporary directory and
prints its path. A repeat measurement can invoke `join-profile` directly with
stdout/stderr redirected to new files. `inspect` prints representative fallback
contours at high precision. `scan [shape-count]` prints diagnostics to stderr and
a multi-polygon/validity summary to stdout; invalid output causes a nonzero exit.
It is a correctness probe, not a benchmark. The `debug` script mode compiles every
source with `-O0 -g -DDEBUG=1` and
`-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG`, then runs `scan 500`.
This enables libc++'s strict-weak-ordering checks without mixing engine libraries.
`frames-timing` and `frames-allocations` run the `frames` subcommand for separate
inner/outer widths; `frames-debug` runs `scan-frames 500`. The latter also validates
multi-polygon results after clipping/union. Filenames use the selected mode,
such as `frames-timing.csv`. These cases link the polygon correction helper and
its hash dependency from production sources as well.
Remove an output directory when its artifacts are no longer needed.

[profile.cpp](../../tools/renderer2d-join-profile/profile.cpp) measures:

- `buffer`: the production Boost buffer, reading the input through a span view.
- `earcut`: triangulation of an already buffered polygon.
- `float2`: allocation and conversion of the already buffered rings.
- `write`: copying prepared geometry into a preallocated renderer destination.
- `fallback-total`: stack input preparation and all four buffer-based phases.
- `mesh`: direct topology preparation and destination writing.
- `new-total`: stack input preparation plus the direct builder.
- `legacy-strip-array` and `legacy-strip-stack`: the former default line-strip
  builder with allocating and stack inputs respectively. Omitting the style
  uses Bevel and its direct/fallback paths.

Timing warms each case, calibrates to at least 3 ms per sample, and reports the
median and the second/tenth sorted samples from 11 repetitions. Cases reuse the
same inputs, so branch predictors and caches are warm. Phase timings are measured
independently and should not be summed as an exact reconstruction of total time.
The direct figures use `new-total`, including stack input preparation.
The callback writes real vertices/indices into fixed storage with compiler
barriers, but excludes renderer commands, buffer growth, transfers, and GPU work.

Allocation counting uses a separately compiled executable overriding ordinary
`new`/`new[]`, after warm-up. Counts include temporary allocations freed during
the call. Requested bytes are cumulative allocation traffic, not peak/live
memory. Direct `malloc`, aligned allocation, and GPU/driver allocations are not
counted. The measured geometry containers use ordinary allocation here.

## Results

Measured on Apple M2, 16 GiB RAM, macOS 26.6.2, Xcode 27.0 / Apple Clang 21.0.0,
arm64, with the flags above, after engine builds and tests had finished. Inputs
are a 240-pixel-wide scalene triangle and a
convex quad (exact coordinates in the tool), widths 2, 20, and 80, raster scale 1.
The table below uses width 20. These are CPU geometry timings, not FPS or GPU
speedup measurements. The comparison forces the **current** fallback, including
exact orientation predicates and the same Round quality as the mesh. It does not
attempt to reconstruct the historical, coarser Round path.

| Shape / style | Forced fallback (µs) | Direct (µs) | CPU ratio | Fallback allocations / requested bytes |
| --- | ---: | ---: | ---: | ---: |
| Triangle / Bevel | 16.971 | 0.038 | 445× | 196 / 58,714 |
| Triangle / Round | 17.826 | 0.200 | 89× | 199 / 63,342 |
| Triangle / Miter | 16.666 | 0.036 | 468× | 194 / 58,542 |
| Quad / Bevel | 17.390 | 0.042 | 411× | 203 / 59,496 |
| Quad / Round | 18.366 | 0.222 | 83× | 209 / 64,328 |
| Quad / Miter | 17.438 | 0.042 | 413× | 203 / 59,496 |

Former default line-strip input Array → stack:

- Triangle: 0.101 → 0.094 µs.
- Quad: 0.097 → 0.090 µs.

Every `new-total` case recorded **zero** temporary allocations. For width 20,
`legacy-strip-stack` also removes the one input allocation (80 bytes for Triangle, 96 for Quad).
Its existing polyline scratch storage remains reusable after warm-up. The current
default uses Bevel: at width 20, its measured geometry preparation is
about 2.1–2.5 times faster than the former line-strip path for these inputs.
The default's geometry and compositing also changed, so this is a workload
comparison, not an identical-output algorithm comparison. Common-shape rotation
and inset-collapse limits are covered in the
[implementation guide](../renderer2d/gpu-joins.md#common-shapes-and-width-limits).

For the width-20 quad, the independently measured fallback phases were:

| Style | Buffer (µs) | Earcut (µs) | Float2 (µs) | Write (µs) |
| --- | ---: | ---: | ---: | ---: |
| Bevel | 17.041 | 0.267 | 0.028 | 0.011 |
| Round | 17.473 | 0.737 | 0.032 | 0.030 |
| Miter | 17.072 | 0.263 | 0.028 | 0.011 |


| Shape / style, width 20 | Fallback vertices / indices | Direct vertices / indices |
| --- | ---: | ---: |
| Triangle / Bevel | 7 / 21 | 7 / 21 |
| Triangle / Round | 24 / 72 | 24 / 72 |
| Triangle / Miter | 6 / 18 | 6 / 18 |
| Quad / Bevel | 8 / 24 | 8 / 24 |
| Quad / Round | 26 / 78 | 26 / 78 |
| Quad / Miter | 8 / 24 | 8 / 24 |

Round emits matching counts in both paths for these benchmark inputs, including
18/54 for the width-2 triangle and 24/72 for the width-2 quad. These are larger
than the historical coarser buffer output (13/39 and 16/48). Raster scaling also
increases subdivision when zoomed. GPU workload measurement is still needed
before claiming a rendering-throughput improvement.

## Correctness and limits

The isolated Debug probe examined 6,000 cases: 500 deterministic three/four
point inputs (including repeated points), widths 0, 0.25, 12, and 160, and all
three explicit styles. All 1,500 zero-width results were empty; none of the
results contained more than one polygon, and `is_valid` accepted every result.
This is evidence for this corpus,
not proof that every possible input is covered. The production fallback still
draws only its first polygon.

Uniformly compiled Debug sources reproduced the old libc++ strict-weak-ordering
assertion for `Quad{{61,93},{22,119},{7,208},{221,152}}`, width 160 / Bevel, inside
Boost 1.88's `sort_by_side::side_sorter::apply()`. The GPU-local exact orientation
strategy resolves it with the checks still enabled. Removing double-roundoff
collinear kinks before Earcut also prevents a doubly blended boundary sample
that occurred for some cyclic starting vertices. A half-plane clipping oracle
also validates its filled footprint and input-order variants; the
[implementation guide](../renderer2d/gpu-joins.md#fallback-robustness) explains the
scope of the strategy specialization.

The macOS automated suite, including the dedicated JoinStyle tests, passed.
Windows project syntax and test registration were validated, but D3D11 build,
runtime checks, and CPU/GPU performance remain to be run on Windows. Metal
captures before/after inset collapse and short-edge collapse were visually
inspected at 8x: the Round corners retain their quality and translucent interiors
remain uniform. The manual gallery, including its transition presets, was
syntax-checked. These targeted captured-frame inspections are distinct from a
complete interactive gallery run on both backends.

The large common-case CPU reduction supports keeping this bounded mesh path.
Extending it to concave/crossing rings or collapsed insets needs workload
frequency evidence and topology resolution; the present timings do not justify
removing the fallback or replacing polygon-returning geometric operations.

## Inner/outer widths

The `frames` cases compare the former `stretched((outer-inner)/2)` plus ordinary
closed strip with Bevel's direct mesh. Inputs are
`{{40,30},{280,50},{260,240}}` and
`{{40,30},{280,50},{260,240},{60,220}}`, raster scale 1, on the same Apple M2 /
Clang configuration described above. Both one-sided and asymmetric frames use
the original vertices in the new path. Timings include geometry and destination
writes, and exclude GPU execution. The former strip can differ in corner shape;
this is a migration cost comparison, not an identical-output benchmark.

| Shape | Inner / outer | Former strip (µs) | Bevel direct (µs) | Forced fallback (µs) |
| --- | --- | ---: | ---: | ---: |
| Triangle | 0 / 20 | 0.182 | 0.046 | 19.55 |
| Triangle | 20 / 0 | 0.054 | 0.035 | 19.36 |
| Triangle | 4 / 16 | 0.104 | 0.039 | 45.65 |
| Triangle | 16 / 4 | 0.110 | 0.039 | 46.03 |
| Quad | 0 / 20 | 0.065 | 0.041 | 20.21 |
| Quad | 20 / 0 | 0.056 | 0.041 | 20.00 |
| Quad | 4 / 16 | 0.066 | 0.041 | 49.17 |
| Quad | 16 / 4 | 0.066 | 0.041 | 48.72 |

All measured direct cases, including Round and Miter, have zero temporary heap
allocations. Bevel fallback requests 228–499 allocations / 63,598–142,384 bytes;
it includes filled-region correction, stroke buffer, clipping/union, and
triangulation. Two unequal nonzero sides need more topology work than one side.
The measurements force fallback on ordinary convex inputs; they do not imply
those shapes actually fall back. Common-shape direct eligibility is covered by
the [geometry tests](../../Test/Test_Renderer2DJoinStyle.cpp).

Round direct timings are 0.170–0.247 µs when the outer radius is positive, and
0.034–0.041 µs for inward-only convex frames, whose exterior follows the original
straight edges. Miter direct timings are 0.035–0.041 µs. Optimizing fallback
further should depend on real workload frequency rather than these forced ratios.
