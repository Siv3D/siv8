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
Remove an output directory when its artifacts are no longer needed.

[profile.cpp](../../tools/renderer2d-join-profile/profile.cpp) measures:

- `buffer`: the production Boost buffer, reading the input through a span view.
- `earcut`: triangulation of an already buffered polygon.
- `float2`: allocation and conversion of the already buffered rings.
- `write`: copying prepared geometry into a preallocated renderer destination.
- `fallback-total`: stack input preparation and all four buffer-based phases.
- `mesh`: direct topology preparation and destination writing.
- `new-total`: stack input preparation plus the direct builder.
- `Default-array` and `Default-stack`: the unchanged Default builder with
  allocating and stack inputs respectively.

Timing warms each case, calibrates to at least 3 ms per sample, and reports the
median and the second/tenth sorted samples from 11 repetitions. Cases reuse the
same inputs, so branch predictors and caches are warm. Phase timings are measured
independently and should not be summed as an exact reconstruction of total time.
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
| Triangle / Bevel | 17.101 | 0.038 | 456× | 196 / 58,714 |
| Triangle / Round | 18.176 | 0.200 | 91× | 199 / 63,342 |
| Triangle / Miter | 16.768 | 0.035 | 475× | 194 / 58,542 |
| Quad / Bevel | 17.744 | 0.043 | 414× | 203 / 59,496 |
| Quad / Round | 18.895 | 0.232 | 81× | 209 / 64,328 |
| Quad / Miter | 17.696 | 0.042 | 418× | 203 / 59,496 |

Default input Array → stack:

- Triangle: 0.102 → 0.093 µs.
- Quad: 0.098 → 0.092 µs.

Every `new-total` case recorded **zero** temporary allocations. For width 20,
Default-stack also removes the one input allocation (80 bytes for Triangle, 96 for Quad).
Its existing polyline scratch storage remains reusable after warm-up.

For the width-20 quad, the independently measured fallback phases were:

| Style | Buffer (µs) | Earcut (µs) | Float2 (µs) | Write (µs) |
| --- | ---: | ---: | ---: | ---: |
| Bevel | 17.367 | 0.276 | 0.027 | 0.012 |
| Round | 17.725 | 0.730 | 0.032 | 0.029 |
| Miter | 17.560 | 0.264 | 0.026 | 0.011 |


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
