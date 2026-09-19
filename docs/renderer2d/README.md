# Renderer2D

## Shader design

- [Shader optimization assessment and plan](proposals/shader-optimization-plan.md):
  proposed stages, D3D11/Metal evidence, compatibility boundaries, and validation.
- [MSDF font scale](msdf-font.md): atlas dimensions, UV derivatives, shadow
  offsets, and font shader validation.

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
