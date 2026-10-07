# Renderer2D pipeline state benchmark

This CPU experiment compares repeated Draws that share a pipeline with a control
that changes BlendState on every Draw. Both workloads submit 4,096 off-screen
textured rectangles per frame, with alternating colors to keep Draws separate.
It uses public APIs and creates no output files.

## Execution

1. On macOS, run the registered benchmark from the repository root, outside the
   sandbox:

   ```sh
   CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Renderer2D.pipeline_cpu_benchmark'
   ```

   The benchmark is hidden from the default suite. It discards 60 warm-up frames
   and reports the median, p10, and p90 of 240 frames per workload. It also checks
   the public Draw and triangle counts on every frame.
2. Run the same benchmark before and after a change on the same machine and build
   configuration. Alternate the two versions when repeating measurements.
3. To run independently, paste the complete code below into a separate Siv3D v0.8
   application. Keep the repository's Main.cpp files and `--test-only` entry
   points intact. Read the results from standard output.
4. For pipeline call counts, temporarily count the `get(pipelineStateDesc)` and
   `setRenderPipelineState(pipeline)` calls inside `CRenderer2D_Metal::flush()`.
   Report counters after the command loop. Count only this scene encoder, not
   the separate presentation pass. Remove instrumentation after measurement.
5. To measure the command loop's thread CPU time, take
   `clock_gettime(CLOCK_THREAD_CPUTIME_ID, ...)` snapshots immediately before and
   after that loop, and report their difference after the second snapshot.
   Apply identical instrumentation to both versions. Counters and instrumentation
   can affect timing; also repeat the public benchmark without instrumentation.

## Expected results and interpretation

- Each workload has 4,096 Draws and 8,192 triangles per frame.
- With encoder-local pipeline reuse, the shared-pipeline workload has one scene
  pipeline lookup and one setting per frame. The control has 4,096 of each.
- Every new encoder must set its pipeline on its first Draw. The control checks
  the cost of frequent pipeline changes rather than reuse.
- `engineEndTimeUs` is elapsed time for the engine's end-of-frame work. It includes
  more than command encoding and can include presentation-related delays. It is
  not isolated thread CPU time or GPU execution time.
- Reduced lookup/setting counts demonstrate skipped work. A time reduction is a
  separate observation; this off-screen workload does not establish an FPS gain
  in real applications.
- Rendering correctness is covered by the public-API tests in
  [Test_Renderer2DPipelineState.cpp](../Test_Renderer2DPipelineState.cpp) and
  [Test_Renderer2DBatchBoundary.cpp](../Test_Renderer2DBatchBoundary.cpp), including
  shaders, blend restoration, vertex ranges, and subsequent frames.

## Complete code

```cpp
# include <Siv3D.hpp>
# include <iostream>

void Main()
{
	Scene::SetBackground(Palette::Black);
	const Texture texture{ Image{ 2, 2, Palette::White } };
	if (not texture)
	{
		return;
	}
	constexpr int32 DrawCount = 4096;
	constexpr int32 WarmupFrames = 60;
	constexpr int32 SampleFrames = 240;
	for (const bool changePipeline : { false, true })
	{
		Array<int64> times;
		times.reserve(SampleFrames);
		if (not System::Update())
		{
			return;
		}
		for (int32 frame = 0; frame < (WarmupFrames + SampleFrames); ++frame)
		{
			for (int32 i = 0; i < DrawCount; ++i)
			{
				const ScopedColorMul2D color{ (i % 2) ? Palette::Red : Palette::Blue };
				const ScopedRenderStates2D blend{
					(changePipeline && (i % 2)) ? BlendState::Additive : BlendState::Default2D };
				texture.draw(-10, -10);
			}
			if (not System::Update())
			{
				return;
			}
			const auto metrics = Profiler::GetFrameMetrics();
			if ((metrics.drawCalls != DrawCount) || (metrics.triangleCount != DrawCount * 2))
			{
				std::cout << "Unexpected Draw or triangle count\n";
				return;
			}
			if (frame >= WarmupFrames)
			{
				times.push_back(metrics.engineEndTimeUs);
			}
		}
		times.sort();
		std::cout << "changePipeline=" << changePipeline
			<< ", engineEnd median us=" << times[SampleFrames / 2]
			<< ", p10=" << times[SampleFrames / 10]
			<< ", p90=" << times[SampleFrames * 9 / 10] << '\n';
	}
}
```
