# Bezier tessellation study

This opt-in automated study compares current Bezier2/Bezier3 flattening, a deeper
current baseline, Wang subdivision, and density placement. It captures the
actual renderer, measures stroke mesh counts, and produces a local interactive
report. It does not replace a platform Main.cpp or change a production API.

The complete executable test program is
[Test_BezierTessellation.cpp](../Test_BezierTessellation.cpp), with its candidate
implementations and fixed corpus in
[BezierTessellationStudy.hpp](../BezierTessellationStudy.hpp). The
[comparison and proposed direction](../../docs/geometry2d/proposals/bezier-adaptive-tessellation.md)
explain measurement boundaries and known limitations.

## Run on macOS

Run from the repository root in a graphical desktop session. Use Release for
performance comparisons. Xcode and Metal builds must run outside the sandbox.
The report generator requires Python 3 and matplotlib. The output directory
below belongs to the report tool, not the test runner.

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=BezierTessellation.*' > /tmp/siv8-bezier-study.log 2>&1
python3 tools/bezier_tessellation_report.py /tmp/siv8-bezier-study.log --output /tmp/siv8-bezier-report
open /tmp/siv8-bezier-report/index.html
```

If matplotlib's default configuration directory is not writable, scope its
cache location to the report command:

```sh
MPLCONFIGDIR=/tmp/siv8-matplotlib python3 tools/bezier_tessellation_report.py /tmp/siv8-bezier-study.log --output /tmp/siv8-bezier-report
```

For a separate timing run after the build, keep the app working directory:

```sh
cd macOS/App
./Siv3D-Test.app/Contents/MacOS/Siv3D-Test --test-only '--test-case=BezierTessellation.Benchmark,BezierTessellation.SubmissionBenchmark' > /tmp/siv8-bezier-timing.log 2>&1
```

Do not run competing benchmarks or builds during timing. The report accepts
multiple logs when they contain **disjoint study sections**. Use a separate
output directory for a repeated full benchmark; concatenating full runs would
combine rows without computing cross-run statistics.

On Windows, the same tests are registered in Siv3D-Test. Use the host's
[documented test workflow](../../docs/development/README.md), build Release,
and select the same `BezierTessellation.*` test names. Do not use the macOS
commands there. A macOS timing result does not validate Windows performance.

## Expected results and inspection

The default automated suite runs endpoint, output-budget, storage-reuse,
ordinary-error, and collinear-limit checks. Geometry, raster, animation, and
timing studies are hidden unless explicitly selected. A complete study ends
with passing test results and emits `BSTUDY` measurement records and
`BSTUDY_IMAGE` base64 PNG records on stdout. No capture files are written by the
tests. The report tool exports CSV, JSON, plots, and the unmodified native PNGs.

Open the HTML report at native scale. Static capture rows are the dense
reference, current shipping default, Wang, and density. Columns are 1-, 6-, and
24-pixel round-capped strokes. Check small arches, asymmetric curves, S curves,
loops, and near cusps at each quality setting. The animation sections provide
play/pause and frame scrubbing for the S and near-cusp captures.

Compare count changes separately from changes in the visible outline. Current
midpoint subdivision can change several leaves at once. Wang's straight-chord
shortcut can switch from one segment to many; density should cross the tested
collinear reversal without a large fallback-induced count spike. Integer count
changes and some outline movement remain expected. Cusp join artifacts visible
in the dense reference are not a candidate-only flattening failure.

Inspect error and cap columns together: reaching a work limit can exceed the
quality target. Raster coverage difference is diagnostic, not a perceptual
acceptance score. CPU submission timings exclude frame update and GPU waits;
neither those timings nor triangle counts establish a GPU execution speedup.
