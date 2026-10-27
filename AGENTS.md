# Repository instructions

## Siv3D design priorities

Siv3D is used to create games and other interactive applications in C++. Preserve
its emphasis on expressing useful operations in a small amount of clear code,
with runtime performance suitable for repeated use in a real-time frame loop.

- Start API design with realistic caller code. Favor readable expressions, useful defaults, and operations that compose naturally with Siv3D's existing types. Keep naming, argument order, and behavior consistent across related APIs; add conveniences when they meaningfully simplify common tasks.
- Make ease of use and efficiency work together. Consider the cost of the whole caller operation, including intermediate objects and repeated preparation. Simple drawing calls should have efficient direct paths where worthwhile, and repeated work should be able to reuse storage or prepared data.
- Judge drawing APIs by their visible results as well as their contracts and measured cost. Handle boundaries reached naturally during animation, such as zero size or short paths. Balance visual quality and bounded work; describe practical limits instead of promising expensive perfection for every geometry.
- Keep API documentation useful for creating things: clear conditions and outcomes, concise explanations, and concrete examples. Let implementation and verification details live in development guides.

## Repository layout

Use this map to locate existing work and choose where new files belong.

| Location | Role |
|---|---|
| `Siv3D/include/Siv3D/` | Public headers and Doxygen; inline definitions in `detail/*.ipp`. `Siv3D/include/Siv3D.hpp` is the umbrella header. |
| `Siv3D/src/Siv3D/` | Shared engine implementations and internal helpers, organized by subsystem. |
| `Siv3D/src/Siv3D-Platform/` | Platform-specific implementations and backend integrations. |
| `macOS/`, `WindowsDesktop/` | Platform projects, build/test runners, application entry points, and app resources. |
| `Test/` | Automated tests and shared test support. `Test/data/` holds read-only fixtures; the runner manages `Test/output/`. |
| `Test/Manual/` | Self-contained Markdown programs for visual or interactive checks, with an English description, execution steps, expected results, and complete sample code. |
| `tools/` | Reusable development utilities: validators, diagnostics, benchmarks, and previews. Document usage and output locations in the relevant subject guide. |
| `docs/` | Durable usage, design, and development guides, organized by subject and indexed in [docs/README.md](docs/README.md). Unadopted designs belong in the subject's `proposals/`. |
| `Siv3D/include/ThirdParty/`, `Siv3D/src/ThirdParty*/`, `Test/ThirdParty/`, `Dependencies/` | Bundled external headers, sources, and dependency packages. |
| Root `README.md`, `AGENTS.md`, `TODO.md` | Project introduction, contributor instructions, and unfinished work respectively. |

## Repository care

- Preserve the user's uncommitted changes; do not revert unrelated diffs.
- Use and verify LF in every modified source file. Byte-exact fixtures are exempt: mark them binary in the nearest `.gitattributes`, do not normalize them, and verify their bytes.
- In public classes, keep declarations and Doxygen in `.hpp`, ordinary inline bodies in the corresponding `.ipp` files. Keep non-public class implementations, internal helpers, and hidden friends in `.hpp`; keep `= default` / `= delete` on their declarations.
- Treat `Siv3D/src/ThirdParty/` as vendored. Edit it only when the task requires it; suppress third-party warnings through target- or file-scoped build settings.

## Design and public APIs

- Examine callers, callees, and responsibilities before adding local complexity. Prefer fixes at its source; propose broader refactoring when justified and split it into reviewable stages.
- Treat runtime performance as a real-time constraint. Inspect nearby patterns; avoid unnecessary allocations, copies, and repeated work. Reuse storage/state, and make value-returning conveniences wrap a canonical destination-writing implementation when both forms fit.
- Get the user's explicit approval before adding runtime overhead (branches or extra arithmetic) to a frequently called function to defend against infinities, NaNs, extreme values, or similarly exceptional inputs.
- For public API changes, update concise Doxygen contracts and add focused normal, boundary, and failure or round-trip tests as applicable. Share common contracts instead of duplicating them.
- Treat pre-implementation API documentation as provisional. If a guarantee or permissive precondition requires unusually complex or costly code, ask the user whether to revise the contract instead of silently preserving it.
- Treat GPU upload destinations as write-only. Compute positions, UVs, colors, index offsets, and winding from source data or CPU locals; do not read back, use read/modify/write operations, or chain assignments through destination elements. Keep necessary buffer-growth copies inside backend storage management.
- Preserve the write-only cursors from [Vertex2DBufferPointer](Siv3D/src/Siv3D/Renderer2D/Vertex2DBufferPointer.hpp) throughout geometry helpers. Do not expose raw destination pointers or readable conversions to bypass them. Extend the compile-time rejection checks in [Test_Renderer2DUpload.cpp](Test/Test_Renderer2DUpload.cpp) when changing this boundary.

## Tests and test output

- Keep tests focused on their feature; add a dedicated `Test/Test_<Feature>.cpp` rather than forcing new tests into an unrelated file. Store visual/interactive programs in `Test/Manual/` as described above; do not leave temporary tests in a platform `Main.cpp`.
- Prefer in-memory validation unless file I/O is under test. Use `Test::OutputPath()` for files under `Test/output/`; keep `Test/data/` read-only. Let the runner own output setup and cleanup, and restore process state changed by a test. Retained artifacts belong in manual tests or tools with documented output locations.
- Configure tests through explicit arguments, not ambient environment variables. Limit environment-variable use to platform/tool requirements or environment-handling tests, scoped to the relevant process or restored afterward.

## Validation

- Determine the host OS first; never run another OS's build/test workflow. After shared-code changes, run the full host suite from the repository root: `./macOS/run-tests.sh` on macOS or `./WindowsDesktop/run-tests.ps1` on Windows. If no host workflow is documented, report unverified work rather than substituting another platform's workflow.
- For focused iteration, use `./macOS/run-tests.sh '--test-case=<pattern>'` or, on Windows, `./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=<pattern>'`. Preserve the `--test-only` early-exit block in `macOS/Main.cpp`; keep test-only logging and configuration in `Test/`.
- Run Xcode and Metal builds outside the sandbox. A sandboxed Metal Toolchain lookup failure does not establish that the toolchain is missing.
- For Windows build/test failures, preserve build logs and the reports from `WindowsDesktop/Intermediate/TestReports/` and follow the [diagnosis guide](docs/development/README.md#windows-incremental-build-failures) before retries or source attribution. Treat MSVC `C1001` as a likely toolchain issue; document reusable workarounds, not incident histories.
- For GPU upload-path changes, run `Renderer2DUpload.*` and affected geometry/image tests, then the full host suite. Compare Release performance in ordinary CPU memory and the actual backend's upload memory; a heap-array benchmark alone is insufficient. Follow the [upload guide](docs/renderer2d/README.md#measuring-cpu-writes-to-upload-buffers), including generated-code inspection when destination access is uncertain. Keep hardware timing thresholds out of correctness tests.
- When adding, removing, or renaming shared sources, public headers, or tests, update the Xcode project, Visual Studio project, and `.filters`; add public headers to `Siv3D/include/Siv3D.hpp` when appropriate. Validate changed projects with `./macOS/validate-projects.sh`; report any platform project the host cannot validate.
- Run `git diff --check` before finishing code or documentation changes. For documentation changes, also run `python3 tools/check_docs.py` and update affected links. Documentation-only changes do not require an engine rebuild; verify executable examples if changed.
- At feature completion, follow [docs/AGENTS.md](docs/AGENTS.md) to consolidate durable knowledge and remove superseded galleries, completed handoffs, obsolete proposals, and temporary artifacts within scope. Keep one canonical gallery per feature, separate manual programs only for distinct validation needs, and use Git history instead of archival copies.

## Task-specific rules

Read and follow the applicable sections before the listed work; unrelated sections need not be loaded.

| Work | Required guidance |
|---|---|
| Writing or revising public API documentation | [API documentation](docs/development/api-documentation.md) |
| Adding or changing tests or visual programs | [Test authoring](docs/development/README.md#test-authoring) and [output/configuration](docs/development/README.md#test-output-and-configuration) |
| Building or running tests | [Build and test](docs/development/README.md#build-and-test), using the host's section |
| GPU upload paths in shared code or platform backends | [Upload rules and validation](docs/renderer2d/README.md#measuring-cpu-writes-to-upload-buffers) |
| Documentation changes or feature completion | [Documentation ownership and cleanup](docs/AGENTS.md) |

Optimize this file for reliable work and low lookup cost, not minimum length. Keep frequent decisions, approval conditions, key paths, and routine commands here. Detailed guidance may move to subject guides with direct task-specific links; preserve the underlying knowledge and verify that it remains discoverable. Consolidate repetition without removing useful context or adding unnecessary navigation.
