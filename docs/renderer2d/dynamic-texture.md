# Dynamic texture updates

[`DynamicTexture`](../../Siv3D/include/Siv3D/DynamicTexture.hpp) reuses an existing
texture for CPU-generated images. Its public header owns update, format, and
failure contracts.

## Typed data and color updates

`Grid<uint16>` stores one packed R8G8 pixel per element, matching
`CreateR8G8_Unorm()`. It can be passed to the constructor and all four update
methods. The typed update methods check both size and format, then use the same
transfer path as raw bytes; repeated updates do not repack the Grid into a
temporary image. Constructors with odd-width rows reuse the existing creation
path's alignment handling.

`fillIfNotBusy(color)` and `fillRegionIfNotBusy(color, rect)` offer the same
skip-when-busy choice as image and byte inputs. A busy result leaves the texture
unchanged. Each successful color update converts the color once. Both backends
write the repeated pixel value into the existing upload storage in blocks.
Contiguous pixels are filled in one pass. With row padding, only the pixels
of each row are written.

When the destination is empty, whole-texture Image/Grid updates create it using
the input's format and the default texture settings. Their return value reports whether
creation succeeded. Color, raw-byte, and region updates require an existing
texture.

## Raw pixels and row pitch

The byte-span overloads of `fill()` and `fillRegion()` accept external pixel
storage directly. Create the destination texture with the matching format first;
the raw overloads do not infer a format or create an empty texture. Image/Grid
overloads provide typed checks and automatic creation.

For example, two rows of three R8 pixels with a five-byte source row pitch:

```cpp
DynamicTexture texture{ Size{ 3, 2 }, TextureFormat::R8_Unorm };
const std::array<uint8, 8> source{ 0, 64, 128, 0xCC, 0xCC, 255, 128, 64 };
const bool succeeded = texture.fill(std::as_bytes(std::span{ source }), 5);
```

Only the pixels are copied; the two padding bytes are ignored, and no padding
is needed after the final row. Input storage can be reused or destroyed when the
call returns. `fillRegion(bytes, sourceRowPitch, rect)` updates just that rectangle;
the span starts at its source image's first pixel. The `IfNotBusy` variants accept
the same inputs and preserve their existing non-waiting behavior.

Both native backends use the same source-length validation and row-copy helper.
Validation precedes upload-storage acquisition. Packed source and destination
rows use one memcpy; padded rows copy only their pixels, without reading the
GPU upload destination. No temporary Image/Grid is constructed by the raw path.

## Ordering updates and drawing

Drawing commands can remain buffered in Renderer2D. Call `Graphics2D::Flush()`
before `fill()`, `fillRegion()`, or `generateMips()` when earlier drawing must use
the old contents. The update methods do not flush buffered drawing themselves.
Drawing submitted after the update uses the updated contents. Mip generation is
explicit; updating level zero does not regenerate the other levels.

`fill()` and `fillRegion()` wait when their upload storage is still in use. They
do not skip an update because that storage is busy. The `IfNotBusy` variants
return false instead of waiting, so their success can depend on GPU progress.
One update per frame can still encounter busy storage when the CPU is ahead of
the GPU.

On Metal, a successful update appends a buffer-to-texture copy to the current
frame's command buffer and submits that buffer. Recording continues in a new
command buffer within the same frame. This preserves the order of already
encoded drawing and updates while allowing the upload storage to be reused
after that submission finishes. Mip generation records into the current buffer
without submitting it. Outside a frame, texture operations use standalone
command buffers.

An intermediate submission retains the frame slot and all vertex/constant
buffer cursors. Final submission releases the slot on GPU completion. Cancelling
a partially submitted frame waits for its submitted portion before returning the
slot; only its unsubmitted portion is discarded. Intermediate GPU errors use the
same deferred reporting as final submissions.

## Validation and performance

[`Test_DynamicTexture.cpp`](../../Test/Test_DynamicTexture.cpp) covers update and
mip ordering, explicit Flush, pending drawing, repeated updates, and readback
continuation. Native frame tests cover submission, cancellation, and error
reporting.
Raw tests cover every uncompressed format, unaligned source addresses, omitted
final-row padding, whole/partial updates, source lifetime, mip ordering, and invalid
inputs. Native Metal tests also hold an upload busy to verify that a non-waiting
call does not copy, split the frame, or submit work. Color and RG8 typed paths
are also checked through the public API with GPU progress held by a shared event.
Typed creation tests cover success and failure for every Image/Grid overload.

Run the opt-in upload benchmark in Release:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=DynamicTexture.frame_upload_benchmark'
```

```powershell
./WindowsDesktop/run-tests.ps1 -Configuration Release -TestArguments '--test-case=DynamicTexture.frame_upload_benchmark'
```

The benchmark updates one texture once per frame from alternating, preallocated
RGBA8 images at 640 x 360 and 1920 x 1080. It compares updating at the start of the
frame with updating after an explicit Flush. One or 32 translucent fullscreen
layers provide light and heavier GPU workloads at a fixed 1920 x 1080 scene size,
without scene MSAA or VSync. Each scenario warms up before three measured trials.

Console output reports median and p95 wall time for `fill()` (including copying,
submission, and any upload-storage wait) and the frame loop (including
`System::Update()`). Image construction and final draining are outside the timed
regions. There is no per-frame readback and no timing threshold in the tests.
In Metal's after-Flush scenario, `fill()` also submits the drawing encoded before
the update, moving that submission cost into the measured fill interval.
Compare the same machine and display conditions before and after a change;
presentation pacing may limit frame throughput even with VSync disabled.
Retain runner output when comparing revisions, and consider both fill latency
and total frame time rather than inferring GPU performance from submission time.

The separate `DynamicTexture.Metal.raw_upload_benchmark` in
[Test_MetalDynamicTexture.cpp](../../Test/Test_MetalDynamicTexture.cpp) compares
ordinary CPU row copies and actual Metal updates at 63×31 and 1920×1080. Its four
update paths are typed Image, packed raw bytes, padded raw bytes, and repacking
padded bytes into a reusable Image before a typed update. All storage is prepared
before measurement. Seven batches rotate the path order, with 24 samples each;
warm-up and GPU draining are outside timing. It reports median and p95 CPU wall
times, including native upload submission, and checks the resulting pixels.
These times are not GPU execution measurements or correctness thresholds.

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=DynamicTexture.Metal.raw_upload_benchmark'
```

A reference Release run on Apple M2, with Metal validation disabled, produced
these CPU update times (median / p95, in microseconds):

| Input and update path | 63×31 | 1920×1080 |
|---|---:|---:|
| Image | 12.92 / 18.42 | 170.21 / 212.88 |
| Packed raw bytes | 12.88 / 16.21 | 173.50 / 228.17 |
| Padded raw bytes | 12.83 / 15.71 | 191.04 / 242.88 |
| Repack padded bytes into Image, then update | 12.96 / 15.79 | 414.08 / 482.71 |

For the full-HD padded input, avoiding the extra copy reduced the median CPU
call time by about 54%. Packed raw and Image updates were close; the raw overload
is useful for accepting existing storage without repacking. Ordinary heap copies
of the full-HD image measured 184.42 µs packed and 174.38 µs padded (medians).
Those isolated copies have different cache conditions from native uploads and
must not be used as a substitute for the upload measurements above.

The existing frame benchmark was also run before and after the shared upload
change on the same machine. Across its eight scenarios, the median of the three
trial frame medians remained 16.62–16.67 ms before and 16.59–16.70 ms after. The
1920×1080, one-layer, start-of-frame Image update measured 826.83 µs before and
804.33 µs after. The 32-layer after-Flush case increased from 3829.12 to
3998.42 µs while its frame median stayed near 16.65 ms. Submission, GPU progress,
and display pacing affect these results; they show no clear frame-throughput
regression, but do not establish a speedup or zero overhead for typed updates.

### Whole and regional upload latency

`DynamicTexture.typed_and_color_upload_benchmark` uses the same public API calls
on Metal and D3D11. It measures RG8 (`Grid<uint16>`) and RGBA8 (`Image`) with
typed data, packed bytes, and a solid color. Each input uses both the waiting
and `IfNotBusy` overloads. Both update shapes use a 1920×1080 texture:

| Update | Destination | Bytes per updated row, RG8 / RGBA8 |
|---|---|---:|
| `fill` | Entire texture | 3840 / 7680 |
| `fillRegion` | 63×31 at (317, 193) | 126 / 252 |

The narrow region exercises the D3D11 staging texture's full-width row pitch.
Typed and raw sources carry a marker in a different column of each row.
Every timed call alternates between two prepared inputs. A dependent 1×1 copy
and synchronous read verify the new pixel and make upload storage available
before the next call. Each batch also checks the texture ID and reads back the
whole result for each overload, including the background outside the rectangle.

Only the CPU update call is timed. Source preparation, warm-up, readback, and
`System::Update()` run outside the timer. Six batches rotate the six overloads
through each position, with 24 samples per overload per batch. Each output line
reports the median and p95 of 144 calls in microseconds. The frame benchmark
above covers waits under rendering load.

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=DynamicTexture.typed_and_color_upload_benchmark'
```

Three Release runs on Apple M2 with Metal validation disabled gave the following
waiting-overload times in microseconds. Each cell shows the middle of the three
run medians and the middle of the three run p95s.

| Format and update | Typed median / p95 | Raw median / p95 | Color median / p95 |
|---|---:|---:|---:|
| RG8 whole | 105.0 / 141.5 | 102.2 / 128.7 | 82.1 / 97.0 |
| RGBA8 whole | 191.6 / 243.5 | 188.3 / 239.7 | 146.9 / 175.9 |
| RG8 region | 12.3 / 15.0 | 12.2 / 15.1 | 12.3 / 14.8 |
| RGBA8 region | 12.2 / 14.1 | 12.1 / 13.7 | 12.0 / 13.6 |

The earlier Metal-only comparison used a native queue drain and repeated the
same input. When block filling was introduced, whole-texture color medians
went from 619–775 to 83–91 µs for RG8 and from 614–681 to 137–226 µs for
RGBA8. In ordinary CPU memory, fill medians went from 595–764 to 61–81 µs for
RG8 and from 592–603 to 121–243 µs for RGBA8. The isolated fill is available as
`DynamicTexture.Metal.heap_color_benchmark`.

On Windows, check the optimized executable when changing the color-fill loop.
MSVC can store the first 64-byte color block and use it as the source of
`rep movs` for the rest of the mapped texture. The MSVC compiler barrier in
`FillColorRow` keeps the repeated stores write-only. Check both the contiguous
loop and the per-row loop, then measure full textures and narrow regions on
the D3D11 staging resource.

On a GeForce RTX 4090 Laptop GPU with MSVC 14.51, Release x64 and the D3D11
debug layer off, the waiting color overload gave the following CPU times in
microseconds. Each cell is the middle of three run medians. The per-row
implementation and the final block implementation ran in alternating order.

| Format and update | Before block fill | Block fill without compiler barrier | Block fill with compiler barrier |
|---|---:|---:|---:|
| RG8 whole | 141.7 | 7939.8 | 94.7 |
| RGBA8 whole | 226.5 | 15856.9 | 176.9 |
| RG8 region | 0.8 | 0.9 | 0.6 |
| RGBA8 region | 0.8 | 14.1 | 0.7 |

The same full-HD RGBA8 block fill in ordinary CPU memory took 163–194 µs
without the barrier and 173–191 µs with it (ranges of run medians). Only
the mapped upload showed the multi-millisecond cost of destination reads.

### Comparing two builds on Windows

Use PowerShell 7 in the updated checkout with the shared benchmark present.
For the color-fill optimization, use its implementation commit as `$change`.
The subsequent diff in the shared test file adds the benchmark. Apply that
diff to a fresh checkout of the parent so both executables run the same
workload and each revision keeps its own engine and correctness tests.

```powershell
$change = Read-Host 'Implementation commit to compare with its parent'
$after = (Get-Location).Path
$before = Join-Path (Split-Path $after) 'siv8-upload-before'
$patch = Join-Path ([IO.Path]::GetTempPath()) 'siv8-upload-benchmark.patch'
git diff --output="$patch" $change -- Test/Test_DynamicTexture.cpp &&
    git worktree add --detach "$before" "$change^" &&
    git -C "$before" apply "$patch"
if ($LASTEXITCODE -ne 0) { throw 'Could not prepare the baseline checkout' }
Copy-Item -Recurse ./Dependencies/boost_1_88_0/boost "$before/Dependencies/boost_1_88_0/"
```

Build each checkout and run its DynamicTexture correctness tests. Then measure
each binary three times on the same GPU, driver, and power settings with the
D3D11 debug layer disabled. Switching run order reduces the effect of drift.

```powershell
foreach ($tree in $before, $after) {
    & "$tree/WindowsDesktop/run-tests.ps1" -Configuration Release -TestArguments '--test-case=~[benchmark]DynamicTexture.*'
}
$case = '--test-case=DynamicTexture.typed_and_color_upload_benchmark'
foreach ($tree in $before, $after, $after, $before, $before, $after) {
    & "$tree/WindowsDesktop/run-tests.ps1" -Configuration Release -SkipBuild -TestArguments $case
}
```

Each checkout retains timestamped text and XML reports in
`WindowsDesktop/Intermediate/TestReports/`. Match the `typed-color-upload` lines
by format, rectangle, and overload. Compare `median_us` and `p95_us` across
runs, including the ranges. The whole-color and narrow-region color rows
exercise different destination pitches. Typed and raw rows give the transfer
cost for the same number of pixels.
