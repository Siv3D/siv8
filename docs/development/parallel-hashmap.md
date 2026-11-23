# Parallel Hashmap maintenance

Siv3D bundles eight headers from
[The Parallel Hashmap](https://github.com/greg7mdp/parallel-hashmap) under
[include/ThirdParty/parallel_hashmap](../../Siv3D/include/ThirdParty/parallel_hashmap).
[ThirdParty.md](../../ThirdParty.md) records the release version.
[HashMap](../../Siv3D/include/Siv3D/HashMap.hpp),
[HashSet](../../Siv3D/include/Siv3D/HashSet.hpp),
[BTreeMap](../../Siv3D/include/Siv3D/BTreeMap.hpp), and
[BTreeSet](../../Siv3D/include/Siv3D/BTreeSet.hpp) are direct public aliases.
The hash containers use Siv3D's transparent string hash and equality defaults.

## Updating

Compare every bundled header with the previous upstream tag before updating.
Keep the existing file subset and relative includes. Preserve the braced
statements in `CountLeadingZeros64Slow` and `CountLeadingZeros32Slow` in
`phmap_bits.h`; these replace upstream's comma-expression statements.
The former local `is_always_equal` workaround is covered by upstream's
`std::allocator_traits` implementation. Keep LF line endings and normalize
newly introduced trailing whitespace when required by `git diff --check`.

Preserve the public wrappers' temporary `PHMAP_HAVE_SSE2` and `PHMAP_HAVE_SSSE3`
definitions on SSE hosts and their allocator/hash/equality defaults. Changes to
`phmap::priv` can affect these aliases even when upstream's public APIs remain
compatible. Rebuild the engine and all consumers with the same headers.

Hash mixing can change across versions and architectures, including ARM64.
Do not rely on hash-container iteration order or assume raw dump compatibility
across releases. The engine currently does not use `phmap_dump` or `phmap_load`.

Update `ThirdParty.md` and compare upstream's `LICENSE` and header notices with
both Parallel Hashmap entries in
[LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp), covering
Gregory Popovitch and the Abseil authors under Apache 2.0.

## Validation

[Test_ParallelHashmap.cpp](../../Test/Test_ParallelHashmap.cpp) covers the public
aliases, including rehash capacity and lookup, reserve behavior, transparent
string lookup, move-only mapped values, and B-tree ordered traversal and erase.
Check spare capacity before an absent-key lookup in a rehash regression: a
full-table bug could otherwise hang the test.

Use `--test-case=ParallelHashmap.*,AssetHandleManager.*` for focused iteration,
then run the full suite with the [host test workflow](README.md#build-and-test).
Asset managers, font caches, and renderer caches also exercise the hash
containers. A host run does not establish correctness of another platform's
compiler or SIMD path, or of concurrent parallel-container operations.
