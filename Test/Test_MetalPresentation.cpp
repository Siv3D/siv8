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

// Xcode compiles this file as Objective-C++ with NO_S3D_USING.
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <objc/runtime.h>

namespace s3d
{
	namespace
	{
		constexpr size_t FrameSlots = MetalFrameContext::MaxInflightFrames;

		CAMetalLayer* GetMetalLayer()
		{
			auto* window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
			NSWindow* nativeWindow = glfwGetCocoaWindow(window);
			REQUIRE([nativeWindow.contentView.layer isKindOfClass:[CAMetalLayer class]]);
			return static_cast<CAMetalLayer*>(nativeWindow.contentView.layer);
		}

		// Inject nil only for the engine's layer, whose calls run on the render
		// thread. Other layers use the original method. Always restore the IMP,
		// including when an assertion exits the test early.
		class ScopedDrawableAvailability
		{
		public:

			bool unavailable = true;

			size_t requests = 0;

			size_t successes = 0;

			explicit ScopedDrawableAvailability(CAMetalLayer* layer)
			{
				m_method = class_getInstanceMethod(object_getClass(layer), @selector(nextDrawable));
				REQUIRE(m_method);
				m_original = method_getImplementation(m_method);
				const auto original = reinterpret_cast<id<CAMetalDrawable>(*)(id, SEL)>(m_original);
				auto* probe = this;
				m_replacement = imp_implementationWithBlock(^id<CAMetalDrawable>(CAMetalLayer* receiver)
				{
					if (receiver == layer)
					{
						++probe->requests;
						if (probe->unavailable)
						{
							return nil;
						}
					}
					id<CAMetalDrawable> drawable = original(receiver, @selector(nextDrawable));
					if ((receiver == layer) && drawable)
					{
						++probe->successes;
					}
					return drawable;
				});
				REQUIRE(m_replacement);
				method_setImplementation(m_method, m_replacement);
			}

			~ScopedDrawableAvailability()
			{
				method_setImplementation(m_method, m_original);
				imp_removeBlock(m_replacement);
			}

			ScopedDrawableAvailability(const ScopedDrawableAvailability&) = delete;

			ScopedDrawableAvailability& operator =(const ScopedDrawableAvailability&) = delete;

		private:

			Method m_method = nullptr;

			IMP m_original = nullptr;

			IMP m_replacement = nullptr;
		};

		struct DrawScope
		{
			Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
			Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
			ScopedColorMul2D colorMul{ Palette::White };
			ScopedColorAdd2D colorAdd{ 0.0 };
			ScopedViewport2D viewport{ none };
			ScopedScissorRect2D scissor{ none };
			ScopedRenderStates2D states{ BlendState::Opaque, RasterizerState::Default2D };
		};

		size_t AvailableFrameSlots(dispatch_semaphore_t semaphore)
		{
			size_t available = 0;
			while (dispatch_semaphore_wait(semaphore, DISPATCH_TIME_NOW) == 0)
			{
				++available;
			}
			for (size_t i = 0; i < available; ++i)
			{
				dispatch_semaphore_signal(semaphore);
			}
			return available;
		}
	}

	TEST_CASE("MetalPresentation.missing_drawable_preserves_capture_and_recovers")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		REQUIRE(System::Update());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		ScopedDrawableAvailability probe{ GetMetalLayer() };
		const DrawScope drawScope;
		size_t expectedRequests = 0;
		size_t expectedSuccesses = 0;

		// Six unavailable frames cover two rotations, followed by real drawables.
		for (const bool unavailable : { true, true, false })
		{
			probe.unavailable = unavailable;
			for (const auto color : { Palette::Red, Palette::Blue, Palette::Lime })
			{
				INFO("frame = " << expectedRequests << ", unavailable = " << unavailable);
				RectF{ 0, 0, 16, 16 }.draw(color);
				ScreenCapture::RequestCurrentFrame();
				const auto submitted = NS::RetainPtr(renderer->getFrameContext().getCommandBuffer());
				REQUIRE(submitted);
				REQUIRE(System::Update());
				CHECK(probe.requests == ++expectedRequests);
				expectedSuccesses += (not unavailable);
				REQUIRE(probe.successes == expectedSuccesses);
				CHECK(submitted->status() == MTL::CommandBufferStatusCompleted);
				CHECK(submitted->error() == nullptr);

				REQUIRE(ScreenCapture::HasNewFrame());
				const Image& captured = ScreenCapture::GetFrame();
				REQUIRE(captured.width() >= 16);
				REQUIRE(captured.height() >= 16);
				// Check the current color, not a stale image from a previous frame.
				CHECK(captured[8][8] == color);
				CHECK(AvailableFrameSlots(renderer->getFrameContext().getSemaphore()) == (FrameSlots - 1));
			}
		}
	}

	TEST_CASE("MetalPresentation.missing_drawable_without_capture_releases_slots")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		REQUIRE(System::Update());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		ScopedDrawableAvailability probe{ GetMetalLayer() };
		const DrawScope drawScope;
		NS::SharedPtr<MTL::CommandBuffer> submitted;

		// No per-frame readback wait: GPU completion must recycle the slots itself.
		for (size_t i = 0; i < (FrameSlots * 2); ++i)
		{
			RectF{ 0, 0, 16, 16 }.draw(Palette::Red);
			submitted = NS::RetainPtr(renderer->getFrameContext().getCommandBuffer());
			REQUIRE(submitted);
			REQUIRE(System::Update());
			CHECK(probe.requests == (i + 1));
			CHECK_FALSE(ScreenCapture::HasNewFrame());
		}
		renderer->getFrameContext().waitForLastSubmittedFrame();
		CHECK(submitted->status() == MTL::CommandBufferStatusCompleted);
		CHECK(submitted->error() == nullptr);
		CHECK(probe.successes == 0);
		CHECK(AvailableFrameSlots(renderer->getFrameContext().getSemaphore()) == (FrameSlots - 1));
	}
}

# endif
