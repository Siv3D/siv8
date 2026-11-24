# RenderTextureOps

`RenderTextureOps` writes processed texture pixels into an explicitly supplied
`RenderTexture`. It provides `Copy`, `Resample`, `Downsample`, `GaussianBlur`, and
`DualKawaseBlur`. The [public header](../../Siv3D/include/Siv3D/RenderTextureOps.hpp)
owns the contracts and all overloads. The
[interactive gallery](../../Test/Manual/RenderTextureOps.md) compares the results.

## Typical caller code

Keep outputs and the workspace outside the frame loop:

```cpp
RenderTexture small{ Size{ 320, 180 } };
RenderTexture blurred{ small.size() };
RenderTextureOps::Workspace work;

// Each frame, after updating or drawing source:
RenderTextureOps::Downsample(source, small, work);
RenderTextureOps::GaussianBlur(small, blurred, 3.0, work);
blurred.draw(20, 20);
```

For a broad blur at the source's resolution:

```cpp
RenderTextureOps::DualKawaseBlur(source, result, 8.0, work);
```

`result` has the same size as `source`. Dual Kawase owns its down/up pyramid;
there is no separate caller-side downsample requirement. Gaussian sigma and
Kawase radius use different scales and produce different kernels. Compare the
images when substituting one for the other. Small images or weak blur may favor
Gaussian because Dual Kawase has more render passes.

There is no implicit Scene destination. Processing cost follows the explicit
RT pixel sizes; ordinary drawing controls placement, scaling, and composition.
An MSRenderTexture input exposes its last resolved image. Resolve and generate
mips explicitly where needed.

## Reusing a larger output

Input and output regions are physical pixel rectangles. With a `TextureRegion`
input, its display size is ignored. Both forms below are equivalent:

```cpp
const Rect input{ 32, 16, activeWidth, activeHeight };
const Rect output{ 0, 0, input.size };
RenderTextureOps::GaussianBlur(atlas(input), destination, output, 2.0, work);
RenderTextureOps::GaussianBlur(atlas, input, destination, output, 2.0, work);
```

`destination` can retain its maximum expected capacity as the active region
changes. Input crops always require an output rectangle. Whole-texture overloads
use the entire source and destination, which is convenient when their actual
sizes are the intended processing sizes.

Filters clamp every sampled tap to the active input region. Outside pixels,
including unused workspace capacity, do not participate. The output rectangle's
exterior is preserved. This treats a crop as an independent image, so filtering
an atlas tile does not use neighboring tiles as a halo.

This boundary behavior applies to processing, not to later ordinary drawing.
Enlarging `destination(output)` with linear sampling can sample outside that
region at its edge. Use nearest sampling for pixel inspection, or provide a
suitable border/standalone texture for smooth enlargement. Do not assume a
processing region changes the sampler rules of `TextureRegion::draw()`.

## Choosing an operation

| Operation | Intended use | Cost and quality |
| --- | --- | --- |
| Copy | Same-size, same-format saved texels | Native texture transfer, no shader/color conversion |
| Resample | Nearest/linear resize or color-format conversion | One raster pass; strong reduction can alias |
| Downsample | Antialiased reduction, including one axis | Staged reduction with at most 2× per axis per stage |
| GaussianBlur | Controlled local blur | Two separable passes; work grows with sigma |
| DualKawaseBlur | Broad, inexpensive blur | Down/up pyramid; work is concentrated in its larger levels |

Downsample repeatedly takes `max(destinationAxis, ceil(currentAxis / 2))` until
the final reduction is at most 2×. Exact 1×/2× axes need only a linear sample.
Other ratios average four linear samples at ±¼ of each output pixel's source
footprint along reduced axes. This is a low-pass approximation, not exact area
integration. One-pixel axes and odd dimensions remain valid.

Gaussian uses the normalized discrete weights through `ceil(3 * sigma)`, with
sigma limited to 32. Neighboring positive and negative taps are paired through
bilinear interpolation. Coefficients are cached when sigma stays unchanged.
The finite precision of the GPU sampler matters even with 32-bit float storage;
this API does not promise CPU/GPU bit identity or scientific convolution accuracy.

Dual Kawase maps radius to `log2(radius + 1)` pyramid levels. Each level halves
both axes with rounding upward. The down passes use the same footprint-aware low-pass reduction as Downsample.
This variant avoids repeatedly favoring an aligned bright pixel as odd dimensions halve;
it deliberately shares the reduction filter instead of using a center-heavy down kernel.
An up pass averages four axial
samples at ±2 texels (weight 1) and four diagonal samples at ±1 texel (weight 2),
divided by 12. Offsets use the input level's texels. At fractional levels, the
first up pass blends with the preserved parent before the remaining up passes.
This matches the adjacent integer-level results continuously without evaluating
two complete pyramids. Level count stops at 1×1; radius is limited to 256.
These details explain the radius scale; radius is neither Gaussian sigma nor a
strict support radius.

## Workspace ownership and precision

Workspace holds separate Gaussian, downsample-pyramid, and Kawase-pyramid
resources, plus alias/transition scratch when needed. A fixed downsample →
Gaussian chain does not alternate between incompatible temporary sizes.
There is no global pool, historical size cache, eviction policy, or user-visible
intermediate texture. Moving transfers ownership; a moved-from workspace can be
used again. Release drops ownership without waiting for already recorded work.
Downsample needs no workspace resources when different input/output textures
allow a single pass (at most 2× reduction per axis). This path also leaves a
fresh workspace uninitialized.

Initial textures use the required dimensions. Existing capacity is reused when
both dimensions and the format fit. Growth adds 25% slack along growing axes;
a highly changed aspect ratio falls back to the requested size instead of
combining excessive old width and new height. Shrinking or decreasing the number
of levels does not trim retained capacity. New shapes, formats, alias cases, or
Kawase transition scratch can still allocate until their required capacities
have been encountered. Use separate workspaces for independent pipelines with
persistently incompatible formats; routine release between frames defeats reuse.

Intermediate textures use 16-bit float, or 32-bit float when either endpoint is
a 32-bit float format. When both endpoints have the same one or two channels,
intermediates keep that channel count. All other combinations use RGBA, including
conversions between different channel counts. This preserves missing-channel
defaults while reducing scratch storage and bandwidth for masks and vector fields.
Values use PMA arithmetic with no straight/PMA conversion;
sRGB sampling and output encoding use the native texture format. Intermediate
rounding means 16-bit UNORM endpoints do not imply 16-bit UNORM precision
through a blur. Copy preserves saved texels exactly. The device must support
linear filtering for every sampled format; unsupported filtering throws before
destination writes. Metal checks `supports32BitFloatFiltering`, and D3D11 caches
`D3D11_FORMAT_SUPPORT_SHADER_SAMPLE` per format. See
[Apple's filtering capability](https://developer.apple.com/documentation/metal/mtldevice/supports32bitfloatfiltering)
and [D3D11 format support](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_format_support).

## Backend responsibilities

The shared implementation validates arguments, prepares reusable resources, and
builds a bounded list of passes with explicit input/output rectangles. The
native texture subsystem executes this list after flushing preceding 2D draws.
It does not change logical Renderer2D settings or wait for GPU completion.

- Metal uses blit encoders for Copy and dedicated render encoders for filters.
  Pipelines are prepared before the first write. Partial user outputs use Load;
  full overwrite and private scratch use DontCare. Sampler cache state is reset
  for each new encoder.
- D3D11 uses CopySubresourceRegion or a vertex-ID fullscreen triangle. It unbinds
  outputs and shader resources between passes, and uses the renderer's native
  state caches. Constants use `ConstantBuffer<T>` and the existing shader binding
  functions, backed by dynamic buffers and `WRITE_DISCARD`. The normal next flush
  replays logical shaders, constants, targets, viewport, and states.

Both backends reuse typed `ConstantBuffer` objects for processing constants.
Common processing constants occupy 80 bytes at VS/PS slot 0. Gaussian
coefficients are separate at PS slot 1; `setActiveSize()` selects the used prefix
of the 49-element array, so only active coefficients are uploaded.
The native D3D11 buffer is created on first use, after device initialization;
constructing the typed object does not require a ready renderer.
Other filters do not prepare or upload Gaussian data. Shader selection maps
each kernel explicitly to its whole-input and bounded-input entries and does
not depend on the order of `EnginePS` enumerators.

Filters have whole-input and bounded-input shader entries. Selection depends on
the physical input texture and active input region for each pass. A partial
output alone does not require a bounded-input shader. Thus ordinary whole-texture
processing with exact workspace sizes uses hardware clamp without extra region
clamps. Shrinking a workspace region selects bounded sampling only where needed.

The public API does not expose compute dispatch or arbitrary processing passes.
The private pass-list interface is also used to compare workspace planning with
explicitly prepared intermediate RTs in the benchmark.

## Validation and measurements

[Shared tests](../../Test/Test_RenderTextureOps.cpp) cover raw copy, independent
CPU Gaussian convolution, ROI poison, capacity history, aliasing, odd/one-pixel
sizes, strength transitions, sRGB/HDR, untouched mips, empty/error arguments,
draw ordering, custom shaders/constants/MRT, and resolved MSAA inputs. Narrow
channel processing and format changes through a reused workspace are compared
with an RGBA32Float reference, including missing-channel defaults.
[Metal tests](../../Test/Test_MetalRenderTextureOps.cpp) inject allocation failure,
verify reuse after warm-up, and measure the actual engine backend. Performance
thresholds are not correctness assertions. The macOS full host suite and the
focused functionality tests with Metal API Validation enabled pass. Validation
was disabled for the timing runs.

```sh
./macOS/run-tests.sh '--test-case=RenderTextureOps.*' '~[benchmark]'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=RenderTextureOps.Metal.benchmark'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=RenderTextureOps.Metal.processing_cost_benchmark'
./macOS/run-tests.sh
```

On Windows, this selection checks the processed images and the constant-buffer
bindings used by normal draws before and after a processing pass.

```powershell
./WindowsDesktop/run-tests.ps1 -Configuration Release -TestArguments '--test-case=RenderTextureOps.*,ConstantBuffer.*'
```

The opt-in benchmark warms textures, pipelines, and coefficients, then records
seven batches of 20 repeated operations on resident RGBA16Float images with a deterministic textured input. It reports
median CPU encoding time without GPU waits and command-buffer GPU time per
operation. Cases are interleaved with rotating order to reduce clock and ordering bias.
Explicit Gaussian and Workspace Gaussian use the same shaders,
coefficient pairing, and two-pass algorithm. The oversized case preallocates an
extra 128 pixels on each axis. Repeated resident inputs emphasize steady-state
reuse, not frame latency or upload cost. Dual Kawase radius 32 and Gaussian sigma
8 are different visual strengths; those rows are not equal-quality comparisons.

`processing_cost_benchmark` uses the same batch and repetition counts to measure
Gaussian sigma 8, Dual Kawase radius 7, Resample, half-size Downsample with reused
and fresh workspaces, and native Copy. It covers 64×64 and 1920×1080 images in
R8, R32Float, RG32Float, and RGBA16Float. A scoped interactive QoS override reduces
CPU scheduling noise; each batch advances the frame outside timing. Fresh-workspace
Downsample includes workspace construction and destruction, but requires no
intermediate GPU texture. Compare repeated runs on the same idle machine;
CPU timing, especially for large images, can still vary independently of GPU time.

### Reference measurements

On Apple M2, Release, the processing-cost benchmark gave the following ranges
of per-run medians over two runs. GPU times are milliseconds per operation on
1920×1080 inputs. These are diagnostic samples, not performance guarantees.

| Endpoint format | Gaussian sigma 8 | Dual Kawase radius 7 |
| --- | ---: | ---: |
| R8 UNORM | 1.209–1.261 | 0.365–0.369 |
| R32Float | 1.344–1.375 | 0.440–0.454 |
| RG32Float | 2.432–2.501 | 0.800–0.819 |
| RGBA16Float | 1.578–1.618 | 0.484–0.496 |

Narrow intermediate formats matter most for larger float images. Small images
can be dominated by per-pass overhead; reducing storage or constant bytes does
not guarantee a shorter GPU time. Full-HD CPU encoding times varied substantially
between runs, so they are unsuitable for attributing small changes in preparation
cost. In 64×64 R8 cases, single-pass Downsample took 5.96–6.02 µs with a reused
workspace and 5.82–6.19 µs with a fresh workspace, including its construction and
destruction. The fresh-workspace path does not allocate intermediate handles.

Moving processing constants to typed `ConstantBuffer` objects preserves the
existing active-prefix transfer lengths. The 64×64 R8 Gaussian CPU time was
11.30 µs before the change and 11.10–11.36 µs over the two runs above. Its full-HD
GPU time was 1.248 ms before the change. These measurements show comparable cost,
not an additional filter speedup; the benefit of selecting a smaller prefix for
general custom constants is measured in the [constant-buffer benchmark](README.md#custom-constant-buffers).

The separate path-comparison benchmark gave these GPU times for 1920×1080
RGBA16Float. Both Gaussian variants use identical coefficients and shaders.

| Path | GPU ms |
| --- | ---: |
| Explicit intermediate RTs, Gaussian sigma 8 | 1.592 |
| Workspace, Gaussian sigma 8 | 1.594 |
| Oversized intermediate, Gaussian sigma 8 | 1.590 |
| Bounded source and intermediate, Gaussian sigma 8 | 1.605 |
| Gaussian sigma 12 | 2.255 |
| Dual Kawase radius 7 | 0.492 |
| Downsample to 1/4, Gaussian sigma 3, resize back | 0.296 |
| Dual Kawase radius 32 | 0.662 |
| Native Copy | 0.204 |

The exact-size Workspace and explicit paths have comparable GPU cost. Bounded
sampling had a small cost in this full-HD case; the 512×512 measurements were
more variable, so these results do not define a universal cutoff or clamp
penalty. More passes can outweigh Dual Kawase's sampling savings on small images.

On a GeForce RTX 4090 Laptop GPU in Release x64 with the D3D11 debug layer
off, the same 1920×1080 RGBA16Float processing-cost workload took
0.158–0.177 ms for Gaussian sigma 8 and 0.047–0.049 ms for Dual Kawase radius 7.
These are ranges of three run medians. D3D11 timestamp queries bracketed each
batch of 20 operations after warm-up, with the disjoint flag checked before
converting ticks to elapsed GPU time.

For an isolated central point in a 513×513 float image, Kawase radii 3, 7, and 15
had horizontal RMS widths of approximately 5.39, 10.98, and 21.88 pixels, with
total contributions around 0.99. Thus radius 7 and Gaussian sigma 12 compare
roughly similar widths, not identical kernels. The 1/4 Gaussian pipeline can be
faster still; use the gallery to decide whether its detail loss is acceptable.
The point measurement and odd-size/one-pixel-shift regression tests guard against
brightness amplification during repeated reduction.

The gallery's default configuration was rendered and captured on macOS and
Windows. Both captures showed clean crop edges and the expected differences
among the three blur paths. On D3D11, setting both strengths to zero reproduced
the source crop pixel for pixel in the Gaussian and Dual Kawase panels.
Changing the active width from 256 to 97 to 64 pixels, then restoring the initial
width and strengths, reproduced the original frame. The gallery sliders let
you repeat these checks interactively on another device.

[Backend research](proposals/texture-processing.md) describes the standalone
Compute/MPS diagnostic.
