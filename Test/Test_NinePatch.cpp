//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/NinePatch/NinePatchGeometry.hpp"
# include <chrono>
# include <iostream>

namespace
{
	constexpr NinePatch::Slice Slice{ 4, 6, 8, 2 };
	const ColorF Tint{ 0.75, 0.5, 1.0, 0.625 };

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		MSAASampleCount msaa = Scene::GetMSAASampleCount();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D mul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D add{ 0.0, ScopedColorAdd2D::Target::Set };
		ScopedRenderStates2D states{ BlendState::Default2D, RasterizerState::Default2D, SamplerState::ClampNearest };
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
			Scene::SetBackground(background);
			Scene::SetMSAASampleCount(msaa);
		}
	};

	struct Captured
	{
		Image image;
		int64 triangles, drawCalls;
	};

	template <class Draw>
	Captured Capture(Draw&& draw)
	{
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		const auto metrics = Profiler::GetFrameMetrics();
		return{ ScreenCapture::GetFrame(), metrics.triangleCount, metrics.drawCalls };
	}

	Texture MakeTexture()
	{
		Image image{ 64, 48 };
		for (int32 y = 0; y < image.height(); ++y)
		for (int32 x = 0; x < image.width(); ++x)
		{
			image[y][x] = Color{ uint8(32 + x * 3), uint8(32 + y * 4), uint8(224 - x * 2), uint8(128 + y * 2) };
		}
		image.premultiplyAlpha();
		return Texture{ image, TextureDesc::NoMipmap };
	}

	void CheckNear(const Image& actual, const Image& expected, const std::span<const Line> outline = {})
	{
		REQUIRE(actual.size() == expected.size());
		int32 difference = 0;
		int32 edgeDifference = 0;
		size_t worstPixel = 0;
		for (size_t i = 0; i < actual.pixelCount(); ++i)
		{
			const Color a = actual.data()[i], b = expected.data()[i];
			const int32 delta = Max({ Abs(int32(a.r) - b.r), Abs(int32(a.g) - b.g),
				Abs(int32(a.b) - b.b), Abs(int32(a.a) - b.a) });
			const Vec2 pixel{ double(i % actual.width()) + 0.5, double(i / actual.width()) + 0.5 };
			// Splitting an outer edge can change its rasterizer rounding by one MSAA sample.
			// Keep the strict comparison everywhere else, including all internal patch edges.
			if ((delta > 2) && std::ranges::any_of(outline, [&](const Line& edge) { return edge.distanceFrom(pixel) <= 1; }))
			{
				edgeDifference = Max(edgeDifference, delta);
				continue;
			}
			if (difference < delta) { difference = delta; worstPixel = i; }
		}
		INFO("worst pixel: " << worstPixel % actual.width() << ", " << worstPixel / actual.width()
			<< " actual=" << actual.data()[worstPixel] << " expected=" << expected.data()[worstPixel]);
		CHECK(difference <= 2);
		CHECK(edgeDifference <= 40); // One of four samples at the seam test's tint and color scopes.
	}

	// Independent reference: crop nine TextureRegions and draw each as an ordinary rectangle.
	void DrawReference(TextureRegion source, const NinePatch::Slice slice, const double scale,
		const RectF& rect, const bool center, const ColorF& color, const NinePatch::Tiling tiling = {})
	{
		if (source.size.x < 0) source = source.mirrored();
		if (source.size.y < 0) source = source.flipped();
		source.size = { Abs(source.size.x), Abs(source.size.y) };
		double fit = 1;
		if ((slice.left + slice.right) * scale > 0) fit = Min(fit, rect.w / ((slice.left + slice.right) * scale));
		if ((slice.top + slice.bottom) * scale > 0) fit = Min(fit, rect.h / ((slice.top + slice.bottom) * scale));
		const double xs[]{ 0, slice.left, source.size.x - slice.right, source.size.x };
		const double ys[]{ 0, slice.top, source.size.y - slice.bottom, source.size.y };
		const double xd[]{ rect.x, rect.x + slice.left * scale * fit, rect.x + rect.w - slice.right * scale * fit, rect.x + rect.w };
		const double yd[]{ rect.y, rect.y + slice.top * scale * fit, rect.y + rect.h - slice.bottom * scale * fit, rect.y + rect.h };
		for (size_t y = 0; y < 3; ++y)
		for (size_t x = 0; x < 3; ++x)
		{
			if (((not center) && x == 1 && y == 1) || xd[x + 1] <= xd[x] || yd[y + 1] <= yd[y]) continue;
			const RectF destination{ xd[x], yd[y], xd[x + 1] - xd[x], yd[y + 1] - yd[y] };
			const auto piece = source(xs[x], ys[y], xs[x + 1] - xs[x], ys[y + 1] - ys[y]);
			const bool tx = (scale > 0 && x == 1 && (y == 1 ? tiling.center : tiling.topBottom));
			const bool ty = (scale > 0 && y == 1 && (x == 1 ? tiling.center : tiling.leftRight));
			if (not (tx || ty)) destination(piece).draw(color);
			else
			{
				const SizeF tileSize{ tx ? piece.size.x * scale * fit : destination.w, ty ? piece.size.y * scale * fit : destination.h };
				const auto tile = piece.resized(tileSize);
				for (double yy = destination.y; yy < destination.bottomY(); yy += tileSize.y)
				for (double xx = destination.x; xx < destination.rightX(); xx += tileSize.x)
				{
					tile.drawClipped(Vec2{ xx, yy }, destination, color);
				}
			}
		}
	}
}

TEST_CASE("NinePatch.construction_and_value_semantics")
{
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const NinePatch uniform{ texture, 4 };
	CHECK(uniform.slice() == NinePatch::Slice{ 4, 4, 4, 4 });
	CHECK(uniform.minimumSize() == SizeF{ 8, 8 });
	CHECK(uniform.borderScale() == 1);
	CHECK(uniform.source().texture == texture);
	CHECK(uniform.source().size == Float2{ 64, 48 });
	CHECK(NinePatch{ texture(8, 4, 32, 24), 4 }.slice() == uniform.slice());

	TextureRegion source = texture(8, 4, 32, 24).mirrored().resized(64, 48);
	NinePatch patch{ source, Slice };
	NinePatch copy = patch;
	CHECK(&copy.setBorderScale(2) == &copy);
	CHECK(patch.borderScale() == 1);
	CHECK(copy.minimumSize() == SizeF{ 16, 24 });
	CHECK(copy.tiling() == NinePatch::Tiling{});
	CHECK(&copy.setTiling(NinePatch::Tiling::All()) == &copy);
	CHECK(copy.tiling() == NinePatch::Tiling{ true, true, true });
	CHECK(patch.tiling() == NinePatch::Tiling{});
	source.size = { 1, 1 };
	CHECK(patch.source().size == Float2{ 64, 48 });
	CHECK(patch.source().uvRect.left > patch.source().uvRect.right);
	patch = NinePatch{};
	CHECK(patch.isEmpty());
	CHECK(copy);
	CHECK(copy.source().texture == texture);
	copy.setBorderScale(-1);
	CHECK(copy.borderScale() == 0);

	for (const auto slice : { NinePatch::Slice{ -1, 0, 0, 0 }, NinePatch::Slice{ 0, -1, 0, 0 },
		NinePatch::Slice{ 0, 0, -1, 0 }, NinePatch::Slice{ 0, 0, 0, -1 },
		NinePatch::Slice{ 0, 32, 0, 32 }, NinePatch::Slice{ 24, 0, 24, 0 }, NinePatch::Slice{ 0, 100, 0, 0 } })
	{
		const NinePatch invalid{ texture, slice };
		CHECK(invalid.isEmpty());
		CHECK(invalid.slice() == NinePatch::Slice{});
		CHECK(invalid.minimumSize() == SizeF{ 0, 0 });
		CHECK(invalid.source().texture.isEmpty());
	}
	CHECK(NinePatch{ Texture{}, 0 }.isEmpty());
	CHECK(NinePatch{ texture.resized(0, 24), 0 }.isEmpty());
	CHECK(NinePatch{ texture.resized(32, 0), 0 }.isEmpty());
}

TEST_CASE("NinePatch.layout_and_small_destinations")
{
	NinePatch patch{ MakeTexture(), Slice };
	patch.setBorderScale(2);
	CHECK(patch.innerRect({ 40, 30, 160, 80 }) == RectF{ 44, 38, 144, 56 });
	CHECK(patch.innerRect({ 40, 30, 8, 100 }) == RectF{ 42, 34, 0, 88 });
	CHECK(patch.innerRect({ 40, 30, 100, 12 }) == RectF{ 42, 34, 92, 0 });
	CHECK(patch.innerRect({ 40, 30, 8, 9 }) == RectF{ 41.5, 33, 2, 0 });
	CHECK(patch.innerRect({ 40, 30, 16, 24 }) == RectF{ 44, 38, 0, 0 });
	patch.setBorderScale(0);
	CHECK(patch.innerRect({ 40, 30, 8, 9 }) == RectF{ 40, 30, 8, 9 });
	CHECK(NinePatch{}.innerRect({ 40, 30, 8, 9 }) == RectF{ 40, 30, 0, 0 });
	CHECK(patch.innerRect({ 40, 30, -8, 9 }) == RectF{ 40, 30, 0, 0 });
}

TEST_CASE("NinePatch.mesh_coverage_uv_and_budget")
{
	const std::array<float, 4> u{ 0.75f, 0.625f, 0.25f, 0.125f }, v{ 0.125f, 0.25f, 0.5f, 0.75f };
	for (const auto slice : { Slice, NinePatch::Slice{}, NinePatch::Slice{ 0, 6, 0, 2 }, NinePatch::Slice{ 4, 0, 8, 0 } })
	for (const SizeF size : { SizeF{ 100, 80 }, SizeF{ 4, 80 }, SizeF{ 100, 6 }, SizeF{ 8, 12 } })
	for (const bool center : { false, true })
	{
		const RectF rect{ 10, 20, size };
		const auto layout = detail::MakeNinePatchLayout(rect, slice, 1);
		std::array<Vertex2D, 16> vertices;
		std::array<TriangleIndex, 18> triangles;
		const auto count = detail::BuildNinePatchMesh(layout, u, v, Tint.toFloat4(), center, vertices, triangles);
		CHECK(count <= (center ? 18u : 16u));
		for (size_t y = 0; y < 4; ++y)
		for (size_t x = 0; x < 4; ++x)
		{
			const auto& vertex = vertices[y * 4 + x];
			CHECK(vertex.tex == Float2{ u[x], v[y] });
			CHECK(vertex.color == Tint.toFloat4());
			CHECK(vertex.pos.x >= rect.x);
			CHECK(vertex.pos.x <= rect.rightX());
			CHECK(vertex.pos.y >= rect.y);
			CHECK(vertex.pos.y <= rect.bottomY());
		}
		double area = 0;
		for (uint32 i = 0; i < count; ++i)
		{
			const auto t = triangles[i];
			REQUIRE(Max(t.i0, t.i1, t.i2) < 16);
			const Vec2 a = vertices[t.i0].pos, b = vertices[t.i1].pos, c = vertices[t.i2].pos;
			const double twiceArea = (b - a).cross(c - a);
			CHECK(twiceArea > 0);
			area += twiceArea * 0.5;
		}
		const RectF inner = layout.innerRect();
		CHECK(area == Catch::Approx(rect.area() - (center ? 0 : inner.area())).margin(0.001));
	}
}

TEST_CASE("NinePatch.rendering_regions_and_boundaries")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	for (const SamplerState sampler : { SamplerState::ClampNearest, SamplerState::ClampLinear })
	{
		const ScopedRenderStates2D sampling{ sampler };
		for (int32 variant = 0; variant < 16; ++variant)
		{
			const auto source = texture(8, 4, 32, 24).mirrored(variant & 1).flipped(variant & 2)
				.resized((variant & 4) ? -64 : 64, (variant & 8) ? -48 : 48);
			NinePatch patch{ source, Slice };
			patch.setBorderScale(2);
			for (const bool center : { false, true })
			{
				CAPTURE(variant, center, sampler);
				const RectF rect{ 40, 30, 160, 100 };
				const auto actual = Capture([&] { CHECK((center ? patch.draw(rect, Tint) : patch.drawFrame(rect, Tint)) == rect); });
				const auto expected = Capture([&] { DrawReference(source, Slice, 2, rect, center, Tint); });
				CHECK(actual.triangles == (center ? 18 : 16));
				CHECK(actual.drawCalls == 1);
				CheckNear(actual.image, expected.image);
			}
		}
	}

	for (const auto slice : { Slice, NinePatch::Slice{}, NinePatch::Slice{ 0, 6, 0, 2 }, NinePatch::Slice{ 4, 0, 8, 0 } })
	for (const RectF rect : { RectF{ 60, 40, 4, 80 }, RectF{ 60, 40, 100, 6 }, RectF{ 60, 40, 8, 12 } })
	{
		const NinePatch patch{ texture, slice };
		const auto actual = Capture([&] { patch.draw(rect, Tint); });
		const auto expected = Capture([&] { DrawReference(TextureRegion{ texture }, slice, 1, rect, true, Tint); });
		CHECK(actual.triangles == expected.triangles);
		CheckNear(actual.image, expected.image);
	}

	NinePatch patch{ texture, Slice };
	patch.setBorderScale(0);
	const RectF rect{ 40, 30, 120, 80 };
	const auto actual = Capture([&] { patch.draw(rect, Tint); });
	const auto expected = Capture([&] { texture(2, 4, 56, 36).resized(rect.size).draw(rect.pos, Tint); });
	CHECK(actual.triangles == 2);
	CheckNear(actual.image, expected.image);
	const auto blank = Capture([] {});
	const auto empty = Capture([&]
	{
		CHECK(patch.drawFrame(rect) == RectF{ rect.pos, 0, 0 });
		CHECK(NinePatch{ texture, 0 }.drawFrame(rect) == RectF{ rect.pos, 0, 0 });
		CHECK(NinePatch{}.draw(rect) == RectF{ rect.pos, 0, 0 });
		CHECK(NinePatch{ texture, 32 }.drawFrame(rect) == RectF{ rect.pos, 0, 0 });
		for (const SizeF size : { SizeF{ 0, 80 }, SizeF{ 120, 0 }, SizeF{ -1, 80 }, SizeF{ 120, -1 } })
		{
			CHECK(patch.draw(RectF{ rect.pos, size }) == RectF{ rect.pos, 0, 0 });
			CHECK(patch.drawFrame(RectF{ rect.pos, size }) == RectF{ rect.pos, 0, 0 });
		}
	});
	CHECK(empty.triangles == 0);
	CHECK(empty.image == blank.image);
}

TEST_CASE("NinePatch.tiling_modes_partial_tiles_and_regions")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	const RectF rect{ 40, 30, 76, 42 };
	for (uint32 mode = 1; mode < 8; ++mode)
	for (const bool center : { false, true })
	{
		const NinePatch::Tiling tiling{ bool(mode & 1), bool(mode & 2), bool(mode & 4) };
		const auto source = texture(8, 4, 32, 24);
		NinePatch patch{ source, Slice };
		patch.setTiling(tiling);
		const auto actual = Capture([&] { CHECK((center ? patch.draw(rect, Tint) : patch.drawFrame(rect, Tint)) == rect); });
		const auto expected = Capture([&] { DrawReference(source, Slice, 1, rect, center, Tint, tiling); });
		constexpr int64 FilledTriangles[]{ 18, 30, 30, 44, 50, 50, 50, 50 };
		constexpr int64 FrameTriangles[]{ 16, 24, 24, 32, 16, 32, 32, 32 };
		CAPTURE(mode, center);
		CHECK(actual.triangles == (center ? FilledTriangles[mode] : FrameTriangles[mode]));
		CHECK(actual.drawCalls == 1);
		CheckNear(actual.image, expected.image);
	}
	for (const SamplerState sampler : { SamplerState::ClampNearest, SamplerState::ClampLinear })
	for (const auto tiling : { NinePatch::Tiling::All(), NinePatch::Tiling{ true, true, false } })
	for (uint32 variant = 0; variant < 16; ++variant)
	{
		const ScopedRenderStates2D sampling{ sampler };
		const auto source = texture(8, 4, 32, 24).mirrored(variant & 1).flipped(variant & 2)
			.resized(variant & 4 ? -64 : 64, variant & 8 ? -48 : 48);
		NinePatch patch{ source, Slice };
		patch.setTiling(tiling).setBorderScale(0.75);
		const auto actual = Capture([&] { patch.draw(rect, Tint); });
		const auto expected = Capture([&] { DrawReference(source, Slice, 0.75, rect, true, Tint, tiling); });
		CAPTURE(variant, sampler, tiling.center);
		CheckNear(actual.image, expected.image);
	}
	for (const auto slice : { Slice, NinePatch::Slice{}, NinePatch::Slice{ 0, 6, 0, 2 }, NinePatch::Slice{ 4, 0, 8, 0 } })
	for (const double scale : { 0.0, 1.0, 2.0 })
	{
		NinePatch patch{ texture, slice };
		patch.setTiling(NinePatch::Tiling::All()).setBorderScale(scale);
		const RectF small{ 60, 40, 8, 9 };
		const auto actual = Capture([&] { patch.draw(small, Tint); });
		const auto expected = Capture([&] { DrawReference(TextureRegion{ texture }, slice, scale, small, true, Tint, NinePatch::Tiling::All()); });
		CAPTURE(scale);
		CHECK(actual.triangles == expected.triangles);
		CheckNear(actual.image, expected.image);
	}
}

TEST_CASE("NinePatch.tiling_mesh_and_density_budget")
{
	const std::array<float, 4> u{ 0, 0.0625f, 0.8125f, 1 }, v{ 0, 0.125f, 0.625f, 1 };
	const auto layout = detail::MakeNinePatchLayout({ 10, 20, 76, 42 }, Slice, 1);
	Array<Vertex2D> captured;
	const auto triangles = detail::BuildNinePatchTiles(layout, { 24, 12 }, u, v, NinePatch::Tiling::All(), Tint.toFloat4(), true,
		[&](std::span<const Vertex2D> vertices, std::span<const TriangleIndex> indices)
		{
			CHECK(vertices.size() == 100);
			CHECK(indices.size() == 50);
			captured.insert(captured.end(), vertices.begin(), vertices.end());
		});
	CHECK(triangles == 50);
	bool found = false;
	for (size_t i = 0; i < captured.size(); i += 4)
	{
		if (captured[i].pos != Float2{ 60, 48 }) continue;
		found = true;
		CHECK(captured[i + 3].pos == Float2{ 80, 54 });
		CHECK(captured[i].tex == Float2{ 0.0625f, 0.125f });
		CHECK(captured[i + 3].tex == Float2{ 0.6875f, 0.375f });
	}
	CHECK(found);

	// Both batches and repetitions are bounded; geometry still covers the entire destination.
	for (const SizeF centerSize : { SizeF{ 1, 1 }, SizeF{ 1, 2 } })
	{
		const auto large = detail::MakeNinePatchLayout({ 0, 0, 1002, 1002 }, { 1, 1, 1, 1 }, 1);
		uint32 batches = 0, maxBatch = 0;
		double area = 0;
		bool valid = true;
		const auto count = detail::BuildNinePatchTiles(large, centerSize, u, v, NinePatch::Tiling::All(), Tint.toFloat4(), true,
			[&](std::span<const Vertex2D> vertices, std::span<const TriangleIndex> indices)
			{
				++batches;
				maxBatch = Max(maxBatch, uint32(vertices.size() / 4));
				valid &= (vertices.size() == indices.size() * 2 && not vertices.empty());
				for (const auto& vertex : vertices)
				{
					valid &= (vertex.pos.x >= 0 && vertex.pos.x <= 1002 && vertex.pos.y >= 0 && vertex.pos.y <= 1002);
					valid &= (vertex.color == Tint.toFloat4());
				}
				for (const auto triangle : indices)
				{
					REQUIRE(Max(triangle.i0, triangle.i1, triangle.i2) < vertices.size());
					const Vec2 a = vertices[triangle.i0].pos, b = vertices[triangle.i1].pos, c = vertices[triangle.i2].pos;
					const double twiceArea = (b - a).cross(c - a);
					valid &= (twiceArea > 0);
					area += twiceArea / 2;
				}
				for (size_t i = 0; i < vertices.size(); i += 4)
				{
					if (vertices[i].pos != Float2{ 1, 1 }) continue;
					// Density reduction scales both tile axes together, not the borders.
					CHECK(vertices[i + 3].pos - vertices[i].pos == Float2{ 7.8125f, float(7.8125 * centerSize.y) });
				}
			});
		CHECK(valid);
		CHECK(count == 2u * (centerSize.y == 1 ? 16900u : 8580u));
		CHECK(batches == (count / 2 + detail::NinePatchTileBatchSize - 1) / detail::NinePatchTileBatchSize);
		CHECK(maxBatch == detail::NinePatchTileBatchSize);
		CHECK(area == Catch::Approx(1002.0 * 1002.0));
		CHECK(large.innerRect() == RectF{ 1, 1, 1000, 1000 });
	}

	// Repeating borders around a stretched center must not create a dense center grid.
	const auto large = detail::MakeNinePatchLayout({ 0, 0, 1002, 1002 }, { 1, 1, 1, 1 }, 1);
	double area = 0;
	uint32 vertexCount = 0;
	const auto count = detail::BuildNinePatchTiles(large, { 1, 1 }, u, v, { true, true, false }, Tint.toFloat4(), true,
		[&](std::span<const Vertex2D> vertices, std::span<const TriangleIndex> indices)
		{
			vertexCount += static_cast<uint32>(vertices.size());
			for (const auto triangle : indices)
			{
				REQUIRE(Max(triangle.i0, triangle.i1, triangle.i2) < vertices.size());
				const Vec2 a = vertices[triangle.i0].pos, b = vertices[triangle.i1].pos, c = vertices[triangle.i2].pos;
				const double twiceArea = (b - a).cross(c - a);
				CHECK(twiceArea > 0);
				area += twiceArea / 2;
			}
		});
	CHECK(count == 1544);
	CHECK(vertexCount < 3000);
	CHECK(area == Catch::Approx(1002.0 * 1002.0));
}

TEST_CASE("NinePatch.tiling_batch_rollover")
{
	const SceneScope scene;
	const Texture texture{ Image{ 3, 3, Palette::White }, TextureDesc::NoMipmap };
	NinePatch patch{ texture, 1 };
	patch.setTiling(NinePatch::Tiling::All());
	const RectF rect{ 40, 30, 132, 132 };
	const auto actual = Capture([&] { patch.draw(rect, Tint); });
	const auto expected = Capture([&] { rect(texture).draw(Tint); });
	CHECK(actual.triangles == 16900 * 2);
	CHECK(actual.drawCalls >= 2);
	CHECK(patch.innerRect(rect) == RectF{ 41, 31, 130, 130 });
	CheckNear(actual.image, expected.image);
}

TEST_CASE("NinePatch.frame_preserves_pattern_at_density_limit")
{
	const SceneScope scene;
	Image image{ 3, 4, Palette::White };
	for (int32 x = 0; x < 3; ++x)
	{
		image[1][x] = Palette::Red;
		image[2][x] = Palette::Blue;
	}
	const Texture texture{ image, TextureDesc::NoMipmap };
	NinePatch patch{ texture, 1 };
	patch.setTiling({ .leftRight = true, .center = true });
	const RectF rect{ 20, 30, 258, 34 };
	const auto filled = Capture([&]
	{
		patch.draw(rect);
		patch.innerRect(rect).draw(Palette::Black);
	});
	const auto frame = Capture([&] { patch.drawFrame(rect); });
	CheckNear(frame.image, filled.image);
}

TEST_CASE("NinePatch.transforms_state_and_alpha_seams")
{
	const SceneScope scene;
	const Texture texture{ Image{ 32, 24, Palette::White }, TextureDesc::NoMipmap };
	NinePatch patch{ texture, Slice };
	const RectF rect{ -30.25, -20.125, 60.5, 40.25 };
	const ScopedRenderStates2D sampler{ SamplerState::ClampLinear };
	const ScopedColorMul2D mul{ ColorF{ 0.5, 0.75, 1, 0.8 } };
	const ScopedColorAdd2D add{ ColorF{ 0.1, 0, 0.2, 0 } };
	for (uint32 mode = 0; mode < 8; ++mode)
	for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	for (const auto transform : { Mat3x2::Identity(), Mat3x2::Scale(1.5, 0.75).rotated(30_deg), Mat3x2::Scale(-2, 1.25) })
	{
		CAPTURE(mode, resolution, transform);
		patch.setTiling({ bool(mode & 1), bool(mode & 2), bool(mode & 4) });
		Scene::Configure(SceneConfig::Fixed({ 320, 240 }, resolution));
		const Transformer2D local{ transform, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Translate(160, 120), Transformer2D::Target::SetCamera };
		const Rect clip{ 0, 0, Scene::RenderResolution().x / 2, Scene::RenderResolution().y };
		const ScopedScissorRect2D scissor{ clip };
		const Mat3x3 homography{ 1, 0, 0.001f, 0, 1, 0.0005f, 0, 0, 1 };
		const ScopedQuadWarp2D warp{ homography, ScopedQuadWarp2D::Target::Set };
		std::array<Vec2, 4> corners{ rect.tl(), rect.tr(), rect.br(), rect.bl() };
		const Vec2 renderScale{ Scene::RenderResolution().x / 320.0, Scene::RenderResolution().y / 240.0 };
		for (auto& point : corners) point = homography.transformPoint(transform.transformPoint(point) + Vec2{ 160, 120 }) * renderScale;
		const std::array<Line, 4> outline{ Line{ corners[0], corners[1] }, Line{ corners[1], corners[2] },
			Line{ corners[2], corners[3] }, Line{ corners[3], corners[0] } };
		const auto actual = Capture([&]
		{
			patch.draw(rect, Tint);
			CHECK(Graphics2D::GetPSSamplerState() == SamplerState::ClampLinear);
			CHECK(Graphics2D::GetScissorRect() == Optional<Rect>{ clip });
			RectF{ -32, 32, 64, 12 }.draw(Palette::Orange);
		});
		const auto expected = Capture([&]
		{
			rect(texture).draw(Tint);
			RectF{ -32, 32, 64, 12 }.draw(Palette::Orange);
		});
		CHECK(actual.image.size() == Scene::RenderResolution());
		CheckNear(actual.image, expected.image, outline);
	}
}

TEST_CASE("NinePatch.updated_texture_resource")
{
	const SceneScope scene;
	DynamicTexture texture{ Image{ 32, 24, Palette::Red }, TextureDesc::NoMipmap };
	NinePatch patch{ texture, Slice };
	patch.setTiling(NinePatch::Tiling::All());
	const RectF rect{ 40, 30, 120, 80 };
	const auto before = Capture([&] { patch.draw(rect); });
	Graphics2D::Flush();
	REQUIRE(texture.fill(Palette::Blue));
	const auto after = Capture([&] { patch.draw(rect); });
	const auto expected = Capture([&] { rect.draw(Palette::Blue); });
	CHECK(before.image != after.image);
	CheckNear(after.image, expected.image);
	CHECK(patch.source().texture == texture);
}

TEST_CASE("NinePatch.submission_benchmark", "[.benchmark]")
{
	const SceneScope scene;
	const bool vsync = Graphics::IsVSyncEnabled();
	const ScopeExit restore{ [&] { Graphics::SetVSyncEnabled(vsync); } };
	Graphics::SetVSyncEnabled(false);
	const Texture texture = MakeTexture();
	const TextureRegion source{ texture };
	NinePatch patch{ texture, Slice };
	constexpr int32 Calls = 256;
	for (const bool tiled : { false, true })
	for (const bool center : { true, false })
	for (const bool reference : { false, true })
	{
		const auto tiling = (tiled ? NinePatch::Tiling::All() : NinePatch::Tiling{});
		patch.setTiling(tiling);
		int64 expectedTriangles = 0;
		for (int32 i = 0; i < Calls; ++i)
		{
			const int32 nx = ((152 + i % 32 + 55) / 56);
			expectedTriangles += (tiled ? 2 * (4 + 2 * nx + 6 + (center ? nx * 3 : 0)) : (center ? 18 : 16));
		}
		Array<double> times;
		int64 triangles = 0, draws = 0;
		for (int32 round = -5; round < 21; ++round)
		{
			REQUIRE(System::Update());
			const auto begin = std::chrono::steady_clock::now();
			for (int32 i = 0; i < Calls; ++i)
			{
				const RectF rect{ 10000, 10000, (160 + i % 32), 100 };
				if (reference) DrawReference(source, Slice, 1, rect, center, Tint, tiling);
				else if (center) patch.draw(rect, Tint);
				else patch.drawFrame(rect, Tint);
			}
			const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / Calls;
			if (round >= 0) times.push_back(ns);
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			triangles = metrics.triangleCount;
			draws = metrics.drawCalls;
			CHECK(triangles == expectedTriangles);
		}
		times.sort();
		std::cout << "NinePatch " << (reference ? "quads" : "mesh") << " tiled=" << tiled << " center=" << center
			<< " ns=" << times[10] << " p10=" << times[2] << " p90=" << times[18]
			<< " triangles/call=" << double(triangles) / Calls << " drawCalls/batch=" << draws << '\n';
	}
}
