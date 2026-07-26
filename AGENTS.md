# Repository instructions

- When creating or editing source-code files, use CRLF line endings.
- Preserve CRLF in existing source-code files; do not introduce mixed LF/CRLF line endings.
- Before finishing a source-code change, verify the line endings of every modified source file.

# macOS development

- After changing macOS or shared code, run `./macOS/run-tests.sh` from the repository root.
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
- Preserve the user's uncommitted changes and do not revert unrelated diffs.
