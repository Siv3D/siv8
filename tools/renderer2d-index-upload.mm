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
# include <Siv3D/Renderer2D/CircleArcArrow.hpp>
# include <Siv3D/Renderer2D/RoundRectBalloon.hpp>
# include <Siv3D/Geometry2D/CircularShapes.hpp>
# include <Siv3D/LineCap.hpp>
# include <Siv3D/LineStyle.hpp>
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
		for (const auto axes : { Float2{ 120, 120 }, Float2{ 120, 60 }, Float2{ 120, 12 } })
		for (const auto preset : { LineStyle::DashedFlat, LineStyle::RoundDot })
		{
			const std::string name = ("StyledArc_" + std::to_string(int(axes.y)) + "_" + std::to_string(int(preset.type)));
			MeasureBuilder(device, name.c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildArcStyled(allocate, preset(0.25), { 10, 20 }, axes.x, axes.y,
					0.3f, 4.5f, 3, 5, Float4{ 0.2f, 0.4f, 0.6f, 0.5f }, 1);
			});
		}
		for (const float radius : { 0.0f, 2.0f, 20.0f, 50.0f })
		for (const auto preset : { LineStyle::DashedFlat, LineStyle::RoundDot })
		{
			const std::string name = ("StyledRoundRect_" + std::to_string(int(radius)) + "_" + std::to_string(int(preset.type)));
			MeasureBuilder(device, name.c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildRoundRectFrameStyled(allocate, preset(-0.5), { 13, 23, 207, 117 }, Max(radius - 3, 0.0f),
					{ 5, 15, 215, 125 }, Min(radius + 5, 55.0f), 8, Float4{ 0.2f, 0.4f, 0.6f, 0.5f }, 1);
			});
		}
		for (const bool clipped : { false, true })
		{
			MeasureBuilder(device, (clipped ? "RoundDotClipped" : "RoundDot"), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildRoundDot(allocate, { 10, 20 }, { 16, 24 }, 8,
					(clipped ? Float2{ 0.25f, 0.875f } : Float2{ 0, 1 }), Float4{ 0.2f, 0.4f, 0.6f, 0.5f });
			});
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
		for (const float radius : { 1.0f, 40.0f, 1000.0f })
		{
			const std::string suffix = std::to_string(static_cast<int>(radius));
			MeasureBuilder(device, ("CircleFrame" + suffix).c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildCircleFrame(allocate, { 160, 120 }, radius * 0.9f, radius * 0.2f,
					Float4{ 1, 1, 1, 1 }, Float4{ 1, 1, 1, 1 }, 1);
			});
			MeasureBuilder(device, ("EllipseFrame" + suffix).c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildEllipseFrame(allocate, { 160, 120 }, radius, radius * 0.5f, radius * 0.1f, radius * 0.1f,
					Float4{ 1, 1, 1, 1 }, Float4{ 1, 1, 1, 1 }, 1);
			});
			for (const bool fill : { false, true })
			{
				MeasureBuilder(device, ((fill ? "ShadowFill" : "ShadowRing") + suffix).c_str(), [&](const BufferCreatorFunc& allocate)
				{
					return Vertex2DBuilder::BuildCircleShadow(allocate, Circle{ 160, 120, radius }, radius * 0.25f,
						Float4{ 0, 0, 0, 0.5f }, 1, fill);
				});
			}
		}
		for (const size_t count : { 3u, 40u, 512u })
		for (const bool sharp : { false, true })
		{
			std::vector<Vec2> points(count);
			for (size_t i = 0; i < count; ++i)
			{
				points[i] = { double(i * 8), sharp ? double((i % 2) * 32) : 20 * std::sin(i * 0.1) };
			}
			for (const bool gradient : { false, true })
			{
				const std::string name = (std::string(sharp ? "Sharp" : "Smooth") + (gradient ? "Gradient" : "Solid") + std::to_string(count));
				MeasureBuilder(device, name.c_str(), [&](const BufferCreatorFunc& allocate)
				{
					const Float4 red{ 1, 0, 0, 1 }, blue{ 0, 0, 1, 1 };
					return gradient ? Vertex2DBuilder::BuildLineString(allocate, LineCap::Flat, LineCap::Flat, points, none, 4, false, red, blue, 1)
						: Vertex2DBuilder::BuildLineString(allocate, LineCap::Flat, LineCap::Flat, points, none, 4, false, CloseRing::No, red, 1);
				});
			}
		}
		for (const double shaftLength : { 0.0, 0.1, 100.0 })
		{
			const std::array<Vec2, 2> points{ Vec2{ 100, 100 }, Vec2{ 100 + shaftLength, 100 } };
			const std::span<const Vec2> shaft{ points.data(), (shaftLength == 0 ? size_t{ 1 } : size_t{ 2 }) };
			MeasureBuilder(device, ("DoubleHeadedArrowShaft" + std::to_string(shaftLength)).c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(allocate, shaft,
					{ 60, 110 }, { static_cast<float>(140 + shaftLength), 110 }, 12, 60, 60, Float4{ 1, 1, 1, 0.5f }, 1).value();
			});
		}
		struct ArcCase { const char* name; double radius, sweep; SizeF head; };
		for (const auto& c : { ArcCase{ "Ordinary", 100, 4.7, { 40, 32 } },
			ArcCase{ "Short", 100, 0.2, { 40, 80 } }, ArcCase{ "Full", 100, Math::TwoPi, { 40, 40 } },
			ArcCase{ "Omitted", 100, 4.7, { 0.001, 100 } }, ArcCase{ "Maximum", 1000, 4.7, { 60, 80 } } })
		{
			const Circle circle{ 200, 160, c.radius };
			MeasureBuilder(device, (std::string("CircleArcReference") + c.name).c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildCircleArc(allocate, LineCap::Flat, circle.center, static_cast<float>(c.radius - 4),
					0.3f, static_cast<float>(c.sweep), 8, ColorFillDirection::InOut, Float4{ 1, 1, 1, 1 }, Float4{ 1, 1, 1, 1 }, 1);
			});
			Array<Vec2> points;
			MeasureBuilder(device, (std::string("CircleArcArrow") + c.name).c_str(), [&](const BufferCreatorFunc& allocate)
			{
				const auto p = detail::CircleArcArrow::Place(circle, 0.3, c.sweep, c.head.y);
				detail::CircleArcArrow::WritePoints(points, *p, 8, 1);
				const auto result = Vertex2DBuilder::BuildLineStringArrow(allocate, LineCap::Flat, points, p->tip,
					8, (8 + static_cast<float>(c.head.x * p->sizeScale)), Float4{ 1, 1, 1, 1 }, 1);
				if (result) return *result;
				detail::CircleArcArrow::WritePoints(points, *p, 8, 1, true);
				return Vertex2DBuilder::BuildLineString(allocate, LineCap::Flat, LineCap::Flat, points, none, 8, false, CloseRing::No, Float4{ 1, 1, 1, 1 }, 1);
			});
			MeasureBuilder(device, (std::string("CircleArcDoubleHeadedArrow") + c.name).c_str(), [&](const BufferCreatorFunc& allocate)
			{
				const auto p = detail::CircleArcArrow::PlaceDouble(circle, 0.3, c.sweep, c.head.y);
				detail::CircleArcArrow::WriteDoublePoints(points, *p, 8, 1);
				const float width = (8 + static_cast<float>(c.head.x * p->sizeScale));
				const auto result = Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(allocate, points,
					p->arc.start, p->arc.tip, 8, width, width, Float4{ 1, 1, 1, 1 }, 1);
				if (result) return *result;
				detail::CircleArcArrow::WritePoints(points, p->arc, 8, 1, true);
				return Vertex2DBuilder::BuildLineString(allocate, LineCap::Flat, LineCap::Flat, points, none, 8, false, CloseRing::No, Float4{ 1, 1, 1, 1 }, 1);
			});
		}
		Measure("CircleArcArrowPlacement", [&](const int32 i)
		{
			const auto p = detail::CircleArcArrow::Place(Circle{ 200, 160, 100.0 + i % 7 }, 0.3 + i * 0.001, 4.7, 20.0 + i % 97);
			asm volatile("" : : "g"(&p) : "memory");
		});
		struct CircularCase { const char* name; double radius, width, tipDistance; };
		for (const auto& c : { CircularCase{ "ordinary", 64, 36, 110 }, CircularCase{ "thin", 64, 0.1, 64.01 },
			CircularCase{ "near_circle", 64, 127.9, 64.001 }, CircularCase{ "circle", 64, 128, 40 },
			CircularCase{ "maximum", 1000, 1999, 2500 } })
		{
			const Circle circle{ 200, 160, c.radius };
			MeasureBuilder(device, (std::string("Crescent_") + c.name).c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildCircleCrescent(allocate, circle, 0.7, c.width, Float4{ 1, 1, 1, 0.5f }, 1);
			});
			MeasureBuilder(device, (std::string("Teardrop_") + c.name).c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildCircleTeardrop(allocate, circle, circle.center + Vec2{ c.tipDistance, 0 },
					Float4{ 1, 1, 1, 0.5f }, 1);
			});
		}
		for (const auto size : { Vec2{ 80, 24 }, Vec2{ 80, 0.1 }, Vec2{ 80, 79.999 }, Vec2{ 500, 500 } })
		{
			const auto lens = detail::CircularShapes::PlaceLens({ -size.x, 0 }, { size.x, 0 }, size.y);
			const auto quality = static_cast<uint16>(QualityFactor{}.toPointsPerCircle(size.x).value());
			const uint16 count = detail::CircularShapes::VertexCount(*lens, quality);
			std::vector<Float2> vertices(count);
			std::vector<TriangleIndex> triangles(count - 2);
			size_t next = 0;
			detail::CircularShapes::Write(*lens, quality,
				[&](uint16 i, const Float2& p) { vertices[i] = p; },
				[&](uint16 a, uint16 b, uint16 c) { triangles[next++] = { a, b, c }; });
			const std::string name = ("Lens_" + std::to_string(size.x) + "_" + std::to_string(size.y));
			MeasureBuilder(device, (name + "_fill").c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildPolygon(allocate, vertices, triangles, none, Float4{ 1, 1, 1, 0.5f });
			});
			MeasureBuilder(device, (name + "_frame").c_str(), [&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildShape2DFrame(allocate, vertices, none, 2, Float4{ 1, 1, 1, 0.5f }, 1);
			});
		}
		struct BalloonCase { const char* name; RoundRect body; Vec2 target; double width; float scale; };
		const BalloonCase balloons[]{
			{ "ordinary", { 40, 40, 160, 100, 20 }, { 260, 90 }, 40, 1 },
			{ "square", { 40, 40, 160, 100, 0 }, { 260, 90 }, 40, 1 },
			{ "clamped", { 40, 40, 160, 100, 20 }, { 280, -10 }, 1000, 1 },
			{ "maximum", { 40, 40, 160, 100, 20 }, { 260, 90 }, 40, 512 } };
		for (const auto& c : balloons)
		for (const bool frame : { false, true })
		{
			const auto placement = detail::RoundRectBalloon::Place(c.body, c.target, c.width);
			MeasureBuilder(device, (std::string("RoundRectBalloon_") + c.name + (frame ? "_frame" : "_fill")).c_str(),
				[&](const BufferCreatorFunc& allocate)
			{
				if (frame) return Vertex2DBuilder::BuildRoundRectBalloonFrame(allocate, *placement, 8, Float4{ 1, 1, 1, 1 }, c.scale);
				return Vertex2DBuilder::BuildRoundRectBalloon(allocate, *placement, Float4{ 1, 1, 1, 1 }, c.scale);
			});
		}
		Measure("RoundRectBalloonPlacement", [&](const int32 i)
		{
			const auto p = detail::RoundRectBalloon::Place(RoundRect{ 40, 40, 160, 100, 12.0 + i % 25 }, { 260, 30.0 + i % 120 }, 40);
			asm volatile("" : : "g"(&p) : "memory");
		});
		[device release];
	}
}
