# Development guide

[AGENTS.md](../../AGENTS.md) defines repository-wide rules;
[docs/AGENTS.md](../AGENTS.md) defines documentation ownership and organization.
This page describes the existing validation workflows.

## Build and test

On Windows, use PowerShell 7, Python, and Visual Studio C++ tools. Run from the
repository root:

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=*Mesh3D*', '--duration'
./WindowsDesktop/run-tests.ps1
python tools/check_test_projects.py --windows-only
```

The [Windows runner](../../WindowsDesktop/run-tests.ps1) can be invoked from any
working directory. It checks Visual Studio test registration, builds the x64
solution (default `Release`), and launches the test application with `--test-only`.
Success requires exit code zero, a successful doctest completion report, and at
least one executed test. A filter that matches nothing is an error.

`-Configuration Debug` selects Debug; `-Jobs 4` controls build parallelism.
`-SkipBuild` uses the existing application, which must already match the current
sources and selected configuration. Pass each doctest execution argument as a
separate element of `-TestArguments`. Report output, reporter selection, color,
quiet/minimal output, exit-code suppression and query-only options are reserved
by the runner, including their short and `dt-` aliases. `--first`/`--last` ranges
are also rejected because doctest counts matching tests before applying the
range; use test-case, test-suite or source-file filters instead. Use
`Get-Help ./WindowsDesktop/run-tests.ps1 -Detailed` for script help.

`-TimeoutSeconds 600` limits the test application's runtime, excluding the build;
raise it for intentionally long tests. Timeout or interruption terminates the
launched application and its child processes. The runner holds a file lock
throughout the build and test run, so a second runner in the same checkout fails
immediately. This lock does not coordinate manually launched tests or IDE builds.

Reports are retained in `WindowsDesktop/Intermediate/TestReports/` and can be
removed when no longer needed. The empty `runner.lock` file may remain after a
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

The runner checks test registration, then builds and launches the application.
`CONFIGURATION` selects the build configuration (default `Debug`); other arguments
are passed to doctest.
The project validator checks syntax and registration of `Test/Test_*.cpp` in
Xcode's `Siv3D-Test` Sources, Visual Studio's test project, and its filters.
Engine sources are excluded; platform-specific engine membership is intentional.
The [registration checker](../../tools/check_test_projects.py) has `--self-test`;
it does not compile Windows code.

## Performance experiments

The [Windows BinaryFileReader benchmark](binary-file-reader-benchmark.md) compares
the current Win32 reader with the historical `ifstream` implementation and
experimental buffer sizes using controlled I/O patterns and real asset workloads. It builds and runs independently of the test app.

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
