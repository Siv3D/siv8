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
# include "MetalTextureAllocationFailure.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/RenderTextureOps/TextureProcessing.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <chrono>
# include <iostream>
# include <pthread.h>
# include <pthread/qos.h>

namespace s3d
{
	TEST_CASE("RenderTextureOps.Metal.reuse_allocation_failure_and_retry")
	{
		REQUIRE(System::Update());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const RenderTexture source{ Size{ 65, 49 }, Palette::Red }, output{ source.size(), Palette::Blue };
		const RenderTexture small{ Size{ 13, 9 } }, blurred{ small.size() };
		RenderTextureOps::Workspace work;
		{
			Test::TextureAllocationFailure failure{ renderer->getDevice(), 1 };
			CHECK_THROWS_AS(RenderTextureOps::GaussianBlur(source, output, 2, work), Error);
			CHECK(failure.attempts == 1);
		}
		Image image;
		REQUIRE(output.readAsImage(image));
		CHECK(image == Image{ output.size(), Palette::Blue });
		RenderTextureOps::Downsample(source, small, work);
		RenderTextureOps::GaussianBlur(small, blurred, 2, work);
		RenderTextureOps::DualKawaseBlur(source, output, 2.5, work);
		RenderTextureOps::DualKawaseBlur(source, output, 12.5, work);
		{
			Test::TextureAllocationFailure failure{ renderer->getDevice(), 1 };
			// Warm fixed pipeline and smaller regions/amounts must need no new textures.
			for (int32 frame = 0; frame < 8; ++frame)
			{
				RenderTextureOps::Downsample(source, small, work);
				RenderTextureOps::GaussianBlur(small, blurred, 0.5 + frame, work);
				RenderTextureOps::DualKawaseBlur(source, output, 12.5 - frame, work);
				RenderTextureOps::GaussianBlur(small, Rect{ 1, 1, 7, 5 }, blurred, Rect{ 2, 2, 7, 5 }, 3, work);
			}
			CHECK(failure.attempts == 0);
		}
		work.release();
		{
			Test::TextureAllocationFailure failure{ renderer->getDevice(), 2 };
			CHECK_THROWS_AS(RenderTextureOps::DualKawaseBlur(source, output, 15, work), Error);
			CHECK(failure.attempts == 2);
		}
		RenderTextureOps::DualKawaseBlur(source, output, 15, work);
		REQUIRE(output.readAsImage(image));
		CHECK(image == Image{ output.size(), Palette::Red });
	}

	TEST_CASE("RenderTextureOps.Metal.benchmark", "[.benchmark]")
	{
		// Opt-in: compare the public Workspace path with explicitly prepared RTs using
		// the same native backend/shaders. Timings are diagnostics, never assertions.
		REQUIRE(System::Update());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto& context = renderer->getFrameContext();
		const ScopedLogSilencer silence;
		std::cout << "RenderTextureOps GPU: " << renderer->getDevice()->name()->utf8String() << '\n';
		{
			Grid<float> impulse{ Size{ 513, 513 }, 0.0f };
			impulse[256][256] = 1;
			const RenderTexture source{ impulse }, result{ source.size(), TextureFormat::R32_Float };
			RenderTextureOps::Workspace work;
			for (const double radius : { 3.0, 7.0, 15.0 })
			{
				RenderTextureOps::DualKawaseBlur(source, result, radius, work);
				Grid<float> pixels;
				REQUIRE(result.read(pixels));
				double energy = 0, moment = 0;
				for (int32 y = 0; y < 513; ++y)
				{
					for (int32 x = 0; x < 513; ++x)
					{
						energy += pixels[y][x]; moment += pixels[y][x] * (x - 256.0) * (x - 256.0);
					}
				}
				std::cout << "Kawase radius " << radius << ": impulse energy " << energy << ", horizontal RMS width " << std::sqrt(moment / energy) << " pixels\n";
			}
		}
		for (const Size size : { Size{ 512, 512 }, Size{ 1920, 1080 } })
		{
			Grid<std::array<HalfFloat, 4>> pixels{ size };
			for (int32 y = 0; y < size.y; ++y)
			{
				for (int32 x = 0; x < size.x; ++x)
				{
					pixels[y][x] = { HalfFloat{ (x % 31) / 30.0f }, HalfFloat{ (y % 29) / 28.0f },
						HalfFloat{ ((x * 17 ^ y * 23) & 255) / 255.0f }, HalfFloat{ 1.0f } };
				}
			}
			const RenderTexture source{ pixels };
			const RenderTexture destination{ size, TextureFormat::R16G16B16A16_Float };
			const RenderTexture scratch{ size, TextureFormat::R16G16B16A16_Float };
			const RenderTexture larger{ size + Size{ 128, 128 }, source.format() };
			RenderTextureOps::Workspace exact, oversized, dual, reducedWork;
			const Size quarterSize{ size.x / 4, size.y / 4 };
			const RenderTexture quarter{ quarterSize, source.format() }, quarterBlur{ quarterSize, source.format() };
			RenderTextureOps::GaussianBlur(source, destination, 8, exact);
			RenderTextureOps::GaussianBlur(larger, larger, 8, oversized);
			RenderTextureOps::DualKawaseBlur(source, destination, 32, dual);
			RenderTextureOps::Copy(source, Rect{ size }, larger, Rect{ size });
			std::array<Float4, 13> samples{};
			std::array<double, 25> weights{};
			double sum = 1;
			weights[0] = 1;
			for (size_t i = 1; i < weights.size(); ++i)
			{
				weights[i] = std::exp(-0.5 * (i / 8.0) * (i / 8.0));
				sum += 2 * weights[i];
			}
			samples[0] = Float4{ static_cast<float>(1 / sum), 0, 0, 0 };
			for (size_t i = 1; i <= 24; i += 2)
			{
				const double pair = weights[i] + weights[i + 1];
				samples[(i + 1) / 2] = Float4{ static_cast<float>(pair / sum), static_cast<float>(i + weights[i + 1] / pair), 0, 0 };
			}
			std::array<detail::TextureProcessingPass, 2> passes{};
			passes[0].kernel = passes[1].kernel = detail::TextureProcessingKernel::Gaussian;
			passes[0].source = source.id();
			passes[0].destination = scratch.id();
			passes[1].source = scratch.id();
			passes[1].destination = destination.id();
			for (auto& pass : passes)
			{
				pass.sourceRect = pass.destinationRect = Rect{ size };
				pass.samples = samples;
				pass.preserveDestination = false;
			}
			passes[0].step = Float2{ 1, 0 };
			passes[1].step = Float2{ 0, 1 };
			const auto invoke = [&](const size_t mode)
			{
				if (mode == 0)
				{
					SIV3D_ENGINE(Texture)->processRenderTextures(passes);
				}
				else if (mode == 1)
				{
					RenderTextureOps::GaussianBlur(source, destination, 8, exact);
				}
				else if (mode == 2)
				{
					RenderTextureOps::GaussianBlur(source, destination, 8, oversized);
				}
				else if (mode == 3)
				{
					RenderTextureOps::DualKawaseBlur(source, destination, 32, dual);
				}
				else if (mode == 4)
				{
					RenderTextureOps::Copy(source, destination);
				}
				else if (mode == 5)
				{
					RenderTextureOps::GaussianBlur(source, destination, 12, exact);
				}
				else if (mode == 6)
				{
					RenderTextureOps::DualKawaseBlur(source, destination, 7, dual);
				}
				else if (mode == 7)
				{
					RenderTextureOps::Downsample(source, quarter, reducedWork);
					RenderTextureOps::GaussianBlur(quarter, quarterBlur, 3, reducedWork);
					RenderTextureOps::Resample(quarterBlur, destination);
				}
				else
				{
					RenderTextureOps::GaussianBlur(larger, Rect{ size }, destination, Rect{ size }, 8, oversized);
				}
			};
			constexpr size_t Modes = 9, Batches = 7;
			constexpr int32 Repetitions = 20;
			std::array<std::array<double, Batches>, Modes> cpu{}, gpu{};
			// Interleave and rotate modes to reduce order/DVFS bias. Warm each case
			// outside the timer, including coefficients when switching sigma.
			for (size_t batch = 0; batch < Batches; ++batch)
			{
				for (size_t order = 0; order < Modes; ++order)
				{
					const size_t mode = ((order + batch) % Modes);
					invoke(mode);
					REQUIRE(context.flushAndWait(renderer->getCommandQueue()));
					const auto command = NS::RetainPtr(context.getCommandBuffer());
					const auto start = std::chrono::steady_clock::now();
					for (int32 i = 0; i < Repetitions; ++i)
					{
						invoke(mode);
					}
					cpu[mode][batch] = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start).count() / Repetitions;
					REQUIRE(context.flushAndWait(renderer->getCommandQueue()));
					gpu[mode][batch] = (command->GPUEndTime() - command->GPUStartTime()) * 1e6 / Repetitions;
				}
			}
			const char* labels[] = { "explicit Gaussian", "Workspace Gaussian", "oversized Gaussian", "Dual Kawase 32", "native Copy", "Gaussian 12", "Dual Kawase 7", "quarter Gaussian 3 and resize", "bounded source and scratch Gaussian" };
			for (size_t mode = 0; mode < Modes; ++mode)
			{
				std::sort(cpu[mode].begin(), cpu[mode].end());
				std::sort(gpu[mode].begin(), gpu[mode].end());
				std::cout << size.x << 'x' << size.y << ' ' << labels[mode] << ": CPU " << cpu[mode][Batches / 2]
					<< " us, GPU " << gpu[mode][Batches / 2] << " us\n";
			}
		}
	}

	TEST_CASE("RenderTextureOps.Metal.processing_cost_benchmark", "[.benchmark]")
	{
		const auto qos = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		REQUIRE(qos);
		const ScopeExit restoreQoS{ [=] { pthread_override_qos_class_end_np(qos); } };
		REQUIRE(System::Update());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto& context = renderer->getFrameContext();
		const ScopedLogSilencer silence;
		std::cout << "Processing cost GPU: " << renderer->getDevice()->name()->utf8String() << '\n';
		for (const Size size : { Size{ 64, 64 }, Size{ 1920, 1080 } })
		{
			Grid<Float4> pixels{ size };
			for (int32 y = 0; y < size.y; ++y)
			{
				for (int32 x = 0; x < size.x; ++x)
				{
					pixels[y][x] = Float4{ ((x * 17 ^ y * 23) & 255) / 255.0f,
						(x % 31) / 30.0f, (y % 29) / 28.0f, 1.0f };
				}
			}
			const Texture pattern{ pixels };
			for (const TextureFormat format : { TextureFormat::R8_Unorm, TextureFormat::R32_Float,
				TextureFormat::R32G32_Float, TextureFormat::R16G16B16A16_Float })
			{
				const RenderTexture source{ size, format };
				const RenderTexture destination{ size, format };
				const RenderTexture reduced{ size / 2, format };
				RenderTextureOps::Resample(pattern, source, TextureFilter::Nearest);
				RenderTextureOps::Workspace gaussian;
				RenderTextureOps::Workspace dual;
				RenderTextureOps::Workspace downsample;
				const auto invoke = [&](const size_t mode)
				{
					switch (mode)
					{
					case 0:
						RenderTextureOps::GaussianBlur(source, destination, 8, gaussian);
						break;
					case 1:
						RenderTextureOps::DualKawaseBlur(source, destination, 7, dual);
						break;
					case 2:
						RenderTextureOps::Resample(source, destination);
						break;
					case 3:
						RenderTextureOps::Downsample(source, reduced, downsample);
						break;
					case 4:
						{
							RenderTextureOps::Workspace fresh;
							RenderTextureOps::Downsample(source, reduced, fresh);
							break;
						}
					case 5:
						RenderTextureOps::Copy(source, destination);
						break;
					}
				};
				constexpr size_t Modes = 6;
				constexpr size_t Batches = 7;
				constexpr int32 Repetitions = 20;
				std::array<std::array<double, Batches>, Modes> cpu{};
				std::array<std::array<double, Batches>, Modes> gpu{};
				// Include only encoding in CPU time and completed GPU work in GPU time.
				// Pipeline compilation, first scratch allocation, and waits stay outside both.
				for (size_t batch = 0; batch < Batches; ++batch)
				{
					REQUIRE(System::Update());
					for (size_t order = 0; order < Modes; ++order)
					{
						const size_t mode = ((order + batch) % Modes);
						invoke(mode);
						REQUIRE(context.flushAndWait(renderer->getCommandQueue()));
						const auto command = NS::RetainPtr(context.getCommandBuffer());
						const auto start = std::chrono::steady_clock::now();
						for (int32 i = 0; i < Repetitions; ++i)
						{
							invoke(mode);
						}
						cpu[mode][batch] = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start).count() / Repetitions;
						REQUIRE(context.flushAndWait(renderer->getCommandQueue()));
						gpu[mode][batch] = (command->GPUEndTime() - command->GPUStartTime()) * 1e6 / Repetitions;
					}
				}
				constexpr const char* Labels[] = { "Gaussian 8", "Dual Kawase 7", "Resample", "Downsample reused", "Downsample fresh", "Copy" };
				for (size_t mode = 0; mode < Modes; ++mode)
				{
					std::sort(cpu[mode].begin(), cpu[mode].end());
					std::sort(gpu[mode].begin(), gpu[mode].end());
					std::cout << format.name().toUTF8() << ' ' << size.x << 'x' << size.y << ' ' << Labels[mode]
						<< ": CPU " << cpu[mode][Batches / 2] << " us, GPU " << gpu[mode][Batches / 2] << " us\n";
				}
			}
		}
	}
}
# endif
