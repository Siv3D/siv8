# fast_float maintenance

The [vendored header](../../Siv3D/src/ThirdParty/fast_float/fast_float.h) is the
unmodified single-header distribution from
[fast_float](https://github.com/fastfloat/fast_float).
[ThirdParty.md](../../ThirdParty.md) records its release version.
[ParseFloat](../../Siv3D/src/Siv3D/ParseFloat/SivParseFloat.cpp) uses it for
`float` and `double` parsing from narrow and UTF-32 strings.
[BigFloat](../../Siv3D/src/Siv3D/BigFloat/SivBigFloat.cpp) also uses it when
converting decimal text to native `float` and `double` values.

## Updating

Compare the existing header against the release recorded in `ThirdParty.md`
before replacing it, so any later local changes are preserved. Obtain the new
`fast_float.h` release asset and compare it with the amalgamation generated in
a separate upstream checkout of the matching tag:

```sh
python3 script/amalgamate.py > fast_float.h
```

The default generation retains all three upstream license notices. Keep those
notices and LF line endings. Update `ThirdParty.md` and compare the upstream
`LICENSE-MIT` with the fast_float entry in
[LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp), which uses
the MIT license option.

Leave `FASTFLOAT_ROUNDS_TO_NEAREST` undefined: Siv3D does not guarantee that the
process always uses that floating-point rounding mode. Enabling the macro would
remove the upstream runtime check and change that requirement.

## Validation

[Test_ParseFloat.cpp](../../Test/Test_ParseFloat.cpp) covers both character widths,
normal and special values, errors, representable boundaries, deterministic
round trips, and nearest-even results under all four standard rounding modes.
It restores the floating-point environment after changing it.
[Test_BigFloat.cpp](../../Test/Test_BigFloat.cpp) also checks native conversion,
including subnormals, halfway values, overflow, and signed underflow.

Use `--test-case=ParseFloat*,BigFloat*` for focused iteration, then run the full
suite with the [host test workflow](README.md#build-and-test).
