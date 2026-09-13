# D3D11 integration: Wave, Ripple, Weave, and Truchet

The D3D11 and Metal backends implement these four patterns through the shared
public APIs and existing Pattern drawing path. Their design notes describe the
adopted equations and filtering limits:

- [Wave](wave.md): sine-wave bands with approximate normal-width correction.
- [Ripple](ripple.md): concentric bands and center-safe derivative filtering.
- [Weave](weave.md): alternating crossings with filtered gaps and parity.
- [Truchet](truchet.md): connected quarter-circle tiles and deterministic seeds.

## Data path and registration

[2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl) implements
`PS_PatternWave`, `PS_PatternRipple`, `PS_PatternWeave`, `PS_PatternTruchet`, and
`Pattern_TruchetHash` with the same calculations as
[2d.metal](../../macOS/App/engine/shader/metal/2d.metal).

Shape overloads feed drawing coordinates through the existing `VS_Pattern`.
The [four-Float4 Pattern payload](pattern-payload.md) remains 64 bytes and the
effect constants remain 128 bytes. Foreground and background use the existing
premultiplied color helpers. QuadWarp retains its independent shaders and state.

[CEngineShader_D3D11.cpp](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp)
compiles and loads the following binaries in the shared
[EnginePS](../../Siv3D/src/Siv3D/EngineShader/IEngineShader.hpp) order, after
Halftone:

| Pattern | Tracked shader asset |
| --- | --- |
| Wave | [2d_pattern_wave.ps](../../WindowsDesktop/App/engine/shader/d3d11/2d_pattern_wave.ps) |
| Ripple | [2d_pattern_ripple.ps](../../WindowsDesktop/App/engine/shader/d3d11/2d_pattern_ripple.ps) |
| Weave | [2d_pattern_weave.ps](../../WindowsDesktop/App/engine/shader/d3d11/2d_pattern_weave.ps) |
| Truchet | [2d_pattern_truchet.ps](../../WindowsDesktop/App/engine/shader/d3d11/2d_pattern_truchet.ps) |

The enabled startup compilation block regenerates these assets on Windows.
Preserve their binary bytes; do not apply text line-ending conversion. Existing
shader outputs should remain byte-identical when only adding these entry points.

[CRenderer2D_D3D11](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/CRenderer2D_D3D11.cpp)
initializes the matching shader IDs and selects each dedicated shader from
`PatternType`. The shared enums and public headers are registered on both
platforms. The binary shaders are loaded by path, so this connection needs no
new project entries or shape overloads.

## Filtering and seed representation

Wave differentiates its unwrapped phase before measuring the repeating band.
Ripple and Truchet differentiate continuous UVs, preserving filtering at the
radial center and across tile orientations. Weave filters both the band masks
and crossing parity; an unfiltered integer branch changes maximum-gap boundaries.

Truchet reconstructs its seed numerically from two 16-bit halves, hashes
coordinate keys with `asuint`, and retains unsigned wraparound. All uint32 seeds
are valid. Regular layouts skip hashing and ignore seed changes.

## Verification

Run from the repository root on Windows:

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=Pattern*'
./WindowsDesktop/run-tests.ps1
```

The CPU packing and all four patterns' GPU cases in
[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) run on Windows and macOS. They
check solid interiors, endpoint compositing, continuous-UV filtering, Truchet
hash/layout agreement, transforms, viewports, split geometry, batch counts, and
restoration across different shaders and repeated frames. Existing Pattern,
font, and QuadWarp tests provide regression coverage.

Investigate AA-only readback rounding separately from solid interiors. These
four patterns retain their shared comparison criteria, including exact split
images and state restoration; no additional D3D11 tolerance is applied.

Build and inspect the complete programs in [Wave](../../Test/Manual/Wave.md),
[Ripple](../../Test/Manual/Ripple.md), [Weave](../../Test/Manual/Weave.md), and
[Truchet](../../Test/Manual/Truchet.md), including animation and transforms.
For separate sample builds, use the Windows application's compiler/link settings
with the standard `App/Resource.rc`, put generated sources and intermediates
under `WindowsDesktop/Intermediate/Manual/`, and place the executable beside
`WindowsDesktop/App/engine/`. Each sample's `--capture` option saves its named
PNG in the application working directory; `--animate --capture` captures a
moving phase. Remove temporary sources and captures when no longer needed.
Keep the normal platform `Main.cpp` intact.

Preserve incomplete or failed reports under
`WindowsDesktop/Intermediate/TestReports/` and follow the development guide's
[clean-build diagnosis](../development/README.md#windows-incremental-build-failures)
before retrying an executable with an incomplete report or illegal-instruction
exception. Verify source LF, `git diff --check`, documentation links, and binary
shader bytes. Shared test changes also require a macOS full-suite run; Windows
execution does not validate Metal.

The combined gallery with smaller pages is a separate task in [TODO](../../TODO.md).
Keep the existing recipes intact when assembling it.
