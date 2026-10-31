//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "BezierTessellationStudy.hpp"

namespace
{
	namespace Tessellation = s3d::detail::BezierTessellation;

	struct DrawSceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		MSAASampleCount msaa = Scene::GetMSAASampleCount();
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D colorMul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D colorAdd{ 0.0, ScopedColorAdd2D::Target::Set };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		DrawSceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 320, 240 }));
			Scene::SetBackground(Palette::Black);
			Scene::SetMSAASampleCount(MSAASampleCount::X4MSAA);
		}
		~DrawSceneScope()
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

	void Draw(const BezierStudy::Curve& curve, const LineCap start, const LineCap end,
		const double width, const double error, const int32 limit)
	{
		if (curve.degree == 2)
		{
			const auto q = curve.quadratic();
			CHECK(&q.draw(start, end, width, Palette::White, error, limit) == &q);
		}
		else
		{
			const auto c = curve.cubic();
			CHECK(&c.draw(start, end, width, Palette::White, error, limit) == &c);
		}
	}
}

TEST_CASE("BezierDraw.affine_quality_and_storage")
{
	const std::array matrices{
		Mat3x2::Identity(), Mat3x2::Scale(3.0f), Mat3x2::Scale(4.0f, 0.25f),
		Mat3x2::Rotate(0.7f) * Mat3x2::Scale(0.3f, 4.0f),
		Mat3x2{ 1, 0.8f, -0.3f, 1, 123, -456 }, Mat3x2::Scale(-2.0f, 1.0f),
		Mat3x2::Scale(1.0f, 0.0f), Mat3x2::Scale(0.0f) };
	Tessellation::Workspace workspace;
	Array<Vec2> points;
	points.reserve(4097);
	const auto storage = points.data();
	for (const auto& item : BezierStudy::Cases())
	{
		if (not item.ordinary) continue;
		for (size_t matrixIndex = 0; matrixIndex < matrices.size(); ++matrixIndex)
		for (const Vec2 resolution : { Vec2{ 1, 1 }, Vec2{ 1.5, 2 } })
		{
			const auto& matrix = matrices[matrixIndex];
			CAPTURE(item.name, matrixIndex, resolution);
			const auto metric = Tessellation::TransformMetric(item.curve, matrix, resolution);
			Tessellation::Flatten(points, workspace, item.curve, metric, 0.5, 256);
			CHECK(points.data() == storage);
			CHECK(points.front() == item.curve.p[0]);
			CHECK(points.back() == item.curve.p[item.curve.degree]);
			CHECK(points.size() <= 257);

			// Independently transform the curve and output for the geometric error query.
			BezierStudy::Curve expected = item.curve;
			for (auto& p : expected.p) p = matrix.transformPoint(p) * resolution;
			LineString actual;
			for (const auto& p : points) actual.push_back(matrix.transformPoint(p) * resolution);
			CAPTURE(workspace.pieceCount, points.size());
			CHECK(BezierStudy::SampledError(expected, actual, 2048) <= 0.65);
		}
	}
}

TEST_CASE("BezierDraw.budget_and_cubic_work_limit")
{
	Tessellation::Workspace workspace;
	Array<Vec2> points;
	const Tessellation::Curve source{ Bezier3{ { 0, 0 }, { 0, 400 }, { 600, 400 }, { 600, 0 } } };
	const auto metric = Tessellation::TransformMetric(source, Mat3x2::Scale(10000.0f), Vec2{ 1, 1 });
	for (const uint32 budget : { 1u, 2u, 7u, 256u, 4096u })
	{
		Tessellation::Flatten(points, workspace, source, metric, 0.01, budget);
		CHECK(points.front() == source.p[0]);
		CHECK(points.back() == source.p[3]);
		CHECK(points.size() == budget + 1);
		CHECK(workspace.limited);
		CHECK(workspace.pieceCount <= workspace.pieces.size());
		// The intermediate quadratic budget falls back to uniform Wang placement.
		for (uint32 i = 0; i <= budget; ++i) CHECK(points[i].distanceFrom(source.point(double(i) / budget)) < 1e-9);
	}
}

TEST_CASE("BezierDraw.near_collinear_cubic_reversals")
{
	Tessellation::Workspace workspace;
	LineString points;
	for (const double height : { 0.0, 0.001, 0.2499, 0.25, 0.2501, 0.5, 1.0, 2.0 })
	{
		const BezierStudy::Curve curve{ Bezier3{ { 0, 0 }, { -150, height }, { 225, 0 }, { 900, 0 } } };
		Tessellation::Flatten(points, workspace, curve, curve, 0.5, 256);
		CAPTURE(height, points.size());
		CHECK(BezierStudy::SampledError(curve, points) <= 0.65);
	}
}

TEST_CASE("BezierDraw.count_continuity_near_straight_and_reversal")
{
	Tessellation::Workspace workspace;
	Array<Vec2> points;
	std::array<int32, 4> previous{};
	for (int32 step = 0; step <= 1000; ++step)
	{
		const double u = step / 1000.0;
		const std::array<Tessellation::Curve, 4> curves{
			Bezier2{ { 0, 0 }, { 600, 2 * (u - 0.5) }, { 40, 0 } },
			Bezier2{ { 0, 0 }, { 25, 0.25 + 0.5 * u }, { 600, 0 } },
			Bezier3{ { 0, 0 }, { 0, 0.25 + 0.5 * u }, { 0, -0.25 - 0.5 * u }, { 600, 0 } },
			Bezier3{ { 0, 0 }, { 200, 50 + 450 * u }, { 400, -50 - 450 * u }, { 600, 0 } } };
		for (size_t i = 0; i < curves.size(); ++i)
		{
			Tessellation::Flatten(points, workspace, curves[i], curves[i], 0.5, 256);
			const int32 count = static_cast<int32>(points.size() - 1);
			// Regression for these slow deformations, not a general temporal guarantee.
			if (step) CHECK(Abs(count - previous[i]) <= 1);
			previous[i] = count;
		}
	}
}

TEST_CASE("BezierDraw.public_caps_and_one_segment_budget")
{
	const DrawSceneScope scene;
	const std::array<BezierStudy::Curve, 2> curves{
		Bezier2{ { 40, 100 }, { 160, 10 }, { 280, 120 } },
		Bezier3{ { 40, 100 }, { 80, 0 }, { 240, 200 }, { 280, 120 } } };
	for (const auto& curve : curves)
	for (const auto start : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const auto end : { LineCap::Flat, LineCap::Square, LineCap::Round })
	{
		const Image actual = Capture([&] { Draw(curve, start, end, -12, 0.5, 1); });
		const Image expected = Capture([&] { LineString{ curve.p[0], curve.p[curve.degree] }.draw(start, end, 12); });
		CHECK(actual == expected);
	}
	for (const auto cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	{
		const Bezier2 q{ { 100, 100 }, { 100, 100 }, { 100, 100 } };
		const Bezier3 c{ { 200, 100 }, { 200, 100 }, { 200, 100 }, { 200, 100 } };
		const Image actual = Capture([&] { q.draw(cap, 12); c.draw(cap, 12); });
		const Image expected = Capture([&] { LineString{ q.p0 }.draw(cap, 12); LineString{ c.p0 }.draw(cap, 12); });
		CHECK(actual == expected);
	}
}

TEST_CASE("BezierDraw.public_single_color_overloads")
{
	const DrawSceneScope scene;
	const ColorF color{ 0.3, 0.6, 0.9, 0.5 };
	const auto check = [&](const auto& curve, const Vec2 end)
	{
		for (int32 overload = 0; overload < 4; ++overload)
		{
			// An explicit one-segment budget must draw the chord in every overload.
			const auto actual = Capture([&]
			{
				if (overload == 0) CHECK(&curve.draw(color, 0.25, 1) == &curve);
				else if (overload == 1) CHECK(&curve.draw(-6, color, 0.25, 1) == &curve);
				else if (overload == 2) CHECK(&curve.draw(LineCap::Round, -6, color, 0.25, 1) == &curve);
				else CHECK(&curve.draw(LineCap::Flat, LineCap::Round, -6, color, 0.25, 1) == &curve);
			});
			const auto startCap = ((overload == 3) ? LineCap::Flat : ((overload == 2) ? LineCap::Round : LineCap::Square));
			const auto endCap = ((overload >= 2) ? LineCap::Round : LineCap::Square);
			const auto expected = Capture([&] { LineString{ curve.p0, end }.draw(startCap, endCap, (overload ? 6 : 1), color); });
			CHECK(actual == expected);
		}
	};
	const Bezier2 q{ { 40, 100 }, { 160, 10 }, { 280, 120 } };
	const Bezier3 c{ { 40, 100 }, { 80, 0 }, { 240, 200 }, { 280, 120 } };
	check(q, q.p2);
	check(c, c.p3);
}

TEST_CASE("BezierDraw.public_transform_resolution_and_defaults")
{
	const DrawSceneScope scene;
	const Bezier2 q{ { 5, 5 }, { 30, 50 }, { 60, 10 } };
	const Bezier3 c{ { 5, 25 }, { 0, 60 }, { 55, -5 }, { 60, 40 } };
	const std::array<BezierStudy::Curve, 2> curves{ q, c };
	for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	{
		Scene::Configure(SceneConfig::Fixed({ 320, 240 }, resolution));
		const Transformer2D local{ Mat3x2{ 2, 0.2f, 0.3f, 1, 20, 25 }, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.2f, 2.0f), Transformer2D::Target::SetCamera };
		const auto matrix = Graphics2D::GetLocalTransform() * Graphics2D::GetCameraTransform();
		for (const Mat3x3 warpMatrix : { Mat3x3::Identity(), Mat3x3{ 1, 0, 0.0005f, 0, 1, 0.0002f, 0, 0, 1 } })
		{
			const ScopedQuadWarp2D warp{ warpMatrix, ScopedQuadWarp2D::Target::Set };
			const Vec2 rasterScale = Vec2{ Scene::RenderResolution() } / Vec2{ Scene::Size() };
			for (const auto& curve : curves)
			{
				Tessellation::Workspace workspace;
				LineString expectedLine;
				BezierStudy::Curve metric = curve;
				for (auto& p : metric.p) p = matrix.transformPoint(p) * rasterScale;
				Tessellation::Flatten(expectedLine, workspace, curve, metric, 0.5, 256);
				const Image actual = Capture([&] { Draw(curve, LineCap::Round, LineCap::Flat, 5, 0.5, 256); });
				const Image expected = Capture([&] { expectedLine.draw(LineCap::Round, LineCap::Flat, 5); });
				CHECK(actual == expected);
			}
		}
	}
	for (int32 overload = 0; overload < 3; ++overload)
	{
		const Image actual = Capture([&]
		{
			if (overload == 0) { CHECK(&q.draw() == &q); CHECK(&c.draw() == &c); }
			else if (overload == 1) { q.draw(5); c.draw(5); }
			else { q.draw(LineCap::Round, 5); c.draw(LineCap::Round, 5); }
		});
		const auto cap = overload == 2 ? LineCap::Round : LineCap::Square;
		const auto width = overload == 0 ? 1 : 5;
		const Image expected = Capture([&] { q.draw(cap, cap, width, Palette::White, 0.5, 256); c.draw(cap, cap, width, Palette::White, 0.5, 256); });
		CHECK(actual == expected);
	}
}

TEST_CASE("BezierDraw.gradient_overloads_and_boundaries")
{
	const DrawSceneScope scene;
	const ColorF first{ 0.9, 0.3, 0.1, 0.4 }, last{ 0.1, 0.4, 0.9, 0.8 };
	const auto check = [&](const auto& curve)
	{
		for (const auto start : { LineCap::Flat, LineCap::Square, LineCap::Round })
		for (const auto end : { LineCap::Flat, LineCap::Square, LineCap::Round })
		{
			const auto actual = Capture([&] { CHECK(&curve.draw(start, end, -12, Arg::start = first, Arg::end = last, 0.5, 1) == &curve); });
			const Tessellation::Curve source{ curve };
			const auto expected = Capture([&] { LineString{ source.p[0], source.p[source.degree] }.draw(start, end, 12, first, last); });
			CHECK(actual == expected);
		}
		for (int32 overload = 0; overload < 3; ++overload)
		{
			const auto actual = Capture([&]
			{
				if (overload == 0) curve.draw(Arg::start = first, Arg::end = last);
				else if (overload == 1) curve.draw(5, Arg::start = first, Arg::end = last);
				else curve.draw(LineCap::Round, 5, Arg::start = first, Arg::end = last);
			});
			const auto cap = ((overload == 2) ? LineCap::Round : LineCap::Square);
			const auto expected = Capture([&] { curve.draw(cap, cap, (overload ? 5 : 1), Arg::start = first, Arg::end = last); });
			CHECK(actual == expected);
		}
	};
	check(Bezier2{ { 40, 100 }, { 160, 10 }, { 280, 120 } });
	check(Bezier3{ { 40, 100 }, { 80, 0 }, { 240, 200 }, { 280, 120 } });
	check(Bezier2{ { 100, 100 }, { 100, 100 }, { 100, 100 } });
	check(Bezier3{ { 200, 100 }, { 200, 100 }, { 200, 100 }, { 200, 100 } });
}

TEST_CASE("BezierDraw.gradient_transform_resolution")
{
	const DrawSceneScope scene;
	const ColorF first{ 0.9, 0.3, 0.1, 0.4 }, last{ 0.1, 0.4, 0.9, 0.8 };
	for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	{
		Scene::Configure(SceneConfig::Fixed({ 320, 240 }, resolution));
		const Transformer2D local{ Mat3x2{ 2, 0.2f, 0.3f, 1, 20, 25 }, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.2f, 2.0f), Transformer2D::Target::SetCamera };
		const auto check = [&](const auto& curve)
		{
			const Tessellation::Curve source{ curve };
			const auto metric = Tessellation::TransformMetric(source, Graphics2D::GetLocalTransform() * Graphics2D::GetCameraTransform(), Vec2{ Scene::RenderResolution() } / Vec2{ Scene::Size() });
			Tessellation::Workspace workspace;
			LineString points;
			Tessellation::Flatten(points, workspace, source, metric, 0.5, 256);
			const auto actual = Capture([&] { curve.draw(LineCap::Round, LineCap::Flat, 5, Arg::start = first, Arg::end = last); });
			const auto expected = Capture([&] { points.draw(LineCap::Round, LineCap::Flat, 5, first, last); });
			CHECK(actual == expected);
			const auto equal = Capture([&] { curve.draw(5, Arg::start = first, Arg::end = first); });
			CHECK(equal == Capture([&] { curve.draw(5, first); }));
		};
		check(Bezier2{ { 5, 5 }, { 30, 50 }, { 60, 10 } });
		check(Bezier3{ { 5, 25 }, { 0, 60 }, { 55, -5 }, { 60, 40 } });
	}
}
