//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include "../Siv3D/src/Siv3D/Polygon/ClosedLineString.hpp"

namespace
{
	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
	};

	template <class Build>
	Mesh BuildMesh(Build&& build)
	{
		Mesh mesh;
		uint32 requests = 0;
		const auto allocate = [&](const uint16 vertices, const uint32 indices)
		{
			++requests;
			mesh.vertices.resize(vertices);
			mesh.indices.resize(indices);
			return Vertex2DBufferPointer{ mesh.vertices.data(), mesh.indices.data(), 37 };
		};
		const auto count = build(allocate);
		REQUIRE(count == mesh.indices.size());
		CHECK(requests == (count ? 1 : 0));
		for (const auto index : mesh.indices)
		{
			REQUIRE(37 <= index);
			REQUIRE(index < (37 + mesh.vertices.size()));
		}
		return mesh;
	}

	enum class Shape { Pie, EllipsePie, Arc, SquareArc, RoundArc, Segment };

	uint32 BuildPrimitive(const BufferCreatorFunc& allocate, const Shape shape, const float radius, const float sweep, const float scale)
	{
		const Float4 red{ 1, 0, 0, 0.5f }, blue{ 0, 0, 1, 0.5f };
		switch (shape)
		{
		case Shape::Pie:
			return Vertex2DBuilder::BuildCirclePie(allocate, { 0, 0 }, radius, 0, sweep, red, blue, scale);
		case Shape::EllipsePie:
			return Vertex2DBuilder::BuildEllipsePie(allocate, { 0, 0 }, radius, radius / 4, 0, sweep, red, blue, scale);
		case Shape::Segment:
			return Vertex2DBuilder::BuildCircleSegment(allocate, { 0, 0 }, radius, 0, sweep, red, scale);
		default:
			const auto cap = ((shape == Shape::Arc) ? LineCap::Flat : ((shape == Shape::SquareArc) ? LineCap::Square : LineCap::Round));
			return Vertex2DBuilder::BuildCircleArc(allocate, cap, { 0, 0 }, (radius - 8), 0, sweep, 8,
				ColorFillDirection::LeftRight, red, blue, scale);
		}
	}

	Mesh BuildPrimitive(const Shape shape, const float radius, const float sweep, const float scale = 1)
	{
		return BuildMesh([&](const BufferCreatorFunc& allocate) { return BuildPrimitive(allocate, shape, radius, sweep, scale); });
	}

	double Area(const Mesh& mesh)
	{
		double area = 0;
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			const Vec2 a = mesh.vertices[mesh.indices[i] - 37].pos;
			const Vec2 b = mesh.vertices[mesh.indices[i + 1] - 37].pos;
			const Vec2 c = mesh.vertices[mesh.indices[i + 2] - 37].pos;
			const double cross = (b - a).cross(c - a);
			CHECK(cross > 0);
			area += (cross * 0.5);
		}
		return area;
	}

	const Array<Vec2> JoinPoints{ { -64, 0 }, { 0, -0.5 }, { 64, 0 }, { 0, 48 } };

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D colorMul{ Palette::White };
		ScopedColorAdd2D colorAdd{ 0.0 };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 480, 280 }, SceneResolution::SceneSize));
			Scene::SetBackground(Palette::Black);
		}
		~SceneScope()
		{
			Scene::Configure(config);
			Scene::SetBackground(background);
		}
	};

	struct Captured { Image image; int64 triangles; };

	template <class Draw>
	Captured Capture(Draw&& draw)
	{
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return{ ScreenCapture::GetFrame(), Profiler::GetFrameMetrics().triangleCount };
	}

	void Draw(const int32 shape, const int32 mode, const double scale = 1, const double direction = 1)
	{
		const Circle circle{ 0, 0, (24 * scale) };
		const Ellipse ellipse{ 0, 0, (24 * scale), (6 * scale) };
		const ColorF red{ 1, 0, 0, 0.5 }, blue{ 0, 0, 1, 0.5 };
		const Pattern::PolkaDot pattern{ .primary = red, .background = red };
		const double sweep = (0.2 * direction);
		if (shape == 0)
		{
			if (mode == 0) circle.drawPie(0, sweep, red);
			else if (mode == 1) circle.drawPie(0, sweep, red, blue);
			else circle.drawPie(0, sweep, pattern);
		}
		else if (shape == 1)
		{
			if (mode == 0) ellipse.drawPie(Math::HalfPi, sweep, red);
			else if (mode == 1) ellipse.drawPie(Math::HalfPi, sweep, red, blue);
			else ellipse.drawPie(Math::HalfPi, sweep, pattern);
		}
		else if (shape <= 4)
		{
			const auto cap = ((shape == 2) ? LineCap::Flat : ((shape == 3) ? LineCap::Square : LineCap::Round));
			if (mode == 0) circle.drawArc(cap, 0, sweep, 2 * scale, 2 * scale, red);
			else if (mode == 1) circle.drawArc(cap, 0, sweep, 2 * scale, 2 * scale, Arg::start(red), Arg::end(blue));
			else circle.drawArc(cap, 0, sweep, 2 * scale, 2 * scale, pattern);
		}
		else
		{
			Array<Vec2> points = JoinPoints.map([&](const Vec2 p) { return (p * (scale * 0.5)); });
			if (direction < 0) points.reverse();
			if (shape == 5)
			{
				const Quad quad{ points[0], points[1], points[2], points[3] };
				if (mode == 2) quad.drawFrame(4 * scale, pattern, JoinStyle::Round);
				else quad.drawFrame(4 * scale, red, JoinStyle::Round);
			}
			else
			{
				points << points[0] << points[1];
				if (mode == 2) DrawClosedLineStringFallback(points, JoinStyle::Round, 4 * scale, pattern);
				else DrawClosedLineStringFallback(points, JoinStyle::Round, 4 * scale, red);
			}
		}
	}
}

TEST_CASE("Renderer2D.arc_tessellation_interval_budget")
{
	for (const auto [radius, full] : { std::pair{ 1.0f, 8 }, { 3.0f, 12 }, { 6.0f, 16 }, { 8.0f, 20 },
		{ 80.0f, 56 }, { 472.0f, 252 }, { 100000.0f, 252 } })
	{
		CAPTURE(radius, full);
		const float step = (Math::TwoPiF / full);
		for (const auto [fraction, intervals] : { std::pair{ 0.1f, 1 }, { 0.99f, 1 }, { 1.0f, 1 },
			{ 1.01f, 2 }, { 1.99f, 2 }, { 2.01f, 3 }, { 2.99f, 3 }, { 3.01f, 4 } })
		{
			CAPTURE(fraction);
			CHECK(Vertex2DBuilder::CalculateCircleArcQuality(radius, (step * fraction)) == (intervals + 1));
		}
		CHECK(Vertex2DBuilder::CalculateCircleArcQuality(radius, Math::PiF) == (full / 2 + 1));
		CHECK(Vertex2DBuilder::CalculateCircleArcQuality(radius, Math::TwoPiF) == (full + 1));
	}
}

TEST_CASE("Renderer2D.arc_tessellation_short_meshes")
{
	for (const auto shape : { Shape::Pie, Shape::EllipsePie, Shape::Arc, Shape::SquareArc, Shape::RoundArc })
	for (const float sweep : { 0.03f, -0.03f, 0.15f, -0.15f, 0.28f, -0.28f })
	{
		CAPTURE(static_cast<int32>(shape), sweep);
		const size_t intervals = ((Abs(sweep) < 0.1f) ? 1 : ((Abs(sweep) < 0.2f) ? 2 : 3));
		const auto mesh = BuildPrimitive(shape, 80, sweep);
		const size_t extra = ((shape == Shape::SquareArc) ? 4 : ((shape == Shape::RoundArc) ? 10 : 0));
		CHECK(mesh.indices.size() / 3 == ((shape <= Shape::EllipsePie) ? intervals : (intervals * 2 + extra)));
		CHECK(Area(mesh) > 0);
		if (shape <= Shape::EllipsePie)
		{
			const double b = ((shape == Shape::Pie) ? 80 : 20);
			CHECK(mesh.vertices[0].color == Float4{ 1, 0, 0, 0.5f });
			for (size_t i = 1; i < mesh.vertices.size(); ++i)
			{
				CHECK(mesh.vertices[i].color == Float4{ 0, 0, 1, 0.5f });
			}
			for (size_t i = 1; i + 1 < mesh.vertices.size(); ++i)
			{
				const double angle = (Min(sweep, 0.0f) + Abs(sweep) * (i - 0.5) / intervals);
				const Vec2 reference{ 80 * std::sin(angle), -b * std::cos(angle) };
				const Line chord{ mesh.vertices[i].pos, mesh.vertices[i + 1].pos };
				// At radius 80, one ordinary circle interval deviates by less than 0.126.
				CHECK(chord.closestPointTo(reference).distanceFrom(reference) < 0.126);
			}
		}
	}
}

TEST_CASE("Renderer2D.arc_tessellation_caps_segments_and_limits")
{
	CHECK(BuildPrimitive(Shape::Arc, 80, 0).indices.isEmpty());
	CHECK(BuildPrimitive(Shape::Pie, 80, 0).indices.isEmpty());
	CHECK(BuildPrimitive(Shape::EllipsePie, 80, 0).indices.isEmpty());
	CHECK(BuildPrimitive(Shape::SquareArc, 80, 0).indices.size() == 12);
	CHECK(BuildPrimitive(Shape::RoundArc, 80, 0).indices.size() == 30);
	for (const auto [width, triangles] : { std::pair<float, size_t>{ 2.0f, 8 }, { 1000.0f, 252 } })
	{
		const auto caps = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildCircleArc(allocate, LineCap::Round, { 0, 0 }, 80, 0, 0, width,
				ColorFillDirection::InOut, Float4{ 1, 1, 1, 1 }, Float4{ 1, 1, 1, 1 }, 1);
		});
		CHECK(caps.indices.size() == (triangles * 3));
	}
	for (const float sweep : { 0.03f, -0.03f })
	{
		const auto segment = BuildPrimitive(Shape::Segment, 80, sweep);
		CHECK(segment.indices.size() == 12);
		const double exactArea = (80 * 80 * (Abs(sweep) - std::sin(Abs(sweep))) * 0.5);
		const double area = Area(segment);
		CHECK(area > exactArea * 0.9);
		CHECK(area < exactArea * 1.01);
	}
	for (const float radius : { 472.0f, 16000.0f })
	{
		CHECK(BuildPrimitive(Shape::Pie, radius, Math::TwoPiF).indices.size() == (252 * 3));
		CHECK(BuildPrimitive(Shape::Arc, radius, Math::TwoPiF).indices.size() == (252 * 6));
	}
	const auto fail = [](uint16, uint32) { return Vertex2DBufferPointer{ nullptr, nullptr, 0 }; };
	for (const auto shape : { Shape::Pie, Shape::EllipsePie, Shape::Arc, Shape::SquareArc, Shape::RoundArc, Shape::Segment })
	{
		CHECK(BuildPrimitive(fail, shape, 80, 0.03f, 1) == 0);
	}
}

TEST_CASE("Renderer2D.arc_tessellation_small_join")
{
	for (const bool reverse : { false, true })
	{
		const auto points = (reverse ? JoinPoints.reversed() : JoinPoints);
		const auto mesh = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			const auto count = Vertex2DBuilder::TryBuildSmallConvexFrame(allocate, points, JoinStyle::Round,
				2, 8, Float4{ 1, 1, 1, 0.5f }, 1);
			REQUIRE(count.has_value());
			return *count;
		});
		CHECK(Area(mesh) > 0);
		size_t pointsOnShortJoin = 0;
		for (const auto& vertex : mesh.vertices)
		{
			pointsOnShortJoin += (Abs(Vec2{ vertex.pos }.distanceFrom(JoinPoints[1]) - 8) < 0.0001);
		}
		CHECK(pointsOnShortJoin == 2);
	}
}

TEST_CASE("Renderer2D.arc_tessellation_transforms")
{
	const SceneScope scene;
	for (int32 shape = 0; shape < 7; ++shape)
	for (const int32 mode : { 0, 1, 2 })
	for (const double scale : { 0.5, 4.0 })
	for (const double direction : { -1.0, 1.0 })
	{
		CAPTURE(shape, mode, scale, direction);
		const auto actual = Capture([&]
		{
			const Transformer2D transform{ Mat3x2::Scale(scale).translated(240, 140) };
			Draw(shape, mode, 1, direction);
		});
		const auto expected = Capture([&]
		{
			const Transformer2D placement{ Mat3x2::Translate(240, 140) };
			Draw(shape, mode, scale, direction);
		});
		CHECK(actual.triangles > 0);
		CHECK(actual.triangles == expected.triangles);
		CHECK(actual.image == expected.image);
	}
}

TEST_CASE("Renderer2D.arc_tessellation_scene_resolution")
{
	const SceneScope scene;
	const Size logical{ 480, 280 };
	for (int32 shape = 0; shape < 7; ++shape)
	for (const int32 mode : { 0, 1, 2 })
	{
		CAPTURE(shape, mode);
		const auto draw = [&]
		{
			const Transformer2D local{ Mat3x2::Translate(240, 140), Transformer2D::Target::SetLocal };
			Draw(shape, mode);
		};
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::SceneSize));
		const auto base = Capture(draw);
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::Display));
		const Size pixels = Scene::RenderResolution();
		REQUIRE(pixels.x > logical.x);
		const auto high = Capture(draw);
		CHECK(high.triangles > base.triangles);
		Scene::Configure(SceneConfig::Fixed(pixels, SceneResolution::SceneSize));
		const auto expected = Capture([&]
		{
			const Transformer2D camera{ Mat3x2::Scale(Float2{ float(pixels.x) / logical.x, float(pixels.y) / logical.y }),
				Transformer2D::Target::SetCamera };
			draw();
		});
		CHECK(high.triangles == expected.triangles);
		CHECK(high.image == expected.image);
	}
}

TEST_CASE("Renderer2D.arc_tessellation_join_fallback_pixels")
{
	const SceneScope scene;
	for (const int32 mode : { 0, 2 })
	for (const double direction : { -1.0, 1.0 })
	{
		const Transformer2D placement{ Mat3x2::Translate(240, 140) };
		const auto direct = Capture([&] { Draw(5, mode, 1, direction); });
		const auto fallback = Capture([&] { Draw(6, mode, 1, direction); });
		CHECK(direct.image == fallback.image);
	}
}
