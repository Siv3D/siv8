# Development guide

The operational rules are in the repository's [AGENTS.md](../../AGENTS.md).
This guide explains where development information belongs and how to validate it.

## Sources of truth

| Information | Location |
| --- | --- |
| API signatures, defaults, preconditions, and guarantees | Public-header Doxygen |
| Usage patterns and design rationale | Subject documentation under docs |
| Fixtures, regression checks, execution steps, expected results | Test |
| Unfinished work | [TODO.md](../../TODO.md) |
| Unadopted designs | A subject's proposals directory |

Do not use Test as a general knowledge base. A manual test can explain its fixture
assumptions and expected failures; reusable modeling techniques belong in docs.
Keep complete manual test programs with their instructions so they remain runnable.
The [documentation rules](../AGENTS.md) cover names, links, and indexing.

## Build and test

Determine the host OS first and follow its documented workflow. On macOS, run
these commands from the repository root:

```sh
./macOS/run-tests.sh '--test-case=*Mesh3D*'
./macOS/run-tests.sh
./macOS/validate-projects.sh
git diff --check
```

The first command is a focused iteration example. Shared engine code changes
require the full available host suite. Xcode/Metal builds must run outside the
sandbox, and the test-only entry point in macOS/Main.cpp must remain intact.
For documentation-only changes, validate references and any moved executable
payloads; an unchanged engine does not need a rebuild solely for a document move.

Project-file changes must be validated. Report unverified platforms rather than
substituting another host's build workflow. Repository rules contain the details.

## Test output and configuration

The engine test suite uses `macOS/App/` or `WindowsDesktop/App/` as its working
directory. The macOS runner sets this explicitly, and the Windows test project
sets it for debugging. [RunTest](../../Test/Siv3DTest.cpp) checks this repository
layout before running tests or deleting output. Launching a copied executable
without that layout is not a supported test workflow.

File-I/O tests write beneath `Test/output/`; this includes binary readers/writers,
memory-mapped files, Mesh3D OBJ/MTL export, and the optional heavy compression
tests. Unique Mesh3D directories also have an explicit parent under that root.
The suite resolves its cleanup path before running tests and removes the output
tree before and after execution, including ordinary assertion failures. Cleanup
failure makes the run fail. A crash or forced termination can leave files there;
the next run clears them. Do not store review artifacts there or run two engine
test suites concurrently in one checkout. `Test/data/` contains read-only fixtures.

Other output has a separate, explicit lifetime:

| Workflow | Output and lifetime |
| --- | --- |
| [Array instrumentation](../array/testing.md#isolated-instrumentation-on-macos) | The optional directory argument selects retained build/profile/report output. Otherwise the tool creates a temporary directory and prints its path; remove it after inspection. |
| Python tool self-tests | `TemporaryDirectory` scopes fixture files and removes them on scope exit. These do not retain review output. |
| [Assembly](../../Test/Manual/Mesh3DAssemblyExamples.md), [Loft](../../Test/Manual/Mesh3DLoftExamples.md), and [modeling studies](../../Test/Manual/Mesh3DModelingStudies.md) | Explicit manual applications retain OBJ/MTL files in `assembly_examples/`, `loft_examples/`, and `modeling_studies/` under their working directory. Remove these directories after review. |
| [Mesh3D preview](../mesh3d/preview.md) | The documented `--output` option selects retained previews; the default is `/tmp/mesh3d-preview`. Remove them after review. |

Engine initialization happens before `RunTest` and can create engine resource
caches in the platform's application cache location. Build products and toolchain
caches also follow the platform build configuration. `Test/output/` governs test
fixture writes, not every file produced by engine startup or the build tools.

Test code must not use ambient environment variables to select behavior or export
files. Use doctest arguments for test selection and documented arguments for
development tools. Existing environment settings have narrow purposes:
`CONFIGURATION` selects the macOS build configuration (default `Debug`), `TMPDIR`
selects the standard temporary parent for the Array tool, and `LLVM_PROFILE_FILE`
is set only for its coverage subprocess to keep the profile in the reported output
directory. These are build/toolchain settings, not hidden test export switches.
The optional nanobench benchmarks also inherit the library's `NANOBENCH_ENDLESS`
and `NANOBENCH_SUPPRESS_WARNINGS` settings; benchmarks are disabled by default in
`Test/Siv3DTest.hpp`, and these settings do not select file output destinations.
Environment-handling tests should isolate their process environment or restore
each changed value. The enforceable policy is in [AGENTS.md](../../AGENTS.md#test-output-and-configuration).

## Documentation checks

```sh
python3 tools/check_docs.py
python3 tools/check_docs.py --self-test
```

The [checker](../../tools/check_docs.py) uses only the Python standard library.
It checks local inline links and reference-link destinations in root Markdown,
docs, and Test/Manual, and requires every Markdown file under docs to be reachable
from docs/README.md. Code fences and inline code are excluded. External URLs and
heading fragments are not checked. Use ordinary relative links for source files;
links containing spaces can use angle brackets or percent encoding.

Development tools belong in tools; their usage belongs beside the relevant subject.
For example, the [Mesh3D preview guide](../mesh3d/preview.md) documents a Python
inspection tool and its independent self-tests.
