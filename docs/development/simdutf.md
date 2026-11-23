# simdutf maintenance

Siv3D bundles the official `simdutf.h` and `simdutf.cpp` release amalgamations in
[src/ThirdParty/simdutf](../../Siv3D/src/ThirdParty/simdutf).
[ThirdParty.md](../../ThirdParty.md) records the release version.
[Unicode](../../Siv3D/src/Siv3D/Unicode/SivUnicode.cpp) uses them for character
conversion and validation;
[Base64Value](../../Siv3D/src/Siv3D/Base64Value/SivBase64Value.cpp) also uses
them for Base64 encoding, decoding, and conversion of decoded UTF-8 text.

## Updating

Download both files from the same tagged
[upstream release](https://github.com/simdutf/simdutf/releases), and compare the
bundled files with the previous release before replacing them. The current
amalgamations have no local patches. Preserve any future local changes, the
relative `#include "simdutf.h"`, and LF line endings. Keep the existing per-file
Windows warning suppressions. Xcode suppresses `-Wunused-function` only for
`simdutf.cpp`, since the amalgamation can include unused backend helpers.
No local feature macros override upstream's runtime CPU dispatch.

Rebuild the implementation and its consumers together. Upstream does not
guarantee ABI compatibility across releases, even when source-level calls are
unchanged. Preserve conversion failure handling and code-unit error offsets in
the wrappers when adopting new APIs.

Update `ThirdParty.md` and compare upstream's `LICENSE-MIT` with the simdutf
entry in [LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp).
Siv3D uses the MIT option; retain embedded third-party notices in both files.

## Validation

[Test_Unicode.cpp](../../Test/Test_Unicode.cpp) and
[Test_UnicodeConverter.cpp](../../Test/Test_UnicodeConverter.cpp) cover scalar
boundaries, invalid input, destination reuse, and error offsets around SIMD
block boundaries, including UTF-16 in both byte orders and UTF-32 validation.
[Test_Base64Value.cpp](../../Test/Test_Base64Value.cpp) checks aliasing, decode
errors, decoded Unicode, and output guards with padded, unpadded, and whitespace
inputs. Guards start at the actual decoded length while the allocation still
satisfies `getMaxBinarySize()`: a guard only after that upper bound can miss
extra writes when input contains whitespace.

Use `--test-case=Unicode.*,UnicodeConverter.*,Base64Value*` for focused iteration,
then run the full suite with the [host test workflow](README.md#build-and-test).
A host run exercises its selected implementation; it does not validate every
SIMD backend or establish performance on other CPU architectures.
