//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <iostream>

namespace
{
	struct CapturedPipelineDraw
	{
		Image image;
		FrameMetrics metrics;
	};

	template <class Draw>
	CapturedPipelineDraw CapturePipelineDraw(Draw&& draw)
	{
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return { ScreenCapture::GetFrame(), Profiler::GetFrameMetrics() };
	}

	struct PipelineSceneScope
	{
		ColorF background = Scene::GetBackground();
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		ScopedColorMul2D colorMul{ Palette::White };
		ScopedColorAdd2D colorAdd{ 0.0 };
		ScopedRenderStates2D blend{ BlendState::Default2D };

		PipelineSceneScope()
		{
			Scene::SetBackground(Palette::Black);
		}

		~PipelineSceneScope()
		{
			Scene::SetBackground(background);
		}
	};
}

TEST_CASE("Renderer2D.pipeline_texture_constants")
{
	const PipelineSceneScope scope;
	const Texture red{ Image{ 2, 2, Palette::Red } };
	const Texture blue{ Image{ 2, 2, Palette::Blue } };
	REQUIRE(red);
	REQUIRE(blue);
	REQUIRE(System::Update());
	const auto draw = [&](const bool useStates)
	{
		for (int32 i = 0; i < 64; ++i)
		{
			const int32 x = (20 + (i % 8) * 24), y = (20 + (i / 8) * 24);
			const ColorF mul = ((i % 3) ? Palette::White : Palette::Yellow);
			const ColorF add = ((i % 4) ? Palette::Black : Palette::Green);
			if (useStates)
			{
				const Transformer2D transform{ Mat3x2::Translate(x, y) };
				const ScopedColorMul2D colorMul{ mul };
				const ScopedColorAdd2D colorAdd{ add };
				((i % 2) ? blue : red).resized(20, 20).draw();
			}
			else
			{
				ColorF color = (((i % 2) ? ColorF{ Palette::Blue } : ColorF{ Palette::Red }) * mul);
				color.r += add.r;
				color.g += add.g;
				color.b += add.b;
				RectF{ x, y, 20, 20 }.draw(color);
			}
		}
	};
	const auto reference = CapturePipelineDraw([&] { draw(false); });
	REQUIRE(reference.image.width() >= 208);
	REQUIRE(reference.image.height() >= 208);
	// 各フレームの最初の Draw と、triple-buffer 再利用後も同じ結果になることを確認する。
	for (int32 frame = 0; frame < 4; ++frame)
	{
		const auto actual = CapturePipelineDraw([&] { draw(true); });
		CHECK((actual.image == reference.image));
		CHECK(actual.metrics.drawCalls == 64);
		CHECK(actual.metrics.triangleCount == 128);
	}
}

# if SIV3D_PLATFORM(MACOS)

TEST_CASE("Renderer2D.pipeline_shader_blend_restore")
{
	const PipelineSceneScope scope;
	const std::string source = R"(
#include <metal_stdlib>
using namespace metal;
struct Vertex { float2 pos; float2 uv; float4 color; };
struct VSConstants { float2x4 transform; float4 colorMul; };
struct Varying { float4 position [[position]]; float4 colorPMA; float2 uv; };
vertex Varying Shift(uint id [[vertex_id]], constant Vertex* vertices [[buffer(0)]],
                    constant VSConstants& c [[buffer(1)]])
{
    const float2 pos = vertices[id].pos + float2(20, 0);
    Varying result;
    result.position = float4(c.transform[0].zw + pos.x * c.transform[0].xy + pos.y * c.transform[1].xy, 0, 1);
    result.colorPMA = vertices[id].color * c.colorMul;
    result.colorPMA.rgb *= result.colorPMA.a;
    result.uv = vertices[id].uv;
    return result;
}
fragment float4 Green(Varying input [[stage_in]]) { return float4(0, 1, 0, 1); }
)";
	const VertexShader vs = VertexShader::MSL(source, U"Shift");
	const PixelShader ps = PixelShader::MSL(source, U"Green");
	REQUIRE(vs);
	REQUIRE(ps);
	REQUIRE(System::Update());
	const auto reference = CapturePipelineDraw([]
	{
		RectF{ 20, 20, 20, 20 }.draw(Palette::Red);
		RectF{ 80, 20, 20, 20 }.draw(Palette::Red);
		RectF{ 120, 20, 20, 20 }.draw(Palette::Red);
		RectF{ 20, 60, 20, 20 }.draw(Palette::Lime);
		RectF{ 60, 60, 20, 20 }.draw(Palette::Red);
		RectF{ 20, 100, 20, 20 }.draw(Palette::Lime);
		RectF{ 60, 100, 20, 20 }.draw(Palette::Red);
		RectF{ 100, 100, 20, 20 }.draw(Color{ 255, 128, 0 });
		RectF{ 120, 100, 20, 20 }.draw(Palette::Blue);
	});
	REQUIRE(reference.image.width() >= 140);
	REQUIRE(reference.image.height() >= 120);
	for (int32 frame = 0; frame < 4; ++frame)
	{
		const auto actual = CapturePipelineDraw([&]
		{
			RectF{ 20, 20, 20, 20 }.draw(Palette::Red);
			{
				const ScopedCustomShader2D shader{ vs };
				RectF{ 60, 20, 20, 20 }.draw(Palette::Red);
			}
			RectF{ 120, 20, 20, 20 }.draw(Palette::Red);
			{
				const ScopedCustomShader2D shader{ ps };
				RectF{ 20, 60, 20, 20 }.draw(Palette::Red);
			}
			RectF{ 60, 60, 20, 20 }.draw(Palette::Red);
			{
				const ScopedCustomShader2D shader{ vs, ps };
				RectF{ 0, 100, 20, 20 }.draw(Palette::Red);
			}
			RectF{ 60, 100, 20, 20 }.draw(Palette::Red);
			RectF{ 100, 100, 40, 20 }.draw(Palette::Red);
			{
				const ScopedRenderStates2D blend{ BlendState::Additive };
				RectF{ 100, 100, 40, 20 }.draw(Palette::Green);
			}
			// 加算結果を左半分に残し、右半分では通常のブレンドへの復帰を確認する。
			RectF{ 120, 100, 20, 20 }.draw(Palette::Blue);
		});
		CHECK((actual.image == reference.image));
		CHECK(actual.metrics.drawCalls == 9);
		CHECK(actual.metrics.triangleCount == 20);
	}
}

# endif

TEST_CASE("Renderer2D.pipeline_cpu_benchmark", "[.benchmark]")
{
	const PipelineSceneScope scope;
	const Texture texture{ Image{ 2, 2, Palette::White } };
	REQUIRE(texture);
	constexpr int32 DrawCount = 4096;
	constexpr int32 WarmupFrames = 60;
	constexpr int32 SampleFrames = 240;
	for (const bool changePipeline : { false, true })
	{
		Array<int64> times;
		times.reserve(SampleFrames);
		REQUIRE(System::Update());
		for (int32 frame = 0; frame < (WarmupFrames + SampleFrames); ++frame)
		{
			for (int32 i = 0; i < DrawCount; ++i)
			{
				const ScopedColorMul2D color{ (i % 2) ? Palette::Red : Palette::Blue };
				const ScopedRenderStates2D blend{ (changePipeline && (i % 2)) ? BlendState::Additive : BlendState::Default2D };
				texture.draw(-10, -10);
			}
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			REQUIRE(metrics.drawCalls == DrawCount);
			REQUIRE(metrics.triangleCount == (DrawCount * 2));
			if (frame >= WarmupFrames)
			{
				times.push_back(metrics.engineEndTimeUs);
			}
		}
		times.sort();
		std::cout << "Pipeline benchmark: changePipeline=" << changePipeline
			<< ", draws=" << DrawCount << ", samples=" << SampleFrames
			<< ", engineEnd median us=" << times[SampleFrames / 2]
			<< ", p10=" << times[SampleFrames / 10] << ", p90=" << times[SampleFrames * 9 / 10] << '\n';
	}
}
