# Wave

`Pattern::Wave` repeats parallel sine-wave centerlines. The authoritative API
contract is [Wave.hpp](../../Siv3D/include/Siv3D/Pattern/Wave.hpp); the
[manual sample](../../Test/Manual/Wave.md) contains colored presets and transform
controls. Metal rendering is implemented. The remaining backend port and the
other adopted motifs are tracked in [TODO](../../TODO.md).

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

The Metal entry point is `PS_PatternWave`. `PatternType::Wave` and
`EnginePS::PatternWave` are appended to preserve existing enum values. The Metal
loader and renderer select this shader through the same path as other patterns.
No runtime performance improvement is claimed; the additional sine, cosine, and
width correction are specific to Wave.

## D3D11 port checklist

Port after the Metal implementations of the adopted motifs have stabilized.
Do not add a temporary compatibility representation or change the buffer layout.

1. Translate `PS_PatternWave` from
   [2d.metal](../../macOS/App/engine/shader/metal/2d.metal) to
   [2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl), using the
   existing UV and premultiplied color helpers. Use `frac` and `fwidth` for the
   corresponding Metal operations. Preserve the order of filtering and wrapping.
2. Add `2d_pattern_wave.ps` compilation/loading at the `EnginePS::PatternWave`
   index in the D3D11 engine shader loader. Keep the order synchronized with any
   subsequently appended patterns and regenerate the checked-in DXBC on Windows.
3. Add the renderer's shader ID, initialize it, and select it for `PatternType::Wave`.
   Until then, the existing default selector falls back to a solid shape; it does
   not render Wave. This limitation belongs here, not in public-header Doxygen.
4. Enable the Wave GPU tests for Windows in
   [Test_Pattern.cpp](../../Test/Test_Pattern.cpp). CPU packing tests already run
   on both hosts. Follow the existing Halftone criteria when inspecting any
   backend-specific interpolation rounding; do not relax solid-interior checks.
5. Run focused Pattern tests and the complete Windows suite, then inspect the
   manual sample, including animation and nonuniform transforms.

## Validation

CPU tests cover rotated centerlines, negative periods, independent spacing and
wavelength, colors, packing, and zero/full-width endpoints. Metal readback tests
cover interiors, straight-line equivalence, alpha compositing, local/camera
transforms, split geometry and viewports, extra-vector-only changes, and
restoration across a different Pattern shader and subsequent frames.
