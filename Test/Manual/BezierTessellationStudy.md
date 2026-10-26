# Bezier tessellation study

This opt-in automated study compares legacy Bezier2/Bezier3 flattening, a deeper
legacy baseline, Wang subdivision, and the adopted density placement. It captures
the actual renderer, measures stroke mesh counts, and produces a local interactive
report. Density captures and submission timings use the public `draw`
API. The study does not replace a platform Main.cpp. The [Spline gallery](Spline2D.md)
and [Image gallery](ImageWriting.md) cover those integration paths.

The complete executable test program is
[Test_BezierTessellation.cpp](../Test_BezierTessellation.cpp), with comparison baselines
and the fixed corpus in
[BezierTessellationStudy.hpp](../BezierTessellationStudy.hpp). The recursive
baselines are frozen in [BezierTessellationLegacy.hpp](../BezierTessellationLegacy.hpp);
they do not call the public conversion APIs, which also use density placement. The
[drawing guide and comparison](../../docs/geometry2d/bezier-drawing.md)
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
ordinary-error, and collinear-limit checks. `BezierDraw.*` adds public
API captures, affine and resolution tests, and cubic reversal regressions.
Geometry, raster, animation, and timing studies are hidden unless explicitly selected. A complete study ends
with passing test results and emits `BSTUDY` measurement records and
`BSTUDY_IMAGE` base64 PNG records on stdout. No capture files are written by the
tests. The report tool exports CSV, JSON, plots, and the unmodified native PNGs.

Open the HTML report at native scale. Static capture rows are the dense
reference, legacy default (Current), Wang, and density. Columns are 1-, 6-, and
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

## Interactive quality and transform check

Build the complete program below in a Siv3D application. Keep repository platform
`Main.cpp` files unchanged. Press **1/2** for quadratic/cubic, **Space** to pause,
**Q/W/E** for targets 0.25/0.5/1 pixel, and **B** for a deliberately small
four-segment budget. Hold **Up/Down** to zoom; **A** changes between uniform and
nonuniform scaling, and **H** switches scene/display render resolution. Compare
the drawing with the dense reference using **R**. No output files are created.

Expected: the default follows the animated curve without the large subdivision
steps of the legacy method. Zooming in retains roughly the same centerline
quality in render pixels. A 1-pixel target can look faceted, and the four-segment
budget deliberately overrides quality. The reference can still show stroke-join
artifacts near a sharp turn. Display resolution may equal scene resolution on
some configurations; use a high-DPI display to inspect that difference.

```cpp
# include <Siv3D.hpp>
# include <cmath>

void Main()
{
	Window::Resize(1000, 700);
	Scene::Configure(SceneConfig::Fixed({ 1000, 700 }, SceneResolution::SceneSize));
	Scene::SetBackground(ColorF{ 0.08 });
	Scene::SetMSAASampleCount(MSAASampleCount::X4MSAA);
	const Font font{ 18 };
	double time = 0, targetError = 0.5, zoom = 1;
	bool cubic = true, paused = false, limited = false;
	bool anisotropic = false, display = false, reference = false;
	while (System::Update())
	{
		if (Key1.down()) cubic = false;
		if (Key2.down()) cubic = true;
		if (KeySpace.down()) paused = !paused;
		if (KeyQ.down()) targetError = 0.25;
		if (KeyW.down()) targetError = 0.5;
		if (KeyE.down()) targetError = 1.0;
		if (KeyB.down()) limited = !limited;
		if (KeyA.down()) anisotropic = !anisotropic;
		if (KeyR.down()) reference = !reference;
		if (KeyH.down())
		{
			display = !display;
			Scene::Configure(SceneConfig::Fixed({ 1000, 700 },
				display ? SceneResolution::Display : SceneResolution::SceneSize));
		}
		const double delta = Scene::DeltaTime();
		if (!paused) time += delta;
		zoom = Clamp(zoom + (int(KeyUp.pressed()) - int(KeyDown.pressed())) * delta, 0.25, 2.0);
		const int32 budget = limited ? 4 : 256;
		{
			const Transformer2D transform{ Mat3x2::Scale(zoom, anisotropic ? 0.4 * zoom : zoom)
				* Mat3x2::Translate(500, 400) };
			const Bezier2 q{ { -200, 0 }, { -100 + 300 * std::sin(time * 0.4), 160 * std::sin(time) }, { 200, 0 } };
			const Bezier3 c{ { -200, 0 }, { -150, 160 * std::sin(time) }, { 100, -180 * std::sin(time * 0.7) }, { 200, 0 } };
			if (reference)
			{
				const LineString line = cubic ? c.toLineString(4096) : q.toLineString(4096);
				line.draw(LineCap::Round, 6, Palette::White);
			}
			else if (cubic) c.draw(LineCap::Round, 6, Palette::White, targetError, budget);
			else q.draw(LineCap::Round, 6, Palette::White, targetError, budget);
		}
		font(U"Bezier{} | target={}px | budget={} | zoom={:.2f} | reference={}"_fmt(
			cubic ? 3 : 2, targetError, budget, zoom, reference)).draw(20, 20);
		font(U"1/2: degree   Space: pause   Q/W/E: quality   B: budget   Up/Down: zoom").draw(20, 50);
		font(U"A: nonuniform scale   H: resolution   R: reference | render {}"_fmt(Scene::RenderResolution())).draw(20, 80);
	}
}
```

## Integration performance

Select `CurveAdaptive.Benchmark` in the same Release test workflow for Spline
conversion and Bezier Image writing. It emits `integration_points` (legacy/density point
counts) and `integration_timing` (median, p10, p90 nanoseconds per call) in the
`BSTUDY` format. Select `CurveAdaptive.Gallery` for the native integration image.
Pass these logs alongside the disjoint Bezier study log to the report generator. Spline cases
have 4, 32, and 128 intervals and reuse destination storage for both algorithms;
legacy uses depth 10 and both use a 0.5-unit target. Image cases compare legacy
0.48/depth 5 with density defaults, at sizes 64 and 256, thin/wide strokes, both
AA modes, and paint/overwrite. Destination images are reused and repeatedly
composited; construction is outside timing. Every pair has two warmup rounds,
11 measured rounds, and rotating method order. Results are CPU costs, not GPU
execution timings or guaranteed speedups.

`CurveAdaptive.SubmissionBenchmark` submits 64 offscreen Spline paths per frame,
with 4, 32, or 128 intervals. It emits `integration_timing` in nanoseconds per
path and `integration_triangles` per draw. After eight warmup rounds, 32 measured
rounds alternate the method order; medians and p10/p90 are exported. Legacy
conversion allocates the temporary outline used by its former draw path; density
calls `Spline2D::draw` with its renderer-owned output. Both use target 0.5 and
6-pixel Square strokes. Frame update, presentation, and GPU waits are excluded.
