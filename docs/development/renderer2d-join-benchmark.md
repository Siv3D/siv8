# Renderer2D JoinStyle CPU benchmark

The [implementation guide](../renderer2d/gpu-joins.md) describes the adopted
mesh and fallback boundaries. This experiment measures the CPU preparation for
Triangle and Quad frames, separately from command recording and GPU execution.

## Reproduction

On macOS with Xcode command-line tools and the repository's Boost dependency:

```sh
./tools/renderer2d-join-profile/run.sh timing /tmp/siv8-join-timing
./tools/renderer2d-join-profile/run.sh allocations /tmp/siv8-join-allocations
/tmp/siv8-join-timing/join-profile inspect
/tmp/siv8-join-timing/join-profile scan 500
```

[run.sh](../../tools/renderer2d-join-profile/run.sh) builds the actual buffer,
triangulation, vertex builders, and their required geometry sources together with
`clang++ -std=c++23 -O3 -DNDEBUG`. It does **not** link the engine library: mixed
Debug/Release Boost template instantiations would invalidate the comparison.
The drawing entry points are dead-stripped. No window or GPU context is created.

The output directory retains the executable, `timing.csv` or `allocations.csv`,
and `*-counts.csv`. Omitting the directory creates a temporary directory and
prints its path. A repeat measurement can invoke `join-profile` directly with
stdout/stderr redirected to new files. `inspect` prints representative fallback
contours at high precision. `scan [shape-count]` prints diagnostics to stderr and
a multi-polygon summary to stdout; it is a correctness probe, not a benchmark.
Remove an output directory when its artifacts are no longer needed.

[profile.cpp](../../tools/renderer2d-join-profile/profile.cpp) measures:

- `buffer`: conversion to LineString plus the production Boost buffer.
- `earcut`: triangulation of an already buffered polygon.
- `float2`: allocation and conversion of the already buffered rings.
- `write`: copying prepared geometry into a preallocated renderer destination.
- `old-total`: the old input Array allocation and all four phases.
- `mesh`: direct topology preparation and destination writing.
- `new-total`: stack input preparation plus the direct builder.
- `Default` and `Default-stack`: the unchanged Default builder, with the old
  allocating input and the new stack input respectively.

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
arm64, with the flags above. Inputs are a 240-pixel-wide scalene triangle and a
convex quad (exact coordinates in the tool), widths 2, 20, and 80, raster scale 1.
The table below uses width 20. These are CPU geometry timings, not FPS or GPU
speedup measurements.

| Shape / style | Old total (µs) | Direct total (µs) | CPU ratio | Old allocations / requested bytes |
| --- | ---: | ---: | ---: | ---: |
| Triangle / Bevel | 18.731 | 0.037 | 505× | 198 / 59,642 |
| Triangle / Round | 19.774 | 0.200 | 99× | 201 / 63,642 |
| Triangle / Miter | 18.199 | 0.035 | 515× | 196 / 59,470 |
| Quad / Bevel | 19.421 | 0.042 | 465× | 205 / 60,712 |
| Quad / Round | 20.785 | 0.220 | 94× | 211 / 64,872 |
| Quad / Miter | 19.408 | 0.042 | 467× | 205 / 60,712 |

Triangle Default: 0.100 → 0.092 µs.
Quad Default: 0.101 → 0.090 µs.


Every `new-total` case recorded **zero** temporary allocations. For width 20,
Default also drops its one input allocation (80 bytes for Triangle, 96 for Quad).
Its existing polyline scratch storage remains reusable after warm-up.

For the width-20 quad, the independently measured old phases were:

| Style | Buffer (µs) | Earcut (µs) | Float2 (µs) | Write (µs) |
| --- | ---: | ---: | ---: | ---: |
| Bevel | 18.856 | 0.411 | 0.029 | 0.019 |
| Round | 19.887 | 0.819 | 0.031 | 0.031 |
| Miter | 18.967 | 0.412 | 0.028 | 0.019 |


| Shape / style, width 20 | Old vertices / indices | New vertices / indices |
| --- | ---: | ---: |
| Triangle / Bevel | 13 / 39 | 7 / 21 |
| Triangle / Round | 25 / 75 | 24 / 72 |
| Triangle / Miter | 12 / 36 | 6 / 18 |
| Quad / Bevel | 16 / 48 | 8 / 24 |
| Quad / Round | 28 / 84 | 26 / 78 |
| Quad / Miter | 16 / 48 | 8 / 24 |

Round can emit more geometry for thin strokes or enlarged views: at width 2,
the triangle changes from 13/39 to 18/54, and the quad from 16/48 to 24/72.
Its minimum arc quality improves small corners, and raster scaling increases
subdivision when zoomed. This tradeoff needs GPU workload measurement before
claiming a rendering-throughput improvement.

## Correctness and limits

The isolated optimized probe examined 6,000 cases: 500 deterministic three/four
point inputs (including repeated points), widths 0, 0.25, 12, and 160, and all
three explicit styles. All 1,500 zero-width results were empty; none of the
results contained more than one polygon. This is evidence for this corpus,
not proof that every possible input is covered. The production fallback still
draws only its first polygon.

A separate probe linked against the Debug engine library found a libc++
strict-weak-ordering assertion in Boost 1.88's `sort_by_side::side_sorter::apply()`
for the wide Bevel quad recorded in [TODO.md](../../TODO.md). Its backtrace enters
from the unchanged `MakeBuffer`. The optimized isolated scan completes for that
input. No vendor edit or broad runtime guard was added to hide that distinction.

The macOS automated suite, including the dedicated JoinStyle tests, passed.
Windows project syntax and test registration were validated, but D3D11 build,
runtime checks, and CPU/GPU performance remain to be run on Windows. The manual
gallery program was syntax-checked; interactive inspection on both backends
remains distinct from automated captured-image checks.

The large common-case CPU reduction supports keeping this bounded mesh path.
Extending it to concave/crossing rings or collapsed insets needs workload
frequency evidence and topology resolution; the present timings do not justify
removing the fallback or replacing polygon-returning geometric operations.
