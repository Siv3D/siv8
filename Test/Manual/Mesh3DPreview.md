# Mesh3D OBJ preview

This standalone Python tool previews the triangular OBJ + MTL files exported by
Mesh3D and Mesh3DAssembly. It contains no completed model recipes. Use it to check
placement, silhouettes, smooth and hard edges, and base colors before reviewing
models in a renderer.

## Execution

Requirements: Python 3, NumPy, and Pillow. Run from the repository root. Extract
the complete program below into a temporary file, then run its independent checks:

```sh
python3 - <<'PY'
from pathlib import Path
source = Path('Test/Manual/Mesh3DPreview.md').read_text().split('```python\n', 1)[1].split('\n```', 1)[0]
Path('/tmp/mesh3d_preview.py').write_text(source + '\n')
PY
python3 /tmp/mesh3d_preview.py --self-test
python3 /tmp/mesh3d_preview.py path/to/model.obj --output /tmp/model-preview
python3 /tmp/mesh3d_preview.py path/to/model.obj --output /tmp/model-colors --unlit
```

The tool writes front, back, left, right, top, and isometric PNGs. All views use
the same scale and center. Front faces Siv3D's +Z side, which becomes OBJ -Z.
The top camera is above +Y. Positive elevation always puts the camera above the
model. `--size 1024` increases image resolution.

## Close-ups and part inspection

Assembly group IDs are preserved even when different parts have identical names.
List IDs, decoded names, bounds, counts, and colors before selecting parts:

```sh
python3 /tmp/mesh3d_preview.py path/to/model.obj --list-parts
python3 /tmp/mesh3d_preview.py path/to/model.obj --part 12 --part 18 --focus --highlight --zoom 0.7 --view iso --output /tmp/part-context
python3 /tmp/mesh3d_preview.py path/to/model.obj --part 12 --isolate --view iso --output /tmp/part-alone
python3 /tmp/mesh3d_preview.py path/to/model.obj --focus-box -2 0 -3 2 4 3 --view front --output /tmp/region-context
python3 /tmp/mesh3d_preview.py path/to/model.obj --part-colors --output /tmp/part-colors
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

## Complete program

```python
import argparse
import colorsys
import json
import re
from urllib.parse import unquote
from pathlib import Path
import tempfile

import numpy as np
from PIL import Image


def unit(v):
    return v / np.maximum(np.linalg.norm(v, axis=-1, keepdims=True), 1e-30)


def base_color(material):
    kd = np.array(material.get('Kd', [0.75, 0.75, 0.75]))
    if 'Pm' in material and 'Ks' in material:
        m = np.clip(material['Pm'][0], 0, 1)
        kd = kd + material['Ks'] - 0.04 * (1 - m)
    return np.clip(kd, 0, 1)


def read_obj(path, with_parts=False):
    vertices, normals, triangles, normal_indices, materials = [], [], [], [], []
    palette, current = {}, None
    part_id = -1  # A plain Mesh3D OBJ has no Assembly PartID.
    parts = {-1: dict(id=None, name='', group='', faces=[])}
    face_parts = []
    for raw in path.read_text(encoding='utf-8-sig').splitlines():
        fields = raw.split()
        if not fields or fields[0].startswith('#'):
            continue
        key, values = fields[0], fields[1:]
        if key in ('v', 'vn'):
            if len(values) != 3:
                raise ValueError('Expected three components for ' + key)
            (vertices if key == 'v' else normals).append(list(map(float, values)))
        elif key == 'mtllib':
            material = None
            mtl_path = path.parent / raw.split(maxsplit=1)[1].strip()
            for line in mtl_path.read_text(encoding='utf-8-sig').splitlines():
                items = line.split()
                if not items:
                    continue
                if items[0] == 'newmtl':
                    material = {}
                    palette[line.split(maxsplit=1)[1].strip()] = material
                elif material is not None and items[0] in ('Kd', 'Ks', 'Pm'):
                    count = 1 if items[0] == 'Pm' else 3
                    if len(items[1:]) != count:
                        raise ValueError('Invalid MTL component count')
                    material[items[0]] = np.array(list(map(float, items[1:])))
        elif key == 'g':
            group = raw.split(maxsplit=1)[1].strip()
            match = re.fullmatch(r'part_(\d+)(?:_(.*))?', group)
            if match is None:
                raise ValueError('Expected a Siv3D Assembly group: ' + group)
            part_id = int(match[1])
            if part_id in parts and parts[part_id]['group'] != group:
                raise ValueError('Conflicting names for PartID ' + str(part_id))
            parts.setdefault(part_id, dict(id=part_id, name=unquote(match[2] or ''), group=group, faces=[]))
        elif key == 'usemtl':
            current = raw.split(maxsplit=1)[1].strip()
        elif key == 'f':
            if len(values) != 3:
                raise ValueError('Only triangular OBJ faces are supported')
            face, ns = [], []
            for value in values:
                fields = value.split('/')
                if len(fields) != 3 or not fields[2]:
                    raise ValueError('Each corner requires a normal index')
                face.append(int(fields[0]) - 1)
                ns.append(int(fields[2]) - 1)
            parts[part_id]['faces'].append(len(triangles))
            face_parts.append(part_id)
            triangles.append(face)
            normal_indices.append(ns)
            materials.append(current)
    v, n = np.array(vertices, dtype=float), np.array(normals, dtype=float)
    t, ni = np.array(triangles, dtype=int), np.array(normal_indices, dtype=int)
    if not (len(v) and len(n) and len(t)):
        raise ValueError('Empty geometry or missing normals')
    if not (np.isfinite(v).all() and np.isfinite(n).all()):
        raise ValueError('Non-finite geometry')
    if (np.linalg.norm(n, axis=1) == 0).any():
        raise ValueError('Zero-length normal')
    if t.min() < 0 or t.max() >= len(v) or ni.min() < 0 or ni.max() >= len(n):
        raise ValueError('Invalid or unsupported OBJ index')
    for name in materials:
        if name is not None and name not in palette:
            raise ValueError('Missing material: ' + name)
    colors = np.array([base_color(palette.get(name, {})) for name in materials])
    if not np.isfinite(colors).all():
        raise ValueError('Non-finite material color')
    model = (v, t, unit(n)[ni], colors)
    if not with_parts:
        return model
    metadata = []
    for key, part in sorted(parts.items()):
        if not part['faces']:
            continue
        used = np.unique(t[part['faces']])
        positions = v[used] * [1, 1, -1]  # Report bounds in Siv3D coordinates.
        metadata.append(dict(id=part['id'], name=part['name'], group=part['group'],
                             vertices=len(used), triangles=len(part['faces']),
                             min=positions.min(axis=0).tolist(), max=positions.max(axis=0).tolist(),
                             color=part_color(key).tolist()))
    return model, np.array(face_parts, dtype=np.int64), metadata


def part_color(part_id):
    # Stable across selection and draw order. IDs, not names, identify parts.
    if part_id < 0:
        return np.array([160, 160, 160], dtype=np.uint8)
    rgb = colorsys.hsv_to_rgb((part_id * 0.61803398875) % 1, 0.65, 0.95)
    return np.round(np.array(rgb) * 255).astype(np.uint8)


def select_view(model, face_parts, selected=(), focus=False, focus_box=None,
                isolate=False, highlight=False, part_colors=False):
    vertices, triangles, normals, colors = model
    selected = set(selected)
    missing = selected - set(face_parts.tolist())
    if missing:
        raise ValueError('Unknown PartID: ' + ', '.join(map(str, sorted(missing))))
    if (focus or isolate or highlight) and not selected:
        raise ValueError('--focus, --isolate and --highlight require --part')
    if focus and focus_box is not None:
        raise ValueError('Choose --focus or --focus-box')
    mask = np.isin(face_parts, list(selected))
    bounds = None
    if focus or isolate:
        points = vertices[np.unique(triangles[mask])]
        bounds = (points.min(axis=0), points.max(axis=0))
    if focus_box is not None:
        box = np.asarray(focus_box, dtype=float)
        if box.shape != (6,) or not np.isfinite(box).all() or (box[:3] >= box[3:]).any():
            raise ValueError('--focus-box needs finite MIN < MAX on each axis')
        bounds = (box[[0, 1, 5]] * [1, 1, -1], box[[3, 4, 2]] * [1, 1, -1])
    if part_colors:
        palette = {key: part_color(int(key)) / 255 for key in np.unique(face_parts)}
        colors = np.array([palette[key] for key in face_parts])
    if highlight:
        colors = colors.copy()
        colors[~mask] = colors[~mask] * 0.2 + 0.65
        colors[mask] = [1., 0.4, 0.05]
    if isolate:
        triangles, normals, colors = triangles[mask], normals[mask], colors[mask]
    # Focus only changes the camera bounds. All triangles still occlude normally.
    return (vertices, triangles, normals, colors), bounds


def camera(azimuth, elevation):
    az, el = np.radians([azimuth, elevation])
    # Toward points from the model to the camera. Larger depth is nearer.
    toward = np.array([np.cos(el) * np.sin(az), np.sin(el), np.cos(el) * np.cos(az)])
    reference_up = np.array([0., 0., -1.]) if abs(toward[1]) > 0.999 else np.array([0., 1., 0.])
    right = unit(np.cross(reference_up, toward))
    up = np.cross(toward, right)
    return right, up, toward


def render(model, azimuth, elevation, size=640, unlit=False, bounds=None, zoom=1.0):
    vertices, triangles, normals, colors = model
    right, up, toward = camera(azimuth, elevation)
    if not np.isfinite(zoom) or zoom <= 0:
        raise ValueError('Zoom must be positive and finite')
    low, high = (vertices.min(axis=0), vertices.max(axis=0)) if bounds is None else bounds
    center = (low + high) / 2
    diameter = np.linalg.norm(high - low)
    if diameter == 0:
        raise ValueError('Zero-size model')
    relative = vertices - center
    scale = size * 0.88 * zoom / diameter
    xy = np.column_stack((relative @ right, -(relative @ up))) * scale + size / 2
    depths = relative @ toward
    pixels = np.full((size, size, 3), 0.94)
    zbuffer = np.full((size, size), -np.inf)
    light = unit(toward + up + right * 0.4)
    for index, ids in enumerate(triangles):
        p = vertices[ids]
        geometric_normal = np.cross(p[1] - p[0], p[2] - p[0])
        if geometric_normal @ toward <= 0:
            continue
        a, b, c = xy[ids]
        area = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])
        if abs(area) < 1e-12:
            continue
        xmin, ymin = np.maximum(np.floor(xy[ids].min(axis=0)).astype(int), 0)
        xmax, ymax = np.minimum(np.ceil(xy[ids].max(axis=0)).astype(int), size - 1)
        if xmin > xmax or ymin > ymax:
            continue
        x, y = np.meshgrid(np.arange(xmin, xmax + 1) + 0.5, np.arange(ymin, ymax + 1) + 0.5)
        def edge(u, v):
            return (v[0] - u[0]) * (y - u[1]) - (v[1] - u[1]) * (x - u[0])
        weights = np.stack((edge(b, c), edge(c, a), edge(a, b)), axis=-1) / area
        inside = (weights >= -1e-10).all(axis=-1)
        depth = weights @ depths[ids]
        region = zbuffer[ymin:ymax + 1, xmin:xmax + 1]
        visible = inside & (depth > region)
        if not visible.any():
            continue
        region[visible] = depth[visible]
        color = np.broadcast_to(colors[index], (*depth.shape, 3))
        if not unlit:
            shading_normal = unit(weights @ normals[index])
            shade = 0.3 + 0.7 * np.maximum(shading_normal @ light, 0)
            color = color * shade[..., None]
        pixels[ymin:ymax + 1, xmin:xmax + 1][visible] = color[visible]
    return np.round(np.clip(pixels, 0, 1) * 255).astype(np.uint8)


def self_test():
    vertices, triangles, normals, colors = [], [], [], []
    palette = np.array([[1, 0, 0], [0, 1, 0], [0, 0, 1], [1, 1, 0], [1, 0, 1], [0, 1, 1]])
    # Closed cube with a different color on each outward-facing side.
    for axis in range(3):
        for sign in (1, -1):
            n = np.eye(3)[axis] * sign
            u = np.eye(3)[(axis + 1) % 3] * 0.5
            v = np.cross(n, u)
            offset = len(vertices)
            vertices.extend([n * 0.5 - u - v, n * 0.5 + u - v, n * 0.5 + u + v, n * 0.5 - u + v])
            triangles.extend([[offset, offset + 1, offset + 2], [offset, offset + 2, offset + 3]])
            normals.extend([[n] * 3, [n] * 3])
            colors.extend([palette[len(colors) // 2]] * 2)
    cube = tuple(np.array(x) for x in (vertices, triangles, normals, colors))
    for i, (az, el) in enumerate(((90, 0), (-90, 0), (0, 90), (0, -90), (0, 0), (180, 0))):
        result = render(cube, az, el, 64, True)
        np.testing.assert_array_equal(result[32, 32], palette[i] * 255)
        reverse_order = (cube[0], cube[1][::-1], cube[2][::-1], cube[3][::-1])
        np.testing.assert_array_equal(render(reverse_order, az, el, 64, True), result)
    # Two equally oriented, overlapping planes: the nearer one must win,
    # even when it is submitted first. Culling alone cannot pass this test.
    p = np.array([[-1., -1., 0.], [1., -1., 0.], [0., 1., 0.]])
    planes = (np.concatenate((p, p + [0, 0, 0.5])), np.array([[0, 1, 2], [3, 4, 5]]),
              np.broadcast_to([0., 0., 1.], (2, 3, 3)), np.array([[1., 0., 0.], [0., 1., 0.]]))
    for order in ([0, 1], [1, 0]):
        ordered = (planes[0], planes[1][order], planes[2][order], planes[3][order])
        np.testing.assert_array_equal(render(ordered, 0, 0, 64, True)[32, 32], [0, 255, 0])
    varying = (p, np.array([[0, 1, 2]]), unit(np.array([[[0., 0., 1.], [1., 0., 1.], [0., 1., 1.]]])), np.ones((1, 3)))
    shaded = render(varying, 0, 0, 64)
    assert not np.array_equal(shaded[36, 28], shaded[36, 36])
    # Isolated front face: reverse its winding and it must disappear.
    face = (cube[0], cube[1][8:10], cube[2][8:10], cube[3][8:10])
    inverted = (face[0], face[1][:, ::-1], face[2][:, ::-1], face[3])
    assert (render(inverted, 0, 0, 64, True) == 240).all()
    tilted = (face[0], face[1], np.broadcast_to(unit(np.array([1., 0., 1.])), face[2].shape), face[3])
    assert not np.array_equal(render(face, 0, 0, 64), render(tilted, 0, 0, 64))
    for m in (0., 0.5, 1.):
        base = np.array([0.2, 0.4, 0.8])
        np.testing.assert_allclose(base_color({'Kd': base * (1 - m), 'Ks': base * m + 0.04 * (1 - m), 'Pm': [m]}), base)
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory) / 'triangle.obj'
        path.write_text('mtllib triangle.mtl\nv -1 -1 0\nv 1 -1 0\nv 0 1 0\nvn 0 0 1\nusemtl paint\nf 1//1 2//1 3//1\n')
        path.with_suffix('.mtl').write_text('newmtl paint\nKd 0.1 0.2 0.4\nKs 0.12 0.22 0.42\nPm 0.5\n')
        model = read_obj(path)
        np.testing.assert_allclose(model[3][0], [0.2, 0.4, 0.8])
        assert not np.array_equal(render(model, 0, 0, 64), render(model, 180, 0, 64))
        path.write_text(path.read_text().replace('3//1', '0//1'))
        try:
            read_obj(path)
        except ValueError:
            pass
        else:
            raise AssertionError('Invalid index accepted')
    # Focus is a camera operation: geometry in front still occludes the target,
    # including geometry outside the focus box's depth interval.
    ids = np.array([7, 9])
    focused, bounds = select_view(planes, ids, [7], focus=True)
    np.testing.assert_array_equal(focused[1], planes[1])
    np.testing.assert_array_equal(render(focused, 0, 0, 64, True, bounds)[32, 32], [0, 255, 0])
    isolated, bounds = select_view(planes, ids, [7], isolate=True)
    np.testing.assert_array_equal(render(isolated, 0, 0, 64, True, bounds)[32, 32], [255, 0, 0])
    framed, bounds = select_view(planes, ids, focus_box=[-1, -1, -0.1, 1, 1, 0.1])
    np.testing.assert_array_equal(render(framed, 0, 0, 64, True, bounds)[32, 32], [0, 255, 0])
    _, bounds = select_view(planes, ids, focus_box=[-1, -2, 3, 2, 5, 8])
    np.testing.assert_array_equal(bounds[0], [-1, -2, -8])
    np.testing.assert_array_equal(bounds[1], [2, 5, -3])
    colored, _ = select_view(planes, ids, part_colors=True)
    np.testing.assert_array_equal(render(colored, 0, 0, 64, True)[32, 32], part_color(9))
    highlighted, _ = select_view(planes, ids, [9], highlight=True)
    np.testing.assert_allclose(highlighted[3][1], [1, 0.4, 0.05])
    np.testing.assert_array_equal(planes[3][1], [0, 1, 0])  # Source stays unchanged.
    normal_size = render(planes, 0, 0, 64, True)
    zoomed = render(planes, 0, 0, 64, True, zoom=2)
    assert (zoomed != 240).any(axis=-1).sum() > (normal_size != 240).any(axis=-1).sum()
    for kwargs in (dict(selected=[99]), dict(focus=True), dict(isolate=True), dict(highlight=True),
                   dict(focus_box=[0, 0, 0, 0, 1, 1]), dict(focus_box=[0, 0, 0, 1, 1, np.nan]),
                   dict(selected=[7], focus=True, focus_box=[0, 0, 0, 1, 1, 1])):
        try:
            select_view(planes, ids, **kwargs)
        except ValueError:
            pass
        else:
            raise AssertionError('Invalid selection accepted: ' + str(kwargs))
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory) / 'groups.obj'
        # Identical names must not merge distinct IDs. Non-ASCII names are decoded.
        group = 'same%20%E6%9F%B1'
        path.write_text('v -1 -1 0\nv 1 -1 0\nv 0 1 0\nvn 0 0 1\n'
                        + f'g part_7_{group}\nf 1//1 2//1 3//1\ng part_9_{group}\nf 1//1 2//1 3//1\n')
        _, actual_ids, metadata = read_obj(path, with_parts=True)
        np.testing.assert_array_equal(actual_ids, [7, 9])
        assert [m['id'] for m in metadata] == [7, 9]
        assert [m['name'] for m in metadata] == ['same 柱', 'same 柱']
        assert [m['triangles'] for m in metadata] == [1, 1]
        assert [m['vertices'] for m in metadata] == [3, 3]
        np.testing.assert_array_equal(metadata[0]['color'], part_color(7))
    print('Preview self-tests passed')


def main():
    parser = argparse.ArgumentParser(description='Preview Siv3D OBJ geometry and Assembly parts')
    parser.add_argument('obj', type=Path, nargs='?')
    parser.add_argument('--output', type=Path, default=Path('/tmp/mesh3d-preview'))
    parser.add_argument('--size', type=int, default=640)
    parser.add_argument('--unlit', action='store_true')
    parser.add_argument('--self-test', action='store_true')
    parser.add_argument('--part', type=int, action='append', default=[], help='Select a PartID; repeatable')
    parser.add_argument('--focus', action='store_true', help='Frame selected parts while keeping surrounding geometry')
    parser.add_argument('--focus-box', type=float, nargs=6, metavar=('XMIN', 'YMIN', 'ZMIN', 'XMAX', 'YMAX', 'ZMAX'),
                        help='Frame this box in Siv3D coordinates; retain all geometry')
    parser.add_argument('--zoom', type=float, default=1.0, help='Magnification; use less than 1 for more context')
    parser.add_argument('--isolate', action='store_true', help='Draw only selected parts and frame them')
    parser.add_argument('--highlight', action='store_true', help='Show selected parts in orange, retaining occlusion')
    parser.add_argument('--part-colors', action='store_true', help='Use unlit ID colors matching the JSON legend')
    parser.add_argument('--list-parts', action='store_true', help='Print the part legend without rendering')
    views = dict(front=(180, 0), back=(0, 0), left=(-90, 0), right=(90, 0), top=(0, 90), iso=(140, 25))
    parser.add_argument('--view', choices=views, action='append', help='Choose views; repeatable, default all six')
    args = parser.parse_args()
    if args.self_test:
        self_test()
        return
    if args.obj is None or args.size < 16:
        parser.error('Specify an OBJ file and a size of at least 16')
    try:
        model, face_parts, metadata = read_obj(args.obj, with_parts=True)
        selected, bounds = select_view(model, face_parts, args.part, args.focus, args.focus_box,
                                       args.isolate, args.highlight, args.part_colors)
        if args.highlight and args.part_colors:
            raise ValueError('Choose --highlight or --part-colors')
        if not np.isfinite(args.zoom) or args.zoom <= 0:
            raise ValueError('Zoom must be positive and finite')
    except (ValueError, OSError) as error:
        parser.error(str(error))
    legend = json.dumps(dict(coordinates='Siv3D (+Y up, OBJ Z negated)', colors='unlit RGB 0..255',
                             parts=metadata), ensure_ascii=False, indent=2)
    if args.list_parts:
        print(legend)
        return
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / f'{args.obj.stem}_parts.json').write_text(legend + '\n', encoding='utf-8', newline='\n')
    for name in args.view or views:
        az, el = views[name]
        pixels = render(selected, az, el, args.size, args.unlit or args.part_colors, bounds, args.zoom)
        Image.fromarray(pixels).save(args.output / f'{args.obj.stem}_{name}.png')
    print(f'{len(model[0])} vertices, {len(selected[1])} visible-set triangles; wrote views and part legend to {args.output}')


if __name__ == '__main__':
    main()
```
