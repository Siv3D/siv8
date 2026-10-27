//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

// macOS-only CPU upload diagnostic. Build instructions: docs/renderer2d/README.md.
# import <Foundation/Foundation.h>
# import <Metal/Metal.h>
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>
# include <algorithm>
# include <chrono>
# include <cmath>
# include <cstring>
# include <iostream>
# include <vector>
# include <time.h>

namespace
{
	using namespace s3d;
	using Index = Vertex2D::IndexType;
	constexpr size_t Slots = 32;
	constexpr Index Offset = 37;

	// Representative copy kernels; the actual shared builder is measured below too.
	[[gnu::noinline]]
	void CopyThenAdd(Index* dst, const Index* src, const size_t count, const Index offset)
	{
		std::memcpy(dst, src, count * sizeof(Index));
		for (size_t i = 0; i < count; ++i) *dst++ += offset;
	}

	[[gnu::noinline]]
	void CopyWithOffset(Index* dst, const Index* src, const size_t count, const Index offset)
	{
		for (size_t i = 0; i < count; ++i) dst[i] = static_cast<Index>(src[i] + offset);
	}

	int64 ThreadTime()
	{
		timespec t{};
		if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) != 0) std::abort();
		return (int64(t.tv_sec) * 1'000'000'000 + t.tv_nsec);
	}

	template <class Run>
	void Measure(const char* name, Run run)
	{
		constexpr int32 Loops = 1024;
		std::vector<double> wall, cpu;
		for (int32 round = -3; round < 11; ++round)
		{
			const int64 cpuBegin = ThreadTime();
			const auto begin = std::chrono::steady_clock::now();
			for (int32 i = 0; i < Loops; ++i)
			{
				run(i);
				asm volatile("" ::: "memory");
			}
			const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count();
			const double cpuNs = double(ThreadTime() - cpuBegin);
			if (round >= 0)
			{
				wall.push_back(ns / Loops);
				cpu.push_back(cpuNs / Loops);
			}
		}
		std::sort(wall.begin(), wall.end());
		std::sort(cpu.begin(), cpu.end());
		std::cout << name << " wall_ns=" << wall[5] << " cpu_ns=" << cpu[5]
			<< " p10=" << wall[1] << " p90=" << wall[9] << '\n';
	}
}

int main()
{
	@autoreleasepool
	{
		id<MTLDevice> device = MTLCreateSystemDefaultDevice();
		if (not device) return 2;
		std::cout << "device=" << [[device name] UTF8String] << '\n';
		for (const size_t count : { 6u, 72u, 768u })
		{
			const size_t stride = ((count + 63) / 64 * 64);
			std::vector<Index> source(count), ram(stride * Slots);
			for (size_t i = 0; i < count; ++i) source[i] = static_cast<Index>(i % 24);
			id<MTLBuffer> normal = [device newBufferWithLength:ram.size() * sizeof(Index)
				options:MTLResourceStorageModeShared | MTLResourceCPUCacheModeDefaultCache];
			id<MTLBuffer> combined = [device newBufferWithLength:ram.size() * sizeof(Index)
				options:MTLResourceStorageModeShared | MTLResourceCPUCacheModeWriteCombined];
			if ((not normal) || (not combined)) return 3;
			Index* destinations[]{ ram.data(), static_cast<Index*>(normal.contents), static_cast<Index*>(combined.contents) };
			const char* names[]{ "RAM", "MetalDefault", "MetalWriteCombined" };
			std::cout << "count=" << count << '\n';
			for (size_t memory = 0; memory < 3; ++memory)
			for (int32 method = 0; method < 2; ++method)
			{
				const auto kernel = (method ? CopyWithOffset : CopyThenAdd);
				std::cout << names[memory] << ' ';
				Measure((method ? "source_plus_offset" : "copy_then_add"), [&](const int32 i)
				{
					kernel(destinations[memory] + (i % Slots) * stride, source.data(), count, Offset);
				});
				// Read/validate after timing; no GPU ever accesses these resources.
				for (size_t slot = 0; slot < Slots; ++slot)
				for (size_t i = 0; i < count; ++i)
				{
					if (destinations[memory][slot * stride + i] != (source[i] + Offset)) return 4;
				}
			}
			[normal release];
			[combined release];
		}

		// Invoke the actual shared builder with identical geometry and allocation.
		std::vector<Float2> source(26);
		std::vector<TriangleIndex> triangles(24);
		for (size_t i = 0; i < source.size(); ++i)
		{
			const double angle = (i * 6.283185307179586 / source.size());
			source[i] = { float(std::cos(angle)), float(std::sin(angle)) };
		}
		for (size_t i = 0; i < triangles.size(); ++i) triangles[i] = { 0, Index(i + 1), Index(i + 2) };
		for (int32 mode = 0; mode < 2; ++mode)
		{
			const auto options = (MTLResourceStorageModeShared
				| (mode ? MTLResourceCPUCacheModeWriteCombined : MTLResourceCPUCacheModeDefaultCache));
			id<MTLBuffer> vb = [device newBufferWithLength:sizeof(Vertex2D) * source.size() * Slots options:options];
			id<MTLBuffer> ib = [device newBufferWithLength:sizeof(TriangleIndex) * triangles.size() * Slots options:options];
			if ((not vb) || (not ib)) return 5;
			auto* vertices = static_cast<Vertex2D*>(vb.contents);
			auto* indices = static_cast<Index*>(ib.contents);
			Measure((mode ? "BuildPolygon_WriteCombined" : "BuildPolygon_DefaultCache"), [&](const int32 i)
			{
				const auto allocate = [&](uint16, uint32)
				{
					return Vertex2DBufferPointer{ vertices + (i % Slots) * source.size(),
						indices + (i % Slots) * triangles.size() * 3, Offset };
				};
				const auto count = Vertex2DBuilder::BuildPolygon(allocate, source, triangles, none, Float4{ 1, 1, 1, 1 });
				asm volatile("" : : "g"(count) : "memory");
			});
			for (size_t slot = 0; slot < Slots; ++slot)
			{
				for (size_t i = 0; i < triangles.size(); ++i)
				{
					const auto* triangle = (indices + (slot * triangles.size() + i) * 3);
					if ((triangle[0] != Offset) || (triangle[1] != i + 1 + Offset) || (triangle[2] != i + 2 + Offset)) return 6;
				}
				for (size_t i = 0; i < source.size(); ++i)
				{
					const auto& vertex = vertices[slot * source.size() + i];
					if ((vertex.pos != source[i]) || (vertex.color != Float4{ 1, 1, 1, 1 })) return 7;
				}
			}
			[ib release];
			[vb release];
		}
		[device release];
	}
}
