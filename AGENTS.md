# Repository instructions

- When creating or editing source-code files, use LF line endings.
- Preserve LF in existing source-code files; do not introduce mixed LF/CRLF line endings.
- Before finishing a source-code change, verify the line endings of every modified source file.
- Before finishing any code change, run `git diff --check`.
- Preserve the user's uncommitted changes and do not revert unrelated diffs.
- Treat files under `Siv3D/src/ThirdParty/` as vendored code unless the task explicitly requires changing them. Prefer target- or file-scoped build settings over source edits when suppressing third-party warnings.
- Byte-exact test fixtures are exempt from the source-code line-ending rules. Mark them as binary in the nearest `.gitattributes`, do not normalize their line endings, and verify their actual bytes.

# Runtime performance

- Before adding runtime overhead to a frequently called function to handle infinities, NaNs, extreme values, or similarly exceptional inputs, obtain the user's explicit approval. This includes adding branches or extra arithmetic for such defenses.

# Public APIs and tests

- When adding or changing a public API, update its Doxygen documentation and add focused tests for normal, boundary, and failure or round-trip cases as applicable.
- Do not force new tests into an unrelated existing test file. Add a dedicated `Test/Test_<Feature>.cpp` when appropriate.
- Store visual or interactive test programs under `Test/Manual/` as self-contained Markdown documentation with an English description, execution steps, expected results, and complete sample code. Do not leave temporary visual tests in a platform `Main.cpp`.

# Project files

- When adding, removing, or renaming shared source files, public headers, or tests, update the applicable macOS Xcode project and Windows Visual Studio project and `.filters` file.
- Add new public headers to `Siv3D/include/Siv3D.hpp` when appropriate.
- On macOS, after editing project files, validate them with:

  ```sh
  plutil -lint macOS/OpenSiv3D.xcodeproj/project.pbxproj
  xmllint --noout \
    WindowsDesktop/Siv3D.vcxproj \
    WindowsDesktop/Siv3D.vcxproj.filters \
    WindowsDesktop/Siv3D-Test.vcxproj \
    WindowsDesktop/Siv3D-Test.vcxproj.filters
  ```

- If the current host cannot validate a changed platform project, report that limitation explicitly.

# Platform-specific verification

- Determine the host OS before running platform-specific build or test commands.
- Do not run build or test commands intended for a different host OS.
- After changing shared code, run the full automated test suite available for the current host.
- If no documented automated test workflow is available for the current host, do not substitute another platform's workflow; report the unverified items explicitly.

# macOS development

- Only on macOS, use `./macOS/run-tests.sh '--test-case=<pattern>'` for focused iteration when it saves time.
- On macOS, after changing macOS or shared code, run the full `./macOS/run-tests.sh` from the repository root before finishing.
- Never run `./macOS/run-tests.sh` on a non-macOS host.
- For automated verification, always launch the test app with `--test-only`; do not enter the interactive main loop.
- `macOS/run-tests.sh` depends on the following early-exit block in `macOS/Main.cpp`. Keep it intact:

  ```cpp
  const int32 exitCode = RunTest();

  if (System::GetCommandLineArgs().contains(U"--test-only"))
  {
      System::Exit(exitCode);
      return;
  }
  ```

- Run Xcode and Metal builds outside the sandbox. A Metal Toolchain lookup failure inside the sandbox does not mean that it is not installed.
- Keep test-only logging and configuration in `Test/`, not in `macOS/Main.cpp`.
