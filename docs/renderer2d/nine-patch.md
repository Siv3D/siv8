# NinePatch

`NinePatch` stretches or tiles the edges and center of a texture while retaining
the shape of its corners. Construct it once and reuse it for panels, buttons, and
frames whose destination changes each frame:

```cpp
const Texture texture{ U"panel.png" };
NinePatch panel{ texture, 16 };
panel.setBorderScale(0.5);

const RectF rect{ 40, 40, 320, 160 };
panel.draw(rect);
const RectF content = panel.innerRect(rect).stretched(-8);
```

The source division and the displayed border scale are independent. This lets
a high-resolution asset keep its source detail while using smaller borders in
drawing coordinates. `innerRect()` gives the center patch's actual placement;
text padding is a separate layout decision. `minimumSize()` is the size at which
the borders fit without shrinking, rather than a restriction on the destination.

## Atlas regions and asymmetric borders

```cpp
const NinePatch frame{
    atlas(128, 64, 96, 80),
    NinePatch::Slice{ .top = 12, .right = 16, .bottom = 20, .left = 16 }
};
frame.drawFrame(RectF{ 40, 240, 360, 180 }, ColorF{ 1.0, 0.75 });
```

Slice widths use the source TextureRegion's current drawing size. A region
resized to twice its original dimensions therefore uses twice the slice widths
for the same source divisions. Mirroring, flipping, and signed source sizes
retain their visible orientation; side names refer to the image after those
reflections. The [public header](../../Siv3D/include/Siv3D/NinePatch.hpp) owns
the construction, empty-result, and drawing contracts.

Use the ordinary `ScopedRenderStates2D` sampler to select Nearest or Linear.
Local/camera/projective transforms, color scopes, and scissors apply as they do
to textured rectangles. The class retains the source texture handle; updates to
that resource follow the usual [DynamicTexture ordering](dynamic-texture.md).
Replacing an external texture variable with a different resource does not
replace the retained source.

Shared grid boundaries avoid geometric gaps and overlapping patches. They do
not isolate atlas texels from texture filtering: Linear filtering can sample a
neighboring atlas entry, and existing mip levels may already mix those entries.
Use suitable extruded padding and atlas mipmaps, or NoMipmap with Nearest for
pixel-art assets. UVs are not silently inset by half a texel, which would change
the source image and its proportions.

## Small destinations

When the planned borders do not fit, one common scale factor reduces all four
borders. With planned widths `L`, `R`, `T`, `B`, the factor is
`min(1, W / (L + R), H / (T + B))`, ignoring zero denominators. This keeps the
corners' aspect ratio while the center may collapse along one or both axes.
Drawing and `innerRect()` use the same placement calculation.

Zero side widths support horizontal or vertical three-slice images. All-zero
widths use the entire source as the center. Setting the border scale to zero
stretches only the center patch, even when tiling is enabled.

## Tiling

Choose repetition independently for the horizontal edges, vertical edges, and
center. Unselected regions stretch; corners retain their usual placement:

```cpp
panel.setTiling({ .topBottom = true, .center = true });
panel.draw(rect);

panel.setTiling(NinePatch::Tiling::All());
panel.drawFrame(rect);

panel.setTiling({}); // Restore stretch drawing.
```

Each region starts its pattern at its own top-left corner. The last tile at the
right or bottom is cropped to the destination rather than stretched to fit.
The natural tile size is the source center's size multiplied by the effective
border scale, including any shrinkage needed for a small destination. For
example, a 64 x 48 source with 8-pixel slices has a 48 x 32 center; at border
scale 0.5, its center tiles are 24 x 16. Horizontal edges repeat every 24 pixels
and vertical edges every 16 pixels. A resized source changes these dimensions;
mirroring and flipping preserve the pattern's visible orientation.

To bound geometry work, repetition is limited to 128 tiles along each active
axis. When that would be exceeded, the pattern scale increases uniformly until
it fits, preserving the center pattern's aspect ratio. Border placement and
`innerRect()` are unchanged. The maximum is 16,900 quads including corners and
edges. For example, a 1 x 2 source center filling a 1000 x 1000 inner rectangle
uses 7.8125 x 15.625 tiles, while its border widths stay fixed. Large or very
dense panels can still be substantially more expensive than stretched ones.
`drawFrame()` uses the same pattern scale as `draw()`, including the center's
contribution to this limit, so choosing frame-only drawing does not change the border pattern.

Tiled patches use the ordinary texture sampling path. At a repeated boundary,
Linear filtering may sample the adjacent source patch, and mipmaps may mix
neighboring texels. Atlas outer padding does not isolate the internal slices.
Use compatible colors at patch boundaries, or NoMipmap with Nearest when sharp
pixel-art repetition matters. Sampler wrapping repeats the whole texture, so
selecting Repeat does not make an atlas subregion independently repeat-filtered.
The [gallery](../../Test/Manual/NinePatch.md) lets you compare these choices and
toggle each tiling region alongside animation, alpha, and reflected sources.

## Implementation and validation

The object stores one TextureRegion, the source division, prepared UV boundaries,
a border scale, and three tiling flags. Copies share the texture but retain
independent settings. Stretch drawing uses at most 16 vertices and 18 triangles
(16 triangles for a frame) in fixed CPU arrays and calls the existing textured
`addMesh2D()` once. Tiling emits batches of at most 128 quads in a fixed CPU
array, using shared constant index tables and the same uploader. Adjacent
regions share subdivision positions even when only one of them tiles. This
avoids T-junctions that can cause gaps or alpha overlaps after transformation.
A center stretched between tiled edges uses boundary triangle fans when both
axes are subdivided, reusing the same workspace. Its geometry grows with the
sum of the edge counts rather than their product. Zero-area
patches are omitted. Renderer buffer growth, 16-bit buffer rollover, and batching
remain backend responsibilities; multiple mesh submissions can share a GPU draw
call. The Mesh2D uploader preserves the write-only upload cursor boundary.

Geometry repetition keeps construction cheap, shares the original resource,
and follows its updates without a second texture or cache invalidation. It also
preserves the current sampler and custom shader path. Separate repeat textures
could improve isolated repeat filtering but would introduce copies, storage,
and update synchronization; a dedicated shader would need atlas filtering and
mipmap derivative rules. The bounded geometry path is a compact fit for UI
panels, with the filtering and density limits above. Neither stretch nor tiling
allocates NinePatch heap workspace or overrides drawing state.

[Test_NinePatch.cpp](../../Test/Test_NinePatch.cpp) checks construction, value
semantics, explicit layout examples, mesh area/winding/UVs, and rendering against
independently cropped textured rectangles. Uniform translucent textures compared
with one rectangle exercise internal alpha seams under transforms. Tests also
cover reflected atlas regions, resized sources, both samplers, empty draws,
three-slice inputs, zero border scale, texture updates, and scene render resolution.
Tiling checks include all flag combinations, clipped final tiles, density limits
and aspect ratio, frame/fill pattern consistency, batch boundaries, and the
backend's 16-bit buffer rollover. Mixed stretch/tile modes are included in the
transformed alpha-seam and reflected source checks.
The transformed uniform-texture comparison permits one MSAA sample of coverage
rounding only within one render pixel of the independently transformed outer
edges; internal patch boundaries retain the strict color comparison.

Run the focused checks and then the full host suite using the
[development workflow](../development/README.md#build-and-test). On macOS:

```sh
./macOS/run-tests.sh '--test-case=~[benchmark]NinePatch.*,Renderer2DUpload.*'
./macOS/run-tests.sh
./macOS/validate-projects.sh
```

The opt-in submission comparison draws changing widths through NinePatch and
through individual TextureRegion rectangles, using stretch and tiling modes.
The tiling reference clips full tiles with `drawClipped()`. It reports median/p10/p90 CPU
wall time, triangles per call, and backend draw calls per batch; GPU execution
and presentation are outside timing:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=NinePatch.submission_benchmark'
```

On Windows use `-Configuration Release -TestArguments
'--test-case=NinePatch.submission_benchmark'` with the Windows runner.
The shared [upload diagnostic](README.md#measuring-cpu-writes-to-upload-buffers)
also compares mesh generation/upload with eight or nine textured-quad builders
in ordinary RAM and Metal default/write-combined buffers. Its tiled cases share
tile generation, then compare batch mesh upload with individual textured-quad
builders for 240-quad filled panels and 58-quad frames, and measures tiled borders
with a stretched center using 294 vertices and 170 triangles. It excludes command
registration and texture-handle preparation. Compare both measurements before
introducing a dedicated renderer path; neither is a GPU execution benchmark.
Timing results are diagnostic, not correctness thresholds.

## Migration from v0.6

v0.6's `fallbackToSimple` is replaced by proportional border shrinking. The current
sampler replaces the per-call filter argument; explicit `Texture{ path }` or
`Texture{ image }` handles loading. `Slice` uses top/right/bottom/left order,
matching `RectF::stretched()`; v0.6's four positional widths used a different
order. Select repeated regions with `setTiling()`; the old public repeat-texture
cache is replaced by direct sampling of the original source. Tiling now follows
the ordinary sampler's filtering behavior and the bounded repetition rule above.
