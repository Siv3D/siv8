# Dynamic texture updates

[`DynamicTexture`](../../Siv3D/include/Siv3D/DynamicTexture.hpp) reuses an existing
texture for CPU-generated images. Its public header owns update, format, and
failure contracts.

## Raw pixels and row pitch

The byte-span overloads of `fill()` and `fillRegion()` accept external pixel
storage directly. Create the destination texture with the matching format first;
the raw overloads do not infer a format or create an empty texture. Existing
Image/Grid overloads keep their typed checks and automatic creation behavior.

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
call does not copy, split the frame, or submit work.

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
