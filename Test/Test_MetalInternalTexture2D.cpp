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

// Xcode compiles this file as Objective-C++ with NO_S3D_USING so the native
// Metal headers can coexist with the shared test PCH's Siv3D types.
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>

// metal-cpp forwards newTexture() to this Objective-C selector. Returning nil
// exercises allocation failure without exhausting GPU memory or changing the
// engine's device. Only the selector used by these factories is implemented.
@interface Siv3DTestTextureAllocationFailureDevice : NSObject
- (id<MTLTexture>)newTextureWithDescriptor:(MTLTextureDescriptor*)descriptor;
@end

@implementation Siv3DTestTextureAllocationFailureDevice
- (id<MTLTexture>)newTextureWithDescriptor:(MTLTextureDescriptor*)descriptor
{
	(void)descriptor;
	return nil;
}
@end

namespace s3d
{
	namespace
	{
		MetalInternalTexture2D CreateTexture(MTL::Device* device, const Size size, const uint32 sampleCount)
		{
			return ((sampleCount == 1)
				? MetalInternalTexture2D::CreateRenderTexture(device, size)
				: MetalInternalTexture2D::CreateMSRenderTexture(device, size, sampleCount));
		}
	}

	TEST_CASE("MetalInternalTexture2D.creation_and_reset")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* device = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer))->getDevice();
		REQUIRE(device);
		REQUIRE(device->supportsTextureSampleCount(4));

		for (const uint32 sampleCount : { 1u, 4u })
		{
			for (const Size size : { Size{ 1, 1 }, Size{ 32, 16 } })
			{
				INFO("size = " << size << ", sampleCount = " << sampleCount);
				auto texture = CreateTexture(device, size, sampleCount);
				REQUIRE(texture.getTexture());
				CHECK(texture);
				CHECK_FALSE(texture.isEmpty());
				CHECK(texture.size() == size);
				CHECK(texture.getTexture()->width() == static_cast<NS::UInteger>(size.x));
				CHECK(texture.getTexture()->height() == static_cast<NS::UInteger>(size.y));
				CHECK(texture.getTexture()->sampleCount() == sampleCount);

				texture.reset();
				texture.reset();
				CHECK_FALSE(texture);
				CHECK(texture.isEmpty());
				CHECK(texture.size() == Size{ 0, 0 });
				CHECK(texture.getTexture() == nullptr);
			}
		}
	}

	TEST_CASE("MetalInternalTexture2D.allocation_failure")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		__attribute__((objc_precise_lifetime))
		Siv3DTestTextureAllocationFailureDevice* failingDevice = [[Siv3DTestTextureAllocationFailureDevice alloc] init];
		auto* device = (__bridge MTL::Device*)failingDevice;
		REQUIRE(device);

		for (const uint32 sampleCount : { 1u, 4u })
		{
			INFO("sampleCount = " << sampleCount);
			try
			{
				(void)CreateTexture(device, Size{ 32, 16 }, sampleCount);
				FAIL_CHECK("Texture allocation failure must throw InternalEngineError");
			}
			catch (const InternalEngineError& error)
			{
				const std::string message = error.messageUTF8();
				CHECK(message.contains("newTexture"));
				CHECK(message.contains("32x16"));
				CHECK(message.contains(fmt::format("sampleCount = {}", sampleCount)));
			}
		}
	}
}

# endif
