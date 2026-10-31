//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "BezierTessellationStudy.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <ThirdParty/nanobench/nanobench.h>
# include <chrono>
# include <iostream>
# include <random>

namespace
{
	using namespace BezierStudy;

	struct MeshBuffer
	{
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
		uint32 vertexCount = 0, indexCount = 0;

		void build(const LineString& line, const float thickness = 6, const LineCap cap = LineCap::Round)
		{
			vertexCount = indexCount = 0;
			const auto allocate = [&](const uint16 nv, const uint32 ni)
			{
				vertices.resize(nv);
				indices.resize(ni);
				vertexCount += nv;
				indexCount += ni;
				return Vertex2DBufferPointer{ vertices.data(), indices.data(), 0 };
			};
			const uint32 emitted = Vertex2DBuilder::BuildLineString(allocate, cap, cap,
				std::span<const Vec2>{ line.data(), line.size() }, none, thickness, false, CloseRing::No, Float4{ 1, 1, 1, 1 }, 1);
			ankerl::nanobench::doNotOptimizeAway(emitted);
		}
	};

	struct RenderScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		MSAASampleCount msaa = Scene::GetMSAASampleCount();
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		ScopedColorMul2D colorMul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D colorAdd{ 0.0, ScopedColorAdd2D::Target::Set };
		ScopedRenderStates2D blend{ BlendState::Default2D };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		RenderScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 1000, 760 }));
			Scene::SetMSAASampleCount(MSAASampleCount::X4MSAA);
			Scene::SetBackground(ColorF{ 0.055 });
		}
		~RenderScope()
		{
			Scene::Configure(config);
			Scene::SetMSAASampleCount(msaa);
			Scene::SetBackground(background);
		}
	};

	template <class Draw>
	Image Capture(Draw&& draw)
	{
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}

	void ExportImage(const std::string& name, const Image& image)
	{
		// The report tool owns retained artifacts. Tests write only to stdout.
		std::cout << "BSTUDY_IMAGE\t" << name << '\t' << image.encodePNG().base64().getBase64() << '\n';
	}

	Curve Fitted(const Curve& curve, const Vec2 center, const double width, const double height)
	{
		const RectF bounds = curve.degree == 2 ? curve.quadratic().computeBoundingRect() : curve.cubic().computeBoundingRect();
		const double scale = Min(1.0, Min(width / Max(bounds.w, 1.0), height / Max(bounds.h, 1.0)));
		return curve.transformed(scale, scale, center - bounds.center() * scale);
	}

	LineString Reference(const Curve& curve, const int32 count = 4096)
	{
		return curve.degree == 2 ? curve.quadratic().toLineString(count) : curve.cubic().toLineString(count);
	}

	void DrawDensity(const Curve& curve, const double width, const ColorF& color, const double tolerance)
	{
		if (curve.degree == 2) curve.quadratic().draw(LineCap::Round, width, color, tolerance);
		else curve.cubic().draw(LineCap::Round, width, color, tolerance);
	}

	double ResidualChange(const Curve& a, const LineString& la, const Curve& b, const LineString& lb)
	{
		double maximum = 0;
		for (int32 i = 0; i <= 256; ++i)
		{
			const Vec2 pa = a.point(i / 256.0), pb = b.point(i / 256.0);
			maximum = Max(maximum, ((ClosestOnPolyline(la, pa) - pa) - (ClosestOnPolyline(lb, pb) - pb)).length());
		}
		return maximum;
	}

	Curve AnimationCurve(const int32 animation, const double u)
	{
		switch (animation)
		{
		case 0: return Bezier2{ { 0, 0 }, { 300, 10 + 65 * u }, { 600, 0 } };
		case 1: return Bezier3{ { 0, 0 }, { 200, 2 * (10 + 65 * u) / 3 }, { 400, 2 * (10 + 65 * u) / 3 }, { 600, 0 } };
		case 2: return Bezier3{ { 0, 0 }, { 200, 50 + 450 * u }, { 400, -50 - 450 * u }, { 600, 0 } };
		case 3: return Bezier3{ { 0, 0 }, { 200, 200 }, { -200 + 100 * (u - 0.5), 200 }, { 400, 0 } };
		case 4: return Bezier2{ { 0, 0 }, { 600, 2 * (u - 0.5) }, { 40, 0 } };
		case 5: return Bezier2{ { 0, 0 }, { 25, 0.25 + 0.5 * u }, { 600, 0 } };
		default: return Bezier3{ { 0, 0 }, { 0, 0.25 + 0.5 * u }, { 0, -0.25 - 0.5 * u }, { 600, 0 } };
		}
	}

	double Quantile(Array<double> values, const double q)
	{
		values.sort();
		return values[static_cast<size_t>((values.size() - 1) * q)];
	}
}

TEST_CASE("BezierTessellation.Candidates.EndpointsAndBudget")
{
	Workspace work;
	LineString line;
	line.reserve(4096);
	const auto* storage = line.data();
	for (const auto& item : Cases())
		for (const auto method : { Method::Wang, Method::Density })
			for (const uint32 budget : { 1u, 2u, 7u, 32u, 256u })
			{
				CAPTURE(item.name, static_cast<int>(method), budget);
				Flatten(line, work, item.curve, method, 0.5, budget);
				REQUIRE(not line.isEmpty());
				CHECK(line.front() == item.curve.p[0]);
				CHECK(line.back() == item.curve.p[item.curve.degree]);
				CHECK(line.size() <= budget + 1);
				CHECK(line.data() == storage);
				for (size_t i = 0; i < line.size(); ++i)
				{
					CHECK(std::isfinite(line[i].x));
					CHECK(std::isfinite(line[i].y));
					if (i) CHECK(line[i - 1] != line[i]);
				}
			}
	for (const auto method : { Method::Wang, Method::Density })
	{
		Flatten(line, work, Curve{ Bezier3{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 100, 0 } } }, method);
		CHECK(line.size() == 2);
	}
}

TEST_CASE("BezierTessellation.Candidates.OrdinaryError")
{
	Workspace work;
	LineString line;
	for (const auto& item : Cases())
	{
		if (not item.ordinary) continue;
		for (const auto method : { Method::Wang, Method::Density })
			for (const double tolerance : { 0.25, 0.5, 1.0, 2.0 })
			{
				CAPTURE(item.name, static_cast<int>(method), tolerance);
				Flatten(line, work, item.curve, method, tolerance);
				const double error = SampledError(item.curve, line);
				// Study acceptance margin, not a proposed public error guarantee.
				CHECK(error <= tolerance * 1.3 + 1e-8);
			}
	}
}

TEST_CASE("BezierTessellation.Candidates.CollinearLimit")
{
	Workspace work;
	LineString exact, nearby;
	for (const double tolerance : { 0.25, 0.5, 1.0, 2.0 })
	{
		const Curve curve{ Bezier2{ { 0, 0 }, { 600, 0 }, { 40, 0 } } };
		Flatten(exact, work, curve, Method::Density, tolerance);
		CHECK(SampledError(curve, exact) <= tolerance);
		for (const double height : { -0.01, -0.00001, 0.00001, 0.01 })
		{
			const Curve near{ Bezier2{ { 0, 0 }, { 600, height }, { 40, 0 } } };
			Flatten(nearby, work, near, Method::Density, tolerance);
			REQUIRE(nearby.size() == exact.size());
			for (size_t i = 0; i < exact.size(); ++i) CHECK(nearby[i].distanceFrom(exact[i]) < 0.1);
		}
	}
}

TEST_CASE("BezierTessellation.GeometryStudy", "[.bezier-study]")
{
	Workspace work;
	LineString line;
	MeshBuffer mesh;
	const auto previousPrecision = std::cout.precision(10);
	const ScopeExit restorePrecision{ [previousPrecision] { std::cout.precision(previousPrecision); } };
	for (const auto& item : Cases())
		for (const double tolerance : { 0.25, 0.48, 0.5, 0.75, 1.0, 1.5, 2.0 })
			for (const Method method : Methods)
			{
				Flatten(line, work, item.curve, method, tolerance);
				const double error = SampledError(item.curve, line);
				std::cout << "BSTUDY\tgeometry\t" << item.name << '\t' << Names[static_cast<int>(method)] << '\t'
					<< item.ordinary << '\t' << item.curve.degree << '\t' << tolerance << '\t' << line.size() - 1 << '\t'
					<< error << '\t' << work.limited;
				for (const float width : { 1.f, 6.f, 24.f })
				{
					mesh.build(line, width);
					std::cout << '\t' << mesh.vertexCount << '\t' << mesh.indexCount / 3;
				}
				std::cout << '\n';
			}
	for (const double tolerance : { 0.5, 0.75, 1.0 })
	for (const int32 animation : { 0, 1, 2, 3, 4, 5, 6 })
		for (const Method method : Methods)
		{
			Curve previous = AnimationCurve(animation, 0);
			LineString previousLine;
			Flatten(previousLine, work, previous, method, tolerance);
			for (int32 frame = 1; frame <= 1000; ++frame)
			{
				const Curve curve = AnimationCurve(animation, frame / 1000.0);
				Flatten(line, work, curve, method, tolerance);
				const double residual = ResidualChange(previous, previousLine, curve, line);
				std::cout << "BSTUDY\ttemporal\t" << tolerance << '\t' << animation << '\t' << Names[static_cast<int>(method)] << '\t'
					<< frame << '\t' << line.size() - 1 << '\t' << residual << '\n';
				previous = curve;
				previousLine = line;
			}
		}
	// Deterministic finite geometry, independent of the hand-selected corpus.
	std::mt19937_64 random{ 20261014 };
	std::uniform_real_distribution<double> coordinate{ -400, 400 };
	for (int32 i = 0; i < 512; ++i)
	{
		const auto point = [&] { return Vec2{ coordinate(random), coordinate(random) }; };
		const Vec2 a = point(), b = point(), c = point(), d = point();
		const Curve curve = i % 2 ? Curve{ Bezier3{ a, b, c, d } } : Curve{ Bezier2{ a, b, c } };
		for (const auto method : Methods)
		{
			Flatten(line, work, curve, method, 0.75);
			const double error = SampledError(curve, line, 2048);
			CHECK(std::isfinite(error));
			std::cout << "BSTUDY\trandom\t" << i << '\t' << Names[static_cast<int>(method)] << '\t'
				<< curve.degree << '\t' << line.size() - 1 << '\t' << error << '\n';
		}
	}
}

TEST_CASE("BezierTessellation.Benchmark", "[.benchmark]")
{
	Workspace work;
	LineString line;
	MeshBuffer mesh;
	Array<Curve> curves;
	for (const auto& item : Cases())
	{
		if (not item.ordinary) continue;
		for (int32 i = 0; i < 16; ++i)
		{
			Curve curve = item.curve.transformed(0.5 + i / 16.0, 0.5 + i / 16.0);
			curve.p[1].y += i * 0.01;
			curves.push_back(curve);
		}
	}
	constexpr int32 Repetitions = 64, Rounds = 11;
	for (const int32 degree : { 2, 3 })
	for (const double tolerance : { 0.5, 0.75, 1.0, 1.5 })
		for (const int32 workload : { 0, 1, 2 }) // allocation, reusable output, reusable output + production mesh builder
		{
			std::array<Array<double>, 4> times;
			std::array<double, 4> counts{};
			for (int32 round = -2; round < Rounds; ++round)
				for (int32 offset = 0; offset < 4; ++offset)
				{
					const int32 index = (offset + round + 4) % 4;
					const Method method = Methods[index];
					double checksum = 0;
					const auto begin = std::chrono::steady_clock::now();
					for (int32 repeat = 0; repeat < Repetitions; ++repeat)
						for (const Curve& curve : curves)
						{
							if (curve.degree != degree) continue;
							if (workload == 0)
							{
								LineString temporary;
								Flatten(temporary, work, curve, method, tolerance);
								checksum += temporary.size() + temporary[temporary.size() / 2].x;
								ankerl::nanobench::doNotOptimizeAway(temporary.data());
							}
							else
							{
								Flatten(line, work, curve, method, tolerance);
								if (workload == 2) mesh.build(line);
								checksum += line.size() + line[line.size() / 2].x;
								ankerl::nanobench::doNotOptimizeAway(line.data());
							}
						}
					const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / (Repetitions * curves.size() / 2);
					ankerl::nanobench::doNotOptimizeAway(checksum);
					if (round >= 0) times[index].push_back(ns);
					counts[index] = checksum;
				}
			for (int32 index = 0; index < 4; ++index)
			{
				CHECK(counts[index] > 0);
				std::cout << "BSTUDY\tbenchmark\t" << degree << '\t' << workload << '\t' << Names[index] << '\t' << tolerance << '\t'
					<< Quantile(times[index], 0.5) << '\t' << Quantile(times[index], 0.1) << '\t' << Quantile(times[index], 0.9) << '\n';
			}
		}
}

TEST_CASE("BezierTessellation.RasterStudy", "[.bezier-study]")
{
	const RenderScope restore;
	const Font font{ 16 };
	Workspace work;
	LineString line;
	const auto cases = Cases();
	for (const int32 index : { 0, 1, 2, 3, 10, 11, 12, 16, 18 })
		for (const double tolerance : { 0.5, 0.75, 1.0, 1.5 })
		{
			const auto& item = cases[index];
			const Image image = Capture([&]
			{
				font(U"{}  | candidate tolerance {} px | 4x MSAA"_fmt(Unicode::FromUTF8(item.name), tolerance)).draw(24, 12);
				for (int32 row = 0; row < 4; ++row)
				{
					const int32 y = 62 + row * 170;
					font(row == 0 ? U"4096 segments" : row == 1 ? U"Current 0.48 / depth 5" : row == 2 ? U"Wang" : U"Density").draw(24, y);
					for (int32 column = 0; column < 3; ++column)
					{
						const float width = std::array{ 1.f, 6.f, 24.f }[column];
						const Curve curve = Fitted(item.curve, { 285.0 + column * 270, y + 85.0 }, 220, 95);
						if (row == 0) line = Reference(curve);
						else Flatten(line, work, curve, row == 1 ? Method::Current : row == 2 ? Method::Wang : Method::Density, row == 1 ? 0.48 : tolerance);
						if (row == 3) DrawDensity(curve, width, ColorF{ 0.85, 0.92, 1 }, tolerance);
						else line.draw(LineCap::Round, width, ColorF{ 0.85, 0.92, 1 });
						font(U"width={}  segments={}"_fmt(width, line.size() - 1)).draw(190 + column * 270, y + 140);
					}
				}
			});
			ExportImage(item.name + std::string{ "-" } + std::to_string(tolerance), image);
		}
	// Actual raster differences against a dense polyline, with the same stroke builder.
	Scene::SetBackground(Palette::Black);
	for (const int32 index : { 1, 3, 10, 11, 12, 16, 18 })
	{
		const Curve curve = Fitted(cases[index].curve, { 500, 380 }, 700, 500);
		for (const double width : { 1.0, 6.0, 24.0 })
		{
			const Image reference = Capture([&] { Reference(curve).draw(LineCap::Round, width); });
			for (const auto method : { Method::Current, Method::Wang, Method::Density })
				for (const double tolerance : { 0.5, 0.75, 1.0, 1.5 })
				{
					Flatten(line, work, curve, method, method == Method::Current ? 0.48 : tolerance);
					const Image actual = Capture([&]
					{
						if (method == Method::Density) DrawDensity(curve, width, Palette::White, tolerance);
						else line.draw(LineCap::Round, width);
					});
					REQUIRE(actual.size() == reference.size());
					double difference = 0, mass = 0;
					int32 changed = 0;
					for (size_t i = 0; i < actual.size_bytes() / sizeof(Color); ++i)
					{
						const int32 a = actual.data()[i].r, b = reference.data()[i].r;
						difference += Abs(a - b) / 255.0;
						mass += b / 255.0;
						changed += Abs(a - b) > 64;
					}
					std::cout << "BSTUDY\traster\t" << cases[index].name << '\t' << Names[static_cast<int>(method)] << '\t'
						<< width << '\t' << tolerance << '\t' << difference / Max(mass, 1.0) << '\t' << changed << '\n';
				}
		}
	}
}

TEST_CASE("BezierTessellation.SubmissionBenchmark", "[.benchmark]")
{
	const RenderScope restore;
	Workspace work;
	LineString line;
	MeshBuffer mesh;
	Array<Curve> curves;
	for (const auto& item : Cases())
	{
		if (not item.ordinary) continue;
		for (int32 i = 0; i < 16; ++i) curves.push_back(item.curve.transformed(0.5 + i / 16.0, 0.5 + i / 16.0));
	}
	std::array<Array<double>, 4> timings;
	std::array<int64, 4> triangles{};
	for (int32 index = 0; index < 4; ++index)
	{
		for (const auto& curve : curves)
		{
			Flatten(line, work, curve, Methods[index], 0.5);
			mesh.build(line);
			triangles[index] += 4 * (mesh.indexCount / 3);
		}
	}
	for (int32 round = -8; round < 48; ++round)
		for (int32 offset = 0; offset < 4; ++offset)
		{
			const int32 index = (offset + round + 8) % 4;
			REQUIRE(System::Update());
			{
				// Isolate CPU submission without a large overlapping-fragment workload.
				// The geometry is still submitted and transformed; this is not a GPU benchmark.
				const Transformer2D transform{ Mat3x2::Translate(2000, 2000) };
				const auto begin = std::chrono::steady_clock::now();
				for (int32 repeat = 0; repeat < 4; ++repeat)
					for (const auto& curve : curves)
					{
						if (Methods[index] == Method::Density)
						{
							DrawDensity(curve, 6, Palette::White, 0.5);
						}
						else
						{
							Flatten(line, work, curve, Methods[index], 0.5);
							line.draw(LineCap::Round, 6);
						}
					}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / (4 * curves.size());
				if (round >= 0) timings[index].push_back(ns);
			}
			REQUIRE(System::Update());
			CHECK(Profiler::GetFrameMetrics().triangleCount == triangles[index]);
		}
	for (int32 index = 0; index < 4; ++index)
	{
		std::cout << "BSTUDY\tsubmission\t" << Names[index] << '\t' << 4 * curves.size() << '\t'
			<< Quantile(timings[index], 0.5) << '\t' << Quantile(timings[index], 0.1) << '\t' << Quantile(timings[index], 0.9)
			<< '\t' << triangles[index] << '\n';
	}
}

TEST_CASE("BezierTessellation.AnimationStudy", "[.bezier-study]")
{
	const RenderScope restore;
	const Font font{ 16 };
	Workspace work;
	LineString line;
	for (const int32 animation : { 2, 3 })
		for (int32 frame = 0; frame <= 60; ++frame)
		{
			const Curve input = AnimationCurve(animation, 0.15 + frame * 0.45 / 60);
			const Image image = Capture([&]
			{
				font(U"{}  | tolerance 0.5px | frame {} | 4x MSAA"_fmt(animation == 2 ? U"S curve" : U"Near cusp", frame)).draw(24, 12);
				for (int32 row = 0; row < 4; ++row)
				{
					const int32 y = 62 + row * 170;
					font(row == 0 ? U"4096 segments" : row == 1 ? U"Current 0.48 / depth 5" : row == 2 ? U"Wang" : U"Density").draw(24, y);
					for (int32 column = 0; column < 3; ++column)
					{
						const float width = std::array{ 1.f, 6.f, 24.f }[column];
						const Curve curve = Fitted(input, { 285.0 + column * 270, y + 85.0 }, 220, 95);
						if (row == 0) line = Reference(curve);
						else Flatten(line, work, curve, row == 1 ? Method::Current : row == 2 ? Method::Wang : Method::Density, row == 1 ? 0.48 : 0.5);
						if (row == 3) DrawDensity(curve, width, ColorF{ 0.85, 0.92, 1 }, 0.5);
						else line.draw(LineCap::Round, width, ColorF{ 0.85, 0.92, 1 });
						font(U"width={}  segments={}"_fmt(width, line.size() - 1)).draw(190 + column * 270, y + 140);
					}
				}
			});
			ExportImage("animation-" + std::to_string(animation) + "-" + std::to_string(frame), image);
		}
}
