//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <objc/runtime.h>

namespace s3d
{
	namespace
	{
		class EncoderFailure
		{
		public:
			int32 attempts = 0;

			explicit EncoderFailure(CRenderer_Metal& renderer)
			{
				id command = (__bridge id)renderer.getFrameContext().getCommandBuffer();
				m_method = class_getInstanceMethod(object_getClass(command), @selector(renderCommandEncoderWithDescriptor:));
				REQUIRE(m_method);
				m_original = method_getImplementation(m_method);
				const auto original = reinterpret_cast<id<MTLRenderCommandEncoder>(*)(id, SEL, MTLRenderPassDescriptor*)>(m_original);
				auto* count = &attempts;
				m_replacement = imp_implementationWithBlock(^id<MTLRenderCommandEncoder>(id receiver, MTLRenderPassDescriptor* descriptor)
				{
					if (receiver == command)
					{
						++*count;
						return nil;
					}
					return original(receiver, @selector(renderCommandEncoderWithDescriptor:), descriptor);
				});
				REQUIRE(m_replacement);
				method_setImplementation(m_method, m_replacement);
			}

			~EncoderFailure()
			{
				method_setImplementation(m_method, m_original);
				imp_removeBlock(m_replacement);
			}

		private:
			Method m_method = nullptr;
			IMP m_original = nullptr;
			IMP m_replacement = nullptr;
		};
	}

	TEST_CASE("ScopedRenderTarget2D.Metal.encoder_failure_restores_target")
	{
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const ScopedRenderTarget2D scene{ none };
		const ScopedViewport2D viewport{ none };
		const ScopedScissorRect2D scissor{ none };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const RenderTexture a{ Size{ 8, 8 }, Palette::Black }, b{ Size{ 4, 4 }, Palette::Black };
		const ScopedRenderTarget2D outer{ a };
		const ScopedViewport2D outerViewport{ Rect{ 0, 0, 8, 8 } };
		Rect{ 8, 8 }.draw(Palette::Red);
		{
			EncoderFailure failure{ *renderer };
			CHECK_THROWS_AS(ScopedRenderTarget2D{ b }, InternalEngineError);
			CHECK(failure.attempts == 1);
			CHECK_THROWS_AS(a.draw(), Error);
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == a.id());
			CHECK(Graphics2D::GetViewport() == Rect{ 0, 0, 8, 8 });
		}
		// Failing on destruction must restore the outer state before propagating.
		{
			EncoderFailure failure{ *renderer };
			CHECK_THROWS_AS(([&]
			{
				const ScopedRenderTarget2D nested{ b };
				a.draw(); // Cache A as a valid input while B is the output.
			}()), InternalEngineError);
			CHECK(failure.attempts == 1);
			CHECK_THROWS_AS(a.draw(), Error);
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == a.id());
			CHECK(Graphics2D::GetViewport() == Rect{ 0, 0, 8, 8 });
		}
		// A second failure during stack unwinding preserves the original exception.
		{
			EncoderFailure failure{ *renderer };
			try
			{
				const ScopedRenderTarget2D nested{ b };
				a.draw(); // Cache A as a valid input while B is the output.
				throw Error{ "original scope failure" };
			}
			catch (const Error& error)
			{
				CHECK(error.messageUTF8() == "original scope failure");
			}
			CHECK(failure.attempts == 1);
			CHECK_THROWS_AS(a.draw(), Error);
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == a.id());
			CHECK(Graphics2D::GetViewport() == Rect{ 0, 0, 8, 8 });
		}
		Rect{ 8, 8 }.draw(Palette::Green);
		Image image;
		REQUIRE(a.readAsImage(image));
		CHECK(image == Image{ 8, 8, Palette::Green });
		REQUIRE(b.readAsImage(image));
		CHECK(image == Image{ 4, 4, Palette::Black });
	}
}
# endif
