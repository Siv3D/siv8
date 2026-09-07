# Mesh3D face materials: design study

Status: **deferred for reconsideration in v0.8.1 or later**. This is not a v0.8
implementation task or a committed release feature. v0.8 retains the current
single-material-per-Part contract. This study preserves the distinction between
assigning materials to triangles and generator-specific face roles.

## Problem and scope

A box with contrasting faces, a cylinder with painted ends, and an arbitrary
hand-authored mesh all need material assignment within one shared shape. Creating
extra overlapping plates changes the geometry and can introduce z-fighting.
Splitting each color into a separate registered mesh can lose useful shape and
part identity. Material assignment should not require either technique.

The initial scope is existing base-color Material values, CPU Assembly/bake,
and OBJ/MTL export. This is not vertex color, a Boolean operation, an editor
selection system, or a commitment to a future GPU draw API.

## Two layers

1. **Triangle assignment:** a shared shape optionally describes material slots;
   a Part binds those slots to the Assembly's MaterialIDs. Geometry remains shared
   when two Parts use different color schemes. A hand-authored mesh can provide
   the same assignment data as a generator.
2. **Face roles:** a generator optionally identifies meaningful subsets, such as
   a cylinder's side/start/end. Role-to-slot assignment happens explicitly.
   A global enum of every possible face name is unnecessary. Roles must not be
   inferred from vertex indices, triangle normals, UV seams, or mesh connectivity.

Names here describe concepts, not new C++ type names. Do not add a public slot,
selection, or role type until the representation and replacement rules below
are settled.

## Representations to compare

| Representation | Benefits | Costs / limitations |
| --- | --- | --- |
| Slot label per triangle | Direct arbitrary assignment; predictable traversal; easy editing | O(triangle count) labels even for a few small subsets; needs a no-assignment fast path |
| Sorted, non-overlapping triangle ranges with slot labels | Compact for generated caps/faces; natural export runs | Fragmented assignments can approach O(triangle count); requires a canonical merge/validation pass |
| Triangle index sets per slot | Convenient for authoring disconnected selections | Coverage/overlap checks and export traversal need additional work; poor canonical runtime format |

Start the comparison with **ranges as the stored candidate** and triangle sets
as an authoring input, not three public interchangeable containers. Retain the
dense-label alternative if fragmented data makes ranges worse. Measure bytes,
bake allocations, and iteration time before choosing. A uniform-material shape
must not allocate per-triangle assignment storage.

## Binding and editing decisions

- Keep shared geometry assignment separate from per-Part material bindings.
  Recoloring an instance must not change other instances or duplicate vertices.
- Preserve the existing Part.material behavior as the fallback for triangles
  without an explicit binding. No parent material inheritance is introduced.
  Distinguish “use fallback” from “explicitly unassigned” if both are supported;
  do not overload one invalid ID with both meanings.
- Reject overlapping or out-of-range assignments during construction/editing,
  before mutating existing data. Do not silently choose the last assignment.
- Face roles are stable semantic identifiers only where the generator defines
  them. A hand-authored group does not become topology-independent automatically.
- `setMesh()` needs an explicit contract. Reusing old ranges after triangle order
  or count changes is unsafe even when all indices remain in bounds. Candidate:
  replace mesh and assignment data together, then validate every referencing
  Part's slot bindings. Whether unmatched bindings cause failure or are explicitly
  cleared is a decision for API review; do not silently reinterpret them.
- A shape-specific geometry recipe may regenerate role assignments from its
  parameters. This provides stability across subdivision changes without
  promising persistent triangle identity across arbitrary mesh edits.

## Bake and OBJ

Keep one BakedPart per source Part, including its current ID, geometry range,
and world transform. Multiple materials must not split a logical Part into fake
Parts. An additional material-run sequence can describe the baked triangle
ranges without reordering or duplicating vertices. OBJ can change `usemtl` inside
one `g` group; keep names/IDs and fallback behavior consistent with current output.

A bake destination should reuse its material-run storage. Adjacent equivalent
runs may be merged, but part identity and original triangle order must remain
recoverable. The no-face-assignment case should retain the existing behavior.
Export and future rendering can consume the baked representation separately.

## Acceptance cases before implementation

1. Box: one contrasting face; unchanged geometry and one Part.
2. Cylinder: side and two ends; change radial subdivisions and verify that role
   assignment follows the generated end faces rather than old triangle numbers.
3. Hand-authored mesh: one material on disconnected triangles, including an
   intentionally fragmented assignment to evaluate range storage.
4. Two Parts sharing the same shape with different bindings; recolor one and
   confirm unchanged mesh storage and unchanged appearance of the other.
5. Replace geometry with reordered triangles and fewer slots. Verify the chosen
   failure/update contract, including no partial changes.
6. Bake repeatedly into one destination; export and parse material runs to check
   coverage, fallback, PartID preservation, no duplicate triangles, and MTL colors.

If this work is resumed for v0.8.1 or later, start with a small internal
representation experiment against these cases. Do not add generator role outputs,
Mesh3D fields, or public binding APIs before that evaluation.
