# Dynamic texture updates

[`DynamicTexture`](../../Siv3D/include/Siv3D/DynamicTexture.hpp) reuses an existing
texture for CPU-generated images. Its public header owns update, format, and
failure contracts.

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
