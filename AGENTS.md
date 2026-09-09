# Repository instructions

- Use LF line endings in source files, preserve LF in existing ones, and verify the line endings of every modified source file before finishing. Byte-exact test fixtures are exempt: mark them as binary in the nearest `.gitattributes`, do not normalize them, and verify their actual bytes.
- Run `git diff --check` before finishing any code change.
- Preserve the user's uncommitted changes and do not revert unrelated diffs.
- Treat `Siv3D/src/ThirdParty/` as vendored code and do not change it unless the task requires it. Suppress third-party warnings with target- or file-scoped build settings instead of source edits.

# Runtime performance

Siv3D is used for games and other real-time applications, so treat runtime performance as an important design constraint for code that may run frequently or process substantial data.

- Consider avoidable allocations, copies, and repeated work, and inspect nearby implementation patterns before choosing an approach.
- Prefer designs that allow storage or state to be reused. When both destination-writing and value-returning forms are appropriate, keep one canonical implementation and make the convenience form a wrapper around it.
- Get the user's explicit approval before adding runtime overhead (branches or extra arithmetic) to a frequently called function to defend against infinities, NaNs, extreme values, or similarly exceptional inputs.

# Public APIs and tests

- When adding or changing a public API, update its Doxygen documentation and add focused tests for normal, boundary, and failure or round-trip cases as applicable.
- Treat pre-implementation API documentation as provisional. If implementing it reveals that a guarantee or permissive precondition would require unusually complex or costly code, ask the user whether to revise the contract instead of silently preserving it.
- Do not force new tests into an unrelated existing test file. Add a dedicated `Test/Test_<Feature>.cpp` when appropriate.
- Store visual or interactive test programs under `Test/Manual/` as self-contained Markdown documentation with an English description, execution steps, expected results, and complete sample code. Do not leave temporary visual tests in a platform `Main.cpp`.

# Public API documentation

- Document facts that affect how callers use the API: preconditions, return values, failure behavior, side effects, ownership, and non-obvious boundary cases. Omit assurances of expected behavior, such as Unicode support or an empty file having size zero.
- Describe observable behavior, not implementation steps, bug-fix history, or verification status. A fix that restores an already documented contract does not by itself require new prose. Add platform notes only for meaningful differences in the public contract; combine identical behavior across platforms.
- Keep each fact in the appropriate Doxygen field. Do not repeat `@param` or `@return` information in `@remark`. Retain examples that help readers understand an operation, including ordinary cases. Prefer input/output pairs, and improve incomplete examples rather than deleting them merely because the behavior can be inferred from prose.
- State concrete conditions for empty or absent results instead of calling every such result a failure. Preserve useful guarantees, such as whether partial results are returned or completed side effects are rolled back.
- When simplifying documentation, check the implementation and tests so that the wording does not silently strengthen guarantees or erase actual platform differences.

# Project files

- When adding, removing, or renaming shared source files, public headers, or tests, update the macOS Xcode project and the Windows Visual Studio project and its `.filters` file. Add new public headers to `Siv3D/include/Siv3D.hpp` when appropriate.
- Validate changed project files with `./macOS/validate-projects.sh`. If the current host cannot validate a changed platform project, report that limitation explicitly.

# Build and test

- Determine the host OS first, and never run build or test commands intended for a different host OS.
- If Windows tests report `EXCEPTION_ILLEGAL_INSTRUCTION` or exit with an incomplete test report after an incremental build, preserve the report and follow the [clean-build diagnosis](docs/development/README.md#windows-incremental-build-failures) before repeating the failing run or attributing it to source changes. Reverting source and relinking incrementally does not rule out stale build artifacts.
- After changing shared code, run the full automated test suite available for the current host. If no automated test workflow is documented for that host, do not substitute another platform's workflow; report the unverified items explicitly.
- On macOS that suite is `./macOS/run-tests.sh`, run from the repository root; pass `'--test-case=<pattern>'` for focused iteration. It relies on the `--test-only` early-exit block in `macOS/Main.cpp`, so keep that block intact and keep test-only logging and configuration in `Test/`.
- Run Xcode and Metal builds outside the sandbox. A Metal Toolchain lookup failure inside the sandbox does not mean that it is not installed.

# Test output and configuration

- Prefer in-memory validation unless file I/O is under test. Use `Test::OutputPath()` for files under `Test/output/`; keep `Test/data/` read-only.
- Let the test runner own output setup and cleanup; restore process state changed by a test. Retained artifacts belong in manual tests or tools with documented output locations. See [the development guide](docs/development/README.md#test-output-and-configuration).
- Configure tests through explicit arguments, not ambient environment variables. Limit environment-variable use to platform/tool requirements or environment-handling tests, with changes scoped to the relevant process or restored afterward.

# Documentation organization

- Store durable usage guides and development knowledge under `docs/<subject>/`, with a subject `README.md` linked from `docs/README.md`. Follow `docs/AGENTS.md` for documentation changes.
- Keep pending work in root `TODO.md`; place unadopted designs in the relevant subject's `proposals/` directory with an explicit status. Root README and AGENTS remain entry points.
- Update affected links when moving documents and run `python3 tools/check_docs.py` after documentation changes.
