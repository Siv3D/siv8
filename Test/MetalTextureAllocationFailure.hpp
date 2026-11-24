//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <objc/runtime.h>
# include <atomic>

namespace s3d::Test
{
	// Confine the failure to the real renderer's device and this lexical scope.
	// This exercises allocation-error propagation without exhausting GPU memory
	// or adding a production failure flag.
	class TextureAllocationFailure
	{
	public:

		std::atomic<int32> attempts = 0;
		std::atomic<int32> successes = 0;

		TextureAllocationFailure(MTL::Device* device, const int32 failOnAttempt)
		{
			id target = (__bridge id)device;
			m_method = class_getInstanceMethod(object_getClass(target), @selector(newTextureWithDescriptor:));
			REQUIRE(m_method);
			m_original = method_getImplementation(m_method);
			typedef id<MTLTexture> (*NewTexture)(id, SEL, MTLTextureDescriptor*) NS_RETURNS_RETAINED;
			const auto original = reinterpret_cast<NewTexture>(m_original);
			auto* count = &attempts;
			auto* succeeded = &successes;
			// Match the +1 ownership of newTextureWithDescriptor:, including successful
			// allocations before a later failure leaves them without command-buffer retains.
			m_replacement = imp_implementationWithBlock(^id<MTLTexture>(id receiver, MTLTextureDescriptor* descriptor) NS_RETURNS_RETAINED
			{
				if ((receiver == target) && (++(*count) == failOnAttempt))
				{
					return nil;
				}
				id<MTLTexture> texture = original(receiver, @selector(newTextureWithDescriptor:), descriptor);
				if ((receiver == target) && texture) { ++(*succeeded); }
				return texture;
			});
			REQUIRE(m_replacement);
			method_setImplementation(m_method, m_replacement);
		}

		~TextureAllocationFailure()
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

# endif
