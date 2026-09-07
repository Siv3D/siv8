# Mesh3D

v0.8 is stabilizing the CPU modeling APIs: shape generation, editing, assembly,
and OBJ/MTL output. Drawing is outside this scope. Current models use multiple
parts with one material per part; new face-material APIs are deferred.

## Using the APIs

- [Modeling](modeling.md): choosing existing APIs, preparing contours, and returning attachment frames.
- [Assembly and export](assembly.md): shared shapes, materials, hierarchy, frame alignment, and OBJ.
- [Loft](loft.md): contour correspondence, section frames, and migration notes.
- [Preview](preview.md): orthographic inspection, part selection, and tool limitations.

Exact contracts live in the [Mesh3D](../../Siv3D/include/Siv3D/Mesh3D.hpp),
[Mesh3DBuilder](../../Siv3D/include/Siv3D/Mesh3DBuilder.hpp),
[Mesh3DPlacement](../../Siv3D/include/Siv3D/Mesh3DPlacement.hpp), and
[Mesh3DAssembly](../../Siv3D/include/Siv3D/Mesh3DAssembly.hpp) public headers.

## Developing the implementation

- [Implementation guidance](implementation.md): architecture, conventions, and the v0.8 scope.
- [Common workflow](../development/README.md) and [unfinished work](../../TODO.md).

## Executable manual tests

- [Assembly](../../Test/Manual/Mesh3DAssemblyExamples.md)
- [Loft](../../Test/Manual/Mesh3DLoftExamples.md)
- [Attachment frames and open grids](../../Test/Manual/Mesh3DModelingStudies.md)

These documents own execution steps, expected results, and complete test programs.

## Deferred design studies

- [Face materials](proposals/face-materials.md): reconsider in v0.8.1 or later.
- [Open grid surfaces](proposals/open-grid-surface.md): sample-only experiment, not a public API.
