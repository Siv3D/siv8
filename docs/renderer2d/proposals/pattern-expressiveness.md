# Pattern expressiveness

Status: exploration proposal, not adopted or implemented. Release scope: not
scheduled. API names below describe concepts, not available C++ declarations.
The separate [parameter storage extension](../pattern-payload.md) is adopted;
it does not implement the expressions proposed here.

This proposal builds on the adopted [coordinate model](../pattern-coordinates.md).
It explores predictable parameters, additional motifs, and spatial variation of
pattern properties. It does not change backend porting work or the independent
QuadWarp interface.

The [polka-dot size gradient candidate](polka-dot-size-gradient.md) examines a
selected visual in more detail, including a constrained representation that fits
the current payload size and the cost of retaining more independent control.

## Separate three responsibilities

1. Placement: where cells repeat, their spacing, orientation, and origin.
2. Motif: what occupies a cell, including radius, line width, and colors.
3. Variation: how one property changes across the drawing coordinates.

These are design responsibilities, not a proposal for three public base classes
or a general shader graph. Start with concrete typed patterns and share internal
machinery when actual use cases justify it.

## Existing parameters

See the [pattern headers](../../../Siv3D/include/Siv3D/Pattern/) and
[Metal shader implementations](../../../macOS/App/engine/shader/metal/2d.metal).
The following observations describe their conversion and pattern equations;
edge filtering can affect measured raster widths.

| Current parameter | Observation | Direction to explore |
| --- | --- | --- |
| `scale` | Sets a coordinate scale. In PolkaDot it is the center spacing; in Checker it is a cell width, with a full two-color repeat spanning two cells. | Name the repeat or cell dimension explicitly for each type. Avoid a universal rename that promises identical semantics. |
| PolkaDot `radiusScale` | Geometric radius is `scale * radiusScale * sqrt(2) / 4`. | Provide radius in drawing units, or a clearly defined relative size. |
| Stripe/Grid `thicknessScale` | Away from filtering effects, line width is `scale * thicknessScale / 2`. HexGrid uses a different distance construction. | Specify line width in drawing units; define the direction in which it is measured for each pattern. |
| `angle`, `offset` | Conversion rotates and scales drawing coordinates, then subtracts `offset` in pattern coordinates. | Distinguish a drawing-coordinate origin from dimensionless phase. Define which feature is at the origin: a dot center, a cell corner, or a line center. |
| Checker intensities | They weight filtered axis masks before combining them. They are not cell sizes or simple global opacity. | Keep their meaning distinct from aspect ratio, cell proportions, and color opacity. |
| `primary`, `background` | Explicit foreground and background colors already exist. | Reuse these for size variation; color variation is a separate property. |

For example, a candidate dot configuration could mean: center spacing 24 drawing
units, radius 6 drawing units, and a named drawing-coordinate origin. Changing
spacing alone would preserve the radius. Scaling the whole drawing would scale
both. A relative-size convenience factory can calculate radius from spacing;
avoid independent absolute and relative radius fields with conflicting values.

Document and test current behavior before changing it. Do not silently reinterpret
existing fields or defaults. Decide compatibility and migration separately from
the preferred parameter vocabulary. Typed pattern structs should remain the main
entry point; generic `param0` and `param1` packing is not a suitable model for a
growing user-facing parameter API.

## Size gradients

The motivating example has a fixed lattice, a solid background, and dots that
grow toward the bottom. This changes radius, not spacing or opacity.

Candidate configuration:

```text
motif: circle
center spacing: 24 drawing units
radius: 0 -> 9 drawing units
variation: linear axis from (rect.centerX, rect.y + rect.h * 0.25)
                      to (rect.centerX, rect.y + rect.h)
profile: smoothstep, clamped outside the endpoints
sampling: one radius per dot, evaluated at the dot center
background: explicit opaque color
```

The rectangle supplies endpoints explicitly. The renderer must not infer bounds
independently for every primitive: that would restart the gradient when a shape
is split. The lattice and variation use drawing coordinates before local/camera
transforms, so both follow the adopted attachment behavior. Rotating the lattice
through the pattern's own angle need not rotate the gradient axis; the axis is
an independent explicit choice.

Given endpoints A and B, evaluate
`t = clamp(dot(p - A, B - A) / dot(B - A, B - A), 0, 1)` and interpolate radius
using either t or a profile such as smoothstep(t). Distinct endpoints are a
candidate precondition, to be settled before implementation.

### Sampling is observable behavior

For dots, evaluate the variation at each lattice center and use one radius for
the entire dot. Evaluating radius separately at every pixel makes the threshold
vary within a dot; its size, center, and potentially its shape can change.
Center sampling preserves circular motifs in drawing coordinates, with discrete
radius changes between rows. Subsequent nonuniform drawing transforms still
deform circles as required by the coordinate model.

A gradient endpoint specifies the parameter at motif centers. It does not
necessarily define a sharp pixel boundary for a blank area: a neighboring motif
may extend across it. An exact blank region requires a separately specified mask
or clipping boundary. Radius zero itself should produce exactly background,
including under antialiasing.

For continuous stripes, a spatial width field can intentionally produce tapered
lines. There is no natural finite cell center along an infinite stripe. Share
the ramp representation, but specify sampling semantics per pattern instead of
applying dot-center sampling to every type.

### Radius, coverage, opacity, and spacing

- Radius changes the outline while keeping centers fixed.
- Coverage measures the fraction occupied by the motif. For disjoint circular
  dots on a square lattice of pitch P, coverage is `pi * r * r / (P * P)`.
  Linear radius therefore does not mean linear coverage. A square-root radius
  profile can give linear geometric coverage within the non-overlapping range;
  it does not promise perceptually linear lightness.
- Opacity changes the color mixture without changing the geometric outline.
- Spacing changes the distribution of centers. Substituting a variable scale in
  `position / scale(position)` also changes phase and distorts placement; it is
  not a complete design for a controllable spacing gradient.

Keep the first implementation to radius variation. Consider coverage profiles
and color variation separately. Defer spatial frequency changes until their
placement and continuity rules are defined.

## Additional pattern candidates

| Candidate | Useful controls | Responsibility and scope |
| --- | --- | --- |
| Staggered dots | Row shift, horizontal/vertical pitch | A lattice option for dots; not necessarily a new public pattern type. A half-row shift alone is not an equilateral triangular lattice. |
| Rings | Outer radius, stroke width | Reuses circle distance. Adds a hollow motif and size/width variation opportunities. |
| Rounded squares / diamonds | Size, corner radius | Adds a motif family. Rotating the motif independently of its lattice is distinct from rotating the entire pattern. |
| Dashed lines / dashed grids | Dash length, gap, width, phase | Useful extension of line patterns; define junction behavior for grids. |
| Waves / zigzags | Wavelength, amplitude, width | Adds curved or angular repetition; clarify normal-distance width versus vertical band width. |
| Bricks | Cell dimensions, row shift, joint width | Combines offset rows and rectangular cells; potentially shares lattice machinery. |
| Crosshatch / weave | Direction-specific widths, crossing rule | Overlapping stripes may reuse Grid for simple cases. True over/under weave needs an additional crossing model. |

Prioritize staggered dots, rings, and dashed lines: each adds a different useful
degree of freedom. Avoid adding names that are merely presets of an existing
type. Defer procedural noise, per-cell randomization, and arbitrary masks until
filtering, determinism, and parameter costs are justified by concrete examples.

## Implementation boundaries and cost

An additional simple motif may fit the existing UV mapping plus two scalar
parameters. A general radius ramp with independent endpoints will not fit that
payload without either constraining the feature or extending its representation.
[PatternParameters](../../../Siv3D/include/Siv3D/Pattern/PatternParameters.hpp)
packed three Float4 values before the adopted storage extension. The
[Metal command manager](../../../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer2D/Metal/MetalRenderer2DCommandManager.hpp)
now records four Float4 values. Inspect the full packing,
command-state comparison, shader binding, and custom-shader contract before
choosing a larger or separate payload. Changing only a fragment shader is not
enough for a public gradient feature.

For a first proof of concept, use a custom Metal fragment shader and explicit
constants in a manual sample. This exercises the look and sampling rules before
committing to a public struct layout or renderer changes.

For integration, precompute ramp coefficients on the CPU so the fragment shader
needs no endpoint normalization division. For center sampling, compose the field
with the inverse pattern mapping on the CPU, or derive the coefficients directly
from typed placement parameters. Do not compute a matrix inverse per pixel.
Singular arbitrary UV transforms need an explicit contract before such an inverse
can be required. Dot-cell selection remains fragment work; clamped ramps cannot
generally be replaced by vertex-only evaluation without changing the result.

Prefer a constant-parameter shader path and a limited radius-ramp variant selected
per draw. Avoid evaluating unused gradients in all existing pattern fragments.
Do not expand into every combination of type, profile, sampling, and property:
first support one property and measure whether a small uniform choice or another
variant is preferable. More parameters can increase command storage and state
changes even when GPU arithmetic is modest. Measure draw counts and GPU time
separately; no performance result is implied by this proposal.

## Suggested review stages

### Value of one additional Float4

The leading size-gradient candidate uses one additional Float4. Its value extends
beyond a 45-degree dot preset: it can separate a spatial field from the placement
of a pattern. This remains a proposal, not a commitment to implement every example
below. A new motif alone often fits the current payload; the extension is chiefly
useful when a motif property varies independently across space.

For a linear field, three floats are sufficient for coefficients `(gx, gy, bias)`:

```text
t = saturate(gx * samplePosition.x + gy * samplePosition.y + bias)
```

The fourth component can initially be reserved and initialized to zero. Do not
add a user-facing knob merely to occupy it. A fixed smoothstep profile requires
no additional coefficient. Future profile choices can be reviewed separately.
Existing pattern-specific scalars describe the affected property; the affine UV
mapping continues to describe placement. Thus two independent endpoints on an
axis are converted to a compact field rather than passed as two raw Vec2 values.

For cell-based motifs, transform the linear field into pattern coordinates on
the CPU and evaluate at the motif center. The profile then remains constant
within each dot or tile, even when the field points diagonally. For stripe width,
one useful rule samples at the stripe centerline, quantizing the across-stripe
coordinate while keeping the along-stripe coordinate continuous. This preserves
the line center while allowing taper. Share field coefficients, but document
sampling behavior for each motif family.

| Candidate | Existing two scalar slots | Extra Float4 | Visual opportunity |
| --- | --- | --- | --- |
| Dot size, any square-lattice angle | Minimum and maximum radius | Linear field plus reserved component | Keep dot positions fixed while moving a transition vertically, horizontally, or diagonally. |
| Rounded tile size | Maximum half-size and corner-size ratio; minimum size fixed at zero | Same linear field | Small marks become larger rounded tiles toward an edge; both size and corners scale together. |
| Ring stroke width | Outer radius and maximum stroke width; minimum width fixed at zero | Same linear field | Rings appear and thicken while their outer diameters stay fixed. |
| Stripe width | Minimum and maximum width | Same linear field | Thin hatching grows into a stronger band; centerline sampling allows tapered stripes. |
| Grid width | Minimum and maximum shared line width | Same linear field | A subtle grid becomes progressively stronger. Independently controlled X/Y ranges are not included in this budget. |
| Pattern visibility | Existing constant motif parameters | Same linear field | Fade foreground motif coverage into the background, or reveal it across a moving transition. |

These are individual variants using a shared field, not a promise to vary all
properties independently at once. For example, the ring example has a fixed
outer radius while width varies; independently varying both with separate ramps
would need another design. The rounded-tile example scales corner radius with
tile size, rather than assigning an additional independent corner gradient.

The visibility variant modulates foreground coverage. Existing foreground and
background colors remain available, but an independently specified additional
RGBA endpoint color would use the entire new Float4 by itself. Do not promise
full arbitrary color endpoints and an independent spatial field with this same
single extension.

Plausible next steps include:

- Circular radial variation in pattern coordinates. A separate field variant can
  use four floats for center X/Y, start radius, and inverse transition distance.
  This could create an empty center with increasingly large dots or thicker rings
  around it. It uses a different interpretation of the same storage, adds a
  distance calculation, and does not also retain an independent linear field.
  A circle in pattern coordinates follows their affine deformation; arbitrary
  independently oriented elliptical fields need further representation work.
- A moving reveal or pulse. Updating the field from the application can animate
  existing motifs without adding a shader time uniform. Parameter updates may
  cause state changes; animation is not free rendering work.
- Geometric coverage profiles for halftone-like dots. A radius profile can target
  area coverage within a non-overlapping range. This requires profile and filtering
  work, not merely another motif name.

The first implementation should support a single linear field and dot radius,
then validate reuse through stripe width. Ring width and rounded tiles are useful
subsequent candidates because their geometry and sampling differ. Keep ordinary
constant patterns on shader paths that do not evaluate a field. The expanded
record and constant upload are real costs even on those paths if the shared
layout is enlarged; measure them separately from new fragment arithmetic.

An extra Float4 does not by itself solve arbitrary texture-driven masks, variable
spacing, procedural noise, multiple independent ramps, or arbitrary multi-stop
gradients. Those need additional algorithms or resource bindings. The case for
this extension is the concrete family above, rather than unspecified future use.

### Implementation order

1. Establish parameter units, origin/phase meaning, and compatibility policy
   using the existing gallery and focused conversion tests.
2. Prototype fixed-pitch dot radius variation in a self-contained manual Metal
   sample: constant radius, linear/smooth ramps, center versus pixel evaluation,
   and opacity comparison. Keep the regular Pattern renderer unchanged.
3. After reviewing the appearance, settle one public ramp API and its payload;
   integrate it through the normal pattern draw paths.
4. Add staggered dots and rings to test which abstractions actually generalize.
   Consider dashed lines and width variation after their semantics are clear.

For an adopted API, focused tests should cover constant/ramp equivalence, exact
zero radius, maximum supported radius, negative coordinates and phase, reversed
gradient direction, endpoints and invalid endpoint policy, split-shape
continuity, transforms, and interleaving gradient and ordinary draws. Include
strong ramps, cell edges, subpixel radii, and substantial minification: derivatives
of a cell-constant radius can jump at cell boundaries. Define supported overlap
before allowing radii beyond half the nearest-center distance; a shader that
checks only the current cell cannot generally render neighboring overlaps.
