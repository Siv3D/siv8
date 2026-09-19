# Renderer2D

## Shader design

- [Shader optimization assessment and plan](proposals/shader-optimization-plan.md):
  proposed stages, D3D11/Metal evidence, compatibility boundaries, and validation.
- [MSDF font scale](msdf-font.md): atlas dimensions, UV derivatives, shadow
  offsets, and font shader validation.

## Custom constant buffers

[Graphics2D.hpp](../../Siv3D/include/Siv3D/Graphics2D.hpp) defines the public
SetVSConstantBuffer / SetPSConstantBuffer contract. User slots are 2–13 on both
backends; 0 and 1 are reserved for engine data. In Metal, vertex buffer 0 contains
vertices and vertex buffer 1 contains standard VS constants.

Use explicit layouts shared with the shader, preferably Float4 members and
Float4 arrays. ConstantBuffer aligns the outer storage; it does not pack members
or scalar arrays into the shader's layout. The paired
[constant-buffer sample](../../Test/Manual/ScopedCustomShader2D.md) applies a local
vertex offset and a straight-RGBA tint, with premultiplied output.

Renderer2D records a byte snapshot for each Set call. The source object can then
be changed or destroyed. Each stage/slot has independent values even when the
same object supplies several slots. D3D11 reuses a growing dynamic buffer per
slot and uploads snapshots in command order. Metal uses setBytes up to 4096 bytes;
larger snapshots use distinct, 256-byte-aligned regions in 64 KiB upload pages.
The pages are reused only after MetalFrameContext permits the frame slot to be
reused, including when several renderer flushes occur in one frame. This avoids
requiring shared ownership of ConstantBuffer's built-in backend resource.

A renderer flush clears custom bindings along with custom shader selection.
Set every slot required by a custom shader again before drawing in the next
frame. Shader scopes do not restore constant-buffer settings. Allocation/upload
failure during command execution raises InternalEngineError rather than drawing
with an earlier value.

[Test_ConstantBuffer.cpp](../../Test/Test_ConstantBuffer.cpp) covers storage,
snapshots, slot validation and image comparisons, including independent slots,
PMA, source lifetime, 4 KiB boundaries, 64 KiB and repeated flushes. Run
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
