//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "TextureMipmapTestHelper.hpp"

# if SIV3D_PLATFORM(MACOS)
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <objc/runtime.h>

namespace s3d
{
	namespace
	{
		Method GetMethod(id target, SEL selector)
		{
			Method method = class_getInstanceMethod(object_getClass(target), selector);
			REQUIRE(method);
			return method;
		}

		auto ReplaceMethod(Method method, id block)
		{
			const IMP replacement = imp_implementationWithBlock(block);
			REQUIRE(replacement);
			const IMP original = method_setImplementation(method, replacement);
			return ScopeExit{ [=]
			{
				method_setImplementation(method, original);
				imp_removeBlock(replacement);
			} };
		}

		// Route only the texture creation on this thread to a known command buffer.
		auto UseCommandBuffer(id<MTLCommandQueue> queue, id<MTLCommandBuffer> commands)
		{
			NSThread* thread = [NSThread currentThread];
			Method method = GetMethod(queue, @selector(commandBuffer));
			const auto original = reinterpret_cast<id<MTLCommandBuffer>(*)(id, SEL)>(method_getImplementation(method));
			return ReplaceMethod(method, ^id<MTLCommandBuffer>(id receiver)
			{
				if ((receiver == queue) && ([NSThread currentThread] == thread)) { return commands; }
				return original(receiver, @selector(commandBuffer));
			});
		}

		auto FailUploadBuffer(id<MTLDevice> device, const int failAt, int& attempts)
		{
			int* count = &attempts;
			NSThread* thread = [NSThread currentThread];
			Method method = GetMethod(device, @selector(newBufferWithBytes:length:options:));
			typedef id<MTLBuffer> (*NewBuffer)(id, SEL, const void*, NSUInteger, MTLResourceOptions) NS_RETURNS_RETAINED;
			const auto original = reinterpret_cast<NewBuffer>(method_getImplementation(method));
			return ReplaceMethod(method, ^id<MTLBuffer>(id receiver, const void* bytes, NSUInteger length, MTLResourceOptions options) NS_RETURNS_RETAINED
			{
				if ((receiver == device) && ([NSThread currentThread] == thread) && (++(*count) == failAt)) { return nil; }
				return original(receiver, @selector(newBufferWithBytes:length:options:), bytes, length, options);
			});
		}

		template <class Create>
		void CheckBufferFailureAndRetry(CRenderer_Metal& renderer, const int failAt, Create create, const std::span<const Color> expected)
		{
			id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)renderer.getCommandQueue();
			id<MTLCommandBuffer> commands = [queue commandBuffer];
			REQUIRE(commands);
			int attempts = 0;
			{
				const auto use = UseCommandBuffer(queue, commands);
				const auto fail = FailUploadBuffer((__bridge id<MTLDevice>)renderer.getDevice(), failAt, attempts);
				CHECK_FALSE(create());
			}
			CHECK(attempts == failAt);
			CHECK(commands.status == MTLCommandBufferStatusNotEnqueued);
			Test::CheckMipColors(create(), expected);
		}

		Blob BC1Block(const uint16 rgb565)
		{
			const std::array<uint8, 8> bytes{ static_cast<uint8>(rgb565), static_cast<uint8>(rgb565 >> 8) };
			return Blob{ bytes.data(), bytes.size() };
		}
	}

	TEST_CASE("Texture.Metal.initial_upload_buffer_failure_and_retry")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto& renderer = *static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const std::array<Color, 3> colors{ Palette::Red, Palette::Lime, Palette::Blue };
		const Image base{ 4, 4, colors[0] };
		SECTION("Base only")
		{
			CheckBufferFailureAndRetry(renderer, 1, [&] { return Texture{ base, TextureDesc::NoMipmap }; }, std::span{ colors }.first(1));
		}
		SECTION("Generated mipmaps")
		{
			const std::array<Color, 3> red{ colors[0], colors[0], colors[0] };
			CheckBufferFailureAndRetry(renderer, 1, [&] { return Texture{ base, TextureDesc::Mipmap }; }, red);
		}
		SECTION("Middle of an explicit image chain")
		{
			const Array<Image> mips{ Image{ 2, 2, colors[1] }, Image{ 1, 1, colors[2] } };
			CheckBufferFailureAndRetry(renderer, 2, [&] { return Texture{ base, mips }; }, colors);
		}
		SECTION("Middle of a BC1 chain")
		{
			const BCnData bcn{ Size{ 4, 4 }, TextureFormat::BC1_RGBA_Unorm, { BC1Block(0xF800), BC1Block(0x07E0), BC1Block(0x001F) } };
			CheckBufferFailureAndRetry(renderer, 2, [&] { return Texture{ bcn }; }, colors);
		}
	}

	TEST_CASE("Texture.Metal.initial_upload_command_failure_and_retry")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto& renderer = *static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)renderer.getCommandQueue();
		const std::array<Color, 3> colors{ Palette::Red, Palette::Lime, Palette::Blue };
		const Image base{ 4, 4, colors[0] };
		const Array<Image> mips{ Image{ 2, 2, colors[1] }, Image{ 1, 1, colors[2] } };
		const auto create = [&] { return Texture{ base, mips }; };
		SECTION("No command buffer")
		{
			const auto use = UseCommandBuffer(queue, nil);
			CHECK_FALSE(create());
		}
		SECTION("No blit encoder")
		{
			id<MTLCommandBuffer> commands = [queue commandBuffer];
			REQUIRE(commands);
			const auto use = UseCommandBuffer(queue, commands);
			Method method = GetMethod(commands, @selector(blitCommandEncoder));
			const auto original = reinterpret_cast<id<MTLBlitCommandEncoder>(*)(id, SEL)>(method_getImplementation(method));
			const auto fail = ReplaceMethod(method, ^id<MTLBlitCommandEncoder>(id receiver)
			{
				return (receiver == commands) ? nil : original(receiver, @selector(blitCommandEncoder));
			});
			CHECK_FALSE(create());
			CHECK(commands.status == MTLCommandBufferStatusNotEnqueued);
		}
		SECTION("Error after completion")
		{
			id<MTLCommandBuffer> commands = [queue commandBuffer];
			REQUIRE(commands);
			const auto use = UseCommandBuffer(queue, commands);
			{
				Method method = GetMethod(commands, @selector(status));
				const auto original = reinterpret_cast<MTLCommandBufferStatus(*)(id, SEL)>(method_getImplementation(method));
				const auto fail = ReplaceMethod(method, ^MTLCommandBufferStatus(id receiver)
				{
					const auto status = original(receiver, @selector(status));
					return ((receiver == commands) && (status == MTLCommandBufferStatusCompleted)) ? MTLCommandBufferStatusError : status;
				});
				CHECK_FALSE(create());
			}
			CHECK(commands.status == MTLCommandBufferStatusCompleted);
		}
		Test::CheckMipColors(create(), colors);
	}
}
# endif
