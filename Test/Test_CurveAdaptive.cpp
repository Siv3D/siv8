//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "BezierTessellationStudy.hpp"
# include <ThirdParty/nanobench/nanobench.h>
# include <chrono>
# include <iostream>

namespace
{
	LineString Joined(const Spline2D& path, const double error, const int32 budget)
	{
		LineString result;
		for (const auto& curve : path.segments())
		{
			for (const Vec2 p : curve.toLineStringAdaptive(error, budget))
			{
				if (result.isEmpty() || result.back() != p) result.push_back(p);
			}
		}
		if (path.isClosed() && result.size() > 1 && result.front() == result.back()) result.pop_back();
		return result;
	}

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		MSAASampleCount msaa = Scene::GetMSAASampleCount();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D mul{ Palette::White };
		ScopedColorAdd2D add{ 0.0 };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 320, 240 }));
			Scene::SetBackground(Palette::Black);
			Scene::SetMSAASampleCount(MSAASampleCount::X4MSAA);
		}
		~SceneScope()
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
}

TEST_CASE("CurveAdaptive.BezierConversion.QualityBudgetAndStorage")
{
	LineString output{ { -1, -1 }, { -2, -2 } };
	output.reserve(4097);
	const auto* storage = output.data();
	const auto check = [&](const auto& curve)
	{
		for (const double error : { 0.25, 0.5, 1.0 })
		for (const int32 budget : { 1, 2, 7, 256, 4096 })
		{
			curve.toLineStringAdaptive(output, error, budget);
			CHECK(output.data() == storage);
			CHECK(output == curve.toLineStringAdaptive(error, budget));
			CHECK(output.size() <= static_cast<size_t>(budget + 1));
			CHECK(output.front() == curve.p0);
			CHECK(output.back() == curve.pointAt(1));
			for (size_t i = 1; i < output.size(); ++i) CHECK(output[i] != output[i - 1]);
			if (budget == 256) CHECK(BezierStudy::SampledError(BezierStudy::Curve{ curve }, output, 1024) <= error * 1.3);
		}
	};
	check(Bezier2{ { 5, 20 }, { -20, 180 }, { 200, 0 } });
	check(Bezier2{ { 0, 0 }, { 300, 0 }, { 20, 0 } });
	check(Bezier3{ { 0, 0 }, { -100, 400 }, { 150, 0 }, { 600, 0 } });
	check(Bezier3{ { 0, 0 }, { 100, 100 }, { -100, 100 }, { 0, 0 } });
	check(Bezier2{ { 3, 7 }, { 3, 7 }, { 3, 7 } });
	check(Bezier3{ { 3, 7 }, { 3, 7 }, { 3, 7 }, { 3, 7 } });
}

TEST_CASE("CurveAdaptive.SplineConversion.JoinsClosureAndReuse")
{
	const Array<Bezier3> curves{
		{ { 0, 0 }, { 40, -80 }, { 80, 100 }, { 100, 0 } },
		{ { 100, 0 }, { 100, 0 }, { 100, 0 }, { 100, 0 } },
		{ { 100, 0 }, { 180, 100 }, { 30, 100 }, { 100, 0 } },
		{ { 100, 0 }, { 50, 0 }, { 0, 0 }, { 0, 0 } } };
	LineString output;
	output.reserve(4 * 4096 + 1);
	const auto* storage = output.data();
	for (const CloseRing closed : { CloseRing::No, CloseRing::Yes })
	{
		const auto path = Spline2D::FromBezierSegments(curves, closed);
		for (const int32 budget : { 1, 2, 7, 256, 4096 })
		{
			path.toLineStringAdaptive(output, 0.25, budget);
			CHECK(output == Joined(path, 0.25, budget));
			CHECK(output == path.toLineStringAdaptive(0.25, budget));
			CHECK(output.data() == storage);
			CHECK(output.size() <= curves.size() * budget + (not closed));
			CHECK(output.contains(Vec2{ 100, 0 }));
			CHECK(output.contains(Vec2{ 0, 0 }));
			for (size_t i = 1; i < output.size(); ++i) CHECK(output[i] != output[i - 1]);
			if (closed) CHECK(output.front() != output.back());
			else CHECK(output.front() == output.back()); // Open path returning to its start retains the endpoint.
		}
	}
	const auto degenerate = Spline2D::FromBezierSegments(Array<Bezier3>{
		{ { 7, 8 }, { 7, 8 }, { 7, 8 }, { 7, 8 } },
		{ { 7, 8 }, { 7, 8 }, { 7, 8 }, { 7, 8 } } }, CloseRing::Yes);
	CHECK(degenerate.toLineStringAdaptive() == LineString{ Vec2{ 7, 8 } });
	Spline2D{}.toLineStringAdaptive(output);
	CHECK(output.isEmpty());
	CHECK(output.data() == storage);
}

TEST_CASE("CurveAdaptive.SplineConversion.ManyIntervals")
{
	// A long path must grow its output geometrically, not reserve exactly once per interval.
	Array<Bezier3> curves;
	for (int32 i = 0; i < 512; ++i)
	{
		const double x = i * 20;
		curves.push_back({ { x, 0 }, { x + 5, 10 }, { x + 15, -10 }, { x + 20, 0 } });
	}
	const auto path = Spline2D::FromBezierSegments(curves);
	LineString output;
	path.toLineStringAdaptive(output);
	const auto* storage = output.data();
	path.toLineStringAdaptive(output);
	CHECK(output.data() == storage);
	CHECK(output == Joined(path, 0.5, 256));
	CHECK(output.front() == curves.front().p0);
	CHECK(output.back() == curves.back().p3);
}

TEST_CASE("CurveAdaptive.Image.QualityCapsAndCompositing")
{
	const Color background{ 30, 90, 150, 97 }, color{ 240, 80, 20, 123 };
	const auto check = [&](const auto& curve)
	{
		for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const bool paint : { false, true })
		for (const auto start : { LineCap::Flat, LineCap::Square, LineCap::Round })
		for (const auto end : { LineCap::Flat, LineCap::Square, LineCap::Round })
		for (const int32 budget : { 1, 256 })
		{
			Image actual{ 80, 80, background }, expected = actual;
			const LineString line = budget == 1 ? LineString{ curve.p0, curve.pointAt(1) } : curve.toLineStringAdaptive(0.25, budget);
			if (paint)
			{
				CHECK(&curve.paint(actual, start, end, 7, color, aa, 0.25, budget) == &curve);
				line.paint(expected, start, end, 7, color, aa);
			}
			else
			{
				CHECK(&curve.overwrite(actual, start, end, 7, color, aa, 0.25, budget) == &curve);
				line.overwrite(expected, start, end, 7, color, aa);
			}
			CHECK(actual == expected);
		}
		for (const bool paint : { false, true })
		for (int32 overload = 0; overload < 3; ++overload)
		{
			Image actual{ 80, 80, background }, expected = actual;
			const double width = overload == 0 ? 1 : 7;
			const auto cap = overload == 2 ? LineCap::Flat : LineCap::Round;
			const auto line = curve.toLineStringAdaptive();
			if (paint)
			{
				if (overload == 0) curve.paint(actual, color);
				else if (overload == 1) curve.paint(actual, 7, color);
				else curve.paint(actual, cap, 7, color);
				line.paint(expected, cap, width, color);
			}
			else
			{
				if (overload == 0) curve.overwrite(actual, color);
				else if (overload == 1) curve.overwrite(actual, 7, color);
				else curve.overwrite(actual, cap, 7, color);
				line.overwrite(expected, cap, width, color);
			}
			CHECK(actual == expected);
		}
		Image fine{ 80, 80, background }, coarse = fine;
		curve.paint(fine, 3, color, EnableAntialiasing::Yes, 0.1, 256);
		curve.paint(coarse, 3, color, EnableAntialiasing::Yes, 2.0, 256);
		CHECK(fine != coarse);
	};
	check(Bezier2{ { -10, 60 }, { 35, -20 }, { 90, 50 } });
	check(Bezier3{ { -10, 60 }, { 80, -30 }, { -5, -10 }, { 90, 50 } });
}

TEST_CASE("CurveAdaptive.RendererStateDoesNotAffectGeometryOrImage")
{
	const Bezier2 q{ { 10, 50 }, { 40, -10 }, { 70, 55 } };
	const Bezier3 c{ { 10, 50 }, { -10, -10 }, { 65, 20 }, { 70, 55 } };
	const auto path = Spline2D::FromBezierSegments(Array<Bezier3>{ c });
	const auto qLine = q.toLineStringAdaptive(), cLine = c.toLineStringAdaptive(), pathLine = path.toLineStringAdaptive();
	Image before{ 80, 80, Palette::Black }, after = before;
	q.paint(before, 5, Palette::White); c.overwrite(before, 2, Palette::Red);
	path.paint(before, LineCap::Flat, LineCap::Round, 4, Color{ 200, 100, 50, 127 });
	path.overwrite(before, 1, Color{ 50, 100, 200, 127 });
	{
		const SceneScope scene;
		Scene::Configure(SceneConfig::Fixed({ 320, 240 }, SceneResolution::Display));
		const Transformer2D local{ Mat3x2::Scale(10.0f, 0.1f) };
		const Transformer2D camera{ Mat3x2::Rotate(0.7f), Transformer2D::Target::SetCamera };
		const ScopedQuadWarp2D warp{ Mat3x3{ 1, 0, 0.001f, 0, 1, 0.002f, 0, 0, 1 } };
		CHECK(q.toLineStringAdaptive() == qLine);
		CHECK(c.toLineStringAdaptive() == cLine);
		CHECK(path.toLineStringAdaptive() == pathLine);
		q.paint(after, 5, Palette::White); c.overwrite(after, 2, Palette::Red);
		path.paint(after, LineCap::Flat, LineCap::Round, 4, Color{ 200, 100, 50, 127 });
		path.overwrite(after, 1, Color{ 50, 100, 200, 127 });
	}
	CHECK(before == after);
}

TEST_CASE("CurveAdaptive.SplineDraw.TransformsAndJoins")
{
	const SceneScope scene;
	const Array<Vec2> knots{ { 10, 30 }, { 25, 5 }, { 60, 20 }, { 45, 55 } };
	for (const CloseRing closed : { CloseRing::No, CloseRing::Yes })
	for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	for (const int32 budget : { 1, 256 })
	{
		Scene::Configure(SceneConfig::Fixed({ 320, 240 }, resolution));
		const auto path = Spline2D::FromCatmullRom(knots, closed);
		const Transformer2D local{ Mat3x2{ 2.0f, 0.2f, 0.3f, 1.0f, 10.0f, 20.0f }, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.3f, 2.0f), Transformer2D::Target::SetCamera };
		const Mat3x2 matrix = Graphics2D::GetLocalTransform() * Graphics2D::GetCameraTransform();
		const Vec2 resolutionScale = Vec2{ Scene::RenderResolution() } / Vec2{ Scene::Size() };
		LineString expectedLine, piece;
		detail::BezierTessellation::Workspace workspace;
		for (const auto& curve : path.segments())
		{
			const auto metric = detail::BezierTessellation::TransformMetric(curve, matrix, resolutionScale);
			detail::BezierTessellation::Flatten(piece, workspace, curve, metric, 0.5, budget);
			for (const auto point : piece)
				if (expectedLine.isEmpty() || expectedLine.back() != point) expectedLine.push_back(point);
		}
		if (closed) expectedLine.pop_back();
		const ColorF color{ 0.8, 0.7, 0.6, 0.5 }; // Reveals overlapping caps at internal joins.
		const Image actual = Capture([&] { CHECK(&path.draw(-6, color, 0.5, budget) == &path); });
		const Image expected = Capture([&]
		{
			if (closed) expectedLine.drawClosed(6, color);
			else expectedLine.draw(6, color);
		});
		CHECK(actual == expected);
	}
	const auto path = Spline2D::FromCatmullRom(knots);
	CHECK(Capture([&] { path.draw(); }) == Capture([&] { path.draw(1, Palette::White, 0.5, 256); }));
	CHECK(Capture([&] { Spline2D{}.draw(); }) == Capture([] {}));
	CHECK_THROWS_AS(path.draw(1, Palette::White, 0), std::invalid_argument);
	CHECK_THROWS_AS(path.draw(1, Palette::White, 0.5, 0), std::invalid_argument);
	CHECK_THROWS_AS(path.draw(1, Palette::White, 0.5, 4097), std::invalid_argument);
}

namespace
{
	void LegacyPath(LineString& out, const Spline2D& path, const double error)
	{
		out.clear();
		if (path.isEmpty()) return;
		out.push_back(path.segment(0).p0);
		for (const auto& curve : path.segments()) BezierStudy::Legacy::AppendBezier3Polyline(out, curve, error * error, 10);
		if (path.isClosed() && out.size() > 1 && out.front() == out.back()) out.pop_back();
	}

	template <class Run>
	void TimingPair(const std::string& name, const int32 repetitions, Run&& run)
	{
		std::array<Array<double>, 2> times;
		for (int32 round = -2; round < 11; ++round)
		for (int32 index = 0; index < 2; ++index)
		{
			const int32 method = (index + round + 2) % 2;
			const auto begin = std::chrono::steady_clock::now();
			for (int32 repeat = 0; repeat < repetitions; ++repeat) run(method);
			const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / repetitions;
			if (round >= 0) times[method].push_back(ns);
		}
		for (int32 method = 0; method < 2; ++method)
		{
			times[method].sort();
			std::cout << "BSTUDY\tintegration_timing\t" << name << '\t' << (method ? "density" : "legacy")
				<< '\t' << times[method][5] << '\t' << times[method][1] << '\t' << times[method][9] << '\n';
		}
	}
}

TEST_CASE("CurveAdaptive.Benchmark", "[.benchmark]")
{
	for (const int32 count : { 4, 32, 128 })
	{
		Array<Vec2> knots;
		for (int32 i = 0; i <= count; ++i) knots.push_back({ i * 30.0, std::sin(i * 0.7) * 120 });
		const auto path = Spline2D::FromCatmullRom(knots);
		std::array<LineString, 2> output;
		LegacyPath(output[0], path, 0.5);
		path.toLineStringAdaptive(output[1], 0.5);
		std::cout << "BSTUDY\tintegration_points\tspline/" << count << '\t' << output[0].size() << '\t' << output[1].size() << '\n';
		TimingPair("spline/" + std::to_string(count), 128, [&](const int32 method)
		{
			if (method) path.toLineStringAdaptive(output[method], 0.5);
			else LegacyPath(output[method], path, 0.5);
			ankerl::nanobench::doNotOptimizeAway(output[method].data());
			ankerl::nanobench::doNotOptimizeAway(output[method].size());
		});
	}
	for (const int32 size : { 64, 256 })
	{
		const double s = size;
		const auto check = [&](const auto& curve, const char* name)
		{
			std::cout << "BSTUDY\tintegration_points\t" << name << '/' << size << '\t'
				<< BezierStudy::Legacy::Flatten(curve, 0.48, 5).size() << '\t' << curve.toLineStringAdaptive().size() << '\n';
			for (const bool paint : { false, true })
			for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			for (const double width : { 1.5, 12.0 })
			{
				std::array<Image, 2> images{ Image{ size, Color{ 40, 70, 100, 113 } }, Image{ size, Color{ 40, 70, 100, 113 } } };
				const Color color{ 211, 83, 147, 113 };
				const std::string label = std::string{ name } + '/' + std::to_string(size) + (paint ? "/paint" : "/overwrite")
					+ (aa ? "/AA" : "/noAA") + (width == 1.5 ? "/thin" : "/wide");
				TimingPair(label, 32, [&](const int32 method)
				{
					Image& image = images[method];
					if (method)
					{
						if (paint) curve.paint(image, width, color, aa);
						else curve.overwrite(image, width, color, aa);
					}
					else
					{
						const auto line = BezierStudy::Legacy::Flatten(curve, 0.48, 5);
						if (paint) line.paint(image, width, color, aa);
						else line.overwrite(image, width, color, aa);
					}
					ankerl::nanobench::doNotOptimizeAway(image.data());
				});
			}
		};
		check(Bezier2{ { s * 0.1, s * 0.8 }, { s * 0.25, -s * 0.2 }, { s * 0.9, s * 0.7 } }, "bezier2");
		check(Bezier3{ { s * 0.1, s * 0.8 }, { s * 0.7, -s * 0.2 }, { s * 0.2, s * 0.9 }, { s * 0.9, s * 0.7 } }, "bezier3");
	}
}

TEST_CASE("CurveAdaptive.Gallery", "[.bezier-study]")
{
	const SceneScope scene;
	Scene::Configure(SceneConfig::Fixed({ 1000, 700 }, SceneResolution::SceneSize));
	const Font font{ 16 };
	const Bezier2 quadratic{ { 10, 125 }, { 60, -40 }, { 150, 115 } };
	const Bezier3 cubic{ { 10, 125 }, { 160, -50 }, { -30, 0 }, { 150, 130 } };
	const auto path = Spline2D::FromCatmullRom(Array<Vec2>{ { 20, 20 }, { 135, 35 }, { 125, 120 }, { 45, 115 } }, CloseRing::Yes);
	Array<Texture> textures;
	const Color ink{ 170, 215, 255, 160 };
	for (int32 row = 0; row < 3; ++row)
	for (int32 column = 0; column < 4; ++column)
	{
		Image result{ 160, 155, Color{ 15, 15, 15, 255 } };
		const auto draw = [&](const auto& curve)
		{
			if (column == 0) curve.toLineString(4096).paint(result, LineCap::Square, 6, ink);
			else if (column == 1) BezierStudy::Legacy::Flatten(curve, 0.48, 5).paint(result, LineCap::Square, 6, ink);
			else if (column == 2) curve.paint(result, LineCap::Square, 6, ink);
			else curve.overwrite(result, LineCap::Square, 6, ink);
		};
		if (row == 0) draw(quadratic);
		else if (row == 1) draw(cubic);
		else
		{
			LineString line;
			if (column == 0) line = path.toLineString(1024);
			else if (column == 1) LegacyPath(line, path, 0.48);
			else path.toLineStringAdaptive(line);
			if (column == 3) line.overwriteClosed(result, 6, ink);
			else line.paintClosed(result, 6, ink);
		}
		textures.emplace_back(result);
	}
	const Image capture = Capture([&]
	{
		font(U"Adaptive integration | target 0.5px | 6px strokes | translucent ink").draw(20, 10);
		const std::array labels{ U"Dense CPU", U"Legacy CPU", U"Density paint", U"Density overwrite", U"Density GPU" };
		for (int32 column = 0; column < 5; ++column) font(labels[column]).draw(135 + 173 * column, 42);
		for (int32 row = 0; row < 3; ++row)
		{
			font(row == 0 ? U"Bezier2" : row == 1 ? U"Bezier3" : U"Closed Spline").draw(12, 90 + row * 195);
			for (int32 column = 0; column < 5; ++column)
			{
				const Vec2 pos{ 135.0 + 173 * column, 75.0 + 195 * row };
				RectF{ pos, 160, 155 }.draw(Color{ 15, 15, 15 });
				if (column < 4) textures[row * 4 + column].draw(pos);
				else
				{
					const Transformer2D transform{ Mat3x2::Translate(pos) };
					if (row == 0) quadratic.draw(LineCap::Square, 6, ink);
					else if (row == 1) cubic.draw(LineCap::Square, 6, ink);
					else path.draw(6, ink);
				}
			}
		}
		font(U"Spline CPU columns draw the converted LineString; Image and GPU joins/AA differ.").draw(20, 660);
	});
	std::cout << "BSTUDY_IMAGE\tintegration\t" << capture.encodePNG().base64().getBase64() << '\n';
}

TEST_CASE("CurveAdaptive.SubmissionBenchmark", "[.benchmark]")
{
	const SceneScope scene;
	constexpr int32 Draws = 64;
	for (const int32 count : { 4, 32, 128 })
	{
		Array<Vec2> knots;
		for (int32 i = 0; i <= count; ++i) knots.push_back({ i * 30.0, std::sin(i * 0.7) * 120 });
		const auto path = Spline2D::FromCatmullRom(knots);
		std::array<Array<double>, 2> times;
		std::array<int64, 2> triangles{};
		for (int32 round = -8; round < 32; ++round)
		for (int32 offset = 0; offset < 2; ++offset)
		{
			const int32 method = (offset + round + 8) % 2;
			REQUIRE(System::Update());
			{
				const Transformer2D transform{ Mat3x2::Translate(20000, 20000) };
				const auto begin = std::chrono::steady_clock::now();
				for (int32 repeat = 0; repeat < Draws; ++repeat)
				{
					if (method) path.draw(6, Palette::White, 0.5);
					else
					{
						LineString line;
						LegacyPath(line, path, 0.5);
						line.draw(6);
					}
				}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / Draws;
				if (round >= 0) times[method].push_back(ns);
			}
			REQUIRE(System::Update());
			triangles[method] = Profiler::GetFrameMetrics().triangleCount / Draws;
		}
		for (int32 method = 0; method < 2; ++method)
		{
			times[method].sort();
			std::cout << "BSTUDY\tintegration_timing\tspline-draw/" << count << '\t' << (method ? "density" : "legacy")
				<< '\t' << times[method][15] << '\t' << times[method][3] << '\t' << times[method][27] << '\n';
			CHECK(triangles[method] > 0);
		}
		std::cout << "BSTUDY\tintegration_triangles\tspline-draw/" << count << '\t' << triangles[0] << '\t' << triangles[1] << '\n';
	}
}
