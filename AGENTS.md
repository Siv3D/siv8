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
- Do not force new tests into an unrelated existing test file. Add a dedicated `Test/Test_<Feature>.cpp` when appropriate.
- Store visual or interactive test programs under `Test/Manual/` as self-contained Markdown documentation with an English description, execution steps, expected results, and complete sample code. Do not leave temporary visual tests in a platform `Main.cpp`.

# Project files

- When adding, removing, or renaming shared source files, public headers, or tests, update the macOS Xcode project and the Windows Visual Studio project and its `.filters` file. Add new public headers to `Siv3D/include/Siv3D.hpp` when appropriate.
- Validate changed project files with `./macOS/validate-projects.sh`. If the current host cannot validate a changed platform project, report that limitation explicitly.

# Build and test

- Determine the host OS first, and never run build or test commands intended for a different host OS.
- After changing shared code, run the full automated test suite available for the current host. If no automated test workflow is documented for that host, do not substitute another platform's workflow; report the unverified items explicitly.
- On macOS that suite is `./macOS/run-tests.sh`, run from the repository root; pass `'--test-case=<pattern>'` for focused iteration. It relies on the `--test-only` early-exit block in `macOS/Main.cpp`, so keep that block intact and keep test-only logging and configuration in `Test/`.
- Run Xcode and Metal builds outside the sandbox. A Metal Toolchain lookup failure inside the sandbox does not mean that it is not installed.
