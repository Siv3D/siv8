# D3D11 polka-dot size-gradient port

Implement the D3D11 backend for the shared `Pattern::PolkaDotSizeGradient` API.
Read repository instructions, the [design](polka-dot-size-gradient.md), and the
[64-byte payload layout](pattern-payload.md) first. The shared API, Metal path,
CPU tests, and interactive sample already exist. The previous payload extension
is complete on both backends and does not need to be redone.

## Scope

Port the adopted API and the Metal fragment calculation without changing their
semantics. Keep QuadWarp independent. Do not add profiles, a new field API,
extra constant storage, vertex attributes, or runtime exceptional-input guards.
Public API documentation describes the adopted behavior; keep port status in
this development guide and TODO rather than adding backend-migration remarks to
public headers.

The shared new enum entries are appended. The D3D11 pattern selector currently
falls through to its default shape shader for the new PatternType; it is not an
implementation of the new pattern. Existing patterns and font shader indices
remain unchanged. No placeholder shader or compatibility adapter was added.

## Implementation

1. Add `PS_PatternPolkaDotSizeGradient` to
   [2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl), using the
   [Metal implementation](../../macOS/App/engine/shader/metal/2d.metal) as the reference.
   Use the existing HLSL UV-transform and color helpers. Read the minimum and
   maximum doubled radii from `g_patternUVTransform[1].zw`, and the linear field
   from `g_patternExtraParams.xyz`. The fourth extra component stays unused.
2. Use `frac`/`floor`, `saturate`, `lerp`, and the HLSL derivative equivalents.
   Evaluate radius at `floor(uv) + 0.5`, not at the fragment's original position.
   Differentiate continuous UVs, not cell centers or radius. Keep the zero-radius
   and subpixel attenuation factor; do not substitute the ordinary PolkaDot PS.
3. Add compilation and loading for `2d_pattern_polka_dot_size_gradient.ps` in
   [CEngineShader_D3D11.cpp](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp).
   The load order is indexed by [EnginePS](../../Siv3D/src/Siv3D/EngineShader/IEngineShader.hpp):
   **append the new PS after FontPrint**, not among the original six patterns.
   Regenerate and track the new binary through the existing compilation path.
4. In the D3D11 renderer header and implementation, add the shader ID, initialize
   it from EnginePS, and add the `PatternType::PolkaDotSizeGradient` selector case.
   Shape overloads and the Pattern VS already supply the necessary coordinates
   and four-vector payload. Do not duplicate their drawing implementations.
5. In [Test_Pattern.cpp](../../Test/Test_Pattern.cpp), enable
   `Pattern.polka_dot_size_gradient_rendering` and
   `Pattern.polka_dot_size_gradient_transforms_and_state` on Windows as well as
   macOS. They share drawing code and require no diagnostic-shader rewrite.
   The packing case already runs on both. Preserve their assertions; investigate
   and document real rasterization differences before changing tolerances.
6. Shared headers are already registered in both projects and the umbrella header.
   Register any genuinely new files according to AGENTS.md. Existing compiled
   shader resources are loaded by path; inspect nearby resource conventions.
7. Run the interactive [manual sample](../../Test/Manual/PolkaDotSizeGradient.md)
   and the tests below. Update this guide, design status, manual execution notes,
   and TODO to describe the completed integration when verified.

## Verification

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=Pattern*'
./WindowsDesktop/run-tests.ps1
```

Check the four-neighbor 45-degree square lattice, downward/horizontal/reversed
fields, independent start/end and origin controls, equal/zero radii, negative
cells, object transformations, viewport/split continuity, alpha compositing,
state restoration, and the existing patterns/fonts/QuadWarp rendering.

If an incremental run reports `EXCEPTION_ILLEGAL_INSTRUCTION` or has an incomplete
report, preserve it and follow the
[clean-build diagnosis](../development/README.md#windows-incremental-build-failures)
before rerunning. Verify source LF endings, binary shader bytes, `git diff --check`,
and documentation links. Run project validation available on Windows and report
macOS execution as unverified on that host. Shared changes need a macOS full-suite
run afterward. Report implementation, validation, and any remaining limitations
separately, preserving unrelated changes in the checkout.
