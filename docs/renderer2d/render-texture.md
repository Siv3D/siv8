# RenderTexture resources and target scopes

[RenderTexture](../../Siv3D/include/Siv3D/RenderTexture.hpp) owns a single-sample
color texture. It supports creation, GPU clears, mipmap generation, CPU readback,
and the drawing operations inherited from `Texture`.
[ScopedRenderTarget2D](../../Siv3D/include/Siv3D/ScopedRenderTarget2D.hpp) selects
one color texture for 2D drawing. MRT, depth attachments, and multisample resources are tracked in
[TODO](../../TODO.md#rendertexture--depthtexture).

## Creating and inspecting textures

Use the size-and-color constructor for an initially uniform texture. A format
argument selects the storage; `TextureDesc` selects whether to allocate mipmaps.
For example, inside `Main()` after engine startup:

```cpp
RenderTexture texture{ Size{ 64, 32 }, ColorF{ 2.0, 0.5, 0.25, 1.0 },
    TextureFormat::R16G16B16A16_Float, TextureDesc::Mipmap };

texture.clear(ColorF{ 4.0, 1.0, 0.5, 1.0 });
texture.generateMips();

Grid<std::array<HalfFloat, 4>> pixels;
if (texture.read(pixels))
{
    Print << pixels.size();
}
```

Grid constructors and named factories mirror `DynamicTexture`. Packed formats
such as `R10G10B10A2_Unorm` use named factories or the size/span/format constructor.
The span contains tightly packed rows; backend transfer alignment is internal.
The public header specifies the corresponding readback types and failure rules.
Reading a selected mip level also allows inspection of generated content.

An explicit format controls sRGB metadata. Clear colors are linear, while image
and Grid initialization transfers stored values unchanged. Reading an sRGB
texture returns its encoded values. Use an explicit conversion when displaying
or processing those values as linear colors.

A readback waits for GPU work and can stall a real-time frame loop. Reuse the
output Image/Grid when reading repeatedly. Backend staging storage is allocated
on demand and reused. GPU-only operations do not perform CPU readback.

## Drawing into a texture

A target scope preserves the texture's pixels and starts with a full-target
viewport. Nest scopes to draw intermediate images, then use them in the outer
target. Inside `Main()`:

```cpp
RenderTexture canvas{ Size{ 320, 180 } };
RenderTexture stamp{ Size{ 64, 64 } };

while (System::Update())
{
    {
        const ScopedRenderTarget2D target{ canvas };
        canvas.clear(ColorF{ 0.1, 1.0 });
        {
            const ScopedRenderTarget2D nested{ stamp };
            stamp.clear(ColorF{ 0.0, 0.0 });
            Circle{ 32, 32, 24 }.draw(Palette::Orange);
        }
        stamp.drawAt(160, 90);
    }
    canvas.draw(40, 40);
}
```

`ScopedRenderTarget2D{ none }` temporarily selects Scene; the default constructor
leaves the current state alone. An empty `RenderTexture` is an error. The scope
holds a shared handle, so releasing or replacing the caller's handle does not
change the selected resource. Moving the scope transfers its restoration duty.

`Graphics2D::GetDrawingSize()` and `Graphics2D::GetRenderTargetSize()` return the
texture's pixel dimensions inside a target scope. Camera defaults use that drawing size. Scene's display-resolution scaling
is disabled for textures, including curve subdivision, but Local, Camera and
QuadWarp transforms still apply. Scissor rectangles, colors, blend states,
shaders, and constant buffers carry across target changes. The prior viewport
and target are restored at scope exit; other states use their own scope objects.

To read from a texture while drawing another image, select a different output
first. Drawing the active output as an input, including through a copied handle
or `TextureRegion`, throws `Error` before queuing that draw. This also applies
when using a custom shader with a textured draw. Use two textures for feedback
or iterative image processing.

Clear, readback, mip generation, and `Graphics2D::Flush()` may be used inside a
scope and preserve its target. Drawing and clearing change mip level 0; call
`generateMips()` after those changes when lower levels are needed. A scope can
span a frame boundary: its target remains selected, while Scene still gets its
normal per-frame background and presentation.

## Ordering and backend responsibilities

Clears and mip generation consume earlier queued 2D draws before changing an
existing texture. This gives `draw(); clear(); draw();` an observable order even
though normal draws are batched. Texture creation uploads new resources without
flushing unrelated draws.

Changing targets flushes the preceding draw batch without waiting for the GPU.
Re-entering the same target can retain the batch. Native resource ownership is
retained through command execution; texture contents are loaded on each pass.
Feedback validation runs before geometry allocation, independently of texture
binding and draw submission. It remembers the last successfully validated input
ID for the current output. Repeated inputs skip output comparisons, while failed
validation leaves the cache unchanged. Every logical output change invalidates
it, including scope restoration and Metal's temporary Scene pass at frame end.
Ordinary flushes and re-entering the same output preserve it. Scene draws retain
the early return without an input-ID lookup.

Scopes restore logical target/viewport state even if draw encoding fails. A
normal scope exit propagates that failure; an exit during exception unwinding
logs it while preserving the original exception.

Metal encodes resource operations in the active frame command buffer, or uses a
standalone buffer on the same queue when no frame is recording. Readback appends
a texture-to-buffer copy, submits and waits, then continues recording in a new
command buffer. This retains the acquired frame slot, frame number, scene pass
state, and vertex/index allocation cursors. The synchronous segment completes
before the slot can be reused, including when the remaining frame is canceled.
The continuation buffer is allocated before submitting the current segment.
RenderTexture passes always load/store their single-sample color attachment;
the pipeline uses that attachment's pixel format and sample count. Scene's
initial clear and MSAA store/resolve policy are tracked independently.

D3D11 uses default-usage shader-readable render-target resources and a staging
texture for readback. Format support, including automatic mip generation when
requested, is checked before resource allocation. Copies use the native row
pitch and return tightly packed CPU pixels. Output views are unbound at the end
of each execution, and VS/PS input views are cleared around target binding, so
subsequent sampling, clear, mip generation and readback do not inherit conflicts.

Color textures do not own depth resources. Future depth and multiple-color
bindings should extend the target representation without giving color resources
ownership of depth textures.

## Validation

[Shared tests](../../Test/Test_RenderTexture.cpp) exercise all current
non-compressed color formats, odd row widths, native-value round trips, sRGB and
HDR clears, generated mip contents, handle sharing, invalid inputs, and drawing
around clears and readbacks. [Metal tests](../../Test/Test_MetalRenderTexture.cpp)
cover allocation failure/retry and frame-slot preservation when a continuation
command buffer cannot be allocated.

[Target-scope tests](../../Test/Test_ScopedRenderTarget2D.cpp) check nested pixel
results, viewport/state restoration, all color formats, feedback rejection,
readback and mip generation inside a scope, Scene MSAA and resolution scaling,
and frame continuation. [Metal scope tests](../../Test/Test_MetalScopedRenderTarget2D.cpp)
inject encoder failure to verify entry rollback, exit restoration and exception
unwinding.

Run from the repository root using the host workflow:

```sh
./macOS/run-tests.sh '--test-case=RenderTexture.*,ScopedRenderTarget2D.*'
./macOS/run-tests.sh
./macOS/validate-projects.sh
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=RenderTexture.*,ScopedRenderTarget2D.*'
./WindowsDesktop/run-tests.ps1
```

Platform execution is separate from project registration validation. Follow the
[development guide](../development/README.md#build-and-test) for build settings,
logs, and platform-specific diagnostics.
