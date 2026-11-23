# stb_image_resize2 maintenance

Siv3D bundles the single
[stb_image_resize2.h](../../Siv3D/src/ThirdParty/stb/stb_image_resize2.h)
header from the official
[stb_image_resize repository](https://github.com/nothings/stb_image_resize).
The older copy in `nothings/stb/deprecated/` is no longer the update source.
[ThirdParty.md](../../ThirdParty.md) records the version written in the header.
When upstream has no release tag, record the exact source commit in the change
description so that the imported bytes can be reproduced.

[SivImageProcessing.cpp](../../Siv3D/src/Siv3D/ImageProcessing/SivImageProcessing.cpp)
defines `STB_IMAGE_RESIZE_IMPLEMENTATION` once and calls `stbir_resize` with
packed `STBIR_RGBA_PM`, `STBIR_TYPE_UINT8`, `STBIR_EDGE_CLAMP`, and
`STBIR_FILTER_DEFAULT`. The default filters are Catmull-Rom for enlargement
and Mitchell for reduction. The
[BCn encoder](../../Siv3D/src/Siv3D/ImageFormat/BCn/BCnEncoder.cpp) uses this path
to generate mipmaps. Keep the header's existing path and self-include filename;
no project registration changes are needed when replacing it in place.

## Updating

Compare the bundled header against upstream before replacing it. Preserve this
local patch until upstream resolves
[issue #3](https://github.com/nothings/stb_image_resize/issues/3): remove the
file-scope `stbir__chose_vertical_first` definition and its assignment in
`stbir__should_do_vertical_first`. The value is not read by the implementation,
but independent resize calls write the same variable. Removing these two lines
avoids the data race without adding locks or changing the selected processing
order. Do not rely on optimization eliminating the write.

Normalize downloaded CRLF to LF and remove newly introduced trailing whitespace
as required by `git diff --check`. Retain the embedded MIT / Unlicense choices.
Siv3D displays the MIT option in
[LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp); compare its
notice with the header rather than inferring a copyright year from the release
date. Update the plain version and source link in `ThirdParty.md`.

Version 2.50 changed coefficient precision and processing-order selection, so
output pixels need not match earlier versions even with the same filter names.
BCn mipmap pixels and compressed bytes can consequently change. Recheck the
used API separately from optional callback and split APIs; the latter are not
used by Siv3D. No cache-size override is currently configured. Performance
claims require measurements on the relevant host and image sizes.

## Validation

[Test_ImageProcessing.cpp](../../Test/Test_ImageProcessing.cpp) covers resizing
through the engine wrapper, including independent calls from multiple threads.
Run `--test-case=ImageProcessing.*,BCnEncoder.*,BCnDecoder.*` for focused
iteration, then the full suite with the [host workflow](README.md#build-and-test).

When updating, compare old and new headers in separate translation units using
the same pixel layout, type, edge mode, and filter as the engine. Exercise
enlargement, reduction, odd dimensions, one-pixel axes, patterned premultiplied
alpha, and repeated mipmap reductions. Report changed pixels and maximum/mean
channel differences rather than assuming byte identity or silently updating
goldens. Check that any local race patch leaves the new version's output intact.

Use ThreadSanitizer with optimization disabled to expose diagnostic writes
that release builds may remove. Exercise simultaneous resizes of independent
images and compare them with serial reference results. Keep sanitizer harnesses
and comparison artifacts outside the repository for each update; concurrency
tests supplement source review and do not prove every upstream path race-free.
