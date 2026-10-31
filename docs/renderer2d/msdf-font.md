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

Texture dimensions are queried from the bound texture, so atlas growth requires
no additional inverse-size constant or CPU-side state tracking. Foreground PMA,
outline and shadow colors, coverage thresholds, and texture sampling are separate
from this scale calculation.

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

## Scoped color multiplication

`TextStyle::Outline`, `Shadow`, and `OutlineShadow` use the current
`ScopedColorMul2D` for their decoration colors as well as the text fill. Each
style color retains its own alpha, multiplied by the scope's effective alpha.
The text fill's drawing color remains independent of the style colors.

```cpp
const TextStyle style = TextStyle::OutlineShadow(
    0.2, Palette::Orange, Vec2{ 4, 3 }, ColorF{ 0.0, 0.5 });
const ScopedColorMul2D fade{ 1.0, 0.5 };
font(U"Ready").draw(style, Vec2{ 40, 40 });
```

Here `font` is an MSDF font. The fill, outline, and shadow fade together; a scope
alpha of zero makes all three invisible, including with ColorAdd enabled.
Nested Push and explicit Set use the same
[independent color-scope rules](color-scopes.md). ColorAdd is applied after
the shader combines the fill and decoration coverage.

Style colors are stored in PMA form. The Metal and D3D11 pixel shaders multiply
them component-wise by `(r*a, g*a, b*a, a)` for the effective straight multiplier
`(r, g, b, a)`, before combining their coverage with the text fill. The fill has
already received ColorMul in the vertex shader and is not multiplied again.

The existing pixel-shader constant `g_patternBackgroundColorMul` carries the
effective straight multiplier, despite its historical pattern-specific name.
`g_sdfOutlineColorPMA` and `g_sdfShadowColorPMA` retain the original style PMA
colors. The CPU copies these independent constants without recalculating style
colors or tracking a dependency between ColorMul and the style. Restoring from
zero and flushing inside a scope therefore require no special handling.

The constant-buffer layout is unchanged. Custom pixel shaders reading the style
colors receive untinted PMA values and must apply the multiplier themselves if
they need the same behavior. Print's dedicated grayscale shader does not consume
these style colors.

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
It covers all three decorated styles, translucent style colors, RGB tint,
alpha zero and one, nested fades, Set, scope restoration, and ColorAdd. It
changes ColorMul without changing the style and changes the style without
changing ColorMul, including a flush at alpha zero and repeated frame captures.
Visible reference glyphs prevent vacuous comparisons. Both reference and scoped
draws must leave zero-alpha cells exactly equal to the background, including with
ColorAdd enabled.

Use the [host test workflow](../development/README.md#build-and-test) with
`--test-case=MSDFFont*` for focused iteration, then run the full suite. When
changing `MSDF_Init`, regenerate all five affected Windows pixel-shader assets:
`msdffont.ps`, `msdffont_outline.ps`, `msdffont_shadow.ps`,
`msdffont_outline_shadow.ps`, and `msdffont_print.ps`. Changes to Glow also
require regenerating `msdffont_glow.ps`.
Changes to decorated style-color multiplication require regenerating
`msdffont_outline.ps`, `msdffont_shadow.ps`, and `msdffont_outline_shadow.ps`.
[CEngineShader_D3D11::init](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp)
defines their entry-point and file mappings; its enabled startup compilation
block compiles the HLSL source before loading these assets on Windows. Follow
[backend shader maintenance](pattern-payload.md#backend-shader-maintenance) when
updating the checked-in bytecode. GPU-time measurements are separate from image
regression tests and intermediate instruction counts.
