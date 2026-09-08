# Testing Array

The normal engine suite combines three complementary sources:

- [API tests](../../Test/Test_Array.cpp) cover values, compile-time acceptance and
  rejection, reference categories, generic element types, storage reuse, and
  parallel operations.
- [Exception tests](../../Test/Test_ArrayException.cpp) inject failures at each
  copy, move, or assignment in selected operations and at allocation boundaries.
  They track live objects and allocation ownership, exercise propagating and
  non-propagating allocators, and verify that failed operations leave objects
  destructible and reusable.
- [Model tests](../../Test/Test_ArrayModel.cpp) compare sequences of mutations
  with a separate `std::vector` model after every operation. Fixed seeds and raw
  `mt19937` output keep failures reproducible across standard libraries; failure
  diagnostics include the seed, step, and operation. Algebraic identities check
  splitting, flattening, and reversal or rotation round trips independently.

Exception assertions follow the operation's guarantee. Copying derived results
and failed storage growth preserve the source where the underlying container
provides that guarantee. Consuming compaction and flattening may already have
moved elements when an exception occurs; these tests check lifetime and reuse
without requiring rollback. They do not assume exact moved-from contents.

The bool tests query initial capacity instead of assuming a particular standard
library's inline-storage threshold. They exercise sizes immediately below, at,
and above it, including growth, shrink, move, swap, release, and allocation
failure. `shrink_to_fit()` is non-binding; tests do not require a specific final
capacity.

## Isolated instrumentation on macOS

[run-array-checks.sh](../../tools/run-array-checks.sh) builds the exception and
model tests directly with the real Array implementation and exception helpers.
It avoids engine startup and instruments the instantiated template code along
with the test code. Run from the repository root:

```sh
./tools/run-array-checks.sh asan
./tools/run-array-checks.sh coverage
```

An optional second argument selects the output directory. Otherwise each run
creates a temporary directory and prints its location. `asan` enables AddressSanitizer
and UndefinedBehaviorSanitizer with failure on a sanitizer finding. Live-object
and allocator bookkeeping provide additional explicit leak and ownership checks;
this workflow does not enable LeakSanitizer or ThreadSanitizer.

`coverage` writes `coverage.txt`, LLVM profiles, and `html/index.html`. Its report
is limited to Array source files and code emitted by these two test sources.
Uninstantiated template members do not contribute to the denominator. Therefore
this percentage is **not coverage of the whole Array API**; use the annotated
source to locate untested paths among the operations exercised. The original API
and parallel tests are not part of this isolated executable.

These checks supplement the full macOS suite described in the
[Array guide](README.md#validation). Both new test sources are also registered in
the Windows test project; macOS instrumentation does not validate MSVC or the
Windows standard library.
