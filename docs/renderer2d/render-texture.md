# RenderTexture resources

[RenderTexture](../../Siv3D/include/Siv3D/RenderTexture.hpp) owns a single-sample
color texture. It supports creation, GPU clears, mipmap generation, CPU readback,
and the drawing operations inherited from `Texture`. Render-target switching,
MRT, depth attachments, and multisample resources are tracked in
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

## Ordering and backend responsibilities

Clears and mip generation consume earlier queued 2D draws before changing an
existing texture. This gives `draw(); clear(); draw();` an observable order even
though normal draws are batched. Texture creation uploads new resources without
flushing unrelated draws.

Metal encodes resource operations in the active frame command buffer, or uses a
standalone buffer on the same queue when no frame is recording. Readback appends
a texture-to-buffer copy, submits and waits, then continues recording in a new
command buffer. This retains the acquired frame slot, frame number, scene pass
state, and vertex/index allocation cursors. The synchronous segment completes
before the slot can be reused, including when the remaining frame is canceled.
The continuation buffer is allocated before submitting the current segment.

D3D11 uses default-usage shader-readable render-target resources and a staging
texture for readback. Format support, including automatic mip generation when
requested, is checked before resource allocation. Copies use the native row
pitch and return tightly packed CPU pixels.

Color textures do not own depth resources. Subsequent target binding should
compose color handles with an independent depth resource. Operations on a color
texture must preserve this ownership boundary.

## Validation

[Shared tests](../../Test/Test_RenderTexture.cpp) exercise all current
non-compressed color formats, odd row widths, native-value round trips, sRGB and
HDR clears, generated mip contents, handle sharing, invalid inputs, and drawing
around clears and readbacks. [Metal tests](../../Test/Test_MetalRenderTexture.cpp)
cover allocation failure/retry and frame-slot preservation when a continuation
command buffer cannot be allocated.

Run from the repository root using the host workflow:

```sh
./macOS/run-tests.sh '--test-case=RenderTexture.*'
./macOS/run-tests.sh
./macOS/validate-projects.sh
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=RenderTexture.*'
./WindowsDesktop/run-tests.ps1
```

Platform execution is separate from project registration validation. Follow the
[development guide](../development/README.md#build-and-test) for build settings,
logs, and platform-specific diagnostics.
