# Renderer2D

## Shader design

- [Shader optimization assessment and plan](proposals/shader-optimization-plan.md):
  proposed stages, D3D11/Metal evidence, compatibility boundaries, and validation.
- [MSDF font scale](msdf-font.md): atlas dimensions, UV derivatives, shadow
  offsets, and font shader validation.

## Pattern design

- [Coordinates](pattern-coordinates.md): attachment to local/camera transforms,
  primitive continuity, and custom-shader interfaces.
- [Parameter payload](pattern-payload.md): four-vector packing, effect-buffer
  offsets, state tracking, and backend shader maintenance.
- [Halftone](halftone.md): independent radius field and per-dot sampling.
- [Wave](wave.md): sine-wave bands and the normal-width approximation.
- [Ripple](ripple.md): concentric bands and center-safe filtering.
- [Weave](weave.md): alternating crossings, gaps, and filtered parity.
- [Truchet](truchet.md): connected arcs, layouts, and deterministic seed encoding.

## Gallery and checks

- [Pattern collection](../../Test/Manual/PatternCollection.md): the canonical
  960 x 700 gallery of 84 backgrounds across all eleven patterns and combinations,
  with paging and animation controls.
- [Halftone controls](../../Test/Manual/Halftone.md): independent field/lattice
  controls, guides, split geometry, and transformed comparisons.
- [Pattern transforms](../../Test/Manual/PatternTransforms.md): attachment,
  continuity across primitives, and geometry versus coordinate-system movement.
- [RMS scaling](../../Test/Manual/Renderer2DRMSScaling.md): tessellation and
  line-width compensation under nonuniform transforms.

[Test_Pattern.cpp](../../Test/Test_Pattern.cpp) owns automated packing and GPU
regression checks. See [coordinate validation](pattern-coordinates.md#validation)
for the host-specific test commands.
