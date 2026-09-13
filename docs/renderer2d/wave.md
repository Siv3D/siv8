# Wave

`Pattern::Wave` repeats parallel sine-wave centerlines. The authoritative API
contract is [Wave.hpp](../../Siv3D/include/Siv3D/Pattern/Wave.hpp); the
[Pattern collection](../../Test/Manual/PatternCollection.md) provides appearance
recipes and animation. Metal and D3D11 use the same wave calculation and payload.

## Coordinates and width

With `q = Rotate(-angle) * (drawingPosition - origin)`, the centerlines are
`q.y = amplitude * sin(2*pi*q.x/wavelength) + n*pitch` for integer n.
The origin is a zero crossing with positive slope when amplitude is positive.
The existing [coordinate model](pattern-coordinates.md) carries the entire
pattern with local and camera transforms. A nonuniform object transform also
deforms the wave and its thickness; thickness is not a screen-pixel width.

A vertical distance test alone makes the line appear thinner on steep slopes.
Wave multiplies the vertical half-width by `sqrt(1 + slope*slope)`, where
`slope = 2*pi*amplitude/wavelength * cos(2*pi*q.x/wavelength)`.
This is a first-order normal-width approximation. It avoids iterative closest
point searches, but it is not the exact offset curve of a sine wave. Wide strokes
and high curvature can visibly depart from constant-distance strokes. Adjacent
bands merge when the corrected width reaches their pitch.

The fragment shader applies the existing Stripe edge filter to the unwrapped
wave phase, then wraps for the distance test. Zero amplitude has the same
filtering as Stripe at matching affine phase and normalized width. It does not
mean copying Stripe's public `thicknessScale` value: that API packs half its
value, whereas Wave packs `thickness / pitch`.
The derivative filter is local; it does not integrate many wave periods over a
pixel. Strong minification or a very short wavelength can alias.

## Payload and shader integration

The [64-byte Pattern payload](pattern-payload.md) and 128-byte effect buffer are
unchanged. Conversion precomputes coefficients once; no new draw overload,
vertex shader, command field, or QuadWarp change is required.

| Payload | Meaning |
| --- | --- |
| UV x | q.x / wavelength |
| UV y | q.y / pitch |
| param0 | thickness / pitch |
| param1 | amplitude / pitch |
| extraParams.x | 2*pi*amplitude / wavelength |
| extraParams.yzw | Reserved, zero |

Both backends use `PS_PatternWave`. `PatternType::Wave` and
`EnginePS::PatternWave` select it through the existing Pattern drawing path.
No runtime performance improvement is claimed; the additional sine, cosine, and
width correction are specific to Wave.

## D3D11 integration

[2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl) preserves the
Metal shader's order of filtering and wrapping, using `frac`, `fwidth`, and the
existing UV and premultiplied color helpers. The loader appends
`2d_pattern_wave.ps` after Halftone, and the renderer selects it for
`PatternType::Wave`. See the [shader maintenance notes](pattern-payload.md#backend-shader-maintenance)
for shader generation and verification.

## Validation

CPU tests cover rotated centerlines, negative periods, independent spacing and
wavelength, colors, packing, and zero/full-width endpoints. Metal and D3D11 readback tests
cover interiors, straight-line equivalence, alpha compositing, local/camera
transforms, split geometry and viewports, extra-vector-only changes, and
restoration across a different Pattern shader and subsequent frames.
