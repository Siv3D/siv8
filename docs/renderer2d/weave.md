# Weave

`Pattern::Weave` draws plain weave using alternating over/under crossings, one
foreground color, and background-colored gaps. The authoritative contract is
[Weave.hpp](../../Siv3D/include/Siv3D/Pattern/Weave.hpp); the
[Pattern collection](../../Test/Manual/PatternCollection.md) provides appearance
recipes and animation. Metal and D3D11 use the same filtered band, gap, and parity masks.

## Placement and gaps

In `q = Rotate(-angle) * (drawingPosition - origin) / pitch`, integer coordinates
are crossing centers. At even column-plus-row parity the vertical band passes
above the horizontal band. Odd parity swaps that ordering, including at negative
coordinates. The full pattern repeats every two pitches along either axis;
shifting by one pitch swaps over/under order.

Both bands have the same width. At each crossing, the lower band stops `gap`
units before the upper band's side and resumes the same distance past its other
side. This gives an over/under cue without adding a shading color or a third
color-compositing path. The gap is limited to `(pitch - thickness) / 2`, so cuts
reach at most halfway to the next crossing. At zero gap the result is a continuous
grid. Zero width is background only; full width requires zero gap and fills the
shape with the foreground color.

The [existing coordinate model](pattern-coordinates.md) applies to the lattice,
its gap, and the over/under arrangement together. A nonuniform local or camera
transform stretches the weave with the shape. Moving the shape's geometry alone
does not relocate the pattern origin.

## Filtering and payload

The shader starts with two crossing bands, using the Stripe width filter on each
axis. A second pair of band masks includes the width of both side gaps. Their
differences isolate the cuts, which are removed from the lower band. Zero gap
makes these differences exactly zero. Full width keeps both bands fully covered,
so it does not produce seams at cell boundaries.

Crossing parity uses the existing `Pattern_CheckersFiltered` helper. Filtering
only the bands and selecting parity with an unfiltered integer test would leave
hard cut endpoints at cell boundaries when the gap reaches its maximum. The
parity footprint and band footprints are computed from continuous UV coordinates.
This is a local mask filter, not exact area integration over a woven pattern;
strong minification can lose detail or alias.

The [64-byte pattern payload](pattern-payload.md) remains unchanged:

| Payload | Meaning |
| --- | --- |
| UV transform | q |
| param0 | thickness / pitch |
| param1 | (thickness + 2*gap) / pitch |
| extraParams | Zero; unused |

The expanded width is computed on the CPU during conversion. The existing
Pattern vertex path, shape overloads, command storage, and QuadWarp are unchanged.
The extra mask filtering is specific to Weave; other Pattern shaders incur no
additional work.

## D3D11 integration

`PS_PatternWeave` in
[2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl) preserves the
filtered parity, both band masks, and the premultiplied color path. The loader
appends `2d_pattern_weave.ps` after Ripple, and the renderer selects it for
`PatternType::Weave`. See the [shader maintenance notes](pattern-payload.md#backend-shader-maintenance)
for shader generation and verification.

## Validation

CPU tests cover rotated/negative crossing positions, colors, band/gap packing,
and zero/full-width and zero/maximum-gap endpoints. Metal and D3D11 readbacks compare
interiors with geometric crossing rules, exercise maximum-gap antialiasing,
verify alpha compositing, local/camera transforms, split geometry and viewports,
and restore a gap-only state change across Wave and repeated frames.
