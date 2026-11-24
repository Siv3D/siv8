//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "ImageDrawingTestHelper.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <cmath>

namespace
{
	using Index = Vertex2D::IndexType;
	const Float2 Center{ 180, 140 };
	const Float4 Ink{ 1, 1, 1, 0.5f };

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
		Index offset = 37;
		uint32 allocations = 0;
	};

	Mesh Build(const float inner, const float outer, const double start, const double sweep, const double trim,
		const float scale = 1, const bool fail = false, const bool atLimit = false)
	{
		Mesh mesh;
		Vertex2D guard;
		guard.set(-123, -456, -7, -8, Float4{ -1, -2, -3, -4 });
		const auto allocate = [&](const uint16 vc, const uint32 ic)
		{
			++mesh.allocations;
			if (fail) return Vertex2DBufferPointer{ nullptr, nullptr, 0 };
			mesh.vertices.assign(vc + size_t{ 2 }, guard);
			mesh.indices.assign(ic + size_t{ 2 }, Index{ 65535 });
			mesh.offset = (atLimit ? static_cast<Index>(65536 - vc) : Index{ 37 });
			return Vertex2DBufferPointer{ mesh.vertices.data() + 1, mesh.indices.data() + 1, mesh.offset };
		};
		const auto count = Vertex2DBuilder::BuildCircleTrimmedArc(allocate, Center, inner, outer, start, sweep, trim, Ink, scale);
		if (mesh.vertices.isEmpty())
		{
			CHECK(count == 0);
			return mesh;
		}
		CHECK(mesh.allocations == 1);
		REQUIRE(mesh.indices.size() == count + size_t{ 2 });
		CHECK(mesh.indices.front() == 65535);
		CHECK(mesh.indices.back() == 65535);
		for (const auto* v : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(v->pos == guard.pos);
			CHECK(v->color == guard.color);
		}
		for (const auto& v : mesh.vertices) CHECK(v.tex == guard.tex);
		mesh.vertices.pop_back(); mesh.vertices.erase(mesh.vertices.begin());
		mesh.indices.pop_back(); mesh.indices.erase(mesh.indices.begin());
		for (const auto index : mesh.indices) REQUIRE((mesh.offset <= index && index < mesh.offset + mesh.vertices.size()));
		return mesh;
	}

	Vec2 StartNormal(const double start) { return{ std::cos(start), std::sin(start) }; }
	Vec2 EndNormal(const double end) { return -StartNormal(end); }

	double RayDistance(const Vec2 point, const Vec2 direction)
	{
		return ((point.dot(direction) <= 0) ? point.length() : Abs(point.cross(direction)));
	}

	struct SceneScope
	{
		const SceneConfig config = Scene::GetConfig();
		const ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedRenderStates2D blend{ BlendState::Default2D };
		ScopedColorMul2D mul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D add{ 0.0, ScopedColorAdd2D::Target::Set };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };
		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 360, 280 }, SceneResolution::SceneSize));
			Scene::SetBackground(Palette::Black);
		}
		~SceneScope()
		{
			Scene::Configure(config);
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

	// Independent point oracle: angular containment and distance to the two rays.
	// The rasterizer instead clips analytic horizontal ring spans to half-planes.
	Image TrimmedArcMask(const Size size, const Circle& circle, const double start, const double sweep,
		const double innerThickness, const double outerThickness, const double trim, const bool aa)
	{
		const double inner = Max(Abs(circle.r) - innerThickness, 0.0), outer = (Abs(circle.r) + outerThickness);
		const double clockwiseStart = (start + (sweep < 0 ? sweep : 0));
		const Vec2 first{ std::sin(start), -std::cos(start) };
		const Vec2 last{ std::sin(start + sweep), -std::cos(start + sweep) };
		return ImageDrawingTest::SampleMask(size, aa, [&](const Vec2 point)
			{
				const Vec2 p = (point - circle.center);
				const double radius = p.length();
				if ((radius < inner) || (outer < radius)) return false;
				double angle = std::fmod(std::atan2(p.x, -p.y) - clockwiseStart, Math::TwoPi);
				if (angle < 0) angle += Math::TwoPi;
				return ((angle <= Abs(sweep)) && (trim <= RayDistance(p, first)) && (trim <= RayDistance(p, last)));
			});
	}

	void WriteTrimmedArc(const Circle& circle, Image& image, const bool paint, const double start, const double sweep,
		const double inner, const double outer, const double trim, const Color ink, const EnableAntialiasing aa)
	{
		if (paint) CHECK(&circle.paintTrimmedArc(image, start, sweep, inner, outer, trim, ink, aa) == &circle);
		else CHECK(&circle.overwriteTrimmedArc(image, start, sweep, inner, outer, trim, ink, aa) == &circle);
	}
}

TEST_CASE("CircleTrimmedArc.geometry_and_upload_bounds")
{
	for (const float inner : { 0.0f, 3.0f, 35.0f })
	for (const double sweep : { 0.07, Math::Pi / 3, Math::Pi, 4.7, Math::TwoPi - 0.001 })
	for (const double trim : { 0.2, 4.0, 40.0, 79.0 })
	for (const float scale : { 0.0f, 1.0f, 512.0f })
	{
		CAPTURE(inner, sweep, trim, scale);
		const double start = 0.3;
		const auto mesh = Build(inner, 80, start, sweep, trim, scale, false, true);
		const bool empty = ((sweep < Math::Pi) && (80 * std::sin(sweep / 2) <= trim));
		REQUIRE(mesh.vertices.isEmpty() == empty);
		if (empty) { CHECK(mesh.allocations == 0); continue; }
		CHECK(mesh.vertices.size() <= 512);
		const Vec2 n0 = StartNormal(start), n1 = EndNormal(start + sweep);
		const Vec2 ray0{ n0.y, -n0.x }, ray1{ -n1.y, n1.x };
		for (size_t i = 0; i < mesh.vertices.size(); ++i)
		{
			const auto& v = mesh.vertices[i];
			CHECK(v.color == Ink);
			const Vec2 p = (Vec2{ v.pos } - Center);
			const double r = p.length();
			const double d0 = RayDistance(p, ray0), d1 = RayDistance(p, ray1);
			if (i % 2) CHECK(r == Catch::Approx(80).margin(0.0001));
			else
			{
				CHECK((inner - 0.0001 <= r && r <= 80.0001));
				CHECK((Abs(r - inner) < 0.0001 || Abs(d0 - trim) < 0.0001 || Abs(d1 - trim) < 0.0001));
			}
			CHECK((d0 >= trim - 0.0001 && d1 >= trim - 0.0001));
			if (sweep <= Math::Pi) CHECK((p.dot(n0) >= -0.0001 && p.dot(n1) >= -0.0001));
			else CHECK((p.dot(n0) >= -0.0001 || p.dot(n1) >= -0.0001));
		}
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			const Vec2 a = mesh.vertices[mesh.indices[i] - mesh.offset].pos;
			const Vec2 b = mesh.vertices[mesh.indices[i + 1] - mesh.offset].pos;
			const Vec2 c = mesh.vertices[mesh.indices[i + 2] - mesh.offset].pos;
			CHECK((b - a).cross(c - a) >= -0.0001);
		}
	}
}

TEST_CASE("CircleTrimmedArc.parallel_sides_and_reverse")
{
	for (const double sweep : { 0.4, 1.2, Math::Pi, 4.7, Math::TwoPi - 0.01 })
	{
		const auto forward = Build(40, 100, 0.25, sweep, 6);
		const auto reverse = Build(40, 100, 0.25 + sweep, -sweep, 6);
		REQUIRE(forward.indices == reverse.indices);
		REQUIRE(forward.vertices.size() == reverse.vertices.size());
		for (size_t i = 0; i < forward.vertices.size(); ++i)
		{
			CHECK(forward.vertices[i].pos.distanceFrom(reverse.vertices[i].pos) < 0.0001f);
		}
		const auto next = Build(40, 100, 0.25 + sweep, sweep, 6);
		const Vec2 boundary = StartNormal(0.25 + sweep);
		// All cut-line vertices on the shared boundary are six units from it.
		uint32 firstSide = 0, nextSide = 0;
		for (const auto& v : forward.vertices)
			if (Abs((Vec2{ v.pos } - Center).dot(boundary) + 6) < 0.0001) ++firstSide;
		for (const auto& v : next.vertices)
			if (Abs((Vec2{ v.pos } - Center).dot(boundary) - 6) < 0.0001) ++nextSide;
		CHECK(firstSide >= 2);
		CHECK(nextSide >= 2);
	}
}

TEST_CASE("CircleTrimmedArc.reflex_sweep_keeps_the_opposite_arc")
{
	const float inner = 64, outer = 130;
	for (const double sweep : { 240_deg, 350_deg, 359.9_deg })
	for (const double trim : { 6.0, 60.0, 64.0, 84.0 })
	{
		CAPTURE(sweep, trim);
		const auto mesh = Build(inner, outer, 0, sweep, trim);
		const double midpoint = (sweep / 2);
		const double radius = (Max(static_cast<double>(inner), trim) + 1.0);
		const Vec2 point = (Vec2{ Center } + Vec2{ std::sin(midpoint), -std::cos(midpoint) } * radius);
		bool covered = false;
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			const Vec2 a = mesh.vertices[mesh.indices[i] - mesh.offset].pos;
			const Vec2 b = mesh.vertices[mesh.indices[i + 1] - mesh.offset].pos;
			const Vec2 c = mesh.vertices[mesh.indices[i + 2] - mesh.offset].pos;
			if (((b - a).cross(point - a) >= -0.001)
				&& ((c - b).cross(point - b) >= -0.001)
				&& ((a - c).cross(point - c) >= -0.001)) covered = true;
		}
		CHECK(covered);
	}
}

TEST_CASE("CircleTrimmedArc.empty_failure_and_transitions")
{
	struct Empty { float inner, outer; double sweep, trim; };
	for (const auto c : { Empty{ 40, 80, 0, 6 }, Empty{ 80, 80, 1, 6 }, Empty{ 0, -1, 1, 6 },
		Empty{ 0, 80, 1, 80 }, Empty{ 40, 80, 0.01, 1 } })
	{
		const auto mesh = Build(c.inner, c.outer, 0.3, c.sweep, c.trim);
		CHECK(mesh.allocations == 0);
		CHECK(mesh.indices.isEmpty());
	}
	for (const double trim : { 0.0, 6.0 })
	{
		const auto failure = Build(40, 80, 0.3, 1.2, trim, 1, true);
		CHECK(failure.allocations == 1);
		CHECK(failure.indices.isEmpty());
	}
	for (const double sweep : { Math::Pi - 1e-7, Math::Pi, Math::Pi + 1e-7, 1.5 * Math::Pi })
	for (const double trim : { 0.999999, 1.0, 1.000001 })
	{
		const auto mesh = Build(1, 80, 0, sweep, trim);
		CHECK_FALSE(mesh.indices.isEmpty());
	}
}

TEST_CASE("CircleTrimmedArc.raster_analytic_coverage_and_alpha")
{
	const SceneScope scene;
	struct Case { double inner, sweep, trim; double outer = 80; };
	const Case cases[]{ { 40, 1.2, 6 }, { 0, 1.2, 6 }, { 3, -1.2, 6 }, { 40, Math::Pi, 6 },
		{ 40, -4.7, 6 }, { 0, 4.7, 30 }, { 40, Math::TwoPi - 0.1, 6 }, { 0, 4.7, 79 }, { 40, 1.2, 60 },
		{ 64, 179.9_deg, 6, 130 }, { 64, 180_deg, 6, 130 }, { 64, 180.1_deg, 6, 130 },
		{ 64, 240_deg, 6, 130 }, { 64, 340_deg, 6, 130 }, { 64, 349_deg, 6, 130 },
		{ 64, 350_deg, 6, 130 }, { 64, 355_deg, 6, 130 }, { 64, 359.9_deg, 6, 130 },
		{ 64, -350_deg, 6, 130 }, { 64, -359.9_deg, 6, 130 },
		{ 64, 240_deg, 63.99, 130 }, { 64, 240_deg, 64, 130 }, { 64, 240_deg, 64.01, 130 },
		{ 64, 359.9_deg, 84, 130 }, { 0, 359.9_deg, 6, 130 } };
	for (const auto& c : cases)
	{
		CAPTURE(c.inner, c.outer, c.sweep, c.trim);
		const double start = 0.3;
		const Circle circle{ Center, c.outer };
		const Image image = Capture([&] { CHECK(&circle.drawTrimmedArc(start, c.sweep, c.outer - c.inner, 0, c.trim, ColorF{ 1, 0.5 }) == &circle); });
		const Vec2 n0 = StartNormal(start + Min(c.sweep, 0.0));
		const Vec2 n1 = EndNormal(start + Max(c.sweep, 0.0));
		const Vec2 ray0{ n0.y, -n0.x }, ray1{ -n1.y, n1.x };
		uint32 mismatches = 0, checked = 0;
		for (int32 y = 0; y < image.height(); ++y)
		for (int32 x = 0; x < image.width(); ++x)
		{
			const Vec2 p = (Vec2{ x + 0.5, y + 0.5 } - Center);
			const double r = p.length(), a = p.dot(n0), b = p.dot(n1);
			const double d0 = RayDistance(p, ray0), d1 = RayDistance(p, ray1);
			// Ignore only a one-pixel neighborhood of the analytic boundaries.
			if (Min({ Abs(r - c.outer), Abs(r - c.inner), Abs(d0 - c.trim), Abs(d1 - c.trim) }) < 1.0) continue;
			const bool filled = ((c.inner < r && r < c.outer) && (d0 >= c.trim && d1 >= c.trim)
				&& ((Abs(c.sweep) <= Math::Pi) ? (a >= 0 && b >= 0) : (a >= 0 || b >= 0)));
			const int32 expected = (filled ? 128 : 0);
			if (Abs(int32(image[y][x].r) - expected) > 2) ++mismatches;
			++checked;
		}
		CHECK(checked > 30000);
		CHECK(mismatches == 0);
	}
}

TEST_CASE("CircleTrimmedArc.public_fallbacks_and_normalization")
{
	const SceneScope scene;
	const Circle circle{ Center, 80 };
	for (const double sweep : { 1.2, -4.7, Math::TwoPi, -Math::TwoPi, 20.0 })
	{
		CAPTURE(sweep);
		const Image flat = Capture([&] { circle.drawArc(0.3, sweep, 40, 5); });
		CHECK(flat == Capture([&] { circle.drawTrimmedArc(0.3, sweep, 40, 5, 0); }));
		CHECK(flat == Capture([&] { circle.drawTrimmedArc(0.3, sweep, 40, 5, -6); }));
		if (Math::TwoPi <= Abs(sweep))
			CHECK(flat == Capture([&] { circle.drawTrimmedArc(0.3, sweep, 40, 5, 1000); }));
	}
	CHECK(Capture([&] { circle.drawTrimmedArc(0.3, 1.2, 40, 5, 6); })
		== Capture([&] { Circle{ Center, -80 }.drawTrimmedArc(0.3, 1.2, 40, 5, 6); }));
	CHECK(Capture([&] { circle.drawTrimmedArc(0.3, 1.2, 100, 5, 6); })
		== Capture([&] { circle.drawTrimmedArc(0.3, 1.2, 80, 5, 6); }));
	CHECK(Capture([&] { Circle{ Center, 85 }.drawPie(0.3, 1.2); })
		== Capture([&] { circle.drawTrimmedArc(0.3, 1.2, 80, 5, 0); }));
	const Image empty = Capture([] {});
	CHECK(empty == Capture([&]
	{
		circle.drawTrimmedArc(0, 0, 40, 5, 6);
		circle.drawTrimmedArc(0, 1, 0, 0, 6);
		circle.drawTrimmedArc(0, 1, 40, -50, 6);
		circle.drawTrimmedArc(0, 1, 80, -100, 6);
		circle.drawTrimmedArc(0, 1, 40, 5, 100);
	}));
}

TEST_CASE("CircleTrimmedArc.transforms_and_render_resolution")
{
	const SceneScope scene;
	for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	{
		Scene::Configure(SceneConfig::Fixed({ 360, 280 }, resolution));
		const Image transformed = Capture([&]
		{
			const Transformer2D transform{ Mat3x2::Scale(1.5).rotated(0.4).translated(Center) };
			Circle{ 0, 0, 50 }.drawTrimmedArc(0.2, 2.1, 30, 0, 4);
		});
		const Image explicitGeometry = Capture([&] { Circle{ Center, 75 }.drawTrimmedArc(0.6, 2.1, 45, 0, 6); });
		REQUIRE(transformed.size() == explicitGeometry.size());
		uint32 different = 0;
		for (size_t i = 0; i < transformed.pixelCount(); ++i)
			if (Abs(int32(transformed.data()[i].r) - explicitGeometry.data()[i].r) > 2) ++different;
		CHECK(different <= 8);
	}
}

TEST_CASE("CircleTrimmedArc.image_geometry")
{
	const Size size{ 69, 63 };
	const Circle circle{ 34.37, 30.61, 25.2 };
	for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	for (const double start : { 0.0, 0.43, 2.8 })
	for (const double sweep : { 0.03, Math::Pi / 3, Math::Pi, Math::Pi + 0.001, 4.7, Math::TwoPi - 0.001 })
	for (const double innerRadius : { 0.0, 3.2, 18.7 })
	for (const double trim : { 0.15, 3.2, 8.4, 25.1 })
	{
		CAPTURE(aa, start, sweep, innerRadius, trim);
		const Image expected = TrimmedArcMask(size, circle, start, sweep, circle.r - innerRadius, 0, trim,
			(aa == EnableAntialiasing::Yes));
		Image actual{ size, Color{ 0, 0 } }, reversed = actual;
		circle.overwriteTrimmedArc(actual, start, sweep, circle.r - innerRadius, 0, trim, Palette::White, aa);
		circle.overwriteTrimmedArc(reversed, start + sweep, -sweep, circle.r - innerRadius, 0, trim, Palette::White, aa);
		CHECK_THAT(actual, ImageDrawingTest::ImageEquals(expected));
		CHECK_THAT(reversed, ImageDrawingTest::ImageEquals(expected));
	}
}

TEST_CASE("CircleTrimmedArc.image_compositing_and_clipping")
{
	const Size size{ 57, 49 };
	const Point padding{ 40, 40 };
	for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	for (const bool paint : { false, true })
	for (const Color ink : { Color{ 73, 193, 121, 0 }, Color{ 73, 193, 121, 127 }, Color{ 73, 193, 121, 255 } })
	for (const Vec2 center : { Vec2{ 27.37, 24.61 }, Vec2{ 1.37, 3.61 }, Vec2{ 55.37, 47.61 }, Vec2{ -100, -100 } })
	for (const double sweep : { 1.2, -4.7, Math::TwoPi - 0.01 })
	{
		CAPTURE(aa, paint, ink, center, sweep);
		const Circle circle{ center, 23.2 };
		Image actual = ImageDrawingTest::Background(size), expected = actual;
		const Image mask = TrimmedArcMask(size, circle, 0.31, sweep, 22, 3, 4, (aa == EnableAntialiasing::Yes));
		ImageDrawingTest::CompositeMask(expected, mask, ink, paint);
		WriteTrimmedArc(circle, actual, paint, 0.31, sweep, 22, 3, 4, ink, aa);
		CHECK_THAT(actual, ImageDrawingTest::ImageEquals(expected, paint ? 1 : 0));

		Image padded{ size + padding * 2, Color{ 0, 0 } };
		// Use the same varied RGBA background at the translated destination.
		const Image background = ImageDrawingTest::Background(size);
		for (int32 y = 0; y < size.y; ++y)
			for (int32 x = 0; x < size.x; ++x) padded[y + padding.y][x + padding.x] = background[y][x];
		WriteTrimmedArc(circle.movedBy(padding), padded, paint, 0.31, sweep, 22, 3, 4, ink, aa);
		Image cropped{ size };
		for (int32 y = 0; y < size.y; ++y)
			for (int32 x = 0; x < size.x; ++x) cropped[y][x] = padded[y + padding.y][x + padding.x];
		CHECK_THAT(actual, ImageDrawingTest::ImageEquals(cropped));
	}
}

TEST_CASE("CircleTrimmedArc.image_normalization_and_untrimmed")
{
	const Vec2 center{ 31.37, 29.61 };
	const Color ink{ 87, 163, 210, 127 };
	for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	for (const bool paint : { false, true })
	for (const double sweep : { 1.3, -4.7, Math::TwoPi, -9.0 })
	for (const double trim : { -3.0, 0.0, 4.0, 100.0 })
	for (const Vec3 band : { Vec3{ 25, 20, 3 }, Vec3{ -25, 20, 3 }, Vec3{ 25, 40, 3 },
		Vec3{ 25, -2, 5 }, Vec3{ 25, 15, -4 }, Vec3{ 0, 0, 25 } })
	{
		CAPTURE(aa, paint, sweep, trim, band);
		const double inner = Max(Abs(band.x) - band.y, 0.0), outer = (Abs(band.x) + band.z);
		const Circle circle{ center, band.x }, normalized{ center, outer };
		Image actual = ImageDrawingTest::Background({ 65, 61 }), expected = actual;
		WriteTrimmedArc(circle, actual, paint, 0.37, sweep, band.y, band.z, trim, ink, aa);
		if ((trim <= 0) || (Math::TwoPi <= Abs(sweep)))
		{
			if (paint) normalized.paintArc(expected, 0.37, sweep, outer - inner, 0, ink, aa);
			else normalized.overwriteArc(expected, 0.37, sweep, outer - inner, 0, ink, aa);
		}
		else
		{
			const Image mask = TrimmedArcMask(actual.size(), circle, 0.37, sweep, band.y, band.z, trim,
				(aa == EnableAntialiasing::Yes));
			ImageDrawingTest::CompositeMask(expected, mask, ink, paint);
		}
		CHECK_THAT(actual, ImageDrawingTest::ImageEquals(expected, paint ? 1 : 0));
	}
	for (const bool paint : { false, true })
	{
		const Circle circle{ center, 25 };
		Image actual = ImageDrawingTest::Background({ 65, 61 }), expected = actual;
		if (paint) circle.paintTrimmedArc(actual, 0.37, 4.7, 20, 0, 4, ink);
		else circle.overwriteTrimmedArc(actual, 0.37, 4.7, 20, 0, 4, ink);
		WriteTrimmedArc(circle, expected, paint, 0.37, 4.7, 20, 0, 4, ink, EnableAntialiasing::Yes);
		CHECK(actual == expected);
	}
}

TEST_CASE("CircleTrimmedArc.image_empty_boundaries")
{
	const Circle circle{ 30.37, 28.61, 24 };
	const Image background = ImageDrawingTest::Background({ 65, 61 });
	for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	for (const bool paint : { false, true })
	{
		Image actual = background;
		WriteTrimmedArc(circle, actual, paint, 0, 0, 20, 0, 4, Palette::White, aa);
		WriteTrimmedArc(circle, actual, paint, 0, 1, 0, 0, 4, Palette::White, aa);
		WriteTrimmedArc(circle, actual, paint, 0, 1, 4, -5, 4, Palette::White, aa);
		WriteTrimmedArc(circle, actual, paint, 0, 1, 30, -25, 4, Palette::White, aa);
		WriteTrimmedArc(circle, actual, paint, 0, 4.7, 20, 0, 24, Palette::White, aa);
		WriteTrimmedArc(circle, actual, paint, 0, 1.2, 20, 0, 24 * std::sin(0.6), Palette::White, aa);
		CHECK(actual == background);
		Image empty;
		WriteTrimmedArc(circle, empty, paint, 0, 4.7, 20, 0, 4, Palette::White, aa);
		CHECK(empty.isEmpty());
	}
}
