# Development guide

[AGENTS.md](../../AGENTS.md) defines repository-wide rules;
[docs/AGENTS.md](../AGENTS.md) defines documentation ownership and organization.
This page describes the existing validation workflows. For public-header
Doxygen changes, follow [API documentation](api-documentation.md).

For the vendored window library and its local integration changes, see
[GLFW maintenance](glfw.md).

## Build and test

On Windows, use PowerShell 7, Python, and Visual Studio C++ tools. Run from the
repository root:

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=*Mesh3D*', '--durations', 'yes'
./WindowsDesktop/run-tests.ps1
python tools/check_test_projects.py --windows-only
```

The [Windows runner](../../WindowsDesktop/run-tests.ps1) can be invoked from any
working directory. It checks Visual Studio test registration, builds the x64
solution (default `Release`), and launches the test application with `--test-only`.
Success requires exit code zero, a complete Catch2 XML report with no failures,
and at least one executed, non-skipped test. A filter that matches nothing is an error.

`-Configuration Debug` selects Debug; `-Jobs 4` controls build parallelism.
`-SkipBuild` uses the existing application, which must already match the current
sources and selected configuration. Pass each Catch2 execution argument as a
separate element of `-TestArguments`. Report output, reporter selection, color,
verbosity, query-only options and `--allow-running-no-tests` are reserved by the
runner. Pass short options separately from their values (for example, `-d`,
`yes`); combined short options and `--` are rejected. Use
`Get-Help ./WindowsDesktop/run-tests.ps1 -Detailed` for script help.

`-TimeoutSeconds 600` limits the test application's runtime, excluding the build;
raise it for intentionally long tests. Timeout or interruption terminates the
launched application and its child processes. The runner holds a file lock
throughout the build and test run, so a second runner in the same checkout fails
immediately. This lock does not coordinate manually launched tests or IDE builds.

Console (`.txt`) and XML reports are retained in
`WindowsDesktop/Intermediate/TestReports/` and can be
removed when no longer needed. After reading a complete XML report from a
zero-exit run, the runner appends captured test stdout/stderr from
`TestCase/OverallResult` to the console report and prints it, including benchmark
tables. The empty `runner.lock` file may remain after a run; its existence does
not indicate a held lock. The early-exit block in
`WindowsDesktop/Main.cpp` is required by this workflow. Windows registration
validation checks the Visual Studio project and its filters; it does not
validate the Xcode project.

After changing the runner, use `python tools/test_windows_runner.py` to check its
success/failure handling, captured output, argument forwarding, lock release and
timeout cleanup in a disposable checkout. This requires Windows' .NET Framework C# compiler and
PowerShell 7; it uses a small fake application and does not build or launch Siv3D.
The fixture must match the vendored XML reporter's structure. Also run the real
suite to verify the build and application integration.

On macOS, run from the repository root, outside the sandbox:

```sh
./macOS/run-tests.sh '--test-case=*Mesh3D*'  # Focused iteration
./macOS/run-tests.sh                       # Full suite
./macOS/validate-projects.sh               # Project syntax and shared tests
```

For the public Renderer2D CPU benchmark, measurement boundaries, and a standalone
program, see [Renderer2D pipeline state benchmark](../../Test/Manual/Renderer2DPipelineState.md).

The runner checks test registration, then builds and launches the application.
`CONFIGURATION` selects the build configuration (default `Debug`); other arguments
are passed to the shared Catch2 runner. `--test-only` and `--test-verbose` are
application options; the latter enables the normal engine logs.
Preserve the `--test-only` early-exit block in `macOS/Main.cpp`; keep test-only
logging and configuration in `Test/`.
On macOS, the shared runner also consumes Xcode's automatically supplied
`-NSDocumentRevisionsDebugMode YES` (or `NO`) preference before parsing test
options. Running from Xcode with no user arguments executes the default suite.

`--test-case=<pattern>` (or `--test-case <pattern>`) remains a convenience alias
for Catch2's positional test specification. Empty filters are rejected. Use
Catch2 syntax for other options: `--durations yes` for timings and `--list-tests`
when launching the macOS runner or the executable to list cases. Windows' runner
reserves listing options because it requires actual test execution. Unknown
options fail instead of being ignored.

Filter syntax follows [Catch2](https://github.com/catchorg/Catch2/blob/v3.16.0/docs/command-line.md):
`Array*`, `*Mesh3D*`, exact names, and tags are supported. This alias does not
emulate the former framework's full filter syntax. The `[.benchmark]` tag hides
the opt-in TextFileReader benchmark from the default suite. An explicit matching
name, wildcard, or tag runs it; there is no `--no-skip` option. Put
`~[benchmark]` before a name pattern to exclude benchmark cases.

### Draw submission benchmark

The opt-in [draw submission benchmark](../../Test/Test_Renderer2DDrawSubmission.cpp)
measures the CPU work inside public drawing calls, including vertex generation
and command registration. It excludes `System::Update()`, command replay and
presentation; the existing pipeline benchmark measures a different interval.
Run it alone in Release, with other builds and benchmarks stopped:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Renderer2D.draw_submission_cpu_benchmark'
```

```powershell
./WindowsDesktop/run-tests.ps1 -Configuration Release -TestArguments '--test-case=Renderer2D.draw_submission_cpu_benchmark'
```

The benchmark uses offscreen geometry, disables VSync for its duration, warms
the CPU before each sample, and restores the scene and shader settings. On
macOS, it temporarily requests interactive thread QoS to reduce scheduling
variation. Each material is measured with stable state, alternating color/blend state, custom
VS, custom PS, and both custom stages. The custom stages use the corresponding
engine shaders so the same workload is available on both backends. Triangle
and draw counts are checked outside the timed region.

Output reports elapsed median/p10/p90 (`ns`, `p10`, `p90`) and median thread CPU
time (`cpu_ns`), all in nanoseconds per draw. Thread CPU timer resolution depends
on the OS; retain the elapsed distribution as well. Compare repeated runs of
saved before/after Release builds in alternating order under the same conditions.
Inspect optimized code for additional calls, branches or allocations if a result
is unclear. Timing differences within run-to-run variation do not establish a
speedup, and these CPU measurements do not measure GPU time or application FPS.

### Window UI tests

Tests that enter fullscreen, maximize/minimize, move between monitors, or warp
the desktop pointer use `[.window-ui]`. They are excluded when no filter is
specified. Pure scene-policy calculations, rendering, and local window resize
checks remain in the default suite; the default suite still opens a test window.
Keep the existing test bodies, restoration order, and 500 ms transition pauses
when assigning this tag. Do not skip individual assertions based on the run mode.

Use the shared tag to run these native UI checks across all test files:

```sh
./macOS/run-tests.sh '--test-case=[window-ui]'
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=[window-ui]'
```

The existing `[.fullscreen]` and `[.screen-coordinates]` tags remain available:
select `[fullscreen]` for fullscreen transitions across all suites, or
`[screen-coordinates]` for monitor placement and pointer checks. Selecting
`WindowFullscreen.*` only runs cases in that suite, not fullscreen cases in
`WindowSceneSync` or `WindowMacOS`.

A matching name or wildcard also selects hidden tests. For focused runs without
the UI group, put its exclusion before each name pattern:

```sh
./macOS/run-tests.sh '--test-case=~[window-ui]Scene*,~[window-ui]WindowSceneSync.*'
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=~[window-ui]Scene*,~[window-ui]WindowSceneSync.*'
```

Commas separate alternative filters. Repeat the exclusion in each alternative;
a standalone comma-separated exclusion is not a global exclusion. An explicit
`*` also includes other hidden groups such as benchmarks. Prefer no filter for
the normal full suite. To inspect selection without executing UI tests, launch
the built executable with `--test-only --list-tests` and the intended filter;
the Windows wrapper reserves listing options for direct executable use.

For tag-only changes, confirm that test bodies are unchanged, inspect case names
and tags, and run the normal suite. Check selection as sets: the default contains
all non-hidden cases, no hidden cases, and the UI group contains the intended
native scenarios. Avoid fixed expected counts across platforms. Retagging alone
does not require replaying fullscreen transitions.

Run the Windows initial-placement check in a fresh process on its own so that
`SetPos()` runs on the initially hidden HWND before the first `System::Update()`:

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=ScreenCoordinates.Windows.initial_placement'
```

The same case can run later in the full suite; its report records initial
visibility. See [window validation](../window/README.md#validation) for native
coverage and required monitor conditions.

### Validation evidence

Record the host, backend, configuration, selected cases, report outcome, and any
conditions that were unavailable or skipped. Keep logs and detailed results with
the runner's artifacts; durable guides describe procedures, and `TODO.md` holds
remaining work. Distinguish injected inputs, real native API paths, physical
key/mouse observation, and OS setting changes or monitor unplugging. One does
not establish the results of another. Concurrency tests supplement ownership and
lock-scope review; passing tests alone does not establish freedom from data races.

### Catch2

The vendored [Catch2 sources](../../Test/ThirdParty/Catch2/catch_amalgamated.hpp)
and [implementation](../../Test/ThirdParty/Catch2/catch_amalgamated.cpp) are the
unmodified amalgamated distribution from the pinned
[v3.16.0 release](https://github.com/catchorg/Catch2/releases/tag/v3.16.0), under the
[Boost Software License](../../Test/ThirdParty/Catch2/LICENSE.txt). Both test
projects compile the implementation directly with `CATCH_AMALGAMATED_CUSTOM_MAIN`;
`RunTest()` owns the session. Updating Catch2 requires replacing both distribution
files together and validating both platform projects. The Windows test target
uses its normal Release optimization settings, including LTCG. The external
implementation opts out of the application's PCH and forced includes.

[Siv3DTestFramework.hpp](../../Test/Siv3DTestFramework.hpp) supplies the shared
framework include, argument adapter and `Test::Approx`. The comparison helper
uses `abs(actual - expected) < epsilon * (scale + max(abs(actual), abs(expected)))`,
with default `scale = 1` and `epsilon = 100 * numeric_limits<float>::epsilon()`.
This preserves existing numerical tolerances; it differs from `Catch::Approx`.

The project validator checks syntax and registration of `Test/Test_*.cpp` in
Xcode's `Siv3D-Test` Sources, Visual Studio's test project, and its filters.
Engine sources are excluded; platform-specific engine membership is intentional.
The [registration checker](../../tools/check_test_projects.py) has `--self-test`;
it does not compile Windows code.

### Log assertions

Keep `std::clog` redirection inside dedicated test helpers that return the captured
text before assertions run. Catch2's capturing reporters, including XML, retarget
the stream while processing assertions, so a capture must not span `CHECK`,
`REQUIRE`, or other Catch2 reporting calls, including those in called helpers.
[Test_MetalFrameErrors.cpp](../../Test/Test_MetalFrameErrors.cpp) separates capture
of error reporting, completion callbacks, draining, and concurrent completion.
Each helper restores the previous stream buffer through RAII and joins its workers
before reading the captured text. Stream redirection is process-wide; tests using
it must not run concurrently with other tests that write to or redirect that stream.

After changing log capture, validate the focused tests with both reporters on
macOS, then run the full suite once:

```sh
./macOS/run-tests.sh '--test-case=MetalFrameErrors.*' --reporter console
./macOS/run-tests.sh '--test-case=MetalFrameErrors.*' --reporter xml --out /tmp/metal-frame-errors.xml
./macOS/run-tests.sh --reporter xml --out /tmp/siv3d-tests.xml
```

### Isolated Image checks on macOS

Run [run-image-checks.sh](../../tools/run-image-checks.sh) outside the sandbox:

```sh
./tools/run-image-checks.sh
./tools/run-image-checks.sh /tmp/siv8-image-checks
./tools/run-image-checks.sh /tmp/siv8-image-checks-x86 x86_64
```

The first optional argument selects the output directory; otherwise the script
creates and prints a temporary directory. The second selects `arm64` or `x86_64`
and defaults to the host architecture. Running an x86_64 build on Apple Silicon
requires Rosetta. The directory retains the instrumented executable
and may be removed after inspection. The checker compiles the real Image sources
and [Image tests](../../Test/Test_Image.cpp) with AddressSanitizer and
UndefinedBehaviorSanitizer, without starting the engine. It checks pixel-count
boundaries, writes beyond the logical image, storage reuse, and alignment. The
normal engine tests use only Image's public API. In the isolated build, the test
translation unit includes the SIMD implementation `.cpp` directly, so it can also
check individual kernels without exporting them or adding an engine header.
The kernels retain internal linkage in an unnamed namespace. SSE fallback tests
also run on AVX2-capable CPUs; only kernels available on the executing architecture
are tested. The x86_64 build uses
SSE4.2 as its baseline and enables AVX2 only for the AVX2 kernel functions. AVX2
tests run only when the executing CPU supports them.

The isolated executable additionally replaces `posix_memalign` at link time.
A scoped, thread-local probe records allocation requests or returns `ENOMEM` to
verify that allocation failure preserves the image. The probe is compiled only
when the script supplies `SIV3D_IMAGE_ALLOCATION_TEST`; the engine and its normal
test application contain no allocation override. These checks supplement the full
engine suite and do not verify the Windows allocator or MemorySanitizer behavior.

### MSVC warnings in tests

For warning cleanup, compare clean test-application builds in Release and Debug
with the existing `/W4` settings. When the engine library already matches the
sources, configuration and toolchain, the application-only `Rebuild` command below
regenerates every test translation unit without rebuilding the engine. Keep the build logs and follow
template instantiation traces: a warning reported in a standard-library header
can originate in either an expected test value or an engine template.

- For C4244/C4305, check which layer owns the numeric conversion before adding
  casts to callers. APIs intended to accept arithmetic inputs should perform the
  conversion at their implementation boundary. Otherwise, match fixture types to
  the API while preserving calculation precision and comparison tolerances.
- For C4244 in expected values, construct templated wrappers with typed values:
  `Optional<Int>{ Int{ -1 } }`. An integer literal passed through a forwarding
  constructor can otherwise cause a narrowing warning inside the standard library.
- For C4389, compare file I/O counts in the API's signed count type. For small fixtures,
  convert the known representable fixture size once and reuse it for the write and
  expected result; retain negative-result detection.
- For C4702, Catch2 `FAIL` does not return. Remove unreachable returns after it;
  keep the fatal assertion rather than changing it to a nonfatal check.

Preserve the input types when conversion or overload selection is what a test
exercises. If the warning originates in an engine implementation, keep its test
coverage and handle the implementation separately from test-only cleanup. Avoid
blanket warning suppression or edits to vendored Catch2 sources.

### Windows incremental build failures

Distinguish compiler/linker failures from runtime failures. Treat MSVC `C1001`
as a likely toolchain issue and preserve its build log before changing source.
Document reusable workarounds; retain incident-specific evidence in reports.
For an exception such as `EXCEPTION_ILLEGAL_INSTRUCTION` or an incomplete test
report after an incremental build, use the diagnosis below. Missing or incomplete
XML is a failed run even if the process exits with zero. Clean rebuilding is a
diagnostic step, not a requirement for every change.

1. Preserve the console/XML reports under `WindowsDesktop/Intermediate/TestReports/`.
   Keep the build log too. Record the last test started, the exception, configuration,
   toolchain and whether the build reused object files or LTCG output. The last test
   name is a starting point for investigation, not proof that its implementation is faulty.
2. Stop repeated runs of the same binary. Before comparing current and reverted
   source, establish a clean build. An engine rebuild alone does not ensure that
   the test application's PCH, objects or LTCG artifacts have been regenerated.
3. With no other runner or IDE build active in this checkout, rebuild the matching
   configuration using MSBuild `Rebuild`, then repeat the failing selection and
   required regression suite under the same conditions. Use the commands
   below from the repository root in PowerShell 7. Do not manually delete broad
   build directories or alter unrelated source to work around the exception.
4. Require complete XML with no failures and inspect the selected cases and skips.
   A pass after rebuilding alone is not a source fix or proof of the exact faulty artifact.
   If the exception persists, preserve the new evidence and investigate
   the faulting instruction, call stack and CPU instruction requirements instead
   of continuing blind retries.

The conservative recovery rebuilds both the engine and test application:

```powershell
$siv3dVswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
$siv3dVs = & $siv3dVswhere -latest -products '*' -requires Microsoft.Component.MSBuild Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ($LASTEXITCODE -ne 0 -or -not $siv3dVs) { throw 'Visual Studio C++ tools were not found.' }
$siv3dMSBuild = Join-Path $siv3dVs 'MSBuild/Current/Bin/MSBuild.exe'
$siv3dWindowsDir = (Resolve-Path 'WindowsDesktop').Path + [IO.Path]::DirectorySeparatorChar
& $siv3dMSBuild (Join-Path $siv3dWindowsDir 'OpenSiv3D.slnx') /t:Rebuild /nologo /v:minimal /m:4 /p:CL_MPCount=4 /p:Configuration=Release /p:Platform=x64
if ($LASTEXITCODE -ne 0) { throw 'Clean rebuild failed.' }
./WindowsDesktop/run-tests.ps1 -Configuration Release -SkipBuild
```

If the engine library has already been successfully rebuilt with the same
configuration and toolchain, replace the solution `Rebuild` command with this
application-only rebuild, keeping the exit-code check and full-suite run:

```powershell
& $siv3dMSBuild (Join-Path $siv3dWindowsDir 'Siv3D-Test.vcxproj') /t:Rebuild /nologo /v:minimal /m:4 /p:CL_MPCount=4 /p:Configuration=Release /p:Platform=x64 /p:BuildProjectReferences=false "/p:SolutionDir=$siv3dWindowsDir"
```

For a Debug failure, use `Debug` consistently for the rebuild and runner.
`-SkipBuild` is appropriate here only after the explicit rebuild succeeds and no
source or build settings have changed. These manual MSBuild commands do not acquire
the runner's lock; never run them concurrently with another build or test session.

If a `C1001` diagnostic identifies `HostX86/x64/CL.exe`, try the matching clean
rebuild above with `/p:PreferredToolArchitecture=x64` to select the 64-bit host
compiler. Keep the same target, toolset, configuration, and optimization settings.
Preserve both build logs and run the full suite after a successful rebuild.
Changing the host compiler and regenerating build artifacts together does not
isolate which change avoided the failure.

If MSBuild itself exits with an access violation before producing compiler
diagnostics, preserve its log and the Windows Application error events separately
from the test application's reports. On a 64-bit host, select the 64-bit MSBuild
executable explicitly before running the same recovery commands:

```powershell
$siv3dMSBuild = Join-Path $siv3dVs 'MSBuild/Current/Bin/amd64/MSBuild.exe'
```

Add `/p:PreferredToolArchitecture=x64` to select the 64-bit compiler host as well;
the target configuration and toolset stay unchanged. Require a successful build
and complete full-suite reports before accepting the recovery. A successful run
with different tool hosts does not establish the cause of the original crash.

## Performance experiments

The [Renderer2D JoinStyle benchmark](renderer2d-join-benchmark.md) separates
polygon buffering, triangulation, vertex conversion, and direct mesh emission
for Triangle and Quad GPU drawing. It measures CPU work and temporary allocations.

The [Windows BinaryFileReader benchmark](binary-file-reader-benchmark.md) compares
the current Win32 reader with the historical `ifstream` implementation and
experimental buffer sizes using controlled I/O patterns and real asset workloads. It builds and runs independently of the test app.

The [TextFileReader benchmark](text-file-reader-benchmark.md) measures UTF-8
whole-file and line reading with generated application workloads, memory/file
inputs, reusable destinations, and the JSON loading path. It is an opt-in test.

## Test authoring

Keep tests focused on the feature they exercise. Do not force new cases into an
unrelated test file; add a dedicated `Test/Test_<Feature>.cpp` when appropriate.

Store visual or interactive programs under `Test/Manual/` as self-contained
Markdown with an English description, execution steps, expected results, and
complete sample code. Do not leave temporary programs in a platform `Main.cpp`.

## Test output and configuration

Prefer in-memory checks unless file I/O is under test. Let the runner own output
setup and cleanup, and restore any process state changed by a test. Configure
tests with explicit arguments, not ambient environment variables. Use environment
variables only for platform/tool requirements or environment-handling tests,
scoped to the relevant process or restored afterward.

[RunTest](../../Test/Siv3DTest.cpp) verifies the platform `App/` working directory
and clears `Test/output/` before and after the suite. Cleanup failure fails the
run; crashes can leave output for the next run to clear. The directory is shared,
so run only one engine test suite per checkout. `Test/data/` holds read-only
fixtures. Engine startup and build tools manage their own platform caches.
Use `Test::OutputPath(U"feature/file.bin")` for test output: it returns an absolute
path beneath that root, independent of later working-directory changes.

Retained output is documented with its producer:
[Array instrumentation](../array/testing.md#isolated-instrumentation-on-macos),
[Mesh3D preview](../mesh3d/preview.md), and the complete programs in
[Test/Manual](../../Test/Manual/). Keep retained artifacts in manual tests or tools
with documented output locations. Temporary Python self-test fixtures are removed
on scope exit.

The [RemoveContents Trash check](../../Test/Manual/FileSystemRemoveContentsTrash.md)
uses a separate macOS application to verify native Trash behavior and documents
cleanup of its retained fixture and Trash items.

The [Renderer2D RMS scaling check](../../Test/Manual/Renderer2DRMSScaling.md)
compares tessellation, line-width compensation, and patterns under nonuniform
scaling and rotation on D3D11 and Metal.

The [Pattern collection](../../Test/Manual/PatternCollection.md) provides the
integrated gallery for all eleven patterns. Dedicated
[transform checks](../../Test/Manual/PatternTransforms.md) compare pattern
attachment and continuity across split geometry.

The [Polygon simplification check](../../Test/Manual/PolygonSimplification.md)
compares hole preservation, rejected simplifications, and triangulated rendering
across a range of distance thresholds.

## Documentation checks

```sh
python3 tools/check_docs.py
python3 tools/check_docs.py --self-test  # When changing the checker

git diff --check
```

The [checker](../../tools/check_docs.py) validates local Markdown file links in
root documents, docs, and Test/Manual, plus reachability from docs/README.md.
It excludes code examples, external URLs, and heading fragments. Documentation-only
changes do not require an engine rebuild; verify executable examples if changed.
