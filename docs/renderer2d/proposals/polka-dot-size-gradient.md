# Polka-dot size gradient

Status: superseded by the adopted [design](../polka-dot-size-gradient.md).
The independent-field API and Metal shader are implemented; D3D11 is tracked in
[TODO](../../../TODO.md). The compact alternative below is historical exploration,
not the adopted API or payload.
This is a feasibility review following the broader
[pattern expressiveness exploration](pattern-expressiveness.md).

The selected visual is a square dot lattice rotated by 45 degrees, with circular
dots growing toward the bottom. It is not an equilateral triangular lattice.
The radius is evaluated once per dot center. The gradient and lattice follow
the adopted [drawing-coordinate model](../pattern-coordinates.md).

## Recommendation

Controlling the height where the size transition starts is required. The working
design assumption is to preserve the interactive exploration's behavior: changing
that height keeps dot centers fixed. Prefer an explicit payload extension for
this behavior over hiding extra values in matrix elements or borrowing QuadWarp
storage. The adopted storage stage adds one Float4; this document retains the
earlier size comparison to explain that choice.

Keep lattice origin, transition start, transition end, and maximum radius as
separate concepts. A rectangle helper can translate a relative start height into
drawing coordinates; the shader receives the resulting field coefficients.
Moving the start with the end held fixed changes the transition length, as in the
exploration. Neither operation should move the lattice under this assumption.

The compact alternative below can preserve the existing 48-byte pattern payload
and 112-byte effect constant structure by sharing origin and orientation. It can
control start height, but doing so also moves dot centers. It is no longer the
leading recommendation for the assumed interaction.

## Baseline storage before the extension

[PatternParameters](../../../Siv3D/include/Siv3D/Pattern/PatternParameters.hpp)
and its [packing function](../../../Siv3D/include/Siv3D/Pattern/PatternParameters.ipp)
used the following three-vector effect payload before the storage extension:

| Data | float count | Bytes |
| --- | ---: | ---: |
| Affine UV mapping | 6 | 24 |
| Pattern-specific scalars | 2 | 8 |
| Background RGBA | 4 | 16 |
| Total | 12 | 48 |

There is no unused scalar in the generic payload. PolkaDot leaves one of its two
pattern-specific scalars unused, but a maximum radius and independent ramp slope
and intercept would require three scalars.

Foreground color travels with shape vertices. Pattern type selects a pixel
shader; it does not occupy another slot in this effect payload. The
[Metal command manager](../../../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer2D/Metal/MetalRenderer2DCommandManager.mm)
records and compares `std::array<Float4, 3>` values. The
[D3D11 command manager](../../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/D3D11Renderer2DCommandManager.cpp)
has the same pattern payload shape.

The shared
[PSEffectConstants2D](../../../Siv3D/src/Siv3D/Renderer2D/Renderer2DCommon.hpp)
contains these three Float4 values plus four Float4 values for QuadWarp, for
112 bytes altogether. The
[Metal renderer](../../../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer2D/Metal/CRenderer2D_Metal.mm)
passes the whole effect constant structure through `setFragmentBytes` when it
is dirty. Thus 48 bytes is the pattern record size, not the size of this Metal
constant upload. Driver allocation and alignment are separate from these sizes.

The CPU-facing PatternParameters struct also contains primary color and type;
it is distinct from the packed payload. The compact candidate adds no fields to
that struct. A new typed pattern description may contain more user-facing fields
without enlarging the existing packed representation.

## Compact candidate: shared origin and orientation

Let:

- P be the nearest-center spacing in drawing units, with P > 0.
- R be the maximum radius in drawing units, initially restricted to 0 <= R <= P/2.
- L be the positive transition length in drawing units.
- O be the shared pattern origin and a point on the zero-radius boundary.
- a be the square-lattice angle; 45 degrees gives a downward gradient.

Use the existing affine mapping without repurposing any matrix elements:

```text
q = Rotate(-a) * (drawingPosition - O) / P
f = fract(q)
cellCenter = q - f + 0.5

param0 = 2 * R / P
param1 = P / (sqrt(2) * L)

t = saturate((cellCenter.x + cellCenter.y) * param1)
radiusInRepeatCoordinates = param0 * t * t * (3 - 2 * t)
repeat = 2 * f - 1
```

Use the same `length(repeat)` distance convention as
[PolkaDot](../../../macOS/App/engine/shader/metal/2d.metal). The radius expression
above is in its doubled cell coordinates. It preserves exact circles before
local/camera deformation. This is a geometry and packing sketch, not a finished
antialiasing implementation.

At a = 45 degrees, `q.x + q.y = sqrt(2) * (drawingY - O.y) / P`.
The ramp expression consequently equals `(dotCenterY - O.y) / L`. At other
angles, the ramp direction rotates with the lattice; it always bisects the two
positive lattice axes. A CPU numerical check of this equality across several
spacings, lengths, angles, and positive/negative cells agrees to floating-point
roundoff. No GPU result or timing is established by that check.

The shader can reuse the existing fractional cell coordinate and derive its
center by subtraction. No per-pixel matrix inverse, endpoint normalization
division, additional vertex attribute, or additional varying is required.

### User-visible tradeoffs

- Moving O moves both the lattice and the gradient. Their phases cannot be
  animated independently with this representation.
- Increasing L stretches the transition without moving the lattice. Increasing
  R changes the largest dots without changing their centers.
- Rotating a rotates both the lattice and the gradient. For the selected
  45-degree/downward case, no independently adjustable gradient angle is needed.
- The minimum radius is zero. The proposed first profile is smoothstep. A
  separately chosen minimum radius or arbitrary profile is outside this payload.
- Shapes do not supply implicit bounds. The caller sets O and L explicitly, so
  splitting a rectangle into triangles cannot restart the pattern or gradient.
- The zero-radius boundary is about dot centers. It is not an additional clipping
  boundary for neighboring nonzero circles that may extend across it.

For example, the following is provisional API syntax, not executable sample code:

```cpp
const Pattern::PolkaDotSizeGradient pattern{
    .primary = Palette::Cornflowerblue,
    .background = Palette::White,
    .pitch = 36.0,
    .maxRadius = 12.0,
    .origin = Vec2{ rect.x, rect.y + rect.h * 0.25 },
    .transitionLength = rect.h * 0.75,
    .angle = 45_deg,
};

rect.draw(pattern);
circle.draw(pattern);
```

The conversion operator would compute the existing PatternParameters layout.
The type name makes size variation explicit. `pitch`, `maxRadius`, and `origin`
are candidate names with drawing-coordinate units; they do not silently change
the existing `PolkaDot::scale`, `radiusScale`, or dimensionless `offset` fields.
Naming consistency with other patterns remains a separate API review item.

## More independent control

Keeping the full affine mapping, radius control, and background color while
adding an independent linear field needs additional scalar storage in the
straightforward layout. An additional Float4 can hold a gradient coefficient
vector `(gx, gy)`, a bias, and one reserved or profile value. The existing two
pattern scalars could then hold minimum and maximum radii.

| Representation | Pattern record | Effect constants | Main consequence |
| --- | ---: | ---: | --- |
| Before extension | 48 B | 112 B | Constant parameters |
| Compact candidate | 48 B | 112 B | Shared origin and orientation, zero minimum |
| Append one Float4 | 64 B | 128 B | Room for an independent field |

The last row extends the shared pattern record and adds a field to the effect
structure, grouping Pattern in its first 64 bytes and QuadWarp in its second
64 bytes. It means 16 more bytes per stored pattern state and
16 more bytes per effect upload in that design; it does not imply a proportional
frame-time increase. C++ and shader field offsets must be updated together.
An independent extra buffer for gradient draws could retain ordinary record
sizes, but adds command-state and binding machinery and needs a separate review.

A specialized compressed placement representation could also fit more controls
into 48 bytes by representing only similarity transforms rather than all six
affine coefficients. It would change the existing matrix/payload semantics or
require a new tagged representation. That is a broader API/data-model change,
not unused space in the current layout. Do not take this route merely to avoid
one Float4 before comparing its restrictions and maintenance cost.

Do not use float bit packing with reduced precision, vertex color components,
or the currently unused QuadWarp fields as hidden storage. QuadWarp command
state is independently tracked, and aliasing its storage would introduce state
restoration obligations. Keep its shader interface and optimization work separate.

## Integration size and rendering cost

For the compact candidate, add one typed pattern header, one PatternType, one
engine pixel-shader entry, and a case in each backend's pattern shader selection.
Append enum/shader entries consistently with the loader's index-based lookup.
The existing Pattern vertex shader and the ordinary pattern geometry entry
points can remain as they are: they already forward PatternParameters and
select a fragment shader from its type. Register the header in the umbrella
include and both platform project files when implementing it.

Keep a dedicated fragment shader for the size gradient. Existing constant
patterns then acquire no gradient arithmetic. The new path adds cell-center and
ramp evaluation; shader/pipeline resources also increase with an extra shader.
The same parameter equality tracking can batch adjacent matching draws. Changing
pattern type or parameters remains a state transition; a larger or smaller
payload does not remove those transitions.

Radius zero must yield exactly background, rather than leave an antialiased
residual dot. Obtain derivatives in valid uniform execution before any early
return. Review filtering at cell boundaries, where radius is cell-constant and
can jump, and under strong minification. These are implementation checks, not
reasons to add exceptional-input guards to ordinary draw functions.

## Next review step

Prepare a manual Metal prototype with a fixed lattice and independently editable
transition start and end. Animate the start height with the end held fixed and
verify that dot centers remain stationary. Review the payload extension and its
effect on ordinary Pattern records before selecting the public API. Keep the
initial sample in Test/Manual and preserve the normal platform Main.cpp after
running it.

If adopted, test packing size, radius-zero output, ramp endpoints, constant-size
regions, negative cells, local/camera transforms, split-shape continuity, and
interleaving ordinary Pattern, gradient Pattern, texture, and QuadWarp draws.
Verify actual Metal compilation, rendered output, draw counts, and GPU cost;
numeric packing checks alone do not establish those results. Coordinate the
backend registration with the separate D3D11 port without changing that work in
this feasibility stage.
