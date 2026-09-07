# Mesh3D OBJ preview

This standalone Python tool previews the triangular OBJ + MTL files exported by
Mesh3D and Mesh3DAssembly. It contains no completed model recipes. Use it to check
placement, silhouettes, smooth and hard edges, and base colors before reviewing
models in a renderer.

## Execution

Requirements: Python 3, NumPy, and Pillow. Run from the repository root.
The executable source is [tools/mesh3d_preview.py](../../tools/mesh3d_preview.py).

```sh
python3 tools/mesh3d_preview.py --self-test
python3 tools/mesh3d_preview.py path/to/model.obj --output /tmp/model-preview
python3 tools/mesh3d_preview.py path/to/model.obj --output /tmp/model-colors --unlit
```

The tool writes front, back, left, right, top, and isometric PNGs. All views use
the same scale and center. Front faces Siv3D's +Z side, which becomes OBJ -Z.
The top camera is above +Y. Positive elevation always puts the camera above the
model. `--size 1024` increases image resolution.

## Close-ups and part inspection

Assembly group IDs are preserved even when different parts have identical names.
List IDs, decoded names, bounds, counts, and colors before selecting parts:

```sh
python3 tools/mesh3d_preview.py path/to/model.obj --list-parts
python3 tools/mesh3d_preview.py path/to/model.obj --part 12 --part 18 --focus --highlight --zoom 0.7 --view iso --output /tmp/part-context
python3 tools/mesh3d_preview.py path/to/model.obj --part 12 --isolate --view iso --output /tmp/part-alone
python3 tools/mesh3d_preview.py path/to/model.obj --focus-box -2 0 -3 2 4 3 --view front --output /tmp/region-context
python3 tools/mesh3d_preview.py path/to/model.obj --part-colors --output /tmp/part-colors
```

- `--part ID` selects a part; repeat it to select several. A missing ID is an
  error. Shape-free pivots do not have OBJ faces and cannot be selected.
- `--focus` frames the selected parts while drawing the entire model. Adjacent
  and foreground geometry still occludes the selected parts. This is useful for
  inspecting support and connections. It does not clip depth or cut the mesh.
- `--focus-box XMIN YMIN ZMIN XMAX YMAX ZMAX` frames a region in **Siv3D
  coordinates**, converting Z to OBJ coordinates internally. All geometry remains.
- `--zoom` magnifies the chosen framing. Use values below 1 to show more context.
- `--highlight` colors the selection orange and mutes surrounding colors. It
  preserves occlusion; a hidden selected part remains hidden.
- `--isolate` explicitly removes other parts from the view and frames the
  selection. It does not modify the OBJ. Combine with `--focus-box` if needed.
- `--part-colors` uses stable, unlit ID colors. The accompanying
  `<model>_parts.json` maps each RGB color to its PartID and decoded name, together
  with vertex/triangle counts and bounds in Siv3D coordinates. IDs remain the
  authoritative identity; colors of different parts may look similar.
- `--view` selects one of front/back/left/right/top/iso; repeat for multiple
  views. The default is all six. Output filenames depend on the model and view;
  use separate output directories to keep different inspection modes.

A plain Mesh3D OBJ without groups can use region focus and zoom; its legend has
one entry with a null ID. Group selection requires Assembly exports.

## Expected results and limits

- Self-tests cover all six axis directions, depth ordering independent of draw
  order, backface rejection, interpolated vertex normals, OBJ parsing, and
  reconstruction of Siv3D base colors at metallic values 0, 0.5, and 1. They also
  cover focused-view occlusion, isolation, zoom, bounds conversion, duplicate
  names with different IDs, percent decoding, and invalid selections.
- A top view shows upper surfaces. Near surfaces occlude far surfaces. Reversed
  triangles are culled. Smooth vertex normals are interpolated and normalized.
- Colors are reconstructed from Siv3D's Kd, Ks, and Pm export values. An MTL
  without Pm uses Kd. Unlit mode displays these colors without shading.
- This is an orthographic diagnostic preview, not a PBR renderer. It does not
  simulate textures, transparency, emission, shadows, or metallic reflections.
  Shaded mode uses a simple camera-relative light to reveal shape. It does not
  implement a color-management pipeline. Do not use it to judge final lighting.
- Input is restricted to finite vertices and nonzero normals, triangular faces
  with positive position/normal indices, and a single local MTL file as emitted
  by Siv3D. Unsupported face formats fail explicitly. Degenerate faces are
  skipped for rendering; a successful preview is not a geometry validity test.
- A preview cannot prove intended contact, support, clearance, or absence of
  coplanar overlap. Check those against the model's dimensions and purpose.

