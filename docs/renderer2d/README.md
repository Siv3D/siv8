# Renderer2D

Renderer design notes and related checks:

- [Pattern coordinate design](pattern-coordinates.md): adopted coordinate model
  and the data path for making patterns follow local and camera transforms.
- [Pattern parameter payload](pattern-payload.md): four-vector packing, effect
  constant offsets, state tracking, and storage-only validation.
- [D3D11 payload validation](d3d11-pattern-payload-handoff.md): data flow,
  shader-bytecode regeneration, and Windows verification.
- [Pattern expressiveness proposal](proposals/pattern-expressiveness.md):
  unadopted exploration of parameter units, additional motifs, and size gradients.
- [Polka-dot size gradient](polka-dot-size-gradient.md): adopted API, coordinate
  field, packing, fragment filtering, and validation.
- [D3D11 size-gradient integration](d3d11-polka-dot-size-gradient-handoff.md):
  shader loading and Windows verification.
- [Size-gradient manual sample](../../Test/Manual/PolkaDotSizeGradient.md): presets,
  independent field/lattice controls, and object transformations.
- [Earlier size-gradient proposal](proposals/polka-dot-size-gradient.md):
  superseded compact-layout exploration and payload size tradeoffs.
- [Pattern gallery](../../Test/Manual/PatternGallery.md): interactive parameter
  variations and a comparison of pattern size under zoom.
- [Pattern transforms](../../Test/Manual/PatternTransforms.md): attachment,
  continuity across primitives, and geometry versus coordinate-system movement.
- [RMS scaling check](../../Test/Manual/Renderer2DRMSScaling.md): tessellation and
  the backend-specific pattern behavior under nonuniform transforms.

Pending implementation stages are tracked in [TODO](../../TODO.md).
