# Weave

`Pattern::Weave` draws plain weave using alternating over/under crossings, one
foreground color, and background-colored gaps. The authoritative contract is
[Weave.hpp](../../Siv3D/include/Siv3D/Pattern/Weave.hpp); the
[manual sample](../../Test/Manual/Weave.md) provides nine presets and transform
controls. Metal is implemented; D3D11 remains in [TODO](../../TODO.md).

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

## D3D11 port checklist

1. Translate `PS_PatternWeave` from
   [2d.metal](../../macOS/App/engine/shader/metal/2d.metal) to
   [2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl), preserving the
   filtered parity, the two band masks, and the existing premultiplied color path.
   The HLSL file already has `Pattern_CheckersFiltered`.
2. Append `2d_pattern_weave.ps` compilation/loading at `EnginePS::PatternWeave`,
   after Ripple, and regenerate the DXBC on Windows. Register the renderer's
   shader ID, initialization, and `PatternType::Weave` selection.
3. Enable the Weave GPU cases in
   [Test_Pattern.cpp](../../Test/Test_Pattern.cpp) for Windows. Keep exact
   zero/full-width compositing and solid-interior checks. Inspect any differences
   at antialiased edges separately.
4. Run focused Pattern tests and the complete Windows suite, then inspect the
   gallery, animation, negative coordinates, transforms, and maximum-gap cuts.

Until the renderer selection is ported, its existing default draws a solid shape.
This temporary state belongs in development notes, not public-header Doxygen.

## Validation

CPU tests cover rotated/negative crossing positions, colors, band/gap packing,
and zero/full-width and zero/maximum-gap endpoints. Metal readbacks compare
interiors with geometric crossing rules, exercise maximum-gap antialiasing,
verify alpha compositing, local/camera transforms, split geometry and viewports,
and restore a gap-only state change across Wave and repeated frames.
