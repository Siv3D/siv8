//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)
# include "MetalTextureAllocationFailure.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>

namespace s3d
{
	namespace
	{
		class MethodOverride
		{
		public:
			MethodOverride(Method method, id block)
				: m_method{ method }, m_original{ method_getImplementation(method) }, m_replacement{ imp_implementationWithBlock(block) }
			{
				method_setImplementation(m_method, m_replacement);
			}
			~MethodOverride()
			{
				method_setImplementation(m_method, m_original);
				imp_removeBlock(m_replacement);
			}
		private:
			Method m_method;
			IMP m_original;
			IMP m_replacement;
		};
	}

	TEST_CASE("MSRenderTexture.Metal.two_allocations_and_retry")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* device = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer))->getDevice();
		const MSRenderTexture existing{ Size{ 3, 2 }, Palette::Blue };
		for (const int32 failOn : { 1, 2 })
		{
			Test::TextureAllocationFailure failure{ device, failOn };
			CHECK_FALSE(MSRenderTexture{ Size{ 7, 5 }, Palette::Red, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap });
			CHECK(failure.attempts == failOn);
			CHECK(failure.successes == (failOn - 1));
		}
		Image pixels;
		REQUIRE(existing.readAsImage(pixels));
		CHECK(pixels == Image{ 3, 2, Palette::Blue });
		const MSRenderTexture retry{ Size{ 7, 5 }, Palette::Red, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
		REQUIRE(retry.readAsImage(pixels, 2));
		CHECK(pixels == Image{ 1, 1, Palette::Red });
	}

	TEST_CASE("MSRenderTexture.Metal.native_descriptors_and_unsupported_samples")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* device = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer))->getDevice();
		id target = (__bridge id)device;
		Method method = class_getInstanceMethod(object_getClass(target), @selector(newTextureWithDescriptor:));
		REQUIRE(method);
		typedef id<MTLTexture> (*NewTexture)(id, SEL, MTLTextureDescriptor*) NS_RETURNS_RETAINED;
		const auto original = reinterpret_cast<NewTexture>(method_getImplementation(method));
		for (const auto count : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			if (not device->supportsTextureSampleCount(FromEnum(count))) { continue; }
			std::array<NSUInteger, 2> samples{}, levels{};
			int32 allocations = 0;
			auto* sampleData = samples.data();
			auto* levelData = levels.data();
			auto* allocationCount = &allocations;
			{
				MethodOverride replacement{ method, ^id<MTLTexture>(id receiver, MTLTextureDescriptor* desc) NS_RETURNS_RETAINED
				{
					if ((receiver == target) && (*allocationCount < 2))
					{
						sampleData[*allocationCount] = desc.sampleCount;
						levelData[*allocationCount] = desc.mipmapLevelCount;
						++*allocationCount;
					}
					return original(receiver, @selector(newTextureWithDescriptor:), desc);
				} };
				const MSRenderTexture texture{ Size{ 7, 5 }, count, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
				REQUIRE(texture);
			}
			CHECK(allocations == 2);
			CHECK(samples == std::array<NSUInteger, 2>{ 1, FromEnum(count) });
			CHECK(levels == std::array<NSUInteger, 2>{ 3, 1 });
		}
		Method support = class_getInstanceMethod(object_getClass(target), @selector(supportsTextureSampleCount:));
		REQUIRE(support);
		const auto originalSupport = reinterpret_cast<BOOL(*)(id, SEL, NSUInteger)>(method_getImplementation(support));
		{
			MethodOverride replacement{ support, ^BOOL(id receiver, NSUInteger count)
			{
				return (receiver == target) ? NO : originalSupport(receiver, @selector(supportsTextureSampleCount:), count);
			} };
			Test::TextureAllocationFailure allocation{ device, 1 };
			CHECK_FALSE(MSRenderTexture{ Size{ 3, 2 } });
			CHECK(allocation.attempts == 0);
		}
		Method floatSupport = class_getInstanceMethod(object_getClass(target), @selector(supports32BitMSAA));
		REQUIRE(floatSupport);
		const auto originalFloat = reinterpret_cast<BOOL(*)(id, SEL)>(method_getImplementation(floatSupport));
		{
			MethodOverride replacement{ floatSupport, ^BOOL(id receiver)
			{
				return (receiver == target) ? NO : originalFloat(receiver, @selector(supports32BitMSAA));
			} };
			Test::TextureAllocationFailure allocation{ device, 1 };
			CHECK_FALSE(MSRenderTexture{ Size{ 3, 2 }, TextureFormat::R32_Float });
			CHECK(allocation.attempts == 0);
		}
	}

	TEST_CASE("MSRenderTexture.Metal.resolve_encoder_failure_preserves_result")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		REQUIRE(System::Update());
		const MSRenderTexture texture{ Size{ 3, 2 }, Palette::Blue };
		texture.clear(Palette::Red);
		id command = (__bridge id)renderer->getFrameContext().getCommandBuffer();
		Method method = class_getInstanceMethod(object_getClass(command), @selector(renderCommandEncoderWithDescriptor:));
		REQUIRE(method);
		const auto original = reinterpret_cast<id<MTLRenderCommandEncoder>(*)(id, SEL, MTLRenderPassDescriptor*)>(method_getImplementation(method));
		{
			MethodOverride replacement{ method, ^id<MTLRenderCommandEncoder>(id receiver, MTLRenderPassDescriptor* pass)
			{
				return (receiver == command) ? nil : original(receiver, @selector(renderCommandEncoderWithDescriptor:), pass);
			} };
			CHECK_THROWS_AS(texture.resolve(), InternalEngineError);
		}
		Image pixels;
		REQUIRE(texture.readAsImage(pixels));
		CHECK(pixels == Image{ 3, 2, Palette::Blue });
		texture.resolve();
		REQUIRE(texture.readAsImage(pixels));
		CHECK(pixels == Image{ 3, 2, Palette::Red });
	}
}
# endif
