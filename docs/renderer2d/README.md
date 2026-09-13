# Renderer2D

Renderer design notes and related checks:

- [Pattern coordinate design](pattern-coordinates.md): adopted coordinate model
  and the data path for making patterns follow local and camera transforms.
- [Pattern parameter payload](pattern-payload.md): four-vector packing, effect
  constant offsets, state tracking, and storage-only validation.
- [Windows payload handoff](d3d11-pattern-payload-handoff.md): implementation and
  verification instructions for the D3D11 storage extension.
- [Pattern expressiveness proposal](proposals/pattern-expressiveness.md):
  unadopted exploration of parameter units, additional motifs, and size gradients.
- [Polka-dot size gradient candidate](proposals/polka-dot-size-gradient.md):
  a 45-degree square lattice with a growing radius, independent transition-start
  control, and parameter payload size tradeoffs.
- [Pattern gallery](../../Test/Manual/PatternGallery.md): interactive parameter
  variations and a comparison of pattern size under zoom.
- [Pattern transforms](../../Test/Manual/PatternTransforms.md): attachment,
  continuity across primitives, and geometry versus coordinate-system movement.
- [RMS scaling check](../../Test/Manual/Renderer2DRMSScaling.md): tessellation and
  the backend-specific pattern behavior under nonuniform transforms.

Pending implementation stages are tracked in [TODO](../../TODO.md).
