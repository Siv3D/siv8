# D3D11 pattern payload validation

D3D11 uses the shared four-vector pattern payload and the 128-byte effect
constant layout described in the [payload design](pattern-payload.md). The
storage extension is implemented in both D3D11 and Metal. It adds no pattern,
gradient, or profile interpretation to the additional vector.

## Data path

1. Shape drawing passes `PatternParameters::toFloat4Array()` to
   [D3D11Renderer2DCommandManager](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/D3D11Renderer2DCommandManager.hpp).
   Its input, record, current value, and getters all use `std::array<Float4, 4>`.
   Initial values are zero; whole-array comparison and assignment include the
   additional vector during state changes, batching, and frame restoration.
2. The [renderer](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/CRenderer2D_D3D11.cpp)
   replays the record through the shared
   [PSEffectConstants2D::setPattern](../../Siv3D/src/Siv3D/Renderer2D/Renderer2DCommon.hpp)
   and uploads the dirty effect buffer to pixel-shader slot b1.
3. [2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl) places
   `g_patternExtraParams` immediately after `g_patternBackgroundColor`, matching
   the shared offsets. Pattern owns bytes 0-63 and QuadWarp owns bytes 64-127.

QuadWarp and SDF command payloads retain their three-vector representation.
Pattern and QuadWarp retain independent vertex and pixel shaders. The existing
[drawing-coordinate model](pattern-coordinates.md), six pattern calculations,
and vertex format are unchanged by this extension.

## Shader bytecode

[CEngineShader_D3D11::init](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp)
contains the engine's shader-compilation path. Its compile block is enabled with
`#if 1`; application initialization recompiles the listed entry points using
`Platform::Windows::CompileHLSLFromFile`, then loads their `.vs`/`.ps` files.

When changing the effect layout, regenerate the tracked bytecode through that
path and inspect the resulting binary changes. The six pattern pixel shaders
carry the effect layout, and the QuadWarp pixel shader also consumes its second
half. Rebuild all listed entry points so no tracked shader retains an old layout.
Treat `.vs` and `.ps` files as binary; never normalize their line endings.
The Windows SDK's `fxc /dumpbin` can inspect their reflected constant offsets
without recompiling them.

## Windows verification

Run from the repository root:

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=Pattern*'
./WindowsDesktop/run-tests.ps1
```

[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) owns the packing, field-offset,
state-transition, and rendering assertions. Its HLSL diagnostic shader reads
all four components at b1 byte offset 48 and returns
`float4(extra.xyz * extra.w, 1)`. The Metal diagnostic shader uses the same
common drawing, capture, and assertion body.

The checks cover zero/A/A/B/A/zero, equal-value batching, repeated frames,
Pattern/QuadWarp independence, and identical output from all six built-in
patterns with zero and nonzero additional values. Existing coordinate,
viewport, color-state, custom-shader, shape-path, and batch-boundary tests remain
enabled. Keep their assertions intact when diagnosing a failure.

If an incremental run reports `EXCEPTION_ILLEGAL_INSTRUCTION` or leaves an
incomplete report, preserve the console/XML reports and follow the
[clean-build diagnosis](../development/README.md#windows-incremental-build-failures)
before rerunning or assigning the failure to source changes.

Verify LF endings for modified source files, run `git diff --check`, and run
`python tools/check_docs.py`. Windows validates the HLSL branch and common test
body; changes to shared tests also require a macOS suite run to validate the
Metal branch. Do not infer a GPU-time improvement from draw counts or byte sizes.

New expressions remain separate [candidates](proposals/pattern-expressiveness.md)
and pending work in [TODO](../../TODO.md).
