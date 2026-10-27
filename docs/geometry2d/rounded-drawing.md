# Rounded drawing tessellation

`Circle::draw()` deliberately prioritizes bounded rendering cost: its full
contour stops at 252 segments. Higher magnification may increase chord error
without increasing detail. Use this as the reference budget for automatic
circle- and ellipse-related drawing, rather than maintaining a fixed pixel
error at arbitrarily large scales.

Count **curve intervals**, **GPU vertices/triangles**, and **CPU-only samples**
separately. A frame needs two vertices and two triangles per interval; shadows
add bands; separate dashes need separate endpoints. These costs do not by
themselves indicate excessive curve subdivision.

## Shared rules and scope

The relevant geometry builders are in
[Vertex2DBuilder_Rounded.cpp](../../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder_Rounded.cpp).
`CalculateCircleQuality(R)` returns a quadrant quality `Q` from 2 to 63, where
`R` is an effective radius in the raster RMS metric. For `R >= 8`,
`Q = min(5 + floor((R - 8) / 8), 63)`. The cap is reached at `R = 472`.

`CalculateCircleArcQuality(R, angle)` returns **points including endpoints**:
first compute `clamp(ceil((angle / (2*pi)) * (4*Q)), 1, 4*Q)` intervals, then
add one endpoint. The input sweep is nonnegative and at most one revolution.
Public circle sweep arguments are clamped to one revolution. Ellipse pie drawing
converts the sweep to ellipse parameters and uses ordinary fill for a full
revolution. The full-curve budget is at most 252 intervals. Counting intervals
before endpoints keeps partial arcs at the ordinary circle's angular density.

[Metal](../../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer2D/Metal/CRenderer2D_Metal.mm)
and [D3D11](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/CRenderer2D_D3D11.cpp)
pass the same shape arguments and raster RMS scale to these shared builders.
The scale composes Local, Camera, and scene render resolution. In these builders
it controls detail; it does not multiply source geometry a second time.
The scalar metric is not a directional screen-space error bound under
nonuniform transforms, projective warps, or custom vertex shaders.

The inventory covers GPU drawing, including solid, gradient, pattern, textured,
stroke-cap, round-join, and shadow paths where available. It excludes Image
writing and public geometry generation such as `outer()`, `asPolygon()`, and
`Shape2D` factories. Bezier/Spline drawing has an explicit per-curve budget and
is described in [adaptive curve drawing](bezier-drawing.md); circle and ellipse
primitives do not route their contours through that tessellator.

## Budget inventory

| Drawing path | Detail criterion and limit | Assessment |
| --- | --- | --- |
| Circle fill / frame | Radius / outer radius; `4*Q`, at most 252 contour intervals | Matches the reference budget |
| Ellipse fill / frame, including rotated drawing | Larger axis; frames add the larger absolute inner/outer width; `4*Q`, at most 252 | Bounded like Circle; uniform parameter sampling may be conservative for elongated ellipses |
| Circle pie / arc; Ellipse pie; TexturedCircle pie | Radius or larger axis and sweep; 1 to 252 intervals | Short pieces use the same angular density as ordinary circles, with no forced 4-interval minimum |
| Ellipse arc | Merges parameter-angle and normal-angle samples within 252 body intervals; caps have separate budgets | Preserves curved offset edges at elongated tips without exceeding the circle budget |
| Circle segment | Dedicated chord-fill quality, retaining at least 4 intervals | Preserves the thin region between chord and arc; two endpoints alone would collapse it |
| Circle dashed frame | Uses the same arc quality independently for each dash | Short dashes grow from 1 to 2, 3, … intervals as needed |
| Ellipse dashed frame | Short dashes use 2 points when both parameter and normal turns fit one ordinary circle interval; other dashes retain the existing subdivision | Reduces flat portions while preserving detail at high-curvature tips |
| RoundRect fill / frame / textured fill | Up to 63 intervals per corner, plus straight connections | Matches the curved-part budget; up to 256 contour intervals including the four straight sides |
| RoundRect dashed frame | Up to 63 intervals per complete corner, proportional subdivision of each covered arc; straight portions use endpoints | No forced 5-point minimum for short dashes; total includes dash endpoints and component boundaries |
| Circle / RoundRect shadows | Circle quality for outer/blur radius; additional rings or bands | Larger triangle totals reflect the bands, not an increased angular limit |
| Round line caps / Circle arc caps | Half-circle sweep using half the line width | 4 to 126 intervals per cap; two caps represent another full circular contour |
| Round joins on Triangle / Quad, including fallback | Same arc quality applied separately to each corner's sweep | Small turns need only two arc endpoints; side connections retain their own triangles |
| TexturedCircle | Radius; `4*Q`, at most 252 contour intervals | Same density and contour as Circle fill |
| SuperEllipse fill / frame | Exponent-aware deviation target, up to 256 contour intervals | Circle-sized budget; largest-error-first subdivision distributes detail across the contour |

For dashes and joins, total work also depends on the requested number of pieces
or corners. A single whole-shape limit of 252 would remove requested structure;
apply the quality budget to curved detail within that structure.

At the cap, Circle/Ellipse fills have 252 triangles and their frames have 504.
RoundRect/textured RoundRect fills have 258 triangles and frames have 512.
[Arc tessellation tests](../../Test/Test_Renderer2DArcTessellation.cpp),
[ellipse builder tests](../../Test/Test_Vertex2DBuilderEllipse.cpp), and
[ellipse extension tests](../../Test/Test_Renderer2DEllipseExtensions.cpp)
check interval limits, mesh bounds and transformed drawing. These geometry
budgets are distinct from GPU timing and visual quality at extreme scales.

## Independent-corner rectangles

`Rect` and `RectF` accept four radii directly, in top-left, top-right,
bottom-right, bottom-left order:

```cpp
rect.drawRounded(28, 6, 28, 6, color);
rect.drawRoundedFrame(28, 6, 28, 6, 8, color);
rect.drawRoundedFrame(28, 6, 28, 6, 6, 2, color);
```

The frame overloads specify either a centered total width or separate inner and
outer widths. See [RectF](../../Siv3D/include/Siv3D/RectF.hpp) and
[Rect](../../Siv3D/include/Siv3D/Rect.hpp) for normalization and boundary behavior.
A single large corner may exceed half the short side; fitting uses neighboring
radius sums. The fill shares its adjustment with `rounded(tl, tr, br, bl)`.

Both direct APIs write into one renderer buffer reservation without constructing
a Polygon or allocating a temporary contour. The independent-radius fill uses
a convex boundary fan; the frame joins corresponding inner/outer arc samples.
Collapsed inner arcs become fans around a square inner corner. Fitting those
inner radii to the contracted rectangle keeps thick frames simple and bounded,
though their corners need not be exact constant-distance offsets. Touching arc
endpoints can leave degenerate triangles, avoiding a separate compaction pass.
Only the band is drawn, so translucent frames do not accumulate color at joins.

Each corner uses its own bounded circle quality, with at most 63 curved intervals.
The independent-radius fill uses at most 256 vertices / 254 triangles, and its
frame at most 512 vertices / 512 triangles. Equal radii use the existing RoundRect
path (up to 258 fill triangles); zero-radius fills use Rect. Detail follows the
same composed raster RMS metric as other rounded shapes on Metal and D3D11.

Use direct drawing when dimensions or radii change frequently. A stored Polygon
remains useful for geometry queries and repeated drawing of a fixed shape.
The Polygon conversion has its own tessellation rule; pixel-identical outlines
are not implied. Its generic centered polyline frame also differs at collapsed
corners from the dedicated rounded frame.

The [interactive gallery and benchmark instructions](../../Test/Manual/RectRoundedDrawing.md)
cover both APIs and the Polygon comparison. The dedicated
[tests](../../Test/Test_RectRoundedDrawing.cpp) check mesh bounds, winding,
single coverage, radius fitting, collapsed corners, empty inputs, allocation
failure, transforms, scene resolution, and translucent rendering. The optional
benchmark compares CPU submission against temporary and cached Polygons;
it does not measure GPU time.

## Short-arc findings

The following counts were checked by invoking the actual shared builders with
an in-memory buffer allocator. `Circle{0, 0, 80}` and
`Ellipse{0, 0, 80, 80}` use inner/outer widths of 2, RMS scale 1, offset 0, and
dash ratio 0.5. The two shapes have identical circular geometry.

| Dashes | Circle vertices | Circle triangles | Ellipse vertices | Ellipse triangles |
| ---: | ---: | ---: | ---: | ---: |
| 32 | 128 | 64 | 128 | 64 |
| 64 | 256 | 128 | 256 | 128 |
| 256 | 1,024 | 512 | 1,024 | 512 |

These circular cases use two endpoints per dash in both paths. Ellipse dashes
are eligible for this reduction when their arc length is at most
`perimeter / fullQuality`. Each eligible dash also checks its endpoint parameter
span and the change in its outward unit normal against `2*pi / fullQuality`.
The normal check protects offset frames at elongated ellipses' high-curvature
tips, including thick strokes. Other dashes use equal-distance samples with a
minimum of five points.
The quality uses the larger axis plus the larger absolute stroke width and the
raster RMS scale, with the same bounded circle quality as other rounded drawing.

The classification uses a fixed stack bit mask and counts the exact vertices and
indices before a single buffer allocation. It adds endpoint checks but avoids
writing three intermediate vertex pairs and six triangles for each reduced
dash. No temporary heap allocation is needed. Dash placement, lengths, phase,
colors, and arc-length sampling retain their existing conventions. The normal
and parameter criteria are conservative: some short dashes intentionally retain
four intervals.

The [Ellipse dashed-frame tests](../../Test/Test_Renderer2DEllipseDashedFrame.cpp)
check analytic offset curves, flat tips in both axis orientations, inward and
outward widths, endpoints and colors, index limits, uniform transforms, composed
Local/Camera RMS scaling, and high-resolution rendering against an equivalent
explicit transform.

At radius 80 and a 0.1-degree sweep, Circle/Ellipse pies emit **1 triangle**
and a flat Circle arc emits **2 triangles**. For an ordinary circle interval
`delta = 2*pi / (4*Q)`, the shared rule gives:

| Sweep | Curve intervals | Arc points | Pie triangles | Flat arc triangles |
| --- | ---: | ---: | ---: | ---: |
| `0.5*delta` | 1 | 2 | 1 | 2 |
| `1.5*delta` | 2 | 3 | 2 | 4 |
| `2.5*delta` | 3 | 4 | 3 | 6 |

Round caps and circle segments use a separate chord-fill quality calculation,
retaining their existing point counts and sampling. Round caps remain half
circles with at least four intervals. Square caps keep their straight extensions.
Circle segments deliberately retain at least four intervals: collapsing their arc to a chord removes the filled
region, and only two intervals lose a quarter of a very thin segment's area.
Zero-sweep flat arcs and pies remain empty; zero-sweep square and round caps
retain their existing geometry.

[Arc tessellation tests](../../Test/Test_Renderer2DArcTessellation.cpp) check
interval thresholds and the full-circle cap, analytic chord deviation, segment
area, cap minima, small round joins, negative sweeps, gradients, patterns,
transforms, and high-resolution drawing. Direct joins and the drawing-only
polygon-buffer fallback use the same quality rule. This does not change public
Polygon generation. The
[short Circle dash tests](../../Test/Test_Renderer2DCircleDashedFrame.cpp)
compare the rendered dashes against independent, finely subdivided Quad strips.

## Textured circle contours

TexturedCircle uses the same quadrant quality and unit-vector table as
`Circle::draw()`: **8 to 252 intervals** for the full contour. Small circles
retain a nondegenerate contour, and larger circles stop refining at the same
effective radius. Local/Camera transforms and scene render resolution affect
detail through the shared raster RMS scale.

Reflecting the quadrant table avoids per-vertex trigonometry. Vertex positions
and triangle winding match ordinary Circle fill; UVs remain an affine mapping
of the circle's bounding square to the requested texture region, including
reversed regions. The existing absolute-radius handling is retained.

[Textured circle tests](../../Test/Test_Renderer2DTexturedCircle.cpp) compare
meshes at quality thresholds and the cap, check UV mapping and allocation
failure, and compare white-textured circles against ordinary circles under
composed transforms. Colored-texture tests check orientation, uniform scaling,
and high-resolution rendering against an equivalent explicit transform.

## Ellipse dash arc-length tables

Ellipse dashed drawing uses an arc-length table with
`max(64, 8*fullQuality)` intervals, capped at **2,016**. These are CPU samples,
independent of the GPU contour intervals. The table's angular spacing is retained
when optimizing construction; lowering its density can disturb equal-distance
dash placement, particularly around elongated ellipses' tips.

Only the first quarter is evaluated with trigonometric functions and square
roots. Its chord lengths accumulate in double precision before storing float
entries. Reflection reconstructs the other three quarters of the full table,
so construction needs at most **504** trigonometric/square-root evaluations.
The full table preserves the existing binary-search lookup path. Symmetry and
the shorter, double-precision accumulation also limit accumulated rounding error.

Four tables are retained in fixed storage, approximately **32 KiB**, with no
heap allocation. Keys consist of the two semiaxes and table interval count;
center, dash offset, ratio, count, and colors do not change the table. Stroke
width and raster RMS scale matter through the interval count. Cache hits reuse
the table; misses replace entries cyclically and use quarter-only construction.
The cache follows the renderer's single-thread ownership, like other builder
scratch storage. Drawing many different ellipses still benefits from symmetry,
even when none of their tables can be reused.

[Automated tests and a CPU benchmark](../../Test/Test_Renderer2DEllipseDashedFrame.cpp)
cover repeated shapes, alternating shapes, replacement, changes to either axis,
stroke width and scale, and wrapped offsets. An independent full-ellipse Simpson
integration checks phase and equal-distance sample errors, including aspect
ratios of 1,000:1 in both orientations. Rendering comparisons cover gradients,
transforms and scene resolution; gradient channel rounding under different
coordinate-operation orders is allowed by at most one 8-bit channel unit.

Run the CPU benchmark with an optimized build:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Renderer2D.ellipse_dashed_frame_vertices_benchmark'
```

It measures the entire shared builder, including lookup and vertex/index writes,
with a reusable destination buffer. The 1-, 4-, and 64-shape workloads distinguish
reuse from continual replacement, while all animate dash phase. Increasing dash
count shifts cost toward lookup and vertex generation, so table construction
savings do not imply the same percentage reduction for every dashed frame.
These measurements do not include GPU execution.

## SuperEllipse implementation

`draw()` and `drawFrame()` share a
[private tessellator](../../Siv3D/src/Siv3D/Renderer2D/CRenderer2D.cpp).
It retains axis points and splits a normalized octant at the point of greatest
chord deviation. A fixed-capacity max heap chooses the interval with the largest
remaining error next. The metric covers both reflected octants with the ellipse
axes and raster RMS scale. At most 32 intervals in the octant are reflected to
produce at most **256 intervals around the whole contour**. The internal target
is 0.25 render pixels, with at most 16 subdivision levels; the segment budget
takes precedence over that target.

Selecting the largest remaining error avoids spending the budget on the start
of an octant while leaving a coarse tail. Reflection keeps all quadrants
consistent, and a circular shape at the cap receives equal angular intervals.
Further magnification can increase visible chord error without allocating more
segments. The priority queue and contour storage are fixed-size stack buffers;
there is no additional heap allocation for this scheduling.

Frames include half the stroke width in the refinement metric, then use the
existing closed-line stroke builder. The target describes the sampled contour,
not a guarantee for stroke offsets or joins. Fill generation uses fixed scratch
storage; frames reuse the renderer's curve-point buffer. `n = 1` uses an exact
four-point diamond. `outer()` and `asPolygon()` keep their existing quality
settings and output independently of rendering state.

The [interactive validation and benchmark](../../Test/Manual/SuperEllipseTessellation.md)
covers exponent changes, zoom, high-DPI rendering, and reference overlays.
[Automated tests](../../Test/Test_Renderer2DSuperEllipse.cpp) check an independent
analytic reference, capped contour distribution, colors, pixels, transforms,
and generated geometry.
Adaptive error evaluation costs more CPU work than the previous angle table for
many curved shapes; fewer vertices alone are not evidence of faster drawing.

The vertex-generation benchmark includes both ordinary sizes and sizes that
exhaust the budget. Priority scheduling has a CPU cost at ordinary sizes,
especially for exponents whose curve evaluation is cheap; limiting large shapes
reduces tessellation and vertex-write work. Compare both regimes rather than
extrapolating saturated-shape savings to every draw.
