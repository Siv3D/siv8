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
# if SIV3D_PLATFORM(MACOS)
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Renderer2D/ConstantBuffer2DCommands.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <chrono>
# include <iostream>
# include <pthread.h>
# include <pthread/qos.h>

namespace s3d
{
	TEST_CASE("ConstantBuffer.Metal.active_size_benchmark", "[.benchmark]")
	{
		const auto qos = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		REQUIRE(qos);
		const ScopeExit restoreQoS{ [=] { pthread_override_qos_class_end_np(qos); } };
		const ScopedScissorRect2D scissor{ none };
		const std::string source = R"(
#include <metal_stdlib>
using namespace metal;
struct Output { float4 position [[position]]; float4 color; };
vertex Output VS(uint id [[vertex_id]], constant float4& value [[buffer(2)]])
{
    return { float4(id == 2 ? 3.0 : -1.0, id == 1 ? 3.0 : -1.0, 0, 1), value };
}
fragment float4 PS(Output input [[stage_in]], constant float4& value [[buffer(3)]])
{
    return (input.color + value) * 0.5;
}
)";
		const VertexShader vs = VertexShader::MSL(source, U"VS");
		const PixelShader ps = PixelShader::MSL(source, U"PS");
		REQUIRE(vs);
		REQUIRE(ps);
		REQUIRE(System::Update());
		const ScopedCustomShader2D shaders{ vs, ps };
		const ScopedRenderStates2D blend{ BlendState::Opaque };
		const RenderTexture output{ Size{ 1, 1 } };
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto& context = renderer->getFrameContext();
		ConstantBuffer<std::array<Float4, 4096>> cb;
		(*cb)[0] = Float4{ 0, 1, 0, 1 }; // Both shaders read element 0 for every active size.
		ConstantBuffer2DCommands snapshots;
		constexpr std::array<size_t, 6> Sizes{ 16, 208, 784, 4096, 4112, 65536 };
		constexpr size_t Batches = 7;
		constexpr int32 CPURepetitions = 4096;
		constexpr int32 DrawRepetitions = 128;
		std::array<std::array<double, Batches>, Sizes.size()> cpu{}, encoding{}, gpu{};
		const auto draw = [&]
		{
			const ScopedVSConstantBuffer2D vertex{ 2, cb };
			const ScopedPSConstantBuffer2D pixel{ 3, cb };
			Graphics2D::DrawTriangles(1);
		};
		const auto snapshot = [&]
		{
			snapshots.push(ShaderStage::Pixel, 3, cb._data(), cb.activeSize(), cb.size());
			snapshots.flush();
			snapshots.reset();
		};
		for (size_t batch = 0; batch < Batches; ++batch)
		{
			REQUIRE(System::Update());
			const ScopedRenderTarget2D target{ output };
			const ScopedViewport2D viewport{ Rect{ output.size() } };
			for (size_t order = 0; order < Sizes.size(); ++order)
			{
				const size_t mode = ((order + batch) % Sizes.size());
				cb.setActiveSize(Sizes[mode]);
				for (int32 i = 0; i < DrawRepetitions; ++i) { draw(); }
				Graphics2D::Flush();
				REQUIRE(context.flushAndWait(renderer->getCommandQueue()));
				// Separate reusable CPU snapshots from actual Metal uploads and draw submission.
				snapshot();
				snapshot();
				auto start = std::chrono::steady_clock::now();
				for (int32 i = 0; i < CPURepetitions; ++i) { snapshot(); }
				cpu[mode][batch] = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - start).count() / CPURepetitions;
				const auto command = NS::RetainPtr(context.getCommandBuffer());
				start = std::chrono::steady_clock::now();
				for (int32 i = 0; i < DrawRepetitions; ++i) { draw(); }
				Graphics2D::Flush();
				encoding[mode][batch] = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start).count() / DrawRepetitions;
				REQUIRE(context.flushAndWait(renderer->getCommandQueue()));
				gpu[mode][batch] = (command->GPUEndTime() - command->GPUStartTime()) * 1e6 / DrawRepetitions;
			}
		}
		Image actual;
		REQUIRE(output.readAsImage(actual));
		CHECK(actual == Image{ 1, 1, Palette::Lime });
		for (size_t mode = 0; mode < Sizes.size(); ++mode)
		{
			std::sort(cpu[mode].begin(), cpu[mode].end());
			std::sort(encoding[mode].begin(), encoding[mode].end());
			std::sort(gpu[mode].begin(), gpu[mode].end());
			std::cout << "ConstantBuffer active " << Sizes[mode] << '/' << cb.size()
				<< " bytes: snapshot " << cpu[mode][Batches / 2] << " ns, two-stage encoding " << encoding[mode][Batches / 2]
				<< " us, GPU " << gpu[mode][Batches / 2] << " us\n";
		}
	}
}
# endif
