//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Geometry2D/BezierArrowGeometry.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <chrono>
# include <iostream>

namespace
{
	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
	};

	template <class Build>
	Mesh CaptureMesh(Build&& build)
	{
		Mesh mesh;
		Vertex2D guard;
		guard.set(-123, -456, -7, -8, { -1, -2, -3, -4 });
		uint32 requests = 0;
		const auto allocate = [&](uint16 vc, uint32 ic)
		{
			++requests;
			mesh.vertices.assign(vc + size_t{ 2 }, guard);
			mesh.indices.assign(ic + size_t{ 2 }, Vertex2D::IndexType{ 65535 });
			return Vertex2DBufferPointer{ mesh.vertices.data() + 1, mesh.indices.data() + 1, 37 };
		};
		const auto count = build(allocate);
		REQUIRE(count);
		REQUIRE(requests == 1);
		CHECK(*count + size_t{ 2 } == mesh.indices.size());
		for (const auto* vertex : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(vertex->pos == guard.pos);
			CHECK(vertex->tex == guard.tex);
			CHECK(vertex->color == guard.color);
		}
		CHECK(mesh.indices.front() == 65535);
		CHECK(mesh.indices.back() == 65535);
		mesh.vertices.pop_back(); mesh.vertices.erase(mesh.vertices.begin());
		mesh.indices.pop_back(); mesh.indices.erase(mesh.indices.begin());
		for (const auto index : mesh.indices) REQUIRE((37 <= index && index < 37 + mesh.vertices.size()));
		for (const auto& vertex : mesh.vertices)
		{
			CHECK(vertex.tex == Float2{ -7, -8 });
			CHECK(std::isfinite(vertex.pos.x)); CHECK(std::isfinite(vertex.pos.y));
		}
		return mesh;
	}

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedColorMul2D mul{ Palette::White };
		ScopedColorAdd2D add{ 0.0 };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };
		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 360, 260 }));
			Scene::SetBackground(Palette::Black);
		}
		~SceneScope() { Scene::Configure(config); Scene::SetBackground(background); }
	};

	template <class Draw>
	Image Capture(Draw&& draw)
	{
		REQUIRE(System::Update()); draw(); ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update()); REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}
}

TEST_CASE("BezierArrow.geometry_and_scaling")
{
	const Bezier2 quadratic{ { 0, 0 }, { 100, 0 }, { 200, 0 } };
	const Bezier3 cubic{ { 0, 0 }, { 40, -100 }, { 150, 100 }, { 200, 0 } };
	const auto q = detail::FindBezierArrowHead(quadratic, 50);
	REQUIRE(q);
	CHECK(q->parameter == Catch::Approx(0.75).margin(1e-10));
	CHECK(q->base.x == Catch::Approx(150).margin(1e-8));
	CHECK(q->tip == quadratic.p2);
	CHECK(q->sizeScale == 1.0);
	for (const double length : { 1.0, 24.0, 70.0, 140.0 })
	{
		const auto head = detail::FindBezierArrowHead(cubic, length);
		REQUIRE(head);
		CHECK(head->base == cubic.pointAt(head->parameter));
		CHECK(head->tip == cubic.p3);
		CHECK(head->base.distanceFrom(head->tip) == Catch::Approx(length).margin(1e-7));
	}
	const auto oversized = detail::FindBezierArrowHead(quadratic, 400);
	REQUIRE(oversized);
	CHECK(oversized->parameter == 0.0);
	CHECK(oversized->sizeScale == 0.5);
	const auto exact = detail::FindBezierArrowHead(quadratic, 200);
	REQUIRE(exact); CHECK(exact->parameter == 0.0);
	const auto translated = detail::FindBezierArrowHead(cubic.movedBy(12345, -6789), 70);
	REQUIRE(translated);
	CHECK(translated->parameter == Catch::Approx(detail::FindBezierArrowHead(cubic, 70)->parameter).margin(1e-12));
}

TEST_CASE("BezierArrow.stationary_and_repeated_roots")
{
	const Bezier2 loop{ { 0, 0 }, { 100, 0 }, { 0, 0 } };
	const auto last = detail::FindBezierArrowHead(loop, 20);
	REQUIRE(last);
	CHECK(last->parameter == Catch::Approx((1 + std::sqrt(0.6)) * 0.5).margin(1e-10));
	for (const double length : { 50.0, 100.0 })
	{
		const auto maximum = detail::FindBezierArrowHead(loop, length);
		REQUIRE(maximum);
		CHECK(maximum->parameter == Catch::Approx(0.5).margin(1e-10));
		CHECK(maximum->sizeScale == Catch::Approx(50.0 / length));
	}
	const Bezier3 reversal{ { 0, 0 }, { 200, 0 }, { -200, 0 }, { 0, 0 } };
	const auto head = detail::FindBezierArrowHead(reversal, 43.2);
	REQUIRE(head);
	CHECK(head->parameter == Catch::Approx(0.9).margin(1e-10));
	// The start lies outside the circle, but there are multiple crossings.
	const Bezier3 crossings{ { 200, 0 }, { -400, 0 }, { 400, 0 }, { 0, 0 } };
	const auto lastCrossing = detail::FindBezierArrowHead(crossings, 86.6);
	REQUIRE(lastCrossing);
	CHECK(lastCrossing->parameter == Catch::Approx(0.9).margin(1e-10));
	const Bezier3 stationary{ { 0, 0 }, { 200, 0 }, { 200, 0 }, { 200, 0 } };
	const auto stationaryHead = detail::FindBezierArrowHead(stationary, 25);
	REQUIRE(stationaryHead);
	CHECK(stationaryHead->parameter == Catch::Approx(0.5).margin(1e-10));
	CHECK_FALSE(detail::FindBezierArrowHead(Bezier2{ { 5, 7 }, { 5, 7 }, { 5, 7 } }, 20));
	CHECK_FALSE(detail::FindBezierArrowHead(Bezier3{ { 5, 7 }, { 5, 7 }, { 5, 7 }, { 5, 7 } }, 20));
}

TEST_CASE("BezierArrow.mesh_join_caps_and_failure")
{
	const Float4 color{ 0.3f, 0.6f, 0.9f, 0.5f };
	const Array<Vec2> points{ { 0, 0 }, { 50, 0 }, { 100, 0 } };
	for (const auto cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	{
		const auto mesh = CaptureMesh([&](const auto& allocate)
		{
			return Vertex2DBuilder::BuildLineStringArrow(allocate, cap, points, { 130, 10 }, 8, 30, color, 1);
		});
		const Float2 direction = Float2{ 30, 10 }.normalized();
		for (size_t i : { size_t{ 4 }, size_t{ 5 } })
		{
			CHECK((mesh.vertices[i].pos - Float2{ 100, 0 }).dot(direction) == Catch::Approx(0).margin(1e-5));
		}
	}
	const auto fail = [](uint16, uint32) { return Vertex2DBufferPointer{}; };
	const auto failed = Vertex2DBuilder::BuildLineStringArrow(fail, LineCap::Round, points, { 130, 10 }, 8, 30, color, 1);
	REQUIRE(failed); CHECK(*failed == 0);
	CHECK_FALSE(Vertex2DBuilder::BuildLineStringArrow(fail, LineCap::Flat, points, { 100, 30 }, 8, 30, color, 1));
	CHECK_FALSE(Vertex2DBuilder::BuildLineStringArrow(fail, LineCap::Flat, points, { 70, 0 }, 8, 30, color, 1));
	CHECK_FALSE(Vertex2DBuilder::BuildLineStringArrow(fail, LineCap::Flat, points, { 130, 30 }, 8, 8.01f, color, 1));
	// A short last segment uses the bounded local overlap path.
	const Array<Vec2> shortEnd{ { 0, 0 }, { 99, 0 }, { 100, 0 } };
	const auto overlap = CaptureMesh([&](const auto& allocate)
	{
		return Vertex2DBuilder::BuildLineStringArrow(allocate, LineCap::Flat, shortEnd, { 130, 30 }, 8, 30, color, 1);
	});
	CHECK(overlap.indices.size() == 21);
	const Array<Vec2> singleton{ { 100, 0 } };
	CaptureMesh([&](const auto& allocate)
	{
		return Vertex2DBuilder::BuildLineStringArrow(allocate, LineCap::Round, singleton, { 130, 0 }, 8, 30, color, 1);
	});
	// Maximum public segment budget, with scratch-buffer growth and a round cap.
	Array<Vec2> large;
	for (int32 i = 0; i <= 4096; ++i) large.emplace_back(i, 0);
	const auto maximum = CaptureMesh([&](const auto& allocate)
	{
		return Vertex2DBuilder::BuildLineStringArrow(allocate, LineCap::Round, large, { 4130, 0 }, 8, 30, color, 1);
	});
	CHECK(maximum.vertices.size() >= 8197);
}

TEST_CASE("BezierArrow.public_straight_and_alpha")
{
	const SceneScope scene;
	const Bezier2 q{ { 50, 140 }, { 150, 140 }, { 250, 140 } };
	const Bezier3 c{ { 50, 140 }, { 90, 140 }, { 200, 140 }, { 250, 140 } };
	for (const auto cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const double length : { 32.0, 400.0 })
	{
		const SizeF head{ 26, length };
		const ColorF color{ 1.0, 0.5 };
		const auto expected = Capture([&] { Line{ q.p0, q.p2 }.drawArrow(cap, 8, head, color); });
		const auto actualQ = Capture([&] { CHECK(&q.drawArrow(cap, 8, head, color) == &q); });
		const auto actualC = Capture([&] { CHECK(&c.drawArrow(cap, 8, head, color) == &c); });
		CHECK(actualQ == expected); CHECK(actualC == expected);
	}
	const auto noCap = Capture([&] { q.drawArrow(8, SizeF{ 26, 32 }); });
	const auto flat = Capture([&] { q.drawArrow(LineCap::Flat, 8, SizeF{ 26, 32 }); });
	CHECK(noCap == flat);
}

TEST_CASE("BezierArrow.fallback_and_empty")
{
	const SceneScope scene;
	const Bezier2 curve{ { 40, 200 }, { 40, 20 }, { 300, 200 } };
	const ColorF color{ 1.0, 0.5 };
	const auto expected = Capture([&] { curve.draw(LineCap::Round, LineCap::Flat, 20, color); });
	const auto actual = Capture([&] { curve.drawArrow(LineCap::Round, 20, SizeF{ 0.001, 160 }, color); });
	CHECK(actual == expected);
	// An oversized head on a reversal would face back into the incoming shaft.
	const Bezier2 reversing{ { 140, 140 }, { 300, 140 }, { 140, 140 } };
	const auto reversedLine = Capture([&] { reversing.draw(LineCap::Round, LineCap::Flat, 8, color); });
	const auto reversedArrow = Capture([&] { reversing.drawArrow(LineCap::Round, 8, SizeF{ 60, 200 }, color); });
	CHECK(reversedArrow == reversedLine);
	const auto blank = Capture([] {});
	const auto empty = Capture([&]
	{
		curve.drawArrow(0, SizeF{ 20, 30 });
		curve.drawArrow(8, SizeF{ 0, 30 });
		curve.drawArrow(8, SizeF{ 20, -1 });
		Bezier2{ { 10, 10 }, { 10, 10 }, { 10, 10 } }.drawArrow(LineCap::Round, 8, SizeF{ 20, 30 });
		Bezier3{ { 10, 10 }, { 10, 10 }, { 10, 10 }, { 10, 10 } }.drawArrow(8, SizeF{ 20, 30 });
	});
	CHECK(empty == blank);
}

TEST_CASE("BezierArrow.curved_sizes_transforms_and_budgets")
{
	const SceneScope scene;
	const Bezier3 curve{ { 25, 155 }, { 80, -115 }, { 240, 155 }, { 340, 155 } };
	for (const double length : { 24.0, 70.0, 140.0 })
	for (const int32 limit : { 1, 256, 4096 })
	{
		const auto image = Capture([&] { curve.drawArrow(8, SizeF{ length * 0.9 - 8, length }, Palette::White, 0.5, limit); });
		const auto head = detail::FindBezierArrowHead(curve, length);
		REQUIRE(head);
		const Vec2 inside = (head->base * 0.7 + head->tip * 0.3);
		CHECK(image[static_cast<int32>(inside.y)][static_cast<int32>(inside.x)].r > 200);
	}
	const auto normal = Capture([&] { curve.drawArrow(8, SizeF{ 40, 70 }); });
	const auto translated = Capture([&]
	{
		const Transformer2D transform{ Mat3x2::Translate(0, 20) };
		curve.movedBy(0, -20).drawArrow(8, SizeF{ 40, 70 });
	});
	CHECK(normal == translated);
}

TEST_CASE("BezierArrow.curved_alpha_join_and_affine_placement")
{
	const SceneScope scene;
	const Bezier2 curve{ { 30, 170 }, { 160, 60 }, { 300, 170 } };
	const SizeF size{ 60, 70 };
	const auto head = detail::FindBezierArrowHead(curve, size.y);
	REQUIRE(head);
	const auto image = Capture([&] { curve.drawArrow(8, size, ColorF{ 1.0, 0.5 }); });
	// Sample across the join, away from the exterior edge: no gap or double blend.
	const Vec2 direction = (head->tip - head->base).normalized();
	for (int32 offset = -2; offset <= 2; ++offset)
	{
		const Vec2 p = (head->base + direction * offset);
		CHECK(image[static_cast<int32>(p.y)][static_cast<int32>(p.x)].r == Catch::Approx(128).margin(1));
	}
	const Mat3x2 affine{ 0.8f, 0.15f, 0.2f, 0.7f, 10, 15 };
	const Vec2 inside = affine.transformPoint(head->base * 0.6 + head->tip * 0.4);
	for (const double error : { 0.25, 2.0 })
	{
		const auto transformed = Capture([&]
		{
			const Transformer2D transform{ affine };
			curve.drawArrow(8, size, Palette::White, error);
		});
		CHECK(transformed[static_cast<int32>(inside.y)][static_cast<int32>(inside.x)].r > 200);
	}
}

TEST_CASE("BezierArrow.placement_benchmark", "[.benchmark]")
{
	const std::array curves{
		Bezier3{ { 25, 155 }, { 80, -115 }, { 240, 155 }, { 340, 155 } },
		Bezier3{ { 0, 0 }, { 200, 0 }, { -200, 0 }, { 0, 0 } } };
	for (const auto& curve : curves)
	{
		double sum = 0;
		const auto begin = std::chrono::steady_clock::now();
		for (int32 i = 0; i < 10000; ++i)
		{
			const auto head = detail::FindBezierArrowHead(curve, 20.0 + (i % 120));
			if (head) sum += head->parameter;
		}
		const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - begin).count();
		std::cout << "BezierArrow placement ns/call=" << (ns / 10000.0) << " checksum=" << sum << '\n';
	}
}

TEST_CASE("BezierArrow.submission_benchmark", "[.benchmark]")
{
	const SceneScope scene;
	const Bezier2 quadratic{ { 25, 155 }, { 150, -115 }, { 340, 155 } };
	const Bezier3 cubic{ { 25, 155 }, { 80, -115 }, { 240, 155 }, { 340, 155 } };
	const auto measure = [&](const auto& curve, const char* name)
	{
		std::array<Array<double>, 2> times;
		for (int32 round = -2; round < 9; ++round)
		for (int32 offset = 0; offset < 2; ++offset)
		{
			const int32 method = ((round + offset + 2) % 2);
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(2000, 2000) };
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < 256; ++i)
				{
					const auto moved = curve.movedBy(i * 0.1, 0);
					if (method == 0) moved.draw(LineCap::Round, LineCap::Flat, 8);
					else moved.drawArrow(LineCap::Round, 8, SizeF{ 40, 32.0 + (i % 100) });
				}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / 256;
				if (round >= 0) times[method].push_back(ns);
			}
		}
		for (auto& values : times) values.sort();
		std::cout << "BezierArrow submission " << name << " median ns/call: draw=" << times[0][4]
			<< " drawArrow=" << times[1][4] << '\n';
	};
	measure(quadratic, "quadratic");
	measure(cubic, "cubic");
	REQUIRE(System::Update());
}
