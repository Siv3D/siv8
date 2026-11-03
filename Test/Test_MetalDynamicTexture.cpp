//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Texture/Metal/MetalTexture.hpp>
# include <Siv3D/Texture/TextureUtility.hpp>
# include <atomic>
# include <iostream>
# include <pthread.h>
# include <pthread/qos.h>

@interface Siv3DDynamicTextureTestQueue : NSObject
@property(nonatomic, strong) id<MTLCommandQueue> queue;
@property(nonatomic) BOOL fail;
@property(nonatomic) NSUInteger requests;
- (id<MTLCommandBuffer>)commandBuffer;
@end
@implementation Siv3DDynamicTextureTestQueue
- (id<MTLCommandBuffer>)commandBuffer
{
	++self.requests;
	return self.fail ? nil : [self.queue commandBuffer];
}
@end

namespace s3d
{
	namespace
	{
		void CheckPixels(MetalTexture& texture, const Color expected)
		{
			std::array<Color, 16> pixels;
			texture.getTexture()->getBytes(pixels.data(), 4 * sizeof(Color), MTL::Region::Make2D(0, 0, 4, 4), 0);
			for (const auto pixel : pixels) { CHECK(pixel == expected); }
		}
	}

	TEST_CASE("DynamicTexture.Metal.continuation_allocation_failure_and_standalone_updates")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto* queue = renderer->getCommandQueue();
		const Image blue{ 4, 4, Palette::Blue };
		MetalTexture texture{ MetalTexture::Dynamic{}, MetalTexture::NoMipmap{}, renderer->getDevice(), queue,
			blue.size(), std::as_bytes(std::span{ blue }), TextureFormat::R8G8B8A8_Unorm, TextureDesc::NoMipmap };
		REQUIRE(texture.isInitialized());
		Siv3DDynamicTextureTestQueue* proxy = [[Siv3DDynamicTextureTestQueue alloc] init];
		proxy.queue = (__bridge id<MTLCommandQueue>)queue;
		MetalFrameContext frame;
		(void)frame.waitForFrame();
		frame.beginFrame(queue);
		const auto initial = NS::RetainPtr(frame.getCommandBuffer());
		proxy.fail = YES;
		const std::array<Byte, 1> incomplete{};
		CHECK_FALSE(texture.fill((__bridge MTL::CommandQueue*)proxy, frame, incomplete, 16, true));
		CHECK_FALSE(texture.fillRegion((__bridge MTL::CommandQueue*)proxy, frame, incomplete, 16, Rect{ 4, 4 }, true));
		CHECK(proxy.requests == 0);
		CHECK_FALSE(texture.fill((__bridge MTL::CommandQueue*)proxy, frame, Palette::Red, true));
		CHECK(frame.getCommandBuffer() == initial.get());
		CHECK(initial->status() == MTL::CommandBufferStatusNotEnqueued);
		CheckPixels(texture, Palette::Blue);
		proxy.fail = NO;
		REQUIRE(texture.fill((__bridge MTL::CommandQueue*)proxy, frame, Palette::Red, true));
		CHECK(frame.getCommandBuffer() != initial.get());
		REQUIRE(frame.flushAndWait(queue));
		CheckPixels(texture, Palette::Red);
		frame.cancel();
		// Operations before/after an acquired frame still submit and track uploads.
		REQUIRE(texture.fill(queue, frame, Palette::Green, true));
		REQUIRE(texture.fillRegion(queue, frame, Palette::Blue, Rect{ 4, 4 }));
		const auto barrier = NS::RetainPtr(queue->commandBuffer());
		barrier->commit();
		barrier->waitUntilCompleted();
		CheckPixels(texture, Palette::Blue);
	}

	TEST_CASE("DynamicTexture.Metal.busy_probe_does_not_copy_split_or_submit")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto* queue = renderer->getCommandQueue();
		const Image blue{ 4, 4, Palette::Blue };
		MetalTexture texture{ MetalTexture::Dynamic{}, MetalTexture::NoMipmap{}, renderer->getDevice(), queue,
			blue.size(), std::as_bytes(std::span{ blue }), TextureFormat::R8G8B8A8_Unorm, TextureDesc::NoMipmap };
		REQUIRE(texture.isInitialized());
		const auto event = NS::TransferPtr(renderer->getDevice()->newSharedEvent());
		REQUIRE(event);
		MetalFrameContext frame;
		const ScopeExit unblock{ [&] { event->setSignaledValue(1); } };
		(void)frame.waitForFrame();
		frame.beginFrame(queue);
		frame.getCommandBuffer()->encodeWait(event.get(), 1);
		REQUIRE(texture.fill(queue, frame, Palette::Red, true));
		const auto continuation = NS::RetainPtr(frame.getCommandBuffer());
		Siv3DDynamicTextureTestQueue* proxy = [[Siv3DDynamicTextureTestQueue alloc] init];
		proxy.queue = (__bridge id<MTLCommandQueue>)queue;
		CHECK_FALSE(texture.fill((__bridge MTL::CommandQueue*)proxy, frame, Palette::Green, false));
		const Image green{ 4, 4, Palette::Green };
		std::array<Byte, 76> padded{};
		FillWithImage(padded.data(), Size{ 4, 4 }, 20, std::as_bytes(std::span{ green }), 16, 16);
		CHECK_FALSE(texture.fill((__bridge MTL::CommandQueue*)proxy, frame, padded, 20, false));
		CHECK_FALSE(texture.fillRegion((__bridge MTL::CommandQueue*)proxy, frame, padded, 20, Rect{ 4, 4 }, false));
		CHECK(proxy.requests == 0);
		CHECK(frame.getCommandBuffer() == continuation.get());
		CHECK(continuation->status() == MTL::CommandBufferStatusNotEnqueued);
		event->setSignaledValue(1);
		REQUIRE(frame.flushAndWait(queue));
		CheckPixels(texture, Palette::Red);
		REQUIRE(texture.fill(queue, frame, padded, 20, true));
		REQUIRE(frame.flushAndWait(queue));
		CheckPixels(texture, Palette::Green);
	}
	TEST_CASE("DynamicTexture.Metal.row_copy_preserves_padding")
	{
		const Size size{ 3, 2 };
		for (const uint32 sourcePitch : { 3u, 5u, 9u })
		{
			Array<Byte> source(sourcePitch + 3, Byte{ 0xCD });
			for (uint32 y = 0; y < 2; ++y)
			{
				for (uint32 x = 0; x < 3; ++x) { source[y * sourcePitch + x] = static_cast<Byte>(1 + y * 3 + x); }
			}
			const auto original = source;
			CHECK(GetTextureUploadRowBytes(size, TextureFormat::R8_Unorm, source, sourcePitch) == 3);
			for (const uint32 destinationPitch : { 3u, 4u, 9u })
			{
				Array<Byte> destination(destinationPitch * 2 + 4, Byte{ 0xEF });
				FillWithImage(destination.data() + 2, size, destinationPitch, source, sourcePitch, 3);
				for (size_t i = 0; i < destination.size(); ++i)
				{
					const bool pixel = ((2 <= i) && (i < 5)) || ((2 + destinationPitch <= i) && (i < 5 + destinationPitch));
					const Byte expected = pixel ? static_cast<Byte>(1 + (i - 2) / destinationPitch * 3 + (i - 2) % destinationPitch) : Byte{ 0xEF };
					CHECK(destination[i] == expected);
				}
				CHECK(source == original);
			}
		}
	}

	TEST_CASE("DynamicTexture.Metal.raw_upload_benchmark", "[.benchmark]")
	{
		const auto qos = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		REQUIRE(qos);
		const ScopeExit restoreQoS{ [=] { pthread_override_qos_class_end_np(qos); } };
		REQUIRE(System::Update());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto& frame = renderer->getFrameContext();
		constexpr int32 Batches = 7;
		constexpr int32 Repetitions = 24;
		constexpr std::array<const char*, 4> Modes{ "typed", "packed-raw", "padded-raw", "repack-and-typed" };
		for (const Size size : { Size{ 63, 31 }, Size{ 1920, 1080 } })
		{
			Image source{ size, Color{ 32, 128, 224, 255 } };
			Image repacked{ size };
			const auto packed = std::as_bytes(std::span{ source });
			const uint32 rowBytes = source.bytesPerRow();
			const uint32 pitch = (rowBytes + 37);
			Array<Byte> padded((static_cast<size_t>(size.y) - 1) * pitch + rowBytes, Byte{ 0xCD });
			FillWithImage(padded.data(), size, pitch, packed, rowBytes, rowBytes);
			Array<Byte> heap(packed.size());
			DynamicTexture texture{ source };
			const auto originalID = texture.id();
			const RenderTexture output{ size };
			std::array<Array<double>, 4> timings;
			std::array<Array<double>, 2> copies;
			for (auto& times : timings) { times.reserve(Batches * Repetitions); }
			for (auto& times : copies) { times.reserve(Batches * Repetitions); }
			const auto update = [&](const size_t mode)
			{
				switch (mode)
				{
				case 0: return texture.fill(source);
				case 1: return texture.fill(packed, rowBytes);
				case 2: return texture.fill(padded, pitch);
				default:
					FillWithImage(repacked.data(), size, rowBytes, padded, pitch, rowBytes);
					return texture.fill(repacked);
				}
			};
			for (int32 batch = 0; batch < Batches; ++batch)
			{
				const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
				REQUIRE(System::Update());
				for (size_t layout = 0; layout < 2; ++layout)
				{
					const auto data = layout ? std::span<const Byte>{ padded } : packed;
					const uint32 sourcePitch = layout ? pitch : rowBytes;
					FillWithImage(heap.data(), size, rowBytes, data, sourcePitch, rowBytes);
					for (int32 i = 0; i < Repetitions; ++i)
					{
						const int64 start = Time::GetNanosec();
						FillWithImage(heap.data(), size, rowBytes, data, sourcePitch, rowBytes);
						std::atomic_signal_fence(std::memory_order_seq_cst);
						copies[layout].push_back((Time::GetNanosec() - start) / 1000.0);
					}
					CHECK(std::memcmp(heap.data(), packed.data(), heap.size()) == 0);
				}
				for (size_t order = 0; order < Modes.size(); ++order)
				{
					const size_t mode = ((order + batch) % Modes.size());
					REQUIRE(update(mode));
					for (int32 i = 0; i < Repetitions; ++i)
					{
						// Wait outside timing: the measured call copies into the actual Metal upload buffer and submits it.
						REQUIRE(frame.flushAndWait(renderer->getCommandQueue()));
						const int64 start = Time::GetNanosec();
						const bool success = update(mode);
						const int64 elapsed = (Time::GetNanosec() - start);
						REQUIRE(success);
						timings[mode].push_back(elapsed / 1000.0);
					}
					RenderTextureOps::Copy(texture, output);
					Image actual;
					REQUIRE(output.readAsImage(actual));
					CHECK(actual == source);
					CHECK(texture.id() == originalID);
				}
			}
			for (size_t mode = 0; mode < Modes.size(); ++mode)
			{
				auto& times = timings[mode];
				times.sort();
				std::cout << "raw-upload " << size.x << 'x' << size.y << ' ' << Modes[mode]
					<< " median_us=" << times[times.size() / 2] << " p95_us=" << times[times.size() * 95 / 100] << '\n';
			}
			for (size_t layout = 0; layout < copies.size(); ++layout)
			{
				auto& times = copies[layout];
				times.sort();
				std::cout << "heap-row-copy " << size.x << 'x' << size.y << (layout ? " padded" : " packed")
					<< " median_us=" << times[times.size() / 2] << " p95_us=" << times[times.size() * 95 / 100] << '\n';
			}
		}
	}

}
# endif
