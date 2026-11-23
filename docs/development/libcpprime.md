# libcpprime maintenance

Siv3D bundles the compact primality test from
[libcpprime](https://github.com/sortA0329/libcpprime).
[ThirdParty.md](../../ThirdParty.md) records its release version. The former
`Rac75116/libcpprime` repository URL redirects to the current upstream.

[IsPrime.hpp](../../Siv3D/include/Siv3D/IsPrime.hpp) includes the vendored header,
and [IsPrime_impl.ipp](../../Siv3D/include/Siv3D/detail/IsPrime_impl.ipp) forwards
`Math::IsPrime` to `cppr::IsPrimeCompact`. Keep the public `uint64` to `bool`,
`constexpr`, and `noexcept` behavior. There is no separate signed overload;
changing signed-input conversion is outside a dependency update.

## Updating

Compare the bundled files with the recorded upstream tag before replacing them.
The current files have no local patches. Preserve LF line endings, relative
includes, embedded notices, and the wrapper's scoped warning suppression.
Version 1.4 renamed `IsPrimeNoTable` to `IsPrimeCompact`; update the include and
forwarding call together when following future upstream API changes.

Import only the compact dependency closure in
[ThirdParty/libcpprime](../../Siv3D/include/ThirdParty/libcpprime):
`IsPrimeCompact.hpp`, `internal/Environment.hpp`, `internal/IsPrimeCommon.hpp`,
`internal/Utils.hpp`, `internal/IsPrimeTable10.txt`, `internal/IsPrimeBases32.txt`,
and `internal/IsPrimeBases64Compact.txt`, plus upstream's `LICENSE`.
The compact variant uses a 512-byte table for its 64-bit bases in addition to
its small-input and 32-bit tables. Do not switch to upstream's large-table
`cppr::IsPrime` when adapting the similarly named Siv3D API.
Update both platform projects and filters when the file set changes, and rebuild
all consumers of the public header.

Update `ThirdParty.md` and
[LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp) from the
bundled notices. The main code uses MIT, the Bradley Berg algorithm has its own
permission notice, and the libdivide-derived code uses Boost Software License
1.0. Keep all three notices. The upstream `LICENSE` and the libdivide-derived
headers can list different copyright end years; preserve upstream's license
file and use the header-specific attribution when displaying those portions.

## Validation

[Test_IsPrime.cpp](../../Test/Test_IsPrime.cpp) checks the public wrapper against
an independent sieve, known primes and composites, algorithm boundaries, and
compile-time assertions. Use `--test-case=IsPrime.*` for focused iteration, then
run the full suite with the [host workflow](README.md#build-and-test).

For algorithm updates, also compare a deterministic spread of 64-bit inputs
against an independent reference, including strong pseudoprimes, prime squares,
and values around the 32-bit limit, the Montgomery-arithmetic split, and the
largest representable integers. Agreement with the previous version alone is
not an independent correctness check. Keep temporary comparison and sanitizer
harnesses outside the repository.

Native macOS checks exercise Clang's integer-arithmetic paths. They do not
validate MSVC intrinsics or its constant-evaluation limits; run the Windows
workflow on a Windows host when available.
