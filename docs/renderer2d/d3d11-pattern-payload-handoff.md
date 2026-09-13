# Windows handoff: four-vector pattern payload

Use this document as the initial prompt for the Windows task. Implement and
validate the D3D11 parameter-storage extension described below. Do not implement
new patterns or gradients in this stage. The storage work should be completed
before the [expression candidates](proposals/pattern-expressiveness.md) proceed.

## Objective and starting point

Work in the checkout containing `PatternParameters::extraParams` and the
four-element `toFloat4Array()`. Read the repository AGENTS.md and the
[payload design](pattern-payload.md). Explain changes in small stages so the
user can follow the data flow. Preserve unrelated and uncommitted work.

D3D11 and Metal already share the drawing-coordinate model. Do not repeat that
port, restore RMS pattern scaling, or merge the Pattern and QuadWarp shaders.
QuadWarp optimization is a separate task.

Metal already records all four pattern vectors and uses a 128-byte effect
constant layout. The six built-in patterns ignore the new vector and retain
their appearance. The shared packing API returns 64 bytes. The additional vector
has no adopted gradient/profile interpretation in this stage.

D3D11 still uses its original three-vector command-manager input and storage.
There are no compatibility adapters: update this path to the shared four-vector
API before rebuilding. The final effect layout groups Pattern at bytes 0-63 and
QuadWarp at bytes 64-127. Old D3D11 shader bytecode does not match those offsets.
Prioritize this final layout rather than preserving an intermediate build.

## Implementation

1. In
   [D3D11Renderer2DCommandManager.hpp](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/D3D11Renderer2DCommandManager.hpp)
   and its
   [implementation](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/D3D11Renderer2DCommandManager.cpp),
   change the pattern push/input, record/current-state, and getter types to
   `std::array<Float4, 4>`. Initialize all four vectors to zero; compare, record,
   retrieve, and restore all four.
   Leave the unrelated three-vector QuadWarp and SDF payloads intact. Use the
   Metal command manager as a reference.
   The four-vector `setPattern` in
   [Renderer2DCommon.hpp](../../Siv3D/src/Siv3D/Renderer2D/Renderer2DCommon.hpp)
   already maps the record to the correct effect-buffer fields. Do not reintroduce
   a three-vector overload.
2. Insert `float4 g_patternExtraParams` immediately after
   `g_patternBackgroundColor` in
   [2d.hlsl](../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl).
   Required offsets are UV mapping 0, background 32, added data 48, QuadWarp matrix
   64, and QuadWarp UV mapping 112, with a total of 128 bytes. Do not append the
   new field after QuadWarp or retain its previous offsets.
   Preserve all existing shader entry points and calculations.
3. Regenerate the tracked HLSL bytecode through the existing compile path in
   [CEngineShader_D3D11.cpp](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp).
   Inspect its actual compile switch before running. Do not normalize binary
   `.vs`/`.ps` assets as text or substitute an unrelated shader build flow.
4. Enable `Pattern.extra_parameters` in
   [Test_Pattern.cpp](../../Test/Test_Pattern.cpp) on Windows and add an HLSL
   diagnostic shader equivalent to the existing MSL `ReadExtra`. It reads all
   four components at byte offset 48 from pixel constant-buffer slot b1 and
   returns `float4(extra.xyz * extra.w, 1)`. For example, three float4 padding
   vectors followed by one `extra` vector express that test layout. Reuse the
   common draw/capture/assertion body and keep the Metal version intact.
5. Remove temporary staging notes and the D3D11 pending item in
   [TODO](../../TODO.md) after implementation and Windows validation. Update the
   payload document to describe the completed shared behavior. Keep pending
   expression work separate. Do not add migration history or backend progress
   notes to public Doxygen documentation.

No new shared source or test file is needed for this port. If the final work
adds or removes files, follow repository project-registration rules and report
any platform project validation that cannot run on this host.

## Verification

Run the focused tests on Windows, then the full suite:

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=Pattern*'
./WindowsDesktop/run-tests.ps1
```

`PatternParameters.packing` verifies 64-byte packing and nonzero extra data.
`PatternParameters.effect_constants` verifies the 128-byte structure, exact
offsets, zero initialization, and independent Pattern/QuadWarp setters.
`Pattern.extra_parameters` verifies the actual upload and state tracking:

- zero/A/A/B/A/zero with all other pattern parameters equal;
- seven draw calls and sixteen triangles for that sequence plus QuadWarp and
  a subsequent A draw, including the equal-A batching;
- repeated frames and restoration to zero;
- identical output from all six existing patterns with zero/nonzero extra data.

Keep the existing drawing-coordinate, viewport, color-state, custom-shader,
shape-path, and batch-boundary tests enabled. Do not weaken assertions to hide
packing or state-restoration differences. Inspect any platform-specific image
rounding difference before deciding whether a tolerance is appropriate.

If an incremental Windows run reports `EXCEPTION_ILLEGAL_INSTRUCTION` or leaves
an incomplete report, preserve the report and follow the
[clean-build diagnosis](../development/README.md#windows-incremental-build-failures)
before repeating the failing run or assigning the failure to source changes.

Verify LF endings for modified source files, run `git diff --check`, and run
`python tools/check_docs.py`. Report what Windows validated and what still needs
macOS validation after shared-code changes. Do not claim a GPU-time improvement
from draw-count or byte-size checks.

The final response should explain the completed data path, any bytecode changes,
test results, and remaining limitations. New expressive shaders belong to the
next task after this storage-only stage is complete.
