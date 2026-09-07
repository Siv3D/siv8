# Mesh3DAssembly: parts, materials, and export

The [Assembly manual test](../../Test/Manual/Mesh3DAssemblyExamples.md) contains
complete programs and expected results for a colored robot, hinged chest, and
shared wheels. This guide describes the reusable modeling techniques. See the
[public header](../../Siv3D/include/Siv3D/Mesh3DAssembly.hpp) for exact contracts.

## Output and editing contracts

Use `assembly.saveOBJ(path)` for a one-off export. For repeated baking, keep a
`Mesh3DAssembly::BakedMesh`, call `assembly.bake(destination)`, and save it with
`destination.saveOBJ(path)`. Its `encodeOBJ(objWriter, mtlWriter, mtlFileName)`
provides the same data to two separate Writers without intermediate part meshes.

OBJ groups are `part_<ID>_<name>` and material identifiers are `material_<ID>_<name>`.
Names use reversible UTF-8 percent encoding outside ASCII letters, digits, `_`, `-`, and `.`;
empty names omit the final underscore and name. This is the exporter naming convention,
not an OBJ decoding feature. The ID prefix remains directly readable in external viewers.
The MTL filename is escaped by the same rule to avoid whitespace in `mtllib`.
Unassigned parts explicitly use the default material; unused registered materials are omitted.
OBJ stores baked geometry and materials; editing hierarchy and shared meshes remain in Assembly.

Parents precede children; transforms compose as `local * parentWorld`. Names may repeat;
IDs identify parts. Numeric placement preconditions are documented in Mesh3DAssembly.
The sample uses `Result::value()` to stop on a modeling error; applications needing recovery
should inspect the Result and its Mesh3DError. Save failures are returned as bool and logged.

## Frame alignment and linked hierarchy copies

`Mesh3DPlacement::Align(sourceFrame, targetFrame)` computes a placement that maps
an attachment frame in the part's local coordinates to a frame in the parent's
coordinates. It includes position, orientation, scale, and shear. Both frames
must be finite affine transforms, the source must be invertible, and its inverse
and the result must be finite and representable as float values. It is a one-time calculation; changing recipe dimensions
requires rebuilding the frames. In this recipe, the foot housing's bottom is
derived from `footSize.y`, keeping its mounting height fixed when the size changes.
The mounting plane intentionally lies slightly inside the sole.

`cloneSubtree(root, placement, parent)` copies the root and its descendants and
replaces the root's placement and parent. Omitting the parent places the new root
directly in Assembly coordinates. Mesh and material IDs stay shared; names and
local transforms are copied. The returned `ClonedSubtree::find(originalPartID)`
finds the new ID for editing a particular copied joint. Shared resource edits
propagate, while placement and later hierarchy edits remain independent.
The robot builds one arm hierarchy and uses this operation for the other arm.

The shared-resource behavior follows the familiar distinction described in
Blender's [Duplicate Linked](https://docs.blender.org/manual/en/4.1/scene_layout/object/editing/duplicate_linked.html).
Frame alignment is related to [Align to Transform Orientation](https://docs.blender.org/manual/en/5.2/scene_layout/object/editing/transform/align_transform_orientation.html),
but this API maps complete affine frames, including their origins, rather than
replicating Blender's rotation-only operator.

