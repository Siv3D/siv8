# RenderTexture and MSRenderTexture resources and target scopes

[RenderTexture](../../Siv3D/include/Siv3D/RenderTexture.hpp) owns a single-sample
color texture. It supports creation, GPU clears, mipmap generation, CPU readback,
and the drawing operations inherited from `Texture`.
[ScopedRenderTarget2D](../../Siv3D/include/Siv3D/ScopedRenderTarget2D.hpp) selects
up to eight color textures with a common size and sample count for 2D drawing.
[MSRenderTexture](../../Siv3D/include/Siv3D/MSRenderTexture.hpp) owns a multisample
color texture and a separate single-sample resolve result. A target scope writes
to its MSAA image; sampling uses the last explicitly resolved image.
[ColorRenderTarget](../../Siv3D/include/Siv3D/ColorRenderTarget.hpp) shares either
kind through one typed handle, with common clear, resolve, and mip operations.

## Scope of the 2D color-target implementation

The current implementation covers offscreen composition, ping-pong image
processing, HDR intermediates, and single-sample or MSAA passes with multiple
color outputs. These uses can be developed with the existing color resources
and target scopes. Depth resources are a separate extension, to be designed
with the rendering passes that need depth testing, sharing, and sampling.

RenderTexture and MSRenderTexture both derive from Texture; ColorRenderTarget
provides their common target operations without making either resource inherit
the other's semantics. The distinction between the drawing image and the sampled
image remains explicit. The renderer and scopes share one ordered target state,
so single-output and MRT passes use the same restoration and feedback rules.

Future work is tracked in [TODO](../../TODO.md#rendertexture--depthtexture).
Revisit independent output blending when a pass needs different blend settings
per output. Revisit depth bindings with 3D rendering requirements, including
queue ordering for resource operations. Keep color-resource ownership separate
from depth ownership; the current 2D-only Flush policy of clear, mip generation,
and readback does not define how future 2D and 3D queues should be coordinated.

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

Both `RenderTexture` and `DynamicTexture` accept `Grid<uint16>` directly as
`R8G8_Unorm` and also provide `CreateR8G8_Unorm()`. To initialize a
`RenderTexture` from a packed format such as `R10G10B10A2_Unorm`, use its
named factory or the size/span/format constructor.
The span contains tightly packed rows; backend transfer alignment is internal.
The public header specifies the corresponding readback types and failure rules.
Integer Grid reads accept their listed formats, not every format with the same
pixel size. Use `read(std::span<Byte>)` for format-independent raw bytes.
RenderTexture and MSRenderTexture share the typed-read compatibility mapping.
Reading a selected mip level also allows inspection of generated content.

Use `texture.mipSize(level)` to size a raw readback buffer for an allocated mip:

```cpp
const uint32 level = 1;
const Size levelSize = texture.mipSize(level);
Array<Byte> raw(texture.format().packedDataSize(levelSize));
const bool succeeded = texture.read(raw, level);
```

`ImageProcessing::CalculateMipmapSize(baseSize, level)` performs the same size
calculation without a texture. It describes a complete mathematical mip chain;
`Texture::mipSize()` also checks which levels the resource actually has. Mips
halve each axis with truncation, unlike a reduction pyramid that rounds up.

`TextureFormat::packedRowBytes()` and `packedDataSize()` describe tightly packed
data, including BCn block rounding. They do not estimate GPU allocation size.
For a 3×2 R8 image the row size is 3 bytes and the data size is 6 bytes. The older
`bytesPerRow()` retains its four-byte row alignment for uncompressed formats;
its result is 4 bytes for that image. Native transfer alignment remains internal.

`TextureFormat::isFloat32()` identifies R32Float, RG32Float, and RGBA32Float.
Processing code uses it to retain endpoint precision in intermediate textures,
and native code uses it alongside device capability checks. It does not imply
filtering or MSAA support on the active device.

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

## Sharing color target code

Use `ColorRenderTarget` when a function accepts either RenderTexture or
MSRenderTexture. Conversion is implicit and retains the original resources;
copying a target does not allocate a GPU image or copy pixels. An ordinary
Texture cannot be used to construct it.

For example, define this helper outside `Main()`:

```cpp
void DrawInto(const ColorRenderTarget& target)
{
    const ScopedRenderTarget2D scoped{ target };
    target.clear(ColorF{ 0.0, 0.0 });
    Circle{ 100, 100, 40 }.draw(Palette::Orange);
    Graphics2D::Flush();
    target.resolve();
}
```

`DrawInto(rt)` and `DrawInto(msrt)` use the same helper. Flush occurs before
resolve even if the caller already has this target selected, in which case
entering/leaving the nested scope need not flush. Resolve is a no-op for
single-sample and empty targets; it never submits pending draws or generates
mips. Clear and mip generation retain the original texture's operation ordering.

`getTexture()` returns a shared sampling handle by value. For MSAA it refers to
the last resolve result. Obtaining it does not Flush or resolve, and the returned
handle remains valid after the ColorRenderTarget is released. Use the original
RenderTexture or MSRenderTexture for typed CPU readback. Copies, moves, assignment
and swap retain resource identity; equality compares the referenced resource.

The scope accepts `Optional<ColorRenderTarget>`. Direct RenderTexture,
MSRenderTexture and ColorRenderTarget arguments, their Optional forms, and `none`
remain usable. An absent Optional selects Scene. An engaged Optional containing
an empty target is an error, just like a directly supplied empty target.

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

## Multiple color outputs

Use an array of `ColorRenderTarget` handles for a reusable set, or a braced list
for a local pass. The order selects shader output slots. The scope copies the
handles, so a temporary array is safe. Size, sample count, empty handles, and
resource duplication are checked before flushing or changing the current state.
The public header owns the complete selection contract. An explicit empty span
or list is an error; `none` selects Scene and the default constructor is inert.

All outputs use the current blend state. Standard shaders, including engine font
shaders, only update slot 0 and preserve the remaining attachments. A custom
pixel shader must return a value for every selected output on every path that
produces a fragment. Use `SV_TargetN` in HLSL and `[[color(N)]]` in MSL. Output
completeness is a shader-author contract; the engine does not analyze every
shader execution path. For material data, use `BlendState::Opaque`.

For example, inside `Main()`:

```cpp
const RenderTexture color{ Size{ 320, 180 }, Palette::Black };
const RenderTexture normal{ color.size(), ColorF{ 0.0, 0.0 },
    TextureFormat::R16G16B16A16_Float };
const std::array<ColorRenderTarget, 2> targets{ color, normal };
const PixelShader ps = HLSL::FromSource(UR"(
    struct Output { float4 color : SV_Target0; float4 normal : SV_Target1; };
    Output PS()
    {
        Output o;
        o.color = float4(1, 0.5, 0, 1);
        o.normal = float4(0.5, 0.5, 1, 1);
        return o;
    })", U"PS") | MSL::FromSource(UR"(
    #include <metal_stdlib>
    using namespace metal;
    struct Output { float4 color [[color(0)]]; float4 normal [[color(1)]]; };
    fragment Output PS()
    {
        Output o;
        o.color = float4(1, 0.5, 0, 1);
        o.normal = float4(0.5, 0.5, 1, 1);
        return o;
    })", U"PS");
if (not color || not normal || not ps)
{
    return;
}
{
    const ScopedRenderTarget2D target{ targets }; // Also: { color, normal }
    const ScopedCustomShader2D shader{ ps };
    const ScopedRenderStates2D blend{ BlendState::Opaque };
    Circle{ 160, 90, 60 }.draw();
}
color.draw();
normal.draw(320, 0);
```

Both textures have the same size and sample count, while their formats and bits
per pixel differ.
GPU format and attachment-capacity limits still apply; a resource being usable
alone does not promise that every MRT combination is supported. Native pipeline
creation failures are reported, without silently dropping outputs or changing
formats. Different resolutions require separate passes.

MRT scopes nest with Scene, single RenderTexture, and single MSRenderTexture
scopes. The complete ordered set and viewport are restored. Reordering the same
handles changes the binding; selecting the same ordered set can keep pending
draws batched. Each operation on an individual texture retains its existing
clear/read/mip/resolve behavior and leaves the entire set selected. Input
feedback is rejected for any active single-sample output, including aliases.

## Multisample MRT

An MRT set can contain single-sample RenderTextures, 2x MSRenderTextures, or 4x
MSRenderTextures. Every selected image must have the same sample count. Mixing
single-sample and MSAA images, or 2x and 4x images, is rejected before Flush or
state changes. Scene's sample count is independent. Each MSRenderTexture keeps
its own single-sample resolve result; those results are not drawing attachments.

The MRT shader above can also draw to multisample targets. With `ps` from that
example, inside `Main()`:

```cpp
const MSRenderTexture colorMS{ Size{ 320, 180 }, Palette::Black,
    MSAASampleCount::X4MSAA };
const MSRenderTexture normalMS{ colorMS.size(), ColorF{ 0.0, 0.0 },
    MSAASampleCount::X4MSAA, TextureFormat::R16G16B16A16_Float };
if (not colorMS || not normalMS || not ps)
{
    return;
}
const std::array<ColorRenderTarget, 2> targetsMS{ colorMS, normalMS };
{
    const ScopedRenderTarget2D target{ targetsMS };
    const ScopedCustomShader2D shader{ ps };
    const ScopedRenderStates2D blend{ BlendState::Opaque };
    Circle{ 160, 90, 60 }.draw();
    Graphics2D::Flush();
    for (const auto& output : targetsMS)
    {
        output.resolve();
    }
}
colorMS.draw();
normalMS.draw(320, 0);
```

Resolve remains explicit for each output and does not Flush, wait for GPU
completion, or generate mipmaps. Resolve only the outputs needed by subsequent
work. Entering/leaving a scope, reordering outputs, readback, and frame boundaries
do not resolve any output. Re-entering the same ordered set may retain queued
draws, so it cannot replace the explicit Flush in this example.

A multisample set can sample any of its resolved images while drawing. Those
images remain unchanged until explicitly resolved. For queued sampling to see the
old image, Flush before resolving; without Flush, queued draws can see the new
resolve result. Clear changes only the selected resource's MSAA image, and mip
generation uses that resource's last resolve result. Both retain their existing
2D Flush behavior and preserve the entire active MRT set. Switching between
single-sample MRT, 2x MRT, 4x MRT, a single target, and Scene restores the complete
ordered set and viewport, including after readback or a frame boundary.

## Ordering and backend responsibilities

For a pass that needs a sampled MSAA result and its lower mip levels, use
`draw -> Graphics2D::Flush() -> resolve -> generateMips -> sample/read`.
Resolve each required MRT output after the shared Flush. If lower mip levels
are not used, omit mip generation. Readback is optional and waits for the GPU;
sampling the result in a later GPU pass does not require readback.

The operation effects below summarize the public contracts in
[RenderTexture](../../Siv3D/include/Siv3D/RenderTexture.hpp) and
[MSRenderTexture](../../Siv3D/include/Siv3D/MSRenderTexture.hpp). They describe
valid operations on non-empty resources; mip generation requires allocated
lower levels. ColorRenderTarget delegates to the same operations.

| Operation | RenderTexture image | MSRenderTexture images | Issues queued 2D draws | Waits for GPU completion |
|---|---|---|---|---|
| `clear()` | Changes level 0 | Changes only the MSAA image | Yes | No |
| `ColorRenderTarget::resolve()` | No-op | Updates resolved level 0 from the MSAA image | No | No |
| `generateMips()` | Regenerates lower levels from level 0 | Regenerates lower levels from resolved level 0 | Yes | No |
| `read()` / `readAsImage()` on the original resource | Reads the requested level | Reads the requested resolved level | Yes | Yes |
| Switch or restore a different target set | Preserves contents | Preserves both images; no resolve | Yes | No |

The Flush performed by `read()` or `generateMips()` does not resolve an MSAA
image. Calling either after a resolve that preceded queued drawing still uses
that earlier resolve result. Clear gives `draw(); clear(); draw();` an
observable order even though normal draws are batched. Texture creation uploads
new resources without flushing unrelated draws.

Changing targets flushes the preceding draw batch without waiting for the GPU.
Re-entering the same target can retain the batch. Native resource ownership is
retained through command execution; texture contents are loaded on each pass.
Feedback validation runs before geometry allocation, independently of texture
binding and draw submission. On each target change, it records the IDs whose
sampled images alias active outputs; Scene and MSRenderTexture have no such IDs.
The fixed-capacity state retains only active texture handles without allocating
a container. It remembers the last successfully validated input ID for that set. Repeated
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
Texture target passes always load/store all their color attachments. For
MSRenderTexture, this is the MSAA image with all samples retained and no resolve
attachment. The pipeline cache includes the ordered color formats, attachment
count, sample count, common blend state, and output write mask. Scene's
initial clear and MSAA store/resolve policy are tracked independently.

D3D11 uses default-usage shader-readable render-target resources and a staging
texture for readback. Format support, including automatic mip generation when
requested, is checked before resource allocation. For MSRenderTexture, the RTV
selects the MSAA image and the SRV selects the single-sample resolve result.
Copies use the native row
pitch and return tightly packed CPU pixels. Output views are unbound at the end
of each execution, and VS/PS input views are cleared around target binding, so
subsequent sampling, clear, mip generation and readback do not inherit conflicts.

## Validation

[TextureFormat tests](../../Test/Test_TextureFormat.cpp) check format classification,
packed sizes, BCn blocks, overflow, and compatibility of the existing row pitch.
[Mipmap tests](../../Test/Test_TextureMipmap.cpp) compare calculated and allocated
sizes with readbacks. They draw each retained level of explicit Image and BC1
chains in a distinct color, including Image chains truncated at an invalid level.

[MRT tests](../../Test/Test_MRT.cpp) cover array/list selection, one through eight
outputs, standard/custom shader transitions, mixed formats and layout ordering,
sRGB/HDR values, common blending and channel masks, viewport clipping, invalid
sets, feedback for multiple outputs,
nested restoration, resource lifetime, read/mip/clear operations, and frame
continuation under both Scene sample counts. MSAA cases cover 2x/4x sets through
eight outputs, partially covered edge pixels, simultaneous sRGB/HDR output and
common blending, independent explicit resolve,
resolved-image sampling, mixed single/multisample nesting, mip contents, and
continued drawing across multiple frame and readback boundaries.

[Shared tests](../../Test/Test_RenderTexture.cpp) exercise all current
non-compressed color formats, odd row widths, native-value round trips, sRGB and
HDR clears, generated mip contents, handle sharing, invalid inputs, and drawing
around clears and readbacks. [Metal tests](../../Test/Test_MetalRenderTexture.cpp)
cover allocation failure/retry and frame-slot preservation when a continuation
command buffer cannot be allocated.

[Common-target tests](../../Test/Test_ColorRenderTarget.cpp) cover accepted and
rejected conversions, handle sharing and lifetime, copy/move/swap, common resource
operations, non-MS resolve without Flush, explicit MSAA resolve ordering, and
legacy scope argument forms with nested restoration and feedback checks.

[Target-scope tests](../../Test/Test_ScopedRenderTarget2D.cpp) check nested pixel
results, viewport/state restoration, all color formats, feedback rejection,
readback and mip generation inside a scope, Scene MSAA and resolution scaling,
and frame continuation. [Metal scope tests](../../Test/Test_MetalScopedRenderTarget2D.cpp)
inject encoder failure to verify entry rollback, whole-set restoration, rejected
MRT sets without Flush (including mixed sample counts), and exception unwinding
for single-sample and multisample sets.
[Metal pipeline tests](../../Test/Test_MetalRenderPipelineState.cpp) verify cache
separation by attachment order, count, sample count, and write mask, plus native creation
failure/retry.

[Multisample resource tests](../../Test/Test_MSRenderTexture.cpp) check supported
format/sample combinations, initialization, stale resolve results, independent
mips, readback types and failures, handle lifetime, sampled native resources, and
the visible difference between explicit Flush and resolve alone. A typed-read
compatibility matrix checks the same accepted and rejected Image/Grid types for
RenderTexture and 2x/4x MSRenderTexture, comparing accepted reads to raw bytes.
Target tests
cover 2x/4x edge coverage, sRGB/HDR drawing, mixed single/multisample scopes,
viewport and handle restoration, sampling the active MS target's resolved image,
and continuation across readback and frame boundaries without implicit resolve.
MRT cases pair each supported color format with an RGBA8 attachment at 2x/4x,
check resolved bytes, and inspect both native drawing images separately from
their resolve results. Unsupported resource combinations follow the native
capability checks used by the standalone MSRenderTexture tests.
[Metal multisample tests](../../Test/Test_MetalMSRenderTexture.cpp) inspect native
allocation descriptors and inject unsupported capabilities, failure of either
texture allocation, and resolve-encoder failure followed by retry.

Run from the repository root using the host workflow:

```sh
./macOS/run-tests.sh '--test-case=MRT.*,ColorRenderTarget.*,RenderTexture.*,MSRenderTexture.*,ScopedRenderTarget2D.*,MetalRenderPipelineState.*'
./macOS/run-tests.sh
./macOS/validate-projects.sh
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=MRT.*,ColorRenderTarget.*,RenderTexture.*,MSRenderTexture.*,ScopedRenderTarget2D.*'
./WindowsDesktop/run-tests.ps1
```

Platform execution is separate from project registration validation. Follow the
[development guide](../development/README.md#build-and-test) for build settings,
logs, and platform-specific diagnostics.

For validation on another machine, record the backend, build configuration,
executed selection, complete XML results, and actual format/sample support.
Successful creation on a capable GPU does not exercise rejection for an
unsupported capability; Metal failure injection and D3D11 device execution are
separate evidence. The regular suite also excludes opt-in UI and benchmark
cases. Re-run the [texture-update benchmark](dynamic-texture.md) when changing
upload or synchronization behavior, rather than treating correctness results
as a performance comparison.

When using the 64-bit compiler-host workaround on Windows, follow the
[incremental-build diagnosis guide](../development/README.md#windows-incremental-build-failures)
for an explicit build and subsequent `-SkipBuild` runs. A previous successful
binary must match the current sources and build settings.
