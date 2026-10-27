//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Geometry2D/CircularShapes.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <chrono>
# include <atomic>
# include <iostream>
# if SIV3D_PLATFORM(MACOS)
# include <pthread.h>
# include <pthread/qos.h>
# include <time.h>
# endif

namespace
{
	namespace CircularGeometry = detail::CircularShapes;
	using Index = Vertex2D::IndexType;
	constexpr Index Offset = 37;
	constexpr Float4 Ink{ 0.3f, 0.7f, 1.0f, 0.5f };

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
	};

	template <class Build>
	Mesh CaptureMesh(Build&& build)
	{
		Mesh mesh;
		Vertex2D guard;
		guard.set(-123, -456, -7, -8, Float4{ -1, -2, -3, -4 });
		uint32 requests = 0;
		const auto allocate = [&](uint16 vc, uint32 ic)
		{
			++requests;
			mesh.vertices.assign(vc + size_t{ 2 }, guard);
			mesh.indices.assign(ic + size_t{ 2 }, Index{ 65535 });
			return Vertex2DBufferPointer{ mesh.vertices.data() + 1, mesh.indices.data() + 1, Offset };
		};
		const auto count = build(allocate);
		REQUIRE(requests == 1);
		REQUIRE(count + size_t{ 2 } == mesh.indices.size());
		for (const auto* v : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(v->pos == guard.pos); CHECK(v->tex == guard.tex); CHECK(v->color == guard.color);
		}
		CHECK(mesh.indices.front() == 65535); CHECK(mesh.indices.back() == 65535);
		mesh.vertices.pop_back(); mesh.vertices.erase(mesh.vertices.begin());
		mesh.indices.pop_back(); mesh.indices.erase(mesh.indices.begin());
		for (const auto& v : mesh.vertices)
		{
			CHECK(v.color == Ink); CHECK(v.tex == guard.tex);
			CHECK(std::isfinite(v.pos.x)); CHECK(std::isfinite(v.pos.y));
		}
		for (const Index index : mesh.indices) REQUIRE((Offset <= index && index < Offset + mesh.vertices.size()));
		return mesh;
	}

	double CheckMesh(const Mesh& mesh, const Shape2D& shape)
	{
		REQUIRE(mesh.vertices.size() == shape.vertices().size());
		REQUIRE(mesh.indices.size() == shape.indices().size() * 3);
		for (size_t i = 0; i < mesh.vertices.size(); ++i) CHECK(mesh.vertices[i].pos == shape.vertices()[i]);
		double area = 0;
		for (size_t i = 0; i < shape.indices().size(); ++i)
		{
			const auto t = shape.indices()[i];
			CHECK(mesh.indices[i * 3] == Offset + t.i0);
			CHECK(mesh.indices[i * 3 + 1] == Offset + t.i1);
			CHECK(mesh.indices[i * 3 + 2] == Offset + t.i2);
			const Vec2 a = shape.vertices()[t.i0], b = shape.vertices()[t.i1], c = shape.vertices()[t.i2];
			const double triangleArea = (b - a).cross(c - a) * 0.5;
			CHECK(triangleArea >= -1e-6);
			area += triangleArea;
		}
		double boundaryArea = 0;
		for (size_t i = 0; i < shape.vertices().size(); ++i)
		{
			const Vec2 a = shape.vertices()[i], b = shape.vertices()[(i + 1) % shape.vertices().size()];
			boundaryArea += a.cross(b) * 0.5;
		}
		CHECK(area == Catch::Approx(boundaryArea).margin(1e-7));
		const Polygon polygon{ shape };
		CHECK(polygon.area() == Catch::Approx(area).margin(1e-5));
		return area;
	}

	bool Inside(const Vec2 p, const Array<Float2>& boundary)
	{
		bool inside = false;
		for (size_t i = 0, j = boundary.size() - 1; i < boundary.size(); j = i++)
		{
			const Vec2 a = boundary[i], b = boundary[j];
			if (((a.y > p.y) != (b.y > p.y)) && (p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x))
				inside = not inside;
		}
		return inside;
	}

	void CheckCoverage(const Shape2D& shape)
	{
		const RectF bounds = shape.boundingRect();
		for (int32 y = 0; y < 31; ++y)
		for (int32 x = 0; x < 31; ++x)
		{
			const Vec2 p{ bounds.x + bounds.w * ((x + 0.371) / 31), bounds.y + bounds.h * ((y + 0.619) / 31) };
			int32 coverage = 0;
			bool edge = false;
			for (const auto t : shape.indices())
			{
				const Vec2 a = shape.vertices()[t.i0], b = shape.vertices()[t.i1], c = shape.vertices()[t.i2];
				const double s0 = (b - a).cross(p - a), s1 = (c - b).cross(p - b), s2 = (a - c).cross(p - c);
				if ((Abs(s0) < 1e-6) || (Abs(s1) < 1e-6) || (Abs(s2) < 1e-6)) edge = true;
				if ((0 < s0) && (0 < s1) && (0 < s2)) ++coverage;
			}
			if (not edge) CHECK(coverage == int32(Inside(p, shape.vertices())));
		}
	}

	struct SceneScope
	{
		const SceneConfig config = Scene::GetConfig();
		const ColorF background = Scene::GetBackground();
		const bool vsync = Graphics::IsVSyncEnabled();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedRenderStates2D blend{ BlendState::Default2D };
		ScopedColorMul2D mul{ Palette::White };
		ScopedColorAdd2D add{ 0.0 };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };
		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 480, 360 }));
			Scene::SetBackground(Palette::Black);
		}
		~SceneScope()
		{
			Scene::Configure(config); Scene::SetBackground(background); Graphics::SetVSyncEnabled(vsync);
		}
	};

	template <class Draw>
	Image Capture(Draw&& draw)
	{
		REQUIRE(System::Update()); draw(); ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update()); REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}
}

TEST_CASE("CircularShapes.crescent_geometry_mesh_and_quality")
{
	for (const double radius : { -64.0, 64.0 })
	for (const double width : { 0.01, 1.0, 36.0, 64.0, 120.0, 127.999 })
	for (const double angle : { -0.7, 0.0, Math::HalfPi })
	for (const float quality : { 0.0f, 1.0f, 16.0f })
	{
		CAPTURE(radius, width, angle, quality);
		const Circle circle{ 20, -30, radius };
		const auto shape = Shape2D::Crescent(circle, angle, width, QualityFactor{ quality });
		const auto mesh = CaptureMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildCircleCrescent(allocate, circle, angle, width, Ink, quality);
		});
		CHECK(shape.vertices().size() <= 504);
		CHECK(shape.indices().size() + 2 == shape.vertices().size());
		const double area = CheckMesh(mesh, shape);
		const Vec2 axis{ std::sin(angle), -std::cos(angle) }, cut = circle.center + axis * width;
		const auto placement = CircularGeometry::PlaceCrescent(circle, angle, width);
		REQUIRE(placement); REQUIRE_FALSE(placement->circleOnly);
		const size_t n = shape.vertices().size() / 2;
		for (size_t i = 0; i < shape.vertices().size(); ++i)
		{
			const Vec2 p = shape.vertices()[i];
			CHECK(p.distanceFrom((i <= n) ? circle.center : cut) == Catch::Approx(64).margin(1e-5));
		}
		const Vec2 perpendicular{ -axis.y, axis.x };
		const Vec2 origin = circle.center + axis * (width * 0.5);
		CHECK(shape.vertices()[0] == Float2{ origin + perpendicular * placement->halfChord });
		CHECK(shape.vertices()[n] == Float2{ origin - perpendicular * placement->halfChord });
		if (quality == 16)
		{
			const double expected = (2 * 64 * 64 * std::asin(width / 128) + width * std::sqrt(64 * 64 - width * width / 4));
			CHECK(area == Catch::Approx(expected).epsilon(0.001));
		}
	}
}

TEST_CASE("CircularShapes.teardrop_tangents_mesh_and_quality")
{
	for (const double radius : { -32.0, 32.0 })
	for (const double distance : { 32.01, 40.0, 100.0, 1000.0 })
	for (const double angle : { -0.7, 0.0, 1.8 })
	for (const float quality : { 0.0f, 1.0f, 16.0f })
	{
		CAPTURE(radius, distance, angle, quality);
		const Circle circle{ -10, 30, radius };
		const Vec2 tip = circle.center + Vec2{ std::cos(angle), std::sin(angle) } * distance;
		const auto shape = Shape2D::Teardrop(circle, tip, QualityFactor{ quality });
		const auto mesh = CaptureMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildCircleTeardrop(allocate, circle, tip, Ink, quality);
		});
		CHECK(shape.vertices().size() <= 254);
		const double area = CheckMesh(mesh, shape);
		CHECK(shape.vertices().front() == Float2{ tip });
		for (size_t i = 1; i < shape.vertices().size(); ++i)
			CHECK(Vec2{ shape.vertices()[i] }.distanceFrom(circle.center) == Catch::Approx(32).margin(1e-5));
		for (const Vec2 root : { Vec2{ shape.vertices()[1] }, Vec2{ shape.vertices().back() } })
			CHECK(Abs((root - circle.center).dot(tip - root)) < 0.02);
		if (quality == 16)
		{
			const double expected = (32 * 32 * (Math::Pi - std::acos(32 / distance)) + 32 * std::sqrt(distance * distance - 32 * 32));
			CHECK(area == Catch::Approx(expected).epsilon(0.001));
		}
	}
}

TEST_CASE("CircularShapes.single_coverage_and_image_outline")
{
	for (const double width : { 0.1, 24.0, 64.0, 127.9 })
		CheckCoverage(Shape2D::Crescent(Circle{ 0, 0, 64 }, 0.37, width));
	for (const Vec2 tip : { Vec2{ 0, -64.1 }, Vec2{ 100, 150 }, Vec2{ -1000, 0 } })
		CheckCoverage(Shape2D::Teardrop(Circle{ 0, 0, 64 }, tip));
	for (const auto& shape : { Shape2D::Crescent(Circle{ 120, 120, 60 }, Math::HalfPi, 30),
		Shape2D::Teardrop(Circle{ 120, 100, 35 }, { 120, 190 }) })
	{
		Image image{ 240, 240, Palette::Black };
		shape.paint(image, Palette::White, EnableAntialiasing::No);
		int32 white = 0;
		for (const auto pixel : image) if (pixel.r == 255) ++white;
		CHECK(white > 500);
		const Polygon polygon{ shape };
		CHECK(polygon.outer().size() == shape.vertices().size());
	}
}

TEST_CASE("CircularShapes.empty_circle_fallback_and_allocation_failure")
{
	const Circle circle{ 10, 20, 50 };
	for (const double width : { -1.0, 0.0 })
		CHECK(Shape2D::Crescent(circle, 1, width).vertices().empty());
	CHECK(Shape2D::Crescent(Circle{ 0, 0, 0 }, 0, 30).vertices().empty());
	CHECK(Shape2D::Teardrop(Circle{ 0, 0, 0 }, { 20, 20 }).vertices().empty());
	for (const double width : { 100.0, 200.0 })
	{
		const auto p = CircularGeometry::PlaceCrescent(circle, 0.7, width);
		REQUIRE(p); CHECK(p->circleOnly);
		const Polygon polygon{ Shape2D::Crescent(circle, 0.7, width) };
		CHECK(polygon.area() == Catch::Approx(circle.area()).epsilon(0.01));
	}
	for (const Vec2 tip : { Vec2{ 10, 20 }, Vec2{ 10, -30 }, Vec2{ 40, 60 } })
	{
		const auto p = CircularGeometry::PlaceTeardrop(circle, tip);
		REQUIRE(p); CHECK(p->circleOnly);
		const Polygon polygon{ Shape2D::Teardrop(circle, tip) };
		CHECK(polygon.area() == Catch::Approx(circle.area()).epsilon(0.01));
	}
	uint32 calls = 0;
	const auto failure = [&](uint16, uint32) { ++calls; return Vertex2DBufferPointer{}; };
	CHECK(Vertex2DBuilder::BuildCircleCrescent(failure, circle, 0, 0, Ink, 1) == 0);
	CHECK(Vertex2DBuilder::BuildCircleTeardrop(failure, Circle{ 0, 0, 0 }, { 20, 20 }, Ink, 1) == 0);
	CHECK(calls == 0);
	CHECK(Vertex2DBuilder::BuildCircleCrescent(failure, circle, 0, 30, Ink, 1) == 0);
	CHECK(Vertex2DBuilder::BuildCircleTeardrop(failure, circle, { 100, 120 }, Ink, 1) == 0);
	CHECK(Vertex2DBuilder::BuildCircleCrescent(failure, circle, 0, 100, Ink, 1) == 0);
	CHECK(Vertex2DBuilder::BuildCircleTeardrop(failure, circle, circle.center, Ink, 1) == 0);
	CHECK(calls == 4);
}

TEST_CASE("CircularShapes.public_rendering_transform_alpha_and_fallback")
{
	const SceneScope scope;
	const Circle circle{ 230, 170, 64 };
	const Vec2 tip{ 340, 290 };
	const ColorF ink{ 1.0, 0.5 };
	for (const Mat3x2 matrix : { Mat3x2::Identity(), Mat3x2{ -0.8f, 0.1f, 0.15f, 0.9f, 400, 10 } })
	for (const bool crescent : { false, true })
	{
		const auto direct = Capture([&]
		{
			const Transformer2D t{ matrix };
			if (crescent) CHECK(&circle.drawCrescent(0.7, 36, ink) == &circle);
			else CHECK(&circle.drawTeardrop(tip, ink) == &circle);
		});
		const auto saved = Capture([&]
		{
			const Transformer2D t{ matrix };
			const auto q = QualityFactor::FromSceneScaling();
			const Shape2D shape = (crescent ? Shape2D::Crescent(circle, 0.7, 36, q) : Shape2D::Teardrop(circle, tip, q));
			shape.draw(ink);
		});
		CHECK(direct == saved);
		uint8 maximum = 0;
		for (const auto pixel : direct) maximum = Max(maximum, pixel.r);
		// Half-opacity white over black is 127.5 in UNORM8. Allow either
		// adjacent integer while retaining the upper bound against double coverage.
		CHECK(int32(maximum) >= 127);
		CHECK(int32(maximum) <= 128);
	}
	const auto disk = Capture([&] { circle.draw(ink); });
	CHECK(disk == Capture([&] { circle.drawCrescent(0.3, 128, ink); }));
	CHECK(disk == Capture([&] { circle.drawTeardrop(circle.center, ink); }));
	const auto blank = Capture([] {});
	CHECK(blank == Capture([&]
	{
		circle.drawCrescent(0, 0);
		Circle{ 0, 0, 0 }.drawTeardrop({ 30, 40 });
		Circle{ 0, 0, 0 }.drawCrescent(0, 10);
	}));
	const Circle negative{ circle.center, -circle.r };
	CHECK(Capture([&] { circle.drawCrescent(0.3, 36, ink); })
		== Capture([&] { negative.drawCrescent(0.3, 36, ink); }));
	CHECK(Capture([&] { circle.drawTeardrop(tip, ink); })
		== Capture([&] { negative.drawTeardrop(tip, ink); }));
}

namespace
{
	double LensArea(const double a, const double h)
	{
		// Independent quadrature of the intersection of two equal disks.
		// x = a * sin(t) removes the circle limit's endpoint singularity.
		const long double k = (static_cast<long double>(a) * a - static_cast<long double>(h) * h) / (2 * h);
		constexpr int32 Steps = 256;
		long double sum = 0;
		for (int32 i = 0; i <= Steps; ++i)
		{
			const long double c = std::cos(static_cast<long double>(Math::HalfPi) * i / Steps);
			const long double z = (a * c) * (a * c);
			const long double y = z / (std::sqrt(k * k + z) + k);
			sum += (i == 0 || i == Steps ? 1 : i % 2 ? 4 : 2) * y * c;
		}
		return static_cast<double>(4 * a * sum * Math::HalfPi / (3 * Steps));
	}
}

TEST_CASE("CircularShapes.lens_geometry_symmetry_area_and_upload")
{
	for (const double a : { 8.0, 80.0, 500.0 })
	for (const double ratio : { 0.0001, 0.05, 0.4, 0.999999, 1.0 })
	for (const double angle : { 0.0, 0.37, -1.2 })
	for (const double quality : { 0.0, 1.0, 8.0 })
	{
		CAPTURE(a, ratio, angle, quality);
		const Vec2 center{ 20, -30 }, axis{ std::cos(angle), std::sin(angle) }, normal{ axis.y, -axis.x };
		const Vec2 from = center - axis * a, to = center + axis * a;
		// Use the actual distance so the inclusive circle boundary is unambiguous.
		const double halfLength = from.distanceFrom(to) * 0.5, h = (halfLength * ratio);
		const auto shape = Shape2D::Lens(from, to, h, QualityFactor{ quality });
		const auto reversed = Shape2D::Lens(to, from, h, QualityFactor{ quality });
		const size_t count = shape.vertices().size(), n = count / 2;
		REQUIRE(count >= 4); REQUIRE(count <= 252); CHECK(count % 4 == 0);
		REQUIRE(reversed.vertices().size() == count);
		CHECK(shape.vertices()[0] == Float2{ from }); CHECK(shape.vertices()[n] == Float2{ to });
		CHECK(Vec2{ shape.vertices()[count / 4] }.distanceFrom(center + normal * h) < 0.0001);
		CHECK(Vec2{ shape.vertices()[count * 3 / 4] }.distanceFrom(center - normal * h) < 0.0001);
		const auto mesh = CaptureMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildPolygon(allocate, shape.vertices(), shape.indices(), none, Ink);
		});
		const double area = CheckMesh(mesh, shape), expected = LensArea(halfLength, h);
		CHECK(area <= expected + halfLength * 0.0001);
		const double intervals = static_cast<double>(n);
		CHECK(area == Catch::Approx(expected).epsilon(2.0 / (intervals * intervals)).margin(halfLength * 0.0001));
		for (size_t i = 0; i < count; ++i)
		{
			const Vec2 p = shape.vertices()[i], local = p - center;
			const double x = local.dot(axis), y = local.dot(normal);
			CHECK(Abs(x) <= halfLength + 0.0001); CHECK(Abs(y) <= h + 0.0001);
			// The circle equation multiplied by h avoids a huge radius at small h.
			const double residual = h * (x * x + y * y - halfLength * halfLength)
				+ (halfLength * halfLength - h * h) * Abs(y);
			CHECK(Abs(residual) <= halfLength * halfLength * 0.0002);
			CHECK(Vec2{ reversed.vertices()[(i + n) % count] }.distanceFrom(p) < 0.0001);
			CHECK(Vec2{ shape.vertices()[(count - i) % count] }.distanceFrom(center + axis * x - normal * y) < 0.0001);
			CHECK(Vec2{ shape.vertices()[(i + n) % count] }.distanceFrom(center - local) < 0.0001);
		}
	}
}

TEST_CASE("CircularShapes.lens_empty_thin_circle_and_quality")
{
	const Vec2 from{ -80, 0 }, to{ 80, 0 };
	for (const double h : { -1.0, 0.0, std::nextafter(80.0, 81.0), 100.0 })
	{
		const auto shape = Shape2D::Lens(from, to, h);
		CHECK(shape.vertices().empty()); CHECK(shape.indices().empty());
	}
	CHECK(Shape2D::Lens(from, from, 1).vertices().empty());
	for (const double h : { 1e-10, 0.001, 20.0, std::nextafter(80.0, 0.0), 80.0 })
	{
		size_t previous = 0;
		for (const double q : { 0.0, 1.0, 4.0, 100.0 })
		{
			const auto shape = Shape2D::Lens(from, to, h, QualityFactor{ q });
			CHECK(shape.vertices().size() >= previous); previous = shape.vertices().size();
			CHECK(shape.vertices()[previous / 4].y == static_cast<float>(-h));
			if (h == 80)
			{
				CHECK(previous == QualityFactor{ q }.toPointsPerCircle(80).value());
				for (const auto p : shape.vertices()) CHECK(Vec2{ p }.length() == Catch::Approx(80).margin(0.00001));
			}
		}
	}
	const auto disk = Shape2D::Lens(from, to, 80), nearDisk = Shape2D::Lens(from, to, std::nextafter(80.0, 0.0));
	CHECK(disk.vertices() == nearDisk.vertices());
	const auto original = Shape2D::Lens(from, to, 24);
	const Transformer2D scale{ Mat3x2::Scale(4) };
	CHECK(Shape2D::Lens(from, to, 24).vertices() == original.vertices());
	CHECK(Shape2D::Lens(from, to, 24, QualityFactor::FromSceneScaling()).vertices().size() > original.vertices().size());
}

TEST_CASE("CircularShapes.lens_coverage_image_and_public_rendering")
{
	const SceneScope scope;
	for (const double h : { 0.1, 16.0, 60.0, 79.999, 80.0 })
	{
		const auto shape = Shape2D::Lens({ 80, 160 }, { 240, 160 }, h);
		const Polygon polygon{ shape };
		CheckCoverage(shape);
		Image painted{ 320, 320, Palette::Black }, reference = painted;
		shape.paint(painted, Color{ 255, 128 }, EnableAntialiasing::No);
		polygon.paint(reference, Color{ 255, 128 }, EnableAntialiasing::No);
		CHECK(painted == reference);
		shape.overwrite(painted, Palette::White); polygon.overwrite(reference, Palette::White);
		CHECK(painted == reference);
		Image frame{ 320, 320, Palette::Black }, frameReference = frame;
		shape.paintFrame(frame, 2, Palette::White); polygon.paintFrame(frameReference, 2, Palette::White);
		CHECK(frame == frameReference);
		if (h >= 16) CHECK(frame[160][160].r == 0); // No fan spokes in the outline.
		for (const Mat3x2 matrix : { Mat3x2::Identity(), Mat3x2{ -0.8f, 0.1f, 0.15f, 0.9f, 400, 10 } })
		{
			const auto actual = Capture([&] { const Transformer2D t{ matrix }; shape.draw(ColorF{ 1.0, 0.5 }); });
			CHECK(actual == Capture([&] { const Transformer2D t{ matrix }; polygon.draw(ColorF{ 1.0, 0.5 }); }));
			uint8 maximum = 0;
			for (const auto pixel : actual) maximum = Max(maximum, pixel.r);
			CHECK(maximum <= 129); if (h >= 16) CHECK(maximum >= 127);
		}
		const auto outline = Capture([&] { shape.drawFrame(3); });
		CHECK(outline == Capture([&] { polygon.drawFrame(3); }));
	}
	CHECK(Capture([] {}) == Capture([] { Shape2D::Lens({ 80, 80 }, { 120, 80 }, 30).draw().drawFrame(); }));
}

TEST_CASE("CircularShapes.lens_submission_benchmark", "[.benchmark]")
{
	const SceneScope scope;
	Graphics::SetVSyncEnabled(false);
# if SIV3D_PLATFORM(MACOS)
	struct QoS
	{
		pthread_override_t value = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		~QoS() { if (value) pthread_override_qos_class_end_np(value); }
	} qos;
# endif
	const char* methods[]{ "construct", "constructDraw", "cachedShape", "cachedPolygon", "shapeFrame", "polygonFrame" };
	for (const auto size : { Vec2{ 80, 24 }, Vec2{ 80, 0.1 }, Vec2{ 80, 79.999 }, Vec2{ 500, 500 } })
	{
		const Vec2 from{ -size.x, 0 }, to{ size.x, 0 };
		const auto shape = Shape2D::Lens(from, to, size.y);
		const Polygon polygon{ shape };
		std::array<Array<double>, 6> times;
		std::array<int64, 6> triangles{};
		constexpr int32 Batch = 128;
		for (int32 round = -4; round < 21; ++round)
		for (int32 offset = 0; offset < 6; ++offset)
		{
			const int32 method = ((round + 6 + offset) % 6);
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(10000, 10000) };
				const auto warm = std::chrono::steady_clock::now();
				while (std::chrono::steady_clock::now() - warm < std::chrono::milliseconds(5)) std::atomic_signal_fence(std::memory_order_seq_cst);
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < Batch; ++i)
				{
					if (method <= 1)
					{
						const auto temporary = Shape2D::Lens(from, to, size.y);
						if (method == 1) temporary.draw();
						std::atomic_signal_fence(std::memory_order_seq_cst);
					}
					else if (method == 2) shape.draw();
					else if (method == 3) polygon.draw();
					else if (method == 4) shape.drawFrame(2);
					else polygon.drawFrame(2);
				}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / Batch;
				if (round >= 0) times[method].push_back(ns);
			}
			REQUIRE(System::Update());
			const int64 count = Profiler::GetFrameMetrics().triangleCount;
			if (round == 0) triangles[method] = count;
			if (round > 0) CHECK(count == triangles[method]);
		}
		CHECK(triangles[0] == 0); CHECK(triangles[1] == triangles[2]); CHECK(triangles[2] == triangles[3]);
		CHECK(triangles[4] == triangles[5]);
		for (size_t method = 0; method < times.size(); ++method)
		{
			times[method].sort();
			std::cout << "Lens a=" << size.x << " h=" << size.y << ' ' << methods[method]
				<< " vertices=" << shape.vertices().size() << " triangles/call=" << double(triangles[method]) / Batch
				<< " ns/call=" << times[method][10] << " p10=" << times[method][2] << " p90=" << times[method][18] << '\n';
		}
	}
}

TEST_CASE("CircularShapes.submission_benchmark", "[.benchmark]")
{
	const SceneScope scope;
	Graphics::SetVSyncEnabled(false);
# if SIV3D_PLATFORM(MACOS)
	struct QoS
	{
		pthread_override_t value = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		~QoS() { if (value) pthread_override_qos_class_end_np(value); }
	} qos;
	const auto cpuTime = []
	{
		timespec t{};
		REQUIRE(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
		return (int64(t.tv_sec) * 1'000'000'000 + t.tv_nsec);
	};
# endif
	const char* methods[]{ "direct", "temporaryShape", "cachedShape", "cachedPolygon" };
	for (const bool crescent : { true, false })
	for (const double radius : { 64.0, 1000.0 })
	for (const bool changing : { false, true })
	{
		const Circle circle{ 230, 170, radius };
		Array<Shape2D> shapes;
		Array<Polygon> polygons;
		for (int32 i = 0; i < 32; ++i)
		{
			const double phase = i / 31.0;
			const Vec2 tip = circle.center + Vec2{ 1.3 + phase, 1.8 } * radius;
			shapes.push_back(crescent ? Shape2D::Crescent(circle, 0.7 + phase, radius * (0.2 + phase))
				: Shape2D::Teardrop(circle, tip));
			polygons.emplace_back(shapes.back());
		}
		std::array<Array<double>, 4> wall;
# if SIV3D_PLATFORM(MACOS)
		std::array<Array<double>, 4> cpu;
# endif
		std::array<int64, 4> triangles{}, draws{};
		for (int32 round = -8; round < 21; ++round)
		for (int32 offset = 0; offset < 4; ++offset)
		{
			const int32 method = ((round + offset + 8) % 4);
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(10000, 10000) };
# if SIV3D_PLATFORM(MACOS)
				const int64 cpuBegin = cpuTime();
# endif
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < 128; ++i)
				{
					const int32 index = (changing ? (i % 32) : 0);
					const double phase = index / 31.0;
					const Vec2 tip = circle.center + Vec2{ 1.3 + phase, 1.8 } * radius;
					if (method == 0)
					{
						if (crescent) circle.drawCrescent(0.7 + phase, radius * (0.2 + phase));
						else circle.drawTeardrop(tip);
					}
					else if (method == 1)
					{
						if (crescent) Shape2D::Crescent(circle, 0.7 + phase, radius * (0.2 + phase)).draw();
						else Shape2D::Teardrop(circle, tip).draw();
					}
					else if (method == 2) shapes[index].draw();
					else polygons[index].draw();
				}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / 128;
# if SIV3D_PLATFORM(MACOS)
				const double cpuNs = double(cpuTime() - cpuBegin) / 128;
				if (round >= 0) cpu[method].push_back(cpuNs);
# endif
				if (round >= 0) wall[method].push_back(ns);
			}
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			if (round == 0) { triangles[method] = metrics.triangleCount; draws[method] = metrics.drawCalls; }
			if (round >= 0) { CHECK(metrics.triangleCount == triangles[method]); CHECK(metrics.drawCalls == draws[method]); }
		}
		for (int32 method = 0; method < 4; ++method)
		{
			CHECK(triangles[method] == triangles[0]);
			CHECK(draws[method] == draws[0]);
			wall[method].sort();
			std::cout << "CircularShapes " << (crescent ? "crescent" : "teardrop") << " r=" << radius
				<< (changing ? " changing " : " fixed ") << methods[method] << " ns/call=" << wall[method][10]
				<< " p10=" << wall[method][2] << " p90=" << wall[method][18]
				<< " triangles/call=" << double(triangles[method]) / 128 << " draws/batch=" << draws[method];
# if SIV3D_PLATFORM(MACOS)
			cpu[method].sort();
			std::cout << " cpu_ns/call=" << cpu[method][10] << " cpu_p10=" << cpu[method][2] << " cpu_p90=" << cpu[method][18];
# endif
			std::cout << '\n';
		}
	}
}
