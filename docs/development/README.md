# Development guide

[AGENTS.md](../../AGENTS.md) defines repository-wide rules;
[docs/AGENTS.md](../AGENTS.md) defines documentation ownership and organization.
This page describes the existing validation workflows.

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
zero-exit run, the runner appends captured test stdout/stderr to the console
report and prints it, including benchmark tables. The empty `runner.lock` file may remain after a
run; its existence does not indicate a held lock. The early-exit block in
`WindowsDesktop/Main.cpp` is required by this workflow. Windows registration
validation checks the Visual Studio project and its filters; it does not
validate the Xcode project.

After changing the runner, use `python tools/test_windows_runner.py` to check its
success/failure handling, argument forwarding, lock release and timeout cleanup
in a disposable checkout. This requires Windows' .NET Framework C# compiler and
PowerShell 7; it uses a small fake application and does not build or launch Siv3D.
Also run the real suite to verify the build and application integration.

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
name, wildcard, or tag runs it; there is no `--no-skip` option. Use
`~[.benchmark]` alongside a broad positive filter to exclude it.

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

### Windows incremental build failures

If a test run displays `EXCEPTION_ILLEGAL_INSTRUCTION`, inspect the build and
runner reports before rerunning it. An incomplete Catch2 XML report is a failed
run even when the application exits with code zero. Repeatedly running the same
executable can repeatedly display exception dialogs without improving the diagnosis.

A texture-lifetime regression investigation encountered this in
`TOML.getOpt_getOr_get`: the focused texture tests passed, while the full suite
and an isolated TOML run terminated. Restoring the renderer source and rebuilding
incrementally produced the same termination. The engine had been recompiled, but
the test application still reused compilation/link-time artifacts. Rebuilding the
test application, including its PCH, object files and LTCG artifacts, made the
full suite pass without changing TOML code. This supports stale or inconsistent
incremental artifacts as the cause; it does not identify the exact faulty artifact
or establish that every illegal-instruction exception has this cause.

Use this order when investigating a recurrence:

1. Preserve the console/XML reports under `WindowsDesktop/Intermediate/TestReports/`.
   Record the last test started, the exception, configuration, toolchain and whether
   the build reused object files or LTCG output. The last test name is a starting
   point for investigation, not proof that its implementation is faulty.
2. Stop repeated runs of the same binary. Before comparing current and reverted
   source, establish a clean build. An engine rebuild alone does not ensure that
   the test application's PCH, objects or LTCG artifacts have been regenerated.
3. With no other runner or IDE build active in this checkout, rebuild the matching
   configuration using MSBuild `Rebuild`, then run the full suite. Use the commands
   below from the repository root in PowerShell 7. Do not manually delete broad
   build directories or alter unrelated source to work around the exception.
4. Confirm a complete XML report with no failures. If the cleanly rebuilt suite passes,
   describe the artifact explanation as supported by the result, without claiming
   a source fix. If the exception persists, preserve the new evidence and investigate
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

## Performance experiments

The [Windows BinaryFileReader benchmark](binary-file-reader-benchmark.md) compares
the current Win32 reader with the historical `ifstream` implementation and
experimental buffer sizes using controlled I/O patterns and real asset workloads. It builds and runs independently of the test app.

The [TextFileReader benchmark](text-file-reader-benchmark.md) measures UTF-8
whole-file and line reading with generated application workloads, memory/file
inputs, reusable destinations, and the JSON loading path. It is an opt-in test.

## Test output and configuration

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
[Test/Manual](../../Test/Manual/). Temporary Python self-test fixtures are removed
on scope exit. The test configuration policy is in
[AGENTS.md](../../AGENTS.md#test-output-and-configuration).

The [RemoveContents Trash check](../../Test/Manual/FileSystemRemoveContentsTrash.md)
uses a separate macOS application to verify native Trash behavior and documents
cleanup of its retained fixture and Trash items.

The [Renderer2D RMS scaling check](../../Test/Manual/Renderer2DRMSScaling.md)
compares tessellation, line-width compensation, and patterns under nonuniform
scaling and rotation on D3D11 and Metal.

The [Pattern gallery](../../Test/Manual/PatternGallery.md) provides editable presets
for the six constant-parameter pattern shaders and compares object zoom, pattern scale compensation,
and fixed-scene presentation scaling.

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
