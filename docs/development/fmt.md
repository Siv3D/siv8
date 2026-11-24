# fmt maintenance

Siv3D bundles a subset of [fmt](https://github.com/fmtlib/fmt), with its release
version recorded in [ThirdParty.md](../../ThirdParty.md). The public headers are
under [include/ThirdParty/fmt](../../Siv3D/include/ThirdParty/fmt), while
[format.cc](../../Siv3D/src/ThirdParty/fmt/format.cc) and
[format-inl.h](../../Siv3D/src/ThirdParty/fmt/format-inl.h) live under
`src/ThirdParty/fmt` and are built as part of the engine.

## Updating

Compare the bundled files against the previous upstream tag before updating.
Map `format.cc` to upstream `src/format.cc` and all bundled headers, including
the relocated `format-inl.h`, to upstream `include/fmt/`.
Preserve the local include in `format-inl.h`:
`#include <ThirdParty/fmt/format.h>` replaces upstream `#include "format.h"`.
Keep the existing file subset and compiled-library layout; optional upstream
headers such as `compile.h` and `std.h` are not bundled.

Update `ThirdParty.md` and compare the release's `LICENSE` with the fmt entry in
[LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp).
Retain upstream license notices and LF line endings.

## Validation

[Test_FmtExtension.cpp](../../Test/Test_FmtExtension.cpp) covers Siv3D strings,
UTF-32 `_fmt` calls, ranges, null pointers, and UTF-32 buffer growth. The growth
regression requests a capacity whose byte count would overflow `size_t` and
requires `std::bad_alloc` without changing the buffer contents. It then verifies
that normal growth still succeeds; it does not attempt a large real allocation.

Use `--test-case=FmtExtension*,*Formatter` for focused iteration, then run the full
suite with the [host test workflow](README.md#build-and-test) to cover geometry
formatters and other callers. A host-sized overflow check does not establish
that a 32-bit platform builds or runs correctly.
