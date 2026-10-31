# MSDF font rendering

The [glyph generator](../../Siv3D/src/Siv3D/Font/GlyphRenderer/MSDFGlyphRenderer.cpp)
encodes a distance range of 16 texels. The built-in
[Metal](../../macOS/App/engine/shader/metal/2d.metal) and
[HLSL](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl) shaders use the same
`MSDF_PixelRange` when converting sampled distance into coverage.

## Atlas dimensions and derivatives

`MSDF_Init` reads the bound texture dimensions `T` and the component-wise UV
derivative width `F = fwidth(uv)`. The product `T * F` expresses that footprint in
texels. Each component contributes `8 / (T * F)` to the existing scale estimate;
the two contributions are added to form `coverageScale`. This is the
component-wise algebraic reduction of `(16 / T) * (0.5 / F)`. It preserves the existing derivative-based filtering
rule, including its treatment of rotation and nonuniform scaling.

`MSDFState` holds only `coverageScale` and `invTextureSize`; the texture dimensions
are local to `MSDF_Init`. This is per-fragment derived state, separate from the
CPU/shader constant-buffer layout. Normal and Outline text need only the scale.
Shadow, OutlineShadow, and Print also use `invTextureSize` to convert a shadow
displacement in texels into UVs. Compilers can remove its calculation when the
selected pixel shader does not use it. Glow does not call `MSDF_Init`.

## Shader parameter interpretation

[TextStyle::getShaderParams](../../Siv3D/include/Siv3D/detail/TextStyle.ipp)
packs style-specific values into `g_sdfParam`. The consuming shaders give those
values local names while retaining the shared buffer layout.

| Shader | Field | Local meaning |
| --- | --- | --- |
| Outline, OutlineShadow | `x`, `y` | `textThreshold`, `outlineThreshold` in the encoded distance domain |
| Shadow, OutlineShadow | `zw` | `shadowOffsetTexels`, converted to `shadowOffsetUV` with `invTextureSize` |
| Glow | `x` | `glowExponent` applied to `glowBase` to produce `glowFactor` |

For outline thresholds, the CPU applies `fontBaseSize / 64` to the inner and outer
thicknesses around the encoded contour value of 0.5. It applies the same ratio
to the style's shadow displacement, so the shader receives atlas-texel offsets
rather than screen-pixel offsets. For Glow, it supplies the reciprocal of the
scaled glow parameter as the exponent.

Normal and Shadow use the fixed text threshold `MSDF_TextThreshold`. Print
uses its own outline-distance offset and shadow displacement constants, followed
by its square-root alpha and grayscale composition. Glow uses the texture alpha
channel; the other styles obtain `sampledDistance` from the RGB median.

## Color composition

The vertex shader applies ColorMul to the text fill. The Outline, Shadow, and
OutlineShadow pixel shaders receive the original style PMA colors in
`g_sdfOutlineColorPMA` and `g_sdfShadowColorPMA`. They multiply these by
`(r*a, g*a, b*a, a)`, using the effective straight multiplier `(r, g, b, a)` from
`g_patternBackgroundColorMul`, then combine the fill and decoration coverage
before applying ColorAdd.
Print's dedicated grayscale shader does not consume these style colors.

## Validation

[Test_MSDFFont.cpp](../../Test/Test_MSDFFont.cpp) owns the
`MSDFFont.atlas_dimensions` regression. It places identical real-font glyph texels
in square and rectangular atlases, including a non-power-of-two width, and
compares all five shaders that use `MSDF_Init`. The drawing cases include
minification, magnification, rotation combined with nonuniform scaling,
transparent colors, ColorMul, and ColorAdd. The comparison allows one RGBA8 unit
for UV normalization and texture filtering, and requires visible glyph pixels to
avoid accepting empty output.

`MSDFFont.scoped_color_mul_styles` compares actual MSDF text drawing with a
reference that bakes the multiplier into the fill and straight style colors.
It covers independent changes to ColorMul and style, nested Push and Set, Flush,
and ColorAdd. Zero-alpha cells must match the background exactly.

Use the [host test workflow](../development/README.md#build-and-test) with
`--test-case=MSDFFont*` for focused iteration, then run the full suite. Regenerate
affected Windows bytecode following
[backend shader maintenance](pattern-payload.md#backend-shader-maintenance).
[CEngineShader_D3D11::init](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp)
defines the entry-point and asset mappings. GPU-time measurements are separate
from image regression tests and intermediate instruction counts.
