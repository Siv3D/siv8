# RenderTexture and MSRenderTexture resources and target scopes

[RenderTexture](../../Siv3D/include/Siv3D/RenderTexture.hpp) owns a single-sample
color texture. It supports creation, GPU clears, mipmap generation, CPU readback,
and the drawing operations inherited from `Texture`.
[ScopedRenderTarget2D](../../Siv3D/include/Siv3D/ScopedRenderTarget2D.hpp) selects
one color texture for 2D drawing.
[MSRenderTexture](../../Siv3D/include/Siv3D/MSRenderTexture.hpp) owns a multisample
color texture and a separate single-sample resolve result. A target scope writes
to its MSAA image; sampling uses the last explicitly resolved image.
MRT and depth attachments are tracked in
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

## Multisample resources and explicit resolve

`MSRenderTexture` derives directly from `Texture`. Its `Texture` operations,
including regions and shader sampling, use the resolved image. Copies share both
resources. The sample count is fixed at creation: 2x or 4x, with 4x as the default,
independently of Scene's current settings. Unsupported settings produce an empty
handle; the engine does not lower the sample count or change the requested format.
The public header defines creation and readback failure behavior.

Inside `Main()` after engine startup:

```cpp
MSRenderTexture texture{ Size{ 64, 32 }, Palette::Blue,
    MSAASampleCount::X4MSAA, TextureFormat::R8G8B8A8_Unorm,
    TextureDesc::Mipmap };

if (texture)
{
    texture.clear(Palette::Red);
    Image pixels;
    texture.readAsImage(pixels); // Still blue: clear changes only the MSAA image.

    texture.resolve();
    texture.generateMips();
    texture.readAsImage(pixels); // Red, including the newly generated mip levels.
}
```

Creation initializes both images and allocated mip levels to the initial color
(zero in every component when omitted). Thereafter, `clear()` changes only MSAA
contents. `resolve()` updates only level 0 of the single-sample image and preserves
the MSAA contents, allowing repeated resolves. Neither readback nor mip generation
resolves implicitly. Mips belong only to the single-sample image.

`resolve()` does **not** flush pending renderer commands or wait for GPU completion.
To include queued draws into the MSAA image, explicitly flush that renderer
before resolving. The same rule applies when queued draws need to consume the
previous resolved image: without Flush, they may consume the newly resolved image
when eventually issued. Select the renderer and ordering at the call site;
resolve does not choose an order between 2D and future 3D queues.
The other resource operations retain the existing 2D flush behavior described
below. This distinction also applies while another texture is selected.

On D3D11, creation checks the format's render, sample, resolve and requested mip
capabilities, plus the requested multisample quality levels. Sampling always uses
a Texture2D SRV of the single-sample image. On Metal, creation checks the sample
count and the additional [32-bit MSAA resolve capability](https://developer.apple.com/documentation/metal/mtldevice/supports32bitmsaa)
for 32-bit floating-point formats. A render pass with store-and-resolve preserves
the multisample image while updating the resolved result. Both images use private
GPU storage and remain alive through encoded command execution.

## Drawing into a texture

A target scope preserves the texture's pixels and starts with a full-target
viewport. Both `RenderTexture` and `MSRenderTexture` can be selected. Nest scopes
to draw intermediate images, then use them in the outer target. Inside `Main()`:

```cpp
MSRenderTexture canvas{ Size{ 320, 180 } }; // 4x MSAA
RenderTexture stamp{ Size{ 64, 64 } };
if (not canvas || not stamp)
{
    return;
}

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
    canvas.resolve();
    canvas.draw(40, 40);
}
```

`ScopedRenderTarget2D{ none }` temporarily selects Scene; the default constructor
leaves the current state alone. An empty `RenderTexture` or `MSRenderTexture` is
an error. The scope holds a shared handle, so releasing or replacing the caller's
handle does not change the selected resource. Moving the scope transfers its
restoration duty.

`Graphics2D::GetDrawingSize()` and `Graphics2D::GetRenderTargetSize()` return the
texture's pixel dimensions inside a target scope. Camera defaults use that
drawing size. Scene's display-resolution scaling is disabled for textures,
including curve subdivision, but Local, Camera and
QuadWarp transforms still apply. Scissor rectangles, colors, blend states,
shaders, and constant buffers carry across target changes. The prior viewport
and target are restored at scope exit; other states use their own scope objects.

Sampling the active single-sample `RenderTexture`, including through a copied
handle or `TextureRegion`, throws `Error` before queuing that draw. This also
applies when using a custom shader with a textured draw. Use two RenderTextures
for iterative image processing. An active `MSRenderTexture` can sample its own
resolve result because the input and output are separate native images. That
input stays unchanged until the next explicit `resolve()`.

Leaving a scope for a different target submits its draws but does not resolve.
The example resolves after restoring Scene. To resolve while the texture is
still selected, call `Graphics2D::Flush(); canvas.resolve();` in that order.
Re-entering the same target does not itself require a Flush, so an inner scope
for the same texture is not a substitute for this explicit call.

Clear, resolve, readback, mip generation, and `Graphics2D::Flush()` may be used
inside a scope and preserve its target. For RenderTexture, drawing and clearing
change mip level 0. For MSRenderTexture, resolve updates level 0. Call
`generateMips()` afterward when lower levels are needed. A scope can span a
frame boundary: its target remains selected, while Scene still gets its
normal per-frame background and presentation.

## Ordering and backend responsibilities

Clears and mip generation consume earlier queued 2D draws before changing an
existing texture. Explicit MSRenderTexture resolve is the exception described
above. This gives `draw(); clear(); draw();` an observable order even though
normal draws are batched. Texture creation uploads new resources without
flushing unrelated draws.

Changing targets flushes the preceding draw batch without waiting for the GPU.
Re-entering the same target can retain the batch. Native resource ownership is
retained through command execution; texture contents are loaded on each pass.
Feedback validation runs before geometry allocation, independently of texture
binding and draw submission. On each target change, it records the ID whose
sampled image aliases the output; Scene and MSRenderTexture have no such ID.
It remembers the last successfully validated input ID for that output. Repeated
inputs skip output comparisons, while failed validation leaves the cache
unchanged. Every logical output change invalidates
it, including scope restoration and Metal's temporary Scene pass at frame end.
Ordinary flushes and re-entering the same output preserve it. Scene and
MSRenderTexture draws return early without an input-ID lookup. Sample count is
queried only on target changes, not for each textured draw.

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
Texture target passes always load/store their color attachment. For
MSRenderTexture, this is the MSAA image with all samples retained and no resolve
attachment. The pipeline uses the attachment's pixel format and sample count. Scene's
initial clear and MSAA store/resolve policy are tracked independently.

D3D11 uses default-usage shader-readable render-target resources and a staging
texture for readback. Format support, including automatic mip generation when
requested, is checked before resource allocation. For MSRenderTexture, the RTV
selects the MSAA image and the SRV selects the single-sample resolve result.
Copies use the native row
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

[Multisample resource tests](../../Test/Test_MSRenderTexture.cpp) check supported
format/sample combinations, initialization, stale resolve results, independent
mips, readback types and failures, handle lifetime, sampled native resources, and
the visible difference between explicit Flush and resolve alone. Target tests
cover 2x/4x edge coverage, sRGB/HDR drawing, mixed single/multisample scopes,
viewport and handle restoration, sampling the active MS target's resolved image,
and continuation across readback and frame boundaries without implicit resolve.
[Metal multisample tests](../../Test/Test_MetalMSRenderTexture.cpp) inspect native
allocation descriptors and inject unsupported capabilities, failure of either
texture allocation, and resolve-encoder failure followed by retry.

Run from the repository root using the host workflow:

```sh
./macOS/run-tests.sh '--test-case=RenderTexture.*,MSRenderTexture.*,ScopedRenderTarget2D.*'
./macOS/run-tests.sh
./macOS/validate-projects.sh
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=RenderTexture.*,MSRenderTexture.*,ScopedRenderTarget2D.*'
./WindowsDesktop/run-tests.ps1
```

Platform execution is separate from project registration validation. Follow the
[development guide](../development/README.md#build-and-test) for build settings,
logs, and platform-specific diagnostics.
