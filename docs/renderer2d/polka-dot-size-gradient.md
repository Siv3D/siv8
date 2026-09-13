# Polka-dot size gradient

`Pattern::PolkaDotSizeGradient` describes circular dots on a square lattice with
an independently positioned linear field controlling their radius. Its default
lattice angle is 45 degrees; other angles do not rotate the field.
The authoritative API contract is in
[PolkaDotSizeGradient.hpp](../../Siv3D/include/Siv3D/Pattern/PolkaDotSizeGradient.hpp).
The [interactive sample](../../Test/Manual/PolkaDotSizeGradient.md) provides
presets, independent controls, and a transformed comparison.

The shared type and Metal shader are implemented. The D3D11 shader and its
renderer connection remain a separate stage in [TODO](../../TODO.md); use the
[porting guide](d3d11-polka-dot-size-gradient-handoff.md) for that work.

## Why a separate pattern

The six constant-parameter patterns keep their existing APIs and fragment
calculations. A separate type gives the new operation explicit drawing-unit
spacing, radii, origin, and transition endpoints without adding unused settings
or fragment branches to ordinary dots. It converts to the same
[PatternParameters](../../Siv3D/include/Siv3D/Pattern/PatternParameters.hpp), so
existing shape drawing overloads and the Pattern vertex shader can be reused.
QuadWarp keeps its independent interface and calculations.

The first implementation uses a fixed smoothstep profile. Profile selection,
other motifs, and a reusable public field type are future design questions, not
part of this API. The field coefficients can support other motifs later without
requiring their pixel shaders to share a dispatch branch.

## Coordinates and controls

Placement and radius variation use the common drawing coordinates before local
and camera transforms. An object transform carries both together, including
nonuniform scale, reflection, and shear. Changing a shape's geometry alone does
not reset either coordinate system; see the [coordinate model](pattern-coordinates.md).

`origin` is a dot center. With pitch P and lattice rotation a, the other centers
are `origin + Rotate(a) * (P * integerPair)`. This makes the square lattice's
four nearest neighbors P units away, including at 45 degrees. It is not the
six-neighbor equilateral triangular arrangement.

`start` and `end` define a linear projection field rather than clipping bounds.
For dot center C, compute:

```text
d = end - start
t = clamp(dot(C - start, d) / dot(d, d), 0, 1)
f = t*t*(3 - 2*t)
radius = minRadius + (maxRadius - minRadius)*f
```

The field is constant along lines perpendicular to d. Reversing the endpoints
reverses the growth direction. Changing pitch, origin, or lattice angle does not
move those lines, although the centers sampling the field change. Changing the
endpoints does not change the centers. Equal radii produce constant-size dots.

Sampling at the center is necessary to retain circles: evaluating the radius
at every fragment would distort the boundary within a dot. A circle whose
center is beyond the start line may extend back across that line. This is
intentional center-based evaluation, not a mask that enforces a blank half-plane.

Radii are limited to P/2. This lets each fragment evaluate the circle in its own
cell without searching neighboring cells whose potentially larger circles might
otherwise reach it. This bound is both a useful placement rule and a shader
cost boundary.

## Packing the independent field

The existing [64-byte pattern payload](pattern-payload.md) is sufficient.
The CPU computes the affine placement and converts the drawing-space field into
lattice coordinates once per conversion to `PatternParameters`.

```text
q = Rotate(-a) * (drawingPosition - origin) / P + (0.5, 0.5)
cellCenter = floor(q) + (0.5, 0.5)
repeat = 2 * (q - cellCenter)
```

For `g = d / dot(d,d)`, `c = cos(a)`, and `s = sin(a)`:

```text
gx = P * ( c*g.x + s*g.y)
gy = P * (-s*g.x + c*g.y)
bias = dot(origin - start, g) - 0.5*(gx + gy)
```

Then `dot(cellCenter, (gx,gy)) + bias` equals the unsaturated drawing-space
projection at that dot center. The added half-cell translation makes `origin`
a center rather than a cell corner. Negative cells use floor, not truncation.

| Payload | Meaning |
| --- | --- |
| Existing affine mapping | Drawing coordinates to q |
| param0 | `2 * minRadius / P` |
| param1 | `2 * maxRadius / P` |
| extraParams.xyz | gx, gy, bias |
| extraParams.w | Reserved, written as zero and ignored |
| Background / vertex color | Existing color path |

The radius is doubled because `repeat` spans -1 to 1 across a cell. All four
Float4 values still occupy 64 bytes. Effect constants remain 128 bytes; vertices,
interpolators, command payload types, and buffer bindings do not grow.
The conversion requires finite inputs satisfying the header's preconditions;
it does not add exceptional-input guards to the drawing path.

## Metal fragment evaluation

[PS_PatternPolkaDotSizeGradient](../../macOS/App/engine/shader/metal/2d.metal)
computes the cell center, saturates its field value, applies smoothstep, and
interpolates the two packed radii. It uses the same foreground/background color
and premultiplied-alpha operations as the other patterns.

Antialiasing differentiates the continuous q coordinates. It does not
differentiate a radius that jumps between cells. The filter width is
`length(fwidth(q))` in the doubled repeat-coordinate calculation. Coverage is:

```text
coverage = (1 - smoothstep(radius-fw, radius+fw, length(repeat)))
           * clamp(radius/fw, 0, 1)
```

The second factor fades subpixel dots and makes zero radius contribute exactly
zero foreground, including when a pixel center coincides with a dot center.
It is an inexpensive edge filter, not an exact integration of circle area;
strong minification and rapidly varying radii can still alias.

The shader does no per-fragment matrix inversion or endpoint normalization.
There is one additional engine pixel shader. Existing engine enum indices are
preserved by appending its entry after the current font shaders; the Metal
loader appends in the same order. No GPU-time improvement is claimed.

## Validation

[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) includes CPU checks of the packed
field against drawing-space projection, endpoint boundaries, negative cells,
rotated lattices, independent placement, zero radius, and equal radii. GPU checks
compare interior pixels to circles reconstructed in drawing coordinates, cover
zero-background compositing, object/camera transforms, split geometry with a
nonzero viewport, and state changes/repeated frames. The GPU checks currently
run on Metal; the porting guide describes enabling them for D3D11.
