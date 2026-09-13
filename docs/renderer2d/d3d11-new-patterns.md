# D3D11 handoff: Wave, Ripple, Weave, and Truchet

The shared public APIs and Metal implementations of these four patterns are
ready for D3D11 integration. Implement the adopted result; no compatibility
payload or transitional public API documentation is needed. Preserve unrelated
uncommitted work and the independent QuadWarp implementation.

## Sources and registration

Read the repository instructions and the individual design/port notes:

- [Wave](wave.md): sine-wave bands with approximate normal-width correction.
- [Ripple](ripple.md): concentric bands and center-safe derivative filtering.
- [Weave](weave.md): alternating crossings with filtered gaps and parity.
- [Truchet](truchet.md): connected quarter-circle tiles and deterministic seeds.

Port the four entry points and the Truchet hash helper from
[2d.metal](../../macOS/App/engine/shader/metal/2d.metal) into
[2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl).
The shared enums and public-header registrations already exist on both platforms.
The four-Float4 Pattern payload and 128-byte effect constants are already in place.
Do not modify the buffer layout or existing pattern equations.

In `CEngineShader_D3D11.cpp`, append compilation/loading in the shared enum order,
after Halftone: Wave, Ripple, Weave, Truchet. Generate and check in these assets
using the existing Windows shader compilation workflow:

```text
2d_pattern_wave.ps
2d_pattern_ripple.ps
2d_pattern_weave.ps
2d_pattern_truchet.ps
```

In `CRenderer2D_D3D11.hpp/.cpp`, add shader IDs, initialize them from the matching
EnginePS entries, and extend PatternType selection. Mirror the Metal wiring.
Missing selection currently falls through to a solid shape; compiling HLSL alone
is not sufficient to finish the port.

## Checks that matter

- Preserve `VS_Pattern`, drawing-coordinate input, and premultiplied foreground
  and background compositing. No new shape overloads are needed.
- Ripple and Truchet use continuous UV derivatives. Weave also filters parity;
  an unfiltered integer branch changes maximum-gap boundaries.
- Reconstruct Truchet seed from two numeric 16-bit halves. Hash coordinate keys
  with `asuint`, and retain unsigned wraparound. All uint32 seeds are valid.
- In [Test_Pattern.cpp](../../Test/Test_Pattern.cpp), enable the Metal-only block
  containing these four patterns' GPU cases for Windows. CPU packing tests already
  run on both hosts. Run focused Pattern tests and the full suite using the
  [Windows test workflow](../development/README.md). Follow its clean-build
  diagnosis if an incremental test executable reports illegal instructions.
- Investigate AA-only readback rounding separately from solid interiors. Do not
  broadly relax comparisons to hide layout, alpha, or state-restoration errors.
- Build and inspect the complete programs in [Wave](../../Test/Manual/Wave.md),
  [Ripple](../../Test/Manual/Ripple.md), [Weave](../../Test/Manual/Weave.md), and
  [Truchet](../../Test/Manual/Truchet.md), including animation and transforms.
  Preserve the normal platform Main.cpp after temporary verification.

Update TODO and integration notes to reflect the completed port. Report actual
build/test results and remaining limitations. The combined gallery with smaller
pages is a subsequent task; keep the existing recipes intact during this port.
