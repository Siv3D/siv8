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
- [Truchet](truchet.md): connected arcs, regular/random layouts, and seed encoding.
- [Truchet manual sample](../../Test/Manual/Truchet.md): three layouts, seeds,
  animation, and transforms.
- [D3D11 new-pattern handoff](d3d11-new-patterns.md): combined integration steps
  for Wave, Ripple, Weave, and Truchet.
- [Weave](weave.md): alternating crossings, filtered gaps, payload, and D3D11 port.
- [Weave manual sample](../../Test/Manual/Weave.md): fabric and basket presets,
  animation, and transform controls.
- [Ripple](ripple.md): concentric bands, center filtering, payload, and D3D11 port.
- [Ripple manual sample](../../Test/Manual/Ripple.md): off-card centers, layered
  circles, radial motion, and transform controls.
- [Wave](wave.md): sine-wave width approximation, payload, Metal integration,
  and D3D11 port checklist.
- [Wave manual sample](../../Test/Manual/Wave.md): presets, animation, and
  transform controls.
- [Halftone](halftone.md): adopted API, coordinate
  field, packing, fragment filtering, and validation.
- [D3D11 Halftone integration](d3d11-halftone.md):
  shader loading and Windows verification.
- [Halftone manual sample](../../Test/Manual/Halftone.md): presets,
  independent field/lattice controls, and object transformations.
- [Halftone button gallery](../../Test/Manual/HalftoneButtonGallery.md): eight
  background recipes, transparent overlays, and optional lattice motion.
- [Earlier size-gradient proposal](proposals/halftone.md):
  superseded compact-layout exploration and payload size tradeoffs.
- [Pattern collection](../../Test/Manual/PatternCollection.md): a minimal
  1680 x 1050 gallery of 32 backgrounds from seven Pattern types, with
  keyboard-controlled animation and layered combinations.
- [Pattern gallery](../../Test/Manual/PatternGallery.md): interactive parameter
  variations and a comparison of pattern size under zoom.
- [Pattern transforms](../../Test/Manual/PatternTransforms.md): attachment,
  continuity across primitives, and geometry versus coordinate-system movement.
- [RMS scaling check](../../Test/Manual/Renderer2DRMSScaling.md): tessellation and
  the backend-specific pattern behavior under nonuniform transforms.

Pending implementation stages are tracked in [TODO](../../TODO.md).
