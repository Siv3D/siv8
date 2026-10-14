#!/usr/bin/env python3
"""Build a local report from BezierTessellation study stdout (no engine execution).

Usage: python3 tools/bezier_tessellation_report.py LOG [LOG ...] --output DIRECTORY
Requires matplotlib for scientific plots. Images are native engine PNG captures
encoded in the log; this tool decodes them without modifying their pixels.
"""

import argparse
import base64
import csv
import html
import json
from pathlib import Path
import statistics

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


SCHEMAS = {
    "geometry": "case method ordinary degree tolerance segments error limited vertices1 triangles1 vertices6 triangles6 vertices24 triangles24",
    "temporal": "tolerance animation method frame segments residual",
    "random": "case method degree segments error",
    "benchmark": "degree workload method tolerance median p10 p90",
    "raster": "case method width tolerance difference changed",
    "submission": "method draws median p10 p90 triangles",
}
METHODS = ["current", "current-deep", "wang", "density"]
COLORS = ["#ce7137", "#c4a838", "#477bc7", "#22957d"]


def parse(paths, output):
    rows = {name: [] for name in SCHEMAS}
    images = []
    for path in paths:
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            if line.startswith("BSTUDY\t"):
                _, kind, *values = line.split("\t")
                keys = SCHEMAS[kind].split()
                if len(keys) != len(values):
                    raise ValueError(f"Wrong field count in {path}: {kind}")
                rows[kind].append({key: value if key in {"case", "method"} else float(value)
                                   for key, value in zip(keys, values)})
            elif line.startswith("BSTUDY_IMAGE\t"):
                _, name, payload = line.split("\t", 2)
                if not name or any(c not in "abcdefghijklmnopqrstuvwxyz0123456789-._" for c in name):
                    raise ValueError(f"Invalid capture name: {name!r}")
                filename = f"{name}.png"
                (output / filename).write_bytes(base64.b64decode(payload, validate=True))
                images.append(filename)
    return rows, sorted(set(images))


def plot_geometry(rows, output):
    fig, axes = plt.subplots(1, 3, figsize=(13, 4))
    for method, color in zip(METHODS, COLORS):
        data = [x for x in rows if x["method"] == method and x["ordinary"]]
        tolerances = sorted({x["tolerance"] for x in data})
        for ax, key in zip(axes, ["segments", "vertices6", "error"]):
            values = [statistics.mean(x[key] for x in data if x["tolerance"] == t) for t in tolerances]
            ax.plot(tolerances, values, "o-", color=color, label=method)
            ax.set_xlabel("Tolerance (pixels)")
            ax.grid(alpha=.2)
    for ax, title in zip(axes, ["Mean segments", "Mean GPU vertices (6px, round caps)", "Mean sampled centerline error (px)"]):
        ax.set_title(title, fontsize=10)
    axes[0].legend(fontsize=8)
    fig.suptitle("12 ordinary curves, identical tolerance for each method")
    fig.tight_layout()
    fig.savefig(output / "geometry.png", dpi=160)
    plt.close(fig)


def plot_temporal(rows, output):
    titles = ["Quadratic shallow arch", "Same arch as cubic", "Cubic S", "Near a cubic cusp", "Quadratic through collinear reversal", "Quadratic near a straight chord", "Cubic near a straight chord"]
    fig, axes = plt.subplots(len(titles), 2, figsize=(12, 3 * len(titles)))
    for animation in range(len(titles)):
        for method, color in zip(METHODS, COLORS):
            data = [x for x in rows if x["animation"] == animation and x["method"] == method and x["tolerance"] == 0.5]
            axes[animation, 0].plot([x["frame"] for x in data], [x["segments"] for x in data], color=color, label=method)
            axes[animation, 1].plot([x["frame"] for x in data], [x["residual"] for x in data], color=color)
        axes[animation, 0].set_ylabel(titles[animation], fontsize=9)
        for ax in axes[animation]:
            ax.grid(alpha=.2)
    axes[0, 0].set_title("Segment count (tolerance 0.5 px)")
    axes[0, 1].set_title("Max change in approximation residual (px)")
    axes[0, 0].legend(fontsize=8)
    for ax in axes[-1]:
        ax.set_xlabel("Deformation step (1000 equally spaced steps)")
    fig.tight_layout()
    fig.savefig(output / "temporal.png", dpi=160)
    plt.close(fig)


def plot_bench(rows, output):
    fig, axes = plt.subplots(2, 3, figsize=(13, 8))
    for degree, row in zip([2, 3], axes):
        for workload, ax in enumerate(row):
            for method, color in zip(METHODS, COLORS):
                data = sorted([x for x in rows if x["degree"] == degree and x["workload"] == workload and x["method"] == method], key=lambda x: x["tolerance"])
                ax.errorbar([x["tolerance"] for x in data], [x["median"] for x in data],
                            yerr=[[x["median"] - x["p10"] for x in data], [x["p90"] - x["median"] for x in data]],
                            color=color, label=method, marker="o", capsize=3)
            ax.set_xlabel("Tolerance (pixels)")
            ax.set_ylabel(f"Bezier{degree}: CPU ns / curve")
            ax.grid(alpha=.2)
    for ax, title in zip(axes[0], ["Fresh output", "Reusable output*", "Reusable output + stroke mesh*"]):
        ax.set_title(title, fontsize=10)
    axes[0, 0].legend(fontsize=8)
    fig.suptitle("96 ordinary deformations per degree (median, p10-p90); *current APIs still allocate")
    fig.tight_layout()
    fig.savefig(output / "benchmark.png", dpi=160)
    plt.close(fig)


def table(rows, keys):
    def cell(value):
        return f"{value:.3f}" if isinstance(value, float) else str(value)
    return "<table><thead><tr>" + "".join(f"<th>{html.escape(x)}</th>" for x in keys) + "</tr></thead><tbody>" + "".join(
        "<tr>" + "".join(f"<td>{html.escape(cell(row[k]))}</td>" for k in keys) + "</tr>" for row in rows) + "</tbody></table>"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("logs", type=Path, nargs="+")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    rows, images = parse(args.logs, args.output)
    (args.output / "data.json").write_text(json.dumps(rows, indent=2) + "\n", encoding="utf-8")
    for kind, data in rows.items():
        with (args.output / f"{kind}.csv").open("w", newline="", encoding="utf-8") as stream:
            writer = csv.DictWriter(stream, SCHEMAS[kind].split())
            writer.writeheader()
            writer.writerows(data)
    for kind, function in [("geometry", plot_geometry), ("temporal", plot_temporal), ("benchmark", plot_bench)]:
        if rows[kind]:
            function(rows[kind], args.output)
    body = "<h1>Bezier tessellation comparison</h1><p>Density is the adopted Bezier2/Bezier3 drawAdaptive implementation. Current/current-deep retain the legacy recursive conversion as comparison baselines; Wang is the uniform-parameter alternative. Native screenshots use the existing Siv3D stroke renderer and 4x MSAA. The reference is a 4096-segment polyline through the same renderer, not an exact analytic stroke. CPU timings do not measure GPU execution time.</p>"
    for name in ["geometry", "temporal", "benchmark"]:
        if rows[name]:
            body += f'<h2>{name.title()}</h2><img src="{name}.png" alt="{name} plots">'
    if rows["benchmark"]:
        body += "<details><summary>CPU timing table (ns / curve)</summary>" + table(rows["benchmark"], SCHEMAS["benchmark"].split()) + "</details>"
    if rows["submission"]:
        body += "<h2>Engine submission CPU time</h2><p>768 off-screen curves per frame, tolerance 0.5, 6px round caps. Density calls the public drawAdaptive API, including its transform setup and reusable output. Other methods generate a LineString and draw it. The timed region excludes frame update/presentation/GPU wait. This is not a GPU benchmark.</p>" + table(rows["submission"], SCHEMAS["submission"].split())
    worst = sorted(rows["random"], key=lambda x: x["error"], reverse=True)[:24]
    if worst:
        body += "<h2>Largest sampled errors in the random corpus</h2>" + table(worst, SCHEMAS["random"].split())
    body += "<h2>Native animation captures</h2><p>Each frame was rendered by Siv3D. Play or scrub to compare deformation; no interpolation is added by the report.</p>"
    animations = {}
    for name in images:
        if name.startswith("animation-"):
            group = name.split("-")[1]
            animations.setdefault(group, []).append(name)
    for group, names in animations.items():
        names.sort(key=lambda name: int(name.rsplit("-", 1)[1].split(".")[0]))
        body += f'<details><summary>{"S curve" if group == "2" else "Near cusp"}</summary><button type="button" data-play="{group}">Play / pause</button> <input type="range" min="0" max="{len(names)-1}" value="0" data-slider="{group}"><img id="animation-{group}" src="{names[0]}" alt="Native animation comparison (Current is the legacy default)"></details>'
    if animations:
        body += '<script>const captures=' + json.dumps(animations) + ';const timers={};document.querySelectorAll("[data-slider]").forEach(s=>s.oninput=()=>document.getElementById("animation-"+s.dataset.slider).src=captures[s.dataset.slider][+s.value]);document.querySelectorAll("[data-play]").forEach(b=>b.onclick=()=>{const g=b.dataset.play;if(timers[g]){clearInterval(timers[g]);delete timers[g];return;}const s=document.querySelector(`[data-slider="${g}"]`);timers[g]=setInterval(()=>{s.value=(+s.value+1)%captures[g].length;s.oninput();},100);});</script>'
    body += "<h2>Native raster captures</h2><p>Rows: dense reference; legacy default (Current 0.48 / depth 5); Wang; production density. Columns: 1, 6, 24 pixel round-capped strokes. Open an image at its native size to judge game-scale appearance.</p>"
    for filename in (name for name in images if not name.startswith("animation-")):
        body += f'<details><summary>{html.escape(filename)}</summary><a href="{filename}"><img loading="lazy" src="{filename}" alt="{html.escape(filename)}"></a></details>'
    body += "<h2>Raw measurements</h2><p>" + " · ".join(f'<a href="{name}.csv">{name}.csv</a>' for name in SCHEMAS) + "</p>"
    document = '<!doctype html><html lang="en"><meta charset="utf-8"><title>Bezier tessellation comparison</title><style>body{max-width:1160px;margin:32px auto;padding:0 24px;font:16px/1.55 system-ui;color:#263441;background:#f7f9fb}h1,h2{color:#182c3a}img{max-width:100%;height:auto}table{border-collapse:collapse;font-size:13px;background:white}th,td{padding:7px 12px;border-bottom:1px solid #dae1e7;text-align:right}th:first-child,td:first-child{text-align:left}details{margin:12px 0;padding:10px;background:white;border:1px solid #dae1e7;border-radius:7px}summary{cursor:pointer;font-weight:600}a{color:#13736a}</style>' + body + '</html>'
    (args.output / "index.html").write_text(document, encoding="utf-8")
    print(f"Report: {args.output / 'index.html'}")
    print({kind: len(data) for kind, data in rows.items()})


if __name__ == "__main__":
    main()
