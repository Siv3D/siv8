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
# include <Siv3D/LineCap.hpp>
# include <algorithm>
# include <chrono>
# include <cmath>
# include <cstring>
# include <iostream>
# include <string>
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

	template <class Build>
	void MeasureBuilder(id<MTLDevice> device, const char* name, Build build)
	{
		Vertex2D sentinel;
		sentinel.set(-123, -456, -7, -8, Float4{ -1, -2, -3, -4 });
		std::vector<Vertex2D> expectedVertices;
		std::vector<Index> expectedIndices;
		const auto capture = [&](uint16 vertices, uint32 indices)
		{
			expectedVertices.assign(vertices, sentinel);
			expectedIndices.resize(indices);
			return Vertex2DBufferPointer{ expectedVertices.data(), expectedIndices.data(), Offset };
		};
		if (build(capture) != expectedIndices.size()) std::abort();
		std::cout << name << " vertices=" << expectedVertices.size() << " indices=" << expectedIndices.size() << '\n';
		std::vector<Vertex2D> ramVertices(expectedVertices.size() * Slots, sentinel);
		std::vector<Index> ramIndices(expectedIndices.size() * Slots);
		const char* modes[]{ "RAM", "DefaultCache", "WriteCombined" };
		for (int32 mode = 0; mode < 3; ++mode)
		{
			const auto options = (MTLResourceStorageModeShared
				| ((mode == 2) ? MTLResourceCPUCacheModeWriteCombined : MTLResourceCPUCacheModeDefaultCache));
			id<MTLBuffer> vb = nil, ib = nil;
			auto* vertices = ramVertices.data();
			auto* indices = ramIndices.data();
			if (mode)
			{
				vb = [device newBufferWithLength:sizeof(Vertex2D) * ramVertices.size() options:options];
				ib = [device newBufferWithLength:sizeof(Index) * ramIndices.size() options:options];
				if ((not vb) || (not ib)) std::abort();
				vertices = static_cast<Vertex2D*>(vb.contents);
				indices = static_cast<Index*>(ib.contents);
				std::fill_n(vertices, ramVertices.size(), sentinel);
			}
			Measure((std::string(name) + "_" + modes[mode]).c_str(), [&](const int32 i)
			{
				const auto allocate = [&](uint16, uint32)
				{
					return Vertex2DBufferPointer{ vertices + (i % Slots) * expectedVertices.size(),
						indices + (i % Slots) * expectedIndices.size(), Offset };
				};
				const auto count = build(allocate);
				asm volatile("" : : "g"(count) : "memory");
			});
			// Validate all written attributes and indices outside the timed region.
			for (size_t slot = 0; slot < Slots; ++slot)
			{
				for (size_t i = 0; i < expectedVertices.size(); ++i)
				{
					const auto& actual = vertices[slot * expectedVertices.size() + i];
					const auto& expected = expectedVertices[i];
					if ((actual.pos != expected.pos) || (actual.tex != expected.tex) || (actual.color != expected.color)) std::abort();
				}
				if (not std::equal(expectedIndices.begin(), expectedIndices.end(), indices + slot * expectedIndices.size())) std::abort();
			}
			[ib release];
			[vb release];
		}
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
		std::vector<Vertex2D> meshVertices(source.size());
		for (size_t i = 0; i < source.size(); ++i)
		{
			meshVertices[i].set(source[i], Float2{ float(i) / source.size(), 0.75f }, Float4{ 1, 0.5f, 0.25f, 0.5f });
		}
		for (const bool moved : { false, true })
		{
			MeasureBuilder(device, (moved ? "MeshOffset" : "Mesh"), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildMesh2D(allocate, meshVertices, triangles, (moved ? Optional<Float2>{ Float2{ 20, 30 } } : none));
			});
		}
		for (const bool gradient : { false, true })
		{
			MeasureBuilder(device, (gradient ? "TexturedRoundRectGradient" : "TexturedRoundRect"), [&](const BufferCreatorFunc& allocate)
			{
				const FloatRect rect{ 10, 20, 210, 140 }, uv{ 0.8f, 0.9f, 0.2f, 0.3f };
				const Float4 color{ 0.2f, 0.4f, 0.6f, 0.5f };
				return gradient ? Vertex2DBuilder::BuildTexturedRoundRect(allocate, rect, 200, 120, 30, uv,
					ColorFillDirection::TopBottom, color, Float4{ 1, 1, 1, 1 }, 1)
					: Vertex2DBuilder::BuildTexturedRoundRect(allocate, rect, 200, 120, 30, uv, color, 1);
			});
		}
		MeasureBuilder(device, "SquareCircleArc", [&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildCircleArc(allocate, LineCap::Square, { 100, 100 }, 80, 0.3f, 2.0f, 12,
				ColorFillDirection::LeftRight, Float4{ 1, 0, 0, 0.5f }, Float4{ 0, 0, 1, 0.5f }, 1);
		});
		MeasureBuilder(device, "RectShadowHollow", [&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildRectShadow(allocate, { 10, 20, 210, 140 }, 20, Float4{ 0, 0, 0, 0.5f }, false);
		});
		MeasureBuilder(device, "EllipseArcGradient", [&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildEllipseArc(allocate, LineCap::Flat, LineCap::Flat, { 100, 100 }, 80, 40,
				0.3f, 2.0f, 6, 6, ColorFillDirection::LeftRight, Float4{ 1, 0, 0, 0.5f }, Float4{ 0, 0, 1, 0.5f }, 1);
		});
		// Polygon::drawFrame() uses the same closed line builder. Keep inputs
		// outside timing; exercise both smooth contours and inserted sharp joins.
		for (const size_t count : { 4u, 40u, 512u })
		for (const bool sharp : { false, true })
		{
			std::vector<Vec2> contour(count);
			for (size_t i = 0; i < count; ++i)
			{
				const double angle = (i * 6.283185307179586 / count);
				const double radius = ((sharp && (i % 2)) ? 40.0 : 100.0);
				contour[i] = { (160 + radius * std::cos(angle)), (120 + radius * std::sin(angle)) };
			}
			const std::string name = (std::string(sharp ? "SharpFrame" : "SmoothFrame") + std::to_string(count));
			MeasureBuilder(device, name.c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildLineString(allocate, LineCap::Square, LineCap::Square,
					contour, none, 8, false, CloseRing::Yes, Float4{ 1, 1, 1, 0.5f }, 1);
			});
		}
		[device release];
	}
}
