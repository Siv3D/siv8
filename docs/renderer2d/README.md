# Renderer2D

- [Scoped projective drawing](quad-warp.md): transform scopes, homography generation,
  custom VS layout, and migration from dedicated texture warping.

- [Triangle and Quad GPU joins](gpu-joins.md): direct mesh eligibility, alpha
  coverage, compatibility, tests, and CPU benchmarks.

## Scene rendering

- [Scene configuration and render resolution](scene-high-resolution.md): presets,
  fullscreen presentation, scene sizing, resolution scaling, and validation.

## Textured circles

[TexturedCircle](../../Siv3D/include/Siv3D/TexturedCircle.hpp) supports center-to-edge,
vertical and horizontal tint gradients, including alpha fades:

```cpp
Circle{ 240, 140, 80 }(texture).draw(ColorF{ 1.0, 1.0 }, ColorF{ 1.0, 0.0 });
Circle{ 240, 140, 80 }(texture).draw(Arg::top = Palette::White, Arg::bottom = Palette::Skyblue);
Circle{ 0, 0, 80 }(texture).draw(Vec2{ 240, 140 }, Arg::left = Palette::Orange, Arg::right = Palette::White);
```

The gradients use the same vertex color interpolation as `Circle::draw()` and
follow the circle's local axes, independently of UV cropping or flipping.
`draw(x, y, ...)` and `draw(pos, ...)` specify the destination center, keeping the
stored radius. They return the drawn circle; the form without a position returns
a reference to the stored circle.

The existing texture shader and circle tessellation are shared with single-color
draws. Compile-time specialization omits gradient calculations from the
single-color textured path. The [vertex tests](../../Test/Test_Vertex2DBuilderEllipse.cpp)
check positions, colors, UVs and buffer bounds; the
[rendering tests](../../Test/Test_Renderer2DTexturedCircle.cpp) cover the public
overloads, texture and tint alpha, UV cropping/flipping, transforms and scene
render resolution.

`drawPie(startAngle, angle, diffuse)` draws a sector without changing the texture's
placement on the full circle. The overload with `innerColor, outerColor` adds a
center-to-edge tint gradient. This is useful for radial reveals and cooldown UI:

```cpp
Circle{ 240, 140, 80 }(texture).drawPie(0, Math::TwoPi * progress);
Circle{ 240, 140, 80 }(texture).drawPie(-30_deg, 120_deg,
    ColorF{ 1.0, 1.0 }, ColorF{ 1.0, 0.0 });
```

The sector builder shares subdivision selection, arc sampling and fan indices
with `Circle::drawPie()`. It writes positions, UVs and colors in one pass using
that arc's sine/cosine samples; it needs no temporary polygon or additional
shader. Texture support is selected at compile time. The rendering tests above
also cover sector angles, fixed texture placement, allocation failure, buffer
bounds, transforms and scene render resolution.

## Textured rounded rectangles

[TexturedRoundRect](../../Siv3D/include/Siv3D/TexturedRoundRect.hpp) supports vertical
and horizontal tint gradients, including alpha fades. Use the same named color
arguments as `RoundRect` with `draw()`, translated `draw(x, y)` / `draw(pos)`, or
`drawAt()`:

```cpp
RoundRect{ 40, 40, 240, 160, 24 }(texture).draw(
    Arg::top = ColorF{ 1.0, 0.2 }, Arg::bottom = ColorF{ 1.0, 1.0 });
texture.resized(240, 160).rounded(24).drawAt(400, 120,
    Arg::left = Palette::White, Arg::right = ColorF{ 0.4, 0.7, 1.0 });
```

The gradient follows the rectangle's local axes and is independent of UV cropping
or flipping. Tint interpolation matches `RoundRect::draw()`. It uses the existing texture shader and the same tessellation as
single-color textured rounded rectangles. The
[rendering tests](../../Test/Test_Renderer2DTexturedRoundRect.cpp) cover the public
overloads, cropped/flipped UVs, texture and tint alpha, transforms and scene
render resolution. The builder tests below check vertex attributes and allocation
failure.

## Rotated ellipses and arc ends

[Ellipse](../../Siv3D/include/Siv3D/Ellipse.hpp) can draw its fill or frame with a
rotation around its center. Vertical and horizontal gradients rotate with the
ellipse; a pattern keeps its existing coordinate space. This is useful for
orbits, tilted badges and directional effects without a temporary Polygon or a
transform scope:

```cpp
Ellipse{ 240, 140, 100, 40 }.drawRotated(30_deg,
    Arg::top = Palette::Skyblue, Arg::bottom = Palette::Royalblue);
Ellipse{ 240, 140, 110, 50 }.drawFrameRotated(30_deg, 4, Palette::White);
```

`Ellipse::drawArc()` gives an elliptical arc a width measured along the normal
to the ellipse. Its angles use the same center-to-boundary ray directions as
`drawPie()`. It supports a single color, inner/outer colors, named start/end
colors, or a pattern. Named colors follow the sampled stroke length, including
cap extensions. Very wide inward offsets can fold over at a high-curvature tip,
as with `drawFrame()`; this is a directly generated stroke, not a Boolean union.

Both [Circle](../../Siv3D/include/Siv3D/Circle.hpp) and Ellipse accept independent
start/end caps. The cap names stay attached to the requested start and end even
when the sweep is negative:

```cpp
Circle{ 180, 140, 60 }.drawArc(LineCap::Flat, LineCap::Round,
    -60_deg, 240_deg, 8, 8, Arg::start = Palette::Skyblue, Arg::end = Palette::Orange);
Ellipse{ 420, 140, 100, 40 }.drawArc(LineCap::Round, LineCap::Square,
    20_deg, 220_deg, 3, 9, Palette::White);
```

Equal Circle caps reuse the existing path. An Ellipse full sweep has no caps;
single-color, radial-gradient and pattern full sweeps reuse its frame drawing.
Zero sweeps draw only the selected caps. Refer to the public headers for the
boundary behavior and to the [manual gallery](../../Test/Manual/Renderer2DGeometryExtensions.md)
for the complete program, controls and expected results. The gallery also shows
[adaptive Bezier gradients](../geometry2d/bezier-drawing.md).

Rotation is a compile-time specialization of the existing vertex writers;
unrotated drawing does not execute the rotation arithmetic. Ellipse arcs merge
samples spaced in ellipse parameter and in normal angle, keeping curved offset
edges smooth near flat-ellipse tips. The body is bounded to 252 intervals, using
fixed workspace and a single buffer reservation. Caps share their geometry
writer with independent Circle caps. Neither path changes `outer()` or
`asPolygon()` output.

[Ellipse extension tests](../../Test/Test_Renderer2DEllipseExtensions.cpp) cover
analytic positions, normals, line width, gradients, bounds, allocation failure,
public overloads, transforms and scene render resolution. The
[Circle arc tests](../../Test/Test_Renderer2DCircleArc.cpp) also compare equal-cap
overloads against their original rendering paths.

## Shader design

- [Shader optimization assessment and plan](proposals/shader-optimization-plan.md):
  adopted changes, remaining evaluation, D3D11/Metal evidence, and compatibility boundaries.
- [MSDF font scale](msdf-font.md): atlas dimensions, UV derivatives, shadow
  offsets, and font shader validation.

## Development boundaries

| Layer | Responsibility |
| --- | --- |
| Public shape methods | Normalize public arguments and preserve the shape's documented geometry and color semantics |
| Shared `ISiv3DRenderer2D` implementation | Prepare reusable contours for Bezier and SuperEllipse drawing, and share backend-independent resources such as the shadow image |
| `Vertex2DBuilder` | Select bounded drawing detail and write vertices/indices through the backend allocator |
| Backend and command manager | Own GPU resources, buffer allocation, resource retention, command ordering, batching and execution |

Keep sharing at these boundaries. In particular, custom-shader and texture state
updates have different retention rules; their similar control flow is not a
reason to combine them. Engine shader IDs and scoped custom shaders also have
different ownership and remain separate types.

Extend small geometry writers with explicit supported modes when needed. Avoid
a general mesh framework or combinations of flags for hypothetical variants.
Preserve direct writes, storage reuse and the existing fast paths. Draw-time
quality must not alter the vertex output of `outer()` or `asPolygon()`.
The [tessellation inventory](../geometry2d/rounded-drawing.md) records the budgets.
For changes to hot paths, use the relevant CPU benchmarks and Release generated
code alongside regression tests; GPU performance requires separate measurements.

## Draw submission

`Vertex2DBuilder` generates vertices and indices through the backend's buffer
allocator. Each backend checks the resulting index count before submitting a
draw. Buffer allocation, base-vertex handling, command batching and GPU execution
remain backend responsibilities.

The builder implementations share index writers for open and closed fans,
open and closed strips, rounded rectangles and their frames, and copying
indexed meshes with a base offset. Strip writers preserve each layout's
triangle winding and the rounded-frame writer retains its four-corner traversal.
These helpers write directly into the reserved buffer; allocation stays in each
builder. Indexed copies retain a 32-bit element count
independently of the 16-bit vertex indices.
Closed strips and the inner fan of a filled circle shadow emit consecutive
indices for the ordinary segments and handle the closing segment separately.
This avoids per-index remainder operations while preserving triangle order and
winding; open strips retain their different winding.
[Test_Vertex2DBuilderIndices.cpp](../../Test/Test_Vertex2DBuilderIndices.cpp) checks
triangle order, winding, strip closure, cap/body boundaries, positive and negative
sweeps, scaled tessellation, destination bounds, large index counts and allocation
failure.

Polygon frames, Shape2D frames, and line strings share contour preparation.
Reusable CPU scratch holds simplified points, source-color indices, and unit
segment directions. Sharp-corner checks and strip generation reuse those
directions; a second point array is populated only when a corner needs an
auxiliary point. Directions are rebuilt after such insertions. Miter offsets
use the unnormalized bisector because its length cancels in the offset ratio.
Start/end-gradient strokes retain segment lengths and their total while
normalizing directions, then reuse them for color interpolation. Compile-time
specialization omits length storage and accumulation from solid, per-point-color,
and pattern paths. The gradient uses the final contour after any sharp-corner
insertions and preserves cap lengths and the order of distance accumulation.
This keeps scale-dependent preparation at draw time without adding per-Polygon
caches. [Test_Vertex2DBuilderPolyline.cpp](../../Test/Test_Vertex2DBuilderPolyline.cpp)
checks strip width and closure, sharp corners including the first ring point,
source colors, simplification, scratch reuse, gradient distances with all cap
combinations, zero-length edges after Float2 conversion, and allocation failure.

Circle, ellipse and textured-circle fills share a vertex writer for their center
and four quarter arcs. Compile-time specialization keeps the circle's two radial
products per sample and writes textured positions, UVs and colors in one pass.
Textured and untextured fills share radial, vertical and horizontal color
interpolation; untextured UV fields remain untouched. Tessellation selection
stays in each builder.
[Test_Vertex2DBuilderEllipse.cpp](../../Test/Test_Vertex2DBuilderEllipse.cpp) checks
analytic positions and colors, flipped UVs, fan order, destination bounds and
agreement between circles and equal-axis ellipses, including degenerate axes and
scaled tessellation. The existing
[textured-circle tests](../../Test/Test_Renderer2DTexturedCircle.cpp) also cover
transforms and scene render resolution.

Pie, circular-segment and semicircular-cap builders share angular traversal and
arc position generation. Callers retain tessellation selection, sweep direction,
center placement and color interpolation. A templated vertex writer supports
gradient caps without a runtime callback; position-only paths retain their
separate color pass. Both paths leave UVs untouched.
Flat, square and round-capped arcs share writers for their body's inner/outer
positions, along-arc colors and radial colors. Callers retain cap geometry and
gradient endpoints; position and color passes write into a single reservation.
[Test_Vertex2DBuilderArc.cpp](../../Test/Test_Vertex2DBuilderArc.cpp) checks analytic
positions, chord centers, solid and gradient caps, arc-body gradients, fan and
strip order, untouched UVs and destination bounds across signed sweeps, zero and
full sweeps, and tessellation scales. Existing
[arc tessellation](../../Test/Test_Renderer2DArcTessellation.cpp) and
[ellipse pie](../../Test/Test_Renderer2DEllipsePie.cpp) tests cover transforms and
scene render resolution.

Rounded rectangles also share vertex writers for fills and frames. Solid,
gradient and textured fills use the same corner traversal; solid and gradient
frames share the outer/inner traversal. Compile-time specialization omits gradient
calculations from solid paths. Texture coordinates are assigned by the textured
builder after position and color generation.
[Test_Vertex2DBuilderRoundRect.cpp](../../Test/Test_Vertex2DBuilderRoundRect.cpp)
checks positions, gradients, flipped UVs, untouched UVs in untextured draws,
destination bounds and geometry agreement between styles, including zero and
maximum radii and scaled tessellation.

Circle shadows share a vertex writer for their outer/inner ring. Buffer sizing,
tessellation selection, the optional center vertex and index generation remain
in each builder; the ring traversal has no fill-mode branch.
[Test_Vertex2DBuilderCircleShadow.cpp](../../Test/Test_Vertex2DBuilderCircleShadow.cpp)
checks analytic ring positions, UVs, colors, identical rings across fill modes,
center placement and destination bounds, including zero-width rings, a collapsed
inner radius, tessellation limits and offsets near the 16-bit vertex limit.

On macOS, run its opt-in benchmarks from the repository root with:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Vertex2DBuilder.indices_cpu_benchmark'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Vertex2DBuilder.indices_rounded_cpu_benchmark'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Vertex2DBuilder.ellipse_vertices_cpu_benchmark'
```

They measure thread CPU time for the affected builders using reusable buffers,
at low and high tessellation scales and with small and large index arrays.
The rounded benchmark covers pies, segments, capped arcs/lines, solid and dashed
frames, and filled and unfilled circle shadows.
The ellipse benchmark covers solid and all gradient directions for circle and
ellipse fills, plus textured circles, at low and maximum tessellation quality.
Each result reports the median and 10th/90th percentiles in nanoseconds per call.
Compare repeated runs of both revisions on the same idle machine, alternating
their order. These measurements exclude backend submission and GPU execution.

### Measuring CPU writes to upload buffers

Ordinary heap arrays do not model the CPU mapping of every backend's upload
buffers. Metal's vertex/index buffers use
[`writeCombined`](https://developer.apple.com/documentation/metal/mtlcpucachemode/writecombined),
which is intended for CPU writes without CPU reads. D3D11 currently builds into
ordinary CPU arrays and copies them to mapped GPU buffers at submission.

[Vertex2DBufferPointer](../../Siv3D/src/Siv3D/Renderer2D/Vertex2DBufferPointer.hpp)
is the boundary between backend storage and geometry emission. Backends retain
their raw buffers, but builders receive borrowed, write-only vertex and index
cursors. Cursor arithmetic addresses output slots; vertex setters and index
assignment return `void`. The cursors expose neither values nor raw destination
pointers. `copyFrom()` handles bulk copies from CPU source spans without exposing
the destination. Geometry helpers keep these cursor types throughout emission;
readable intermediate geometry belongs in CPU locals or reusable CPU arrays.
Do not expose raw destination pointers or add readable conversions to bypass
these cursors. Extend the compile-time rejection checks below when changing
this boundary.

[Test_Renderer2DUpload.cpp](../../Test/Test_Renderer2DUpload.cpp) is compiled by
both platform test projects. Its compile-time checks reject destination reads,
read/modify/write operations, proxy-to-proxy copies, chained index assignment,
and implicit raw-pointer escapes. Runtime tests check cursor offsets, attribute
preservation, and bulk copies. They inspect the CPU storage owned by the test,
not the output cursors. Keep hardware-dependent timing thresholds out of automated
correctness tests. For upload-path changes, run `Renderer2DUpload.*` and affected
geometry/image tests, followed by the full host suite. On macOS, the focused
command is `./macOS/run-tests.sh '--test-case=Renderer2DUpload.*'`.
Also compare Release performance in ordinary CPU memory and the actual backend's
upload memory; a heap-array benchmark alone is insufficient. Inspect generated
code when destination access is uncertain.

`CopyIndicesWithOffset` reads each source index, adds the base offset, and writes
once to the destination. It never reads the upload destination. The same helper
serves raw indexed polygons, transformed polygons, and Mesh2D. TriangleIndex
members are accessed directly, and span iteration preserves large index counts
independently of the 16-bit index values. Avoid copying first and adjusting the
destination afterward, even if a benchmark using ordinary heap arrays looks fast.

The same rule applies to vertex attributes and topology: add Mesh2D offsets
while reading the source vertices, calculate textured round-rectangle UVs from
local positions, and retain local positions for square arc caps and arrow-head
winding. Hollow rectangle shadows derive their degenerate indices from the
base index. Write a local color to each vertex separately with `setColor()`:
chained assignments through raw vertex fields can also generate destination
reads, including in circle and ellipse arc gradients. Such field access is not
available through the write-only cursor.
Do not use an upload buffer as geometry scratch storage.
Buffer growth still copies existing uploaded data to preserve pending draws;
this occurs when capacity grows, rather than on each draw. Keep these copies
inside backend storage management.

Use [renderer2d-index-upload.mm](../../tools/renderer2d-index-upload.mm) to compare
ordinary RAM, Metal default-cache memory, and Metal write-combined memory. It
measures representative copy kernels and the actual shared Polygon, Mesh2D,
textured round-rectangle, circle/ellipse arc, hollow rectangle shadow, and closed
line-string builders (smooth and sharp contours at several point counts),
and rounded balloon fills and frames,
with preallocated reusable buffers, no renderer initialization, no GPU commands,
and no presentation. Destination checks happen after timing. Run on macOS from
the repository root, outside the sandbox:

```sh
xcrun clang++ -std=c++23 -Os -DNDEBUG \
    -I Siv3D/include -I Siv3D/include/ThirdParty -I Siv3D/src \
    -framework Foundation -framework Metal -Wl,-dead_strip \
    tools/renderer2d-index-upload.mm \
    Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder_StraightEdged.cpp \
    Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder_Rounded.cpp \
    Siv3D/src/Siv3D/QuarterArcTable/SivQuarterArcTable.cpp \
    Siv3D/src/Siv3D/RectF/SivRectF.cpp \
    -o /tmp/siv8-index-upload
/tmp/siv8-index-upload
```

Repeat with `-O3` and inspect generated code when assessing a change: optimization
can change the number and width of destination reads without removing the
underlying hazard. Neither cache mode nor optimizer alone is a universal speed
ratio. The tool prints wall and thread CPU time and checks the written data;
its timings exclude resource allocation and GPU execution.

The [rounded rectangle submission benchmark](../../Test/Manual/RectRoundedDrawing.md#automated-checks-and-cpu-measurements)
complements this isolated measurement with public API calls. It reports geometry
counts and batching, checks a cached Polygon with the exact direct-fill mesh,
and measures both wall and thread CPU time on macOS. Matching GPU work matters
when comparing APIs with different tessellation rules. Use repeated A/B/A runs
on the same machine; a large CPU ratio alone does not establish a GPU cost or a
benefit inherent to a particular public shape API.

[Test_Renderer2DPreparation.cpp](../../Test/Test_Renderer2DPreparation.cpp) provides
opt-in public API benchmarks for closed frames/shadows at small, ordinary and
maximum tessellation, and for solid/gradient line strings and Bezier curves.
Inputs are created before timing. Each case warms the buffers for eight samples,
then measures 21 samples of 128 offscreen draws and reports the median and 10th/90th percentiles
alongside triangle counts and draw calls. System updates and GPU execution are
outside timing. VSync is disabled and restored afterward. On macOS a scoped QoS
override reduces core-scheduling variation.
Run each revision repeatedly on an otherwise idle machine:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Renderer2DPreparation.closed_frames_benchmark'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Renderer2DPreparation.gradient_lines_benchmark'
```

Sharp polyline turns use a small auxiliary segment. Its endpoint must remain
distinct after conversion to the actual float coordinate representation:
high zoom reduces the intended displacement, while translation increases the
local float spacing. The shared builder checks only split corners and advances
a collapsed endpoint by one float step along its dominant direction. Smooth
vertices do not take this path. The [polyline tests](../../Test/Test_Vertex2DBuilderPolyline.cpp)
cover internal and closing corners at both zoomed and translated coordinates;
use those cases when changing stroke preparation or its numeric representation.

The upload diagnostic also measures these frame/shadow builders and smooth/sharp
solid/gradient polylines in RAM and both Metal cache modes. Use the solid cases
as controls when changing gradient-only work; do not infer end-to-end improvement
from a smaller arithmetic count alone.

[Renderer2DDrawSubmission.hpp](../../Siv3D/src/Siv3D/Renderer2D/Renderer2DDrawSubmission.hpp)
shares the subsequent command registration using inline templates for shapes,
patterns, textures and styled lines. The helpers require a nonzero index count.
They select the engine VS and PS independently when the corresponding custom
shader is absent, then bind material parameters or a texture and register the
draw. Pattern and line shader selection only runs when the engine PS is needed;
material bindings also apply when a custom PS is active. New geometry builders
can reuse these helpers without introducing another virtual submission call or
an intermediate mesh.

[Renderer2DEngineShaders.hpp](../../Siv3D/src/Siv3D/Renderer2D/Renderer2DEngineShaders.hpp)
defines the shared engine shader ID cache, its initialization and inline pattern
selection.
[Renderer2DCustomShaders.hpp](../../Siv3D/src/Siv3D/Renderer2D/Renderer2DCustomShaders.hpp)
defines the custom shader state. Each backend owns these members;
draw submission reads cached IDs without accessing the engine shader service.
[Test_Renderer2DEngineShaders.cpp](../../Test/Test_Renderer2DEngineShaders.cpp) checks
the initialization mappings, pattern selection and fallback to the shape PS.

[Test_Renderer2DDrawSubmission.cpp](../../Test/Test_Renderer2DDrawSubmission.cpp)
checks command ordering, independent custom shaders, material bindings and
32-bit index counts. Its opt-in CPU benchmark times public draw calls for solid
Rect/Circle, patterned Rect/Circle and TexturedQuad. See the
[development guide](../development/README.md#draw-submission-benchmark) for
measurement and comparison instructions.

## State update coalescing

[Renderer2DStateUpdate.hpp](../../Siv3D/src/Siv3D/Renderer2D/Renderer2DStateUpdate.hpp)
shares ordinary state updates between the backend command managers. A pending
change is cancelled when its value returns to the last recorded value before
the next draw or flush, allowing adjacent draws to stay in one batch. The helper
reads the recorded value only when that state already has a pending change.
The same header provides separate helpers for custom shader and texture updates.
Texture updates with an unchanged ID return before looking up retained resources;
custom shader updates retain the shader whenever a change is already pending,
including when its ID is unchanged. Texture unbinding uses the ordinary state
helper. Command managers own the retention maps; their `flush()` and change
cancellation preserve these references, while `reset()` clears them.

[Test_Renderer2DStateUpdate.cpp](../../Test/Test_Renderer2DStateUpdate.cpp) checks
recorded-value access, cancellation, replacement, command ordering, draw batching
and independent VS/PS sampler slots using the host's command manager.

[Test_Renderer2DResourceStateUpdate.cpp](../../Test/Test_Renderer2DResourceStateUpdate.cpp)
checks retention lookups, recorded-value access, shader/texture command
coalescing, independent texture slots and resource lifetime through cancellation,
flush, reset and command-manager destruction.

## Transform state

[Renderer2DTransformState.hpp](../../Siv3D/src/Siv3D/Renderer2D/Renderer2DTransformState.hpp)
shares transform storage and calculations between the backend command managers.
The affine transform is `Local * Camera`; the combined transform applies
QuadWarp afterward. RMS scaling uses the affine transform, while raster scaling
uses the affine transform followed by the scene's render-resolution scale.
QuadWarp does not contribute to either scaling value. A unit scene scale reuses
the cached RMS value.

Command managers retain change detection, update scheduling and command
coalescing. Local or Camera changes update both scaling values and the combined
transform; QuadWarp changes update only the combined transform; scene scale
changes update only raster scaling. Getters read cached values. Even when the
combined transform returns to the last recorded matrix and cancels a pending
command, the affine transform and scaling caches reflect the current inputs.

[Test_Renderer2DTransformState.cpp](../../Test/Test_Renderer2DTransformState.cpp)
checks composition, nonuniform scaling, shear, reflection, zero scale,
render-resolution changes, batch preservation and cached state across reset.

## Custom constant buffers

[ScopedConstantBuffer2D.hpp](../../Siv3D/include/Siv3D/ScopedConstantBuffer2D.hpp)
defines the public contract for `ScopedVSConstantBuffer2D` and
`ScopedPSConstantBuffer2D`. User slots are 2–13 on both backends; 0 and 1 are
reserved for engine data. In Metal, vertex buffer 0 contains vertices and vertex
buffer 1 contains standard VS constants.

Use explicit layouts shared with the shader, preferably Float4 members and
Float4 arrays. ConstantBuffer aligns the outer storage; it does not pack members
or scalar arrays into the shader's layout. The paired
[constant-buffer sample](../../Test/Manual/ScopedCustomShader2D.md) applies a local
vertex offset and a straight-RGBA tint, with premultiplied output.

Construct a constant-buffer scope around the draws that use its value.
Renderer2D owns a snapshot taken at construction; changing or destroying the
source object does not affect that snapshot. To apply a changed value, construct
another scope after updating the source buffer. Keep a scope around a render
pass or frame loop when the same value should remain active throughout it.
Shader selection and custom constants survive `Graphics2D::Flush()` and frame
boundaries while their scopes remain alive.

Each constant-buffer scope restores only its specified stage and slot, including
an originally unset slot. Scopes for the same stage and slot must unwind in
reverse order. They save the previous snapshot, not a reference to the original
ConstantBuffer. Scope destruction does not affect already recorded draws.
`ScopedCustomShader2D` independently restores only the specified shader stages
and does not change constant-buffer bindings.

```cpp
const ScopedCustomShader2D shader{ psHit };
const ScopedPSConstantBuffer2D constants{ 2, hitParameters };
enemy.draw();
```

The shared constant-buffer command store separates pending commands from current
bindings and saved scope values. Immutable snapshots use reference counts on the
drawing thread and reusable size-class storage (16 bytes through 64 KiB). Only
constructing a constant-buffer scope copies source bytes; command replay and
scope restoration share snapshots.
Clearing executed commands releases only their references.

State changes are coalesced per stage/slot until the next draw or command flush.
Scope destruction restores the logical snapshot using only reference counts and
a change mask; it does not allocate a command or call the graphics API. If the
same slot is set again before a draw, the intermediate restoration is omitted.
An empty scope returning to the recorded snapshot does not split a draw batch.
This makes a loop of scoped draws require one parameter upload per iteration,
with only a final restoration when that state is committed. Each new binding
copies its source data; distinct snapshots are not deduplicated by comparing
their bytes.

D3D11 reuses a growing dynamic GPU buffer per stage/slot and uploads snapshots in
command order. Metal uses setBytes up to 4096 bytes; larger snapshots use distinct,
256-byte-aligned regions in 64 KiB upload pages. Pages are reused only after
MetalFrameContext permits reuse of that frame slot, including across intermediate
flushes. Each new execution reapplies logical bindings. Restoration to an unset
slot unbinds the backend resource when that state is committed.
Allocation/upload failure during command execution raises InternalEngineError.

The recorded snapshot IDs describe only Renderer2D's command stream, not the
current GPU bindings. Every Renderer2D execution establishes its own binding
state: D3D11 sets all VS/PS constant-buffer slots in two bulk calls, including
standard constants and nulls for the other slots, then replays active custom
snapshots. Thus an unset slot cannot inherit a buffer from another renderer.
Metal creates a new render command encoder for each execution and records its
standard and custom data into that encoder; encoder bindings are not a shared
device-context cache.

Additional renderers must own their logical settings and upload resources, and
execute between Renderer2D executions. The same boundary applies to an
intermediate flush within a frame and to the first 2D execution of the next
frame. They can replace native bindings without notifying Renderer2D; they
must not overwrite its private buffer contents or interleave native commands
inside its execution.

[Test_ConstantBuffer.cpp](../../Test/Test_ConstantBuffer.cpp) covers storage reuse,
snapshots, invalid slots, scoped restoration/moves, and image comparisons for
PMA, source lifetime, independent slots, 4 KiB boundaries and 64 KiB. It also
checks repeated empty/intermediate flushes, shaders/constants held across frames,
and restoring an outer scope after inner frames have completed. Command-stream
tests check scoped loops, empty scopes, independent pending slots, and reset
ordering; image comparisons check changing VS/PS parameters in a scoped loop.
The D3D11 integration test replaces all VS/PS native bindings outside Renderer2D,
then checks all restored slots and rendered images across intermediate flushes
and frames, including slots that were never set or whose scopes have ended.
Tests save and restore the constant-buffer state they modify. Run
`./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=ConstantBuffer.*'` on
Windows or `./macOS/run-tests.sh '--test-case=ConstantBuffer.*'` on macOS, then the
full host suite described in the [development guide](../development/README.md).

## Pattern design

- [Coordinates](pattern-coordinates.md): attachment to local/camera transforms,
  primitive continuity, and custom-shader interfaces.
- [Parameter payload](pattern-payload.md): four-vector packing, effect-buffer
  offsets, state tracking, and backend shader maintenance.
- [Halftone](halftone.md): independent radius field and per-dot sampling.
- [Wave](wave.md): sine-wave bands and the normal-width approximation.
- [Ripple](ripple.md): concentric bands and center-safe filtering.
- [Weave](weave.md): alternating crossings, gaps, and filtered parity.
- [Truchet](truchet.md): connected arcs, layouts, and deterministic seed encoding.

## Gallery and checks

- [Custom shaders](../../Test/Manual/ScopedCustomShader2D.md): paired HLSL/MSL
  examples using standard and user constant buffers, effect comparisons, nested scopes,
  custom VS/PS constants, and premultiplied-alpha checks.
- [Pattern collection](../../Test/Manual/PatternCollection.md): the canonical
  960 x 700 gallery of 84 backgrounds across all eleven patterns and combinations,
  with paging and animation controls.
- [Halftone controls](../../Test/Manual/Halftone.md): independent field/lattice
  controls, guides, split geometry, and transformed comparisons.
- [Pattern transforms](../../Test/Manual/PatternTransforms.md): attachment,
  continuity across primitives, and geometry versus coordinate-system movement.
- [RMS scaling](../../Test/Manual/Renderer2DRMSScaling.md): tessellation and
  line-width compensation under nonuniform transforms.

[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) owns automated packing and GPU
regression checks. See [coordinate validation](pattern-coordinates.md#validation)
for the host-specific test commands.
