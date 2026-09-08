# Development guide

[AGENTS.md](../../AGENTS.md) defines repository-wide rules;
[docs/AGENTS.md](../AGENTS.md) defines documentation ownership and organization.
This page describes the existing validation workflows.

## Build and test

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
