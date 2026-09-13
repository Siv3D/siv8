# MSDF font scale

The [glyph generator](../../Siv3D/src/Siv3D/Font/GlyphRenderer/MSDFGlyphRenderer.cpp)
encodes a distance range of 16 texels. The built-in
[Metal](../../macOS/App/engine/shader/metal/2d.metal) and
[HLSL](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl) shaders use the same
`MSDF_PixelRange` when converting sampled distance into coverage.

## Atlas dimensions and derivatives

`MSDF_Init` reads the bound texture dimensions `T` and the component-wise UV
derivative width `F = fwidth(uv)`. The product `T * F` expresses that footprint in
texels. Each component contributes `8 / (T * F)` to the existing scale estimate;
the two contributions are added. This is the component-wise algebraic reduction
of `(16 / T) * (0.5 / F)`. It preserves the existing derivative-based filtering
rule, including its treatment of rotation and nonuniform scaling.

Normal and Outline text need only this scale. Shadow, OutlineShadow, and Print
also use `invTextureSize` to convert a shadow displacement in texels into UVs, so
`MSDFState` retains that value. Compilers can remove its calculation when the
selected pixel shader does not use it. Glow does not call `MSDF_Init`.

Texture dimensions are queried from the bound texture, so atlas growth requires
no additional inverse-size constant or CPU-side state tracking. Foreground PMA,
outline and shadow colors, coverage thresholds, and texture sampling are separate
from this scale calculation.

## Validation

[Test_MSDFFont.cpp](../../Test/Test_MSDFFont.cpp) owns the
`MSDFFont.atlas_dimensions` regression. It places identical real-font glyph texels
in square and rectangular atlases, including a non-power-of-two width, and
compares all five shaders that use `MSDF_Init`. The drawing cases include
minification, magnification, rotation combined with nonuniform scaling,
transparent colors, ColorMul, and ColorAdd. The comparison allows one RGBA8 unit
for UV normalization and texture filtering, and requires visible glyph pixels to
avoid accepting empty output.

Use the [host test workflow](../development/README.md#build-and-test) with
`--test-case=MSDFFont*` for focused iteration, then run the full suite. When
changing `MSDF_Init`, regenerate all five affected Windows pixel-shader assets:
`msdffont.ps`, `msdffont_outline.ps`, `msdffont_shadow.ps`,
`msdffont_outline_shadow.ps`, and `msdffont_print.ps`.
[CEngineShader_D3D11::init](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp)
defines their entry-point and file mappings. GPU-time measurements are separate
from image regression tests and intermediate instruction counts.
