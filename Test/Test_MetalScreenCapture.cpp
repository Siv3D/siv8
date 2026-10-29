//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)

// Xcode compiles this file as Objective-C++ with NO_S3D_USING.
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <objc/runtime.h>

namespace s3d
{
	namespace
	{
		struct SceneScope
		{
			const SceneConfig config = Scene::GetConfig();
			const MSAASampleCount samples = Scene::GetMSAASampleCount();
			const ColorF background = Scene::GetBackground();
			const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
			const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
			const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
			const ScopedColorMul2D colorMul{ Palette::White };
			const ScopedColorAdd2D colorAdd{ 0.0 };
			const ScopedViewport2D viewport{ none };
			const ScopedScissorRect2D scissor{ none };
			const ScopedRenderStates2D states{ BlendState::Opaque, RasterizerState::Default2D };

			SceneScope()
			{
				Scene::SetBackground(Palette::Black);
				REQUIRE(System::Update());
				Scene::Configure(SceneConfig::Fixed({ 17, 13 }));
			}

			~SceneScope()
			{
				Scene::SetMSAASampleCount(samples);
				Scene::Configure(config);
				Scene::SetBackground(background);
			}
		};

		// Observe actual encoder creation without counters in the renderer. Only
		// the selected command buffer's scene target is observed; presentation
		// and unrelated command buffers still call the original implementation.
		class ScenePassProbe
		{
		public:

			struct Pass
			{
				MTLLoadAction load;
				MTLStoreAction store;
			};

			Array<Pass> passes;

			explicit ScenePassProbe(CRenderer_Metal& renderer)
			{
				id<MTLCommandBuffer> command = (__bridge id<MTLCommandBuffer>)renderer.getFrameContext().getCommandBuffer();
				id<MTLTexture> target = (__bridge id<MTLTexture>)((renderer.getSceneSampleCount() == 1)
					? renderer.getSceneTextureNonMSAA() : renderer.getSceneTextureMSAA()).getTexture();
				m_method = class_getInstanceMethod(object_getClass(command), @selector(renderCommandEncoderWithDescriptor:));
				REQUIRE(m_method);
				m_original = method_getImplementation(m_method);
				const auto original = reinterpret_cast<id<MTLRenderCommandEncoder>(*)(id, SEL, MTLRenderPassDescriptor*)>(m_original);
				auto* probe = this;
				m_replacement = imp_implementationWithBlock(^id<MTLRenderCommandEncoder>(id receiver, MTLRenderPassDescriptor* descriptor)
				{
					const auto attachment = descriptor.colorAttachments[0];
					if ((receiver == command) && (attachment.texture == target))
					{
						probe->passes.push_back({ attachment.loadAction, attachment.storeAction });
					}
					return original(receiver, @selector(renderCommandEncoderWithDescriptor:), descriptor);
				});
				REQUIRE(m_replacement);
				method_setImplementation(m_method, m_replacement);
			}

			~ScenePassProbe()
			{
				method_setImplementation(m_method, m_original);
				imp_removeBlock(m_replacement);
			}

			ScenePassProbe(const ScenePassProbe&) = delete;
			ScenePassProbe& operator =(const ScenePassProbe&) = delete;

		private:

			Method m_method = nullptr;
			IMP m_original = nullptr;
			IMP m_replacement = nullptr;
		};

		// Report a controlled failure only after a real command buffer completes.
		// This exercises the renderer and ScreenCapture failure path without
		// submitting an invalid GPU command or changing production code for tests.
		class FailedCompletion
		{
		public:

			explicit FailedCompletion(MTL::CommandBuffer* command)
				: m_command{ (__bridge id<MTLCommandBuffer>)command }
			{
				m_method = class_getInstanceMethod(object_getClass(m_command), @selector(status));
				REQUIRE(m_method);
				m_original = method_getImplementation(m_method);
				const auto original = reinterpret_cast<MTLCommandBufferStatus(*)(id, SEL)>(m_original);
				id<MTLCommandBuffer> target = m_command;
				m_replacement = imp_implementationWithBlock(^MTLCommandBufferStatus(id receiver)
				{
					const auto status = original(receiver, @selector(status));
					return ((receiver == target) && (status == MTLCommandBufferStatusCompleted))
						? MTLCommandBufferStatusError : status;
				});
				REQUIRE(m_replacement);
				method_setImplementation(m_method, m_replacement);
			}

			~FailedCompletion()
			{
				// The completion handler must finish before the replacement is removed.
				const auto original = reinterpret_cast<MTLCommandBufferStatus(*)(id, SEL)>(m_original);
				if (original(m_command, @selector(status)) >= MTLCommandBufferStatusCommitted)
				{
					[m_command waitUntilCompleted];
				}
				method_setImplementation(m_method, m_original);
				imp_removeBlock(m_replacement);
			}

			FailedCompletion(const FailedCompletion&) = delete;
			FailedCompletion& operator =(const FailedCompletion&) = delete;

		private:

			id<MTLCommandBuffer> m_command;
			Method m_method = nullptr;
			IMP m_original = nullptr;
			IMP m_replacement = nullptr;
		};

		void Capture()
		{
			ScreenCapture::RequestCurrentFrame();
			REQUIRE(System::Update());
			REQUIRE(ScreenCapture::HasNewFrame());
		}
	}

	TEST_CASE("MetalScreenCapture.empty_flush_and_final_store_actions")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		const SceneScope scene;
		auto& renderer = *static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		for (const auto samples : { MSAASampleCount::NoMSAA, MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			if (not renderer.getDevice()->supportsTextureSampleCount(FromEnum(samples))) { continue; }
			CAPTURE(samples);
			Scene::SetMSAASampleCount(samples);
			const bool msaa = (samples != MSAASampleCount::NoMSAA);
			const auto finalStore = (msaa ? MTLStoreActionMultisampleResolve : MTLStoreActionStore);
			const auto intermediateStore = (msaa ? MTLStoreActionStoreAndMultisampleResolve : MTLStoreActionStore);

			// A frame without geometry must still clear and resolve its background.
			{
				ScenePassProbe probe{ renderer };
				Capture();
				REQUIRE(probe.passes.size() == 1);
				CHECK(probe.passes[0].load == MTLLoadActionClear);
				CHECK(probe.passes[0].store == finalStore);
				CHECK((ScreenCapture::GetFrame() == Image{ Size{ 17, 13 }, Palette::Black }));
			}

			// Repeated empty flushes add no passes, including the automatic final
			// flush. State changes made in an empty flush must reach the next draw.
			{
				ScenePassProbe probe{ renderer };
				Graphics2D::Flush();
				Graphics2D::Flush();
				{
					const ScopedColorMul2D green{ Palette::Green };
					Graphics2D::Flush();
					Scene::Rect().draw(Palette::White);
					Graphics2D::Flush();
				}
				Graphics2D::Flush();
				Capture();
				REQUIRE(probe.passes.size() == 2);
				CHECK(probe.passes[0].load == MTLLoadActionClear);
				CHECK(probe.passes[1].load == MTLLoadActionLoad);
				CHECK(probe.passes[0].store == intermediateStore);
				CHECK(probe.passes[1].store == intermediateStore);
				CHECK((ScreenCapture::GetFrame() == Image{ Size{ 17, 13 }, Palette::Green }));
			}

			// Later drawing must preserve the samples from an intermediate flush;
			// only the final pass can discard them after resolving.
			{
				ScenePassProbe probe{ renderer };
				Scene::Rect().draw(Palette::Red);
				Graphics2D::Flush();
				Graphics2D::Flush();
				Rect{ 5, 3, 4, 6 }.draw(Palette::Blue);
				Capture();
				REQUIRE(probe.passes.size() == 2);
				CHECK(probe.passes[0].store == intermediateStore);
				CHECK(probe.passes[1].store == finalStore);
				CHECK(probe.passes[1].load == MTLLoadActionLoad);
				Image expected{ 17, 13, Palette::Red };
				for (int32 y = 3; y < 9; ++y)
				{
					for (int32 x = 5; x < 9; ++x) { expected[y][x] = Palette::Blue; }
				}
				CHECK(ScreenCapture::GetFrame() == expected);
			}

			// The next frame clears the discarded MSAA contents rather than loading.
			{
				ScenePassProbe probe{ renderer };
				Capture();
				REQUIRE(probe.passes.size() == 1);
				CHECK(probe.passes[0].load == MTLLoadActionClear);
				CHECK(probe.passes[0].store == finalStore);
				CHECK((ScreenCapture::GetFrame() == Image{ Size{ 17, 13 }, Palette::Black }));
			}
		}
	}

	TEST_CASE("MetalScreenCapture.failed_frame_preserves_image_and_consumes_requests")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		const SceneScope scene;
		auto& renderer = *static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const FilePath saved = Test::OutputPath(U"metal-screen-capture/saved.png");
		const FilePath failed = Test::OutputPath(U"metal-screen-capture/failed.png");
		for (const Size size : { Size{ 17, 13 }, Size{ 19, 15 } })
		{
			CAPTURE(size);
			Scene::Configure(SceneConfig::Fixed({ 17, 13 }));
			Scene::Rect().draw(Palette::Red);
			ScreenCapture::SaveCurrentFrameTo(saved);
			Capture();
			const Image previous = ScreenCapture::GetFrame();
			const Color* const pixels = renderer.getScreenCapture().data();
			REQUIRE((previous == Image{ Size{ 17, 13 }, Palette::Red }));
			CHECK(Image{ saved } == previous);

			Scene::Configure(SceneConfig::Fixed(size));
			Scene::Rect().draw(Palette::Blue);
			{
				FailedCompletion failure{ renderer.getFrameContext().getCommandBuffer() };
				ScreenCapture::SaveCurrentFrameTo(saved);
				ScreenCapture::SaveCurrentFrameTo(failed);
				ScreenCapture::RequestCurrentFrame();
				REQUIRE(System::Update());
				CHECK_FALSE(ScreenCapture::HasNewFrame());
				CHECK_FALSE(renderer.captureScreenshot());
				CHECK(renderer.getScreenCapture().data() == pixels);
				CHECK(renderer.getScreenCapture() == previous);
				CHECK(Image{ saved } == previous);
				CHECK_FALSE(FileSystem::Exists(failed));
			}

			// Failed requests must not be retried on the next successful frame.
			REQUIRE(System::Update());
			CHECK_FALSE(ScreenCapture::HasNewFrame());
			CHECK_FALSE(FileSystem::Exists(failed));
			CHECK(Image{ saved } == previous);
			Scene::Rect().draw(Palette::Green);
			ScreenCapture::SaveCurrentFrameTo(saved);
			Capture();
			CHECK((ScreenCapture::GetFrame() == Image{ size, Palette::Green }));
			CHECK(Image{ saved } == ScreenCapture::GetFrame());
			if (size == previous.size()) { CHECK(renderer.getScreenCapture().data() == pixels); }
		}
	}

	TEST_CASE("MetalScreenCapture.no_submitted_frame_is_not_success")
	{
		MetalFrameContext context;
		CHECK_FALSE(context.waitForLastSubmittedFrame());
		CRenderer_Metal renderer;
		CHECK_FALSE(renderer.captureScreenshot());
		CHECK(renderer.getScreenCapture().isEmpty());
	}
}

# endif
