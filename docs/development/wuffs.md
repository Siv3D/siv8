# Wuffs maintenance

[Wuffs](https://github.com/google/wuffs) ships generated single-file C libraries.
Siv3D stores one as
[wuffs-v0.4.h](../../Siv3D/src/ThirdParty/wuffs/wuffs-v0.4.h) and compiles it inside
[WuffsCodecs.cpp](../../Siv3D/src/ThirdParty/wuffs/WuffsCodecs.cpp).
The local `.h` filename is retained for integration; it is not a version string.

## Source and version

[ThirdParty.md](../../ThirdParty.md) records the release version.
The bundled header comes from
`release/c/wuffs-v0.4.c`, renamed locally to `.h`. Read `WUFFS_VERSION_STRING`
inside that file for its version, including the alpha label and build metadata.
`v0.4.c` is a distribution filename, not a complete version. An alpha release
is a developer preview without API compatibility guarantees.

Upstream also publishes `wuffs-unsupported-snapshot.c` with zero-valued version
metadata. It is a separate development snapshot; do not substitute it for a
selected numbered alpha release.

## Updating

Compare the bundled header with its matching upstream release before replacing
it. The current generated file has no local patches. Copy the selected generated
file without regenerating it or rewriting its version macros; preserve LF line
endings and embedded notices. Update the version in
`ThirdParty.md` and compare upstream's `LICENSE-MIT` with the Wuffs entry in
[LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp).
Siv3D uses the MIT option. The generated file's older copyright notice and the
root license's copyright date can differ; retain both in their respective places.

Keep the implementation confined to `WuffsCodecs.cpp` with static functions.
Preserve its BASE, ADLER32, CRC32, DEFLATE, ZLIB, GIF, and PNG module selection,
RGBA_NONPREMUL destination allowlist, and MSVC x64 V2 setting. V3 is not enabled.
Keep the wrapper's own APNG probing, interlace rejection, input refill, memory
limits, disposal/compositing, and error mapping when adapting to upstream changes.

## Validation

[Test_AnimatedImage.cpp](../../Test/Test_AnimatedImage.cpp) covers GIF first-frame
decoding, GIF/APNG animation, metadata, frame timing and disposal, memory limits,
truncation, streaming, and one-byte non-seekable reads.
Ordinary static PNG decoding uses a separate implementation, so those tests
alone do not establish Wuffs coverage.

Use `--test-case=GIFDecoder,PNGDecoder animated image info,*AnimatedImage*` for
focused iteration, then run the full suite with the
[host test workflow](README.md#build-and-test). Host results do not validate
another platform's SIMD path, including the explicitly enabled MSVC x64 V2 path.
