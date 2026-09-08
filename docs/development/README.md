# Development guide

[AGENTS.md](../../AGENTS.md) defines repository-wide rules;
[docs/AGENTS.md](../AGENTS.md) defines documentation ownership and organization.
This page describes the existing validation workflows.

## Build and test

On macOS, run from the repository root, outside the sandbox:

```sh
./macOS/run-tests.sh '--test-case=*Mesh3D*'  # Focused iteration
./macOS/run-tests.sh                       # Full suite
./macOS/validate-projects.sh               # Project-file syntax
```

The runner builds and launches the test application. `CONFIGURATION` selects the
build configuration (default `Debug`); other arguments are passed to doctest.
The project validator checks Xcode/Visual Studio syntax, not file-registration
completeness or Windows compilation.

## Test output and configuration

[RunTest](../../Test/Siv3DTest.cpp) verifies the platform `App/` working directory
and clears `Test/output/` before and after the suite. Cleanup failure fails the
run; crashes can leave output for the next run to clear. The directory is shared,
so run only one engine test suite per checkout. `Test/data/` holds read-only
fixtures. Engine startup and build tools manage their own platform caches.

Retained output is documented with its producer:
[Array instrumentation](../array/testing.md#isolated-instrumentation-on-macos),
[Mesh3D preview](../mesh3d/preview.md), and the complete programs in
[Test/Manual](../../Test/Manual/). Temporary Python self-test fixtures are removed
on scope exit. The test configuration policy is in
[AGENTS.md](../../AGENTS.md#test-output-and-configuration).

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
