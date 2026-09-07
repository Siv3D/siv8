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
