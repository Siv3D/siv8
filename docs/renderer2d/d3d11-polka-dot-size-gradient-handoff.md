# D3D11 polka-dot size-gradient integration

The D3D11 backend implements the shared `Pattern::PolkaDotSizeGradient` API and
uses the same center-based radius calculation as Metal. Read the
[design](polka-dot-size-gradient.md) and [payload layout](pattern-payload.md)
for the coordinate and storage model.

## Data path

Shape overloads use the existing Pattern vertex shader and four-vector payload.
The D3D11 renderer's
[shader selector](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/CRenderer2D_D3D11.cpp)
selects the dedicated pixel shader for `PatternType::PolkaDotSizeGradient`.

[PS_PatternPolkaDotSizeGradient](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl)
reads the two normalized radii from `g_patternUVTransform[1].zw` and field
coefficients from `g_patternExtraParams.xyz`. It evaluates the smoothstep field
at `floor(uv) + 0.5`, making the radius constant across each dot. Derivatives
come from continuous UVs. The attenuation factor preserves zero-radius
backgrounds and fades subpixel dots. Existing HLSL helpers handle color and
premultiplied alpha.

Pattern records remain 64 bytes and effect constants remain 128 bytes. The
fourth extra component is unused. Existing patterns, QuadWarp, vertex formats,
and drawing-coordinate transforms retain their existing implementations.

## Shader compilation and loading

[CEngineShader_D3D11.cpp](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp)
compiles `PS_PatternPolkaDotSizeGradient` through the existing enabled startup
compile block and loads
[2d_pattern_polka_dot_size_gradient.ps](../../WindowsDesktop/App/engine/shader/d3d11/2d_pattern_polka_dot_size_gradient.ps).
The new shader is appended **after FontPrint**, matching the appended
[EnginePS](../../Siv3D/src/Siv3D/EngineShader/IEngineShader.hpp) entry. Inserting it
beside the original six patterns would change the font shader indices.

Compiled shaders are loaded by path rather than embedded-resource registration.
The new `.ps` is a binary asset; never apply text line-ending conversion to it.
The existing shader outputs should remain byte-identical after regeneration.
No additional shared headers or project entries are needed for this connection.

## Verification

Run on Windows from the repository root:

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=Pattern*'
./WindowsDesktop/run-tests.ps1
```

The CPU packing and both size-gradient rendering tests in
[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) run on Windows and macOS. They
cover the lattice, field direction, zero/equal radii, transformations, viewport
and split geometry, alpha compositing, repeated frames, and state restoration.
Also run the [interactive sample](../../Test/Manual/PolkaDotSizeGradient.md) and
check existing patterns, fonts, and QuadWarp.

The split test compares a translated rectangle with viewport/split geometry.
D3D11 interpolation can differ by a few floating-point ULPs between these paths,
including the continuous-UV derivatives. The Windows comparison permits one
8-bit RGB level only for antialiased intermediate pixels; solid black/white
pixels and alpha must match exactly. Metal retains exact image equality. This
comparison accounts for interpolation rounding without changing the per-dot
radius calculation or the other rendering assertions.

If an incremental run reports `EXCEPTION_ILLEGAL_INSTRUCTION` or has an
incomplete report, preserve it and follow the
[clean-build diagnosis](../development/README.md#windows-incremental-build-failures).
A complete report containing an ordinary assertion failure is distinct from
this build-artifact diagnosis.

Verify source LF endings, binary shader bytes, `git diff --check`, documentation
links, and Windows test registration. Shared test changes require a macOS
full-suite run as well; Windows execution does not validate the Metal branch.
