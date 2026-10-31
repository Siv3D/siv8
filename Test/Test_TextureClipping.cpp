//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"

namespace
{
	constexpr Size SceneSize{ 320, 240 };
	const ColorF Tint{ 0.75, 0.5, 1.0, 0.625 };

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D colorMul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D colorAdd{ 0.0, ScopedColorAdd2D::Target::Set };
		ScopedRenderStates2D states{ BlendState::Default2D, RasterizerState::Default2D, SamplerState::ClampNearest };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed(SceneSize, SceneResolution::SceneSize));
			Scene::SetBackground(Palette::Black);
		}

		~SceneScope()
		{
			Scene::Configure(config);
			Scene::SetBackground(background);
		}
	};

	struct Captured
	{
		Image image;
		int64 triangles;
	};

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

	Texture MakeTexture()
	{
		Image image{ 64, 48 };
		for (int32 y = 0; y < image.height(); ++y)
		{
			for (int32 x = 0; x < image.width(); ++x)
			{
				image[y][x] = Color{ uint8(32 + x * 3), uint8(32 + y * 4), uint8(224 - x * 2), uint8(128 + y * 2) };
			}
		}
		image.premultiplyAlpha();
		return Texture{ image, TextureDesc::NoMipmap };
	}

	void CheckNear(const Image& actual, const Image& expected)
	{
		REQUIRE(actual.size() == expected.size());
		int32 maxDifference = 0;
		for (size_t i = 0; i < actual.pixelCount(); ++i)
		{
			const Color a = actual.data()[i], b = expected.data()[i];
			maxDifference = Max(maxDifference, Abs(int32(a.r) - b.r), Abs(int32(a.g) - b.g));
			maxDifference = Max(maxDifference, Abs(int32(a.b) - b.b), Abs(int32(a.a) - b.a));
		}
		CHECK(maxDifference <= 1);
	}

	template <class Source>
	RectF DrawVariant(const Source& source, const int32 variant, const Vec2& pos, const SizeF& size,
		const RectF& clipRect, const ColorF& tint = Tint)
	{
		const Vec2 center = (pos + size * 0.5);
		switch (variant)
		{
		case 0: return source.drawClipped(pos.x, pos.y, clipRect, tint);
		case 1: return source.drawClipped(pos, clipRect, tint);
		case 2: return source.drawAtClipped(center.x, center.y, clipRect, tint);
		default: return source.drawAtClipped(center, clipRect, tint);
		}
	}
}

TEST_CASE("TextureClipping.public_overloads")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const TextureRegion region = texture(8, 4, 48, 40).resized(192, 160);
	const Vec2 pos{ 80, 60 };
	const Rect clip{ 96, 72, 32, 24 };
	for (const bool useRegion : { false, true })
	{
		const auto expected = Capture([&]
		{
			const ScopedScissorRect2D scissor{ clip };
			if (useRegion) region.draw(pos, Tint);
			else texture.draw(pos, Tint);
		});
		for (int32 variant = 0; variant < 4; ++variant)
		{
			CAPTURE(useRegion, variant);
			const auto actual = Capture([&]
			{
				const RectF result = (useRegion
					? DrawVariant(region, variant, pos, SizeF{ region.size }, clip)
					: DrawVariant(texture, variant, pos, SizeF{ texture.size() }, clip));
				CHECK(result == RectF{ clip });
			});
			CHECK(actual.triangles == 2);
			CheckNear(actual.image, expected.image);
			CHECK(actual.image[84][112] != Palette::Black);
		}
	}
}

TEST_CASE("TextureClipping.intersections_and_empty_regions")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const Vec2 pos{ 80, 60 };
	const auto blank = Capture([] {});
	for (const auto& [clip, expectedRect] : {
		std::pair{ Rect{ 0, 0, 320, 240 }, RectF{ 80, 60, 64, 48 } },
		std::pair{ Rect{ 80, 60, 64, 48 }, RectF{ 80, 60, 64, 48 } },
		std::pair{ Rect{ 96, 40, 80, 100 }, RectF{ 96, 60, 48, 48 } },
		std::pair{ Rect{ 60, 40, 60, 100 }, RectF{ 80, 60, 40, 48 } },
		std::pair{ Rect{ 60, 72, 100, 60 }, RectF{ 80, 72, 64, 36 } },
		std::pair{ Rect{ 60, 40, 100, 56 }, RectF{ 80, 60, 64, 36 } },
		std::pair{ Rect{ 96, 72, 32, 24 }, RectF{ 96, 72, 32, 24 } },
		std::pair{ Rect{ 0, 0, 20, 20 }, RectF{} },
		std::pair{ Rect{ 144, 60, 20, 48 }, RectF{} },
		std::pair{ Rect{ 80, 108, 64, 20 }, RectF{} },
		std::pair{ Rect{ 96, 72, 0, 24 }, RectF{} },
		std::pair{ Rect{ 96, 72, 32, 0 }, RectF{} },
		std::pair{ Rect{ 96, 72, -8, 24 }, RectF{} },
		std::pair{ Rect{ 96, 72, 32, -8 }, RectF{} } })
	{
		CAPTURE(clip);
		const bool empty = (expectedRect.w == 0);
		const auto actual = Capture([&]
		{
			const RectF result = texture.drawClipped(pos, clip);
			if (empty) CHECK(result.size == SizeF{ 0, 0 });
			else CHECK(result == expectedRect);
		});
		CHECK(actual.triangles == (empty ? 0 : 2));
		if (empty)
		{
			CHECK(actual.image == blank.image);
		}
		else
		{
			const auto expected = Capture([&]
			{
				const ScopedScissorRect2D scissor{ clip };
				texture.draw(pos);
			});
			CheckNear(actual.image, expected.image);
		}
	}

	const auto empty = Capture([&]
	{
		const RectF clip{ 0, 0, SceneSize };
		for (const SizeF size : { SizeF{ 0, 24 }, SizeF{ 32, 0 }, SizeF{ 0, 0 } })
		{
			const auto region = texture.resized(size);
			for (int32 variant = 0; variant < 4; ++variant)
			{
				CHECK(DrawVariant(region, variant, pos, size, clip).size == SizeF{ 0, 0 });
			}
		}
		CHECK(TextureRegion{}.drawClipped(pos, clip).size == SizeF{ 0, 0 });
	});
	CHECK(empty.triangles == 0);
	CHECK(empty.image == blank.image);
}

TEST_CASE("TextureClipping.region_uv_flips_and_signed_sizes")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const auto original = texture(8, 4, 48, 40).resized(192, 160);
	const Rect clip{ 136, 96, 48, 48 };
	for (const bool flipUV : { false, true })
	for (const SizeF scale : { SizeF{ 1, 1 }, SizeF{ -1, 1 }, SizeF{ 1, -1 }, SizeF{ -1, -1 } })
	{
		CAPTURE(flipUV, scale);
		const auto region = original.mirrored(flipUV).flipped(flipUV).scaled(scale);
		const SizeF size{ region.size };
		const Vec2 pos = (Vec2{ 160, 120 } - size * 0.5);
		const auto actual = Capture([&]
		{
			CHECK(region.drawAtClipped(Vec2{ 160, 120 }, clip, Tint) == RectF{ clip });
		});
		const auto expected = Capture([&]
		{
			const ScopedScissorRect2D scissor{ clip };
			region.draw(pos, Tint);
		});
		CHECK(actual.triangles == 2);
		CheckNear(actual.image, expected.image);
		CHECK(actual.image[120][160] != Palette::Black);
	}
}

TEST_CASE("TextureClipping.drawing_coordinates_and_scene_resolution")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const RectF clip{ -20.25, -16.125, 40.5, 28.25 };
	for (const SceneResolution resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	for (const Mat3x2 local : { Mat3x2::Identity(), Mat3x2::Scale(1.5, 0.75).rotated(30_deg), Mat3x2::Scale(-2, 1.25) })
	{
		CAPTURE(resolution, local);
		Scene::Configure(SceneConfig::Fixed(SceneSize, resolution));
		const Transformer2D l{ local, Transformer2D::Target::SetLocal };
		const Transformer2D c{ Mat3x2::Scale(1.25).translated(160, 120), Transformer2D::Target::SetCamera };
		for (const bool useRegion : { false, true })
		{
			CAPTURE(useRegion);
			const auto actual = Capture([&]
			{
				const RectF result = (useRegion
					? TextureRegion{ texture }.drawClipped(Vec2{ -32, -24 }, clip, Tint)
					: texture.drawAtClipped(0, 0, clip, Tint));
				CHECK(result == clip);
			});
			const auto expected = Capture([&]
			{
				// Independently select the source pixels retained by the local clip rectangle.
				texture(11.75, 7.875, 40.5, 28.25).draw(-20.25, -16.125, Tint);
			});
			CHECK(actual.image.size() == Scene::RenderResolution());
			CHECK(actual.triangles == 2);
			CheckNear(actual.image, expected.image);
		}
	}
}

TEST_CASE("TextureClipping.preserves_scissor_and_draw_state")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const ScopedScissorRect2D scissor{ Rect{ 100, 60, 64, 64 } };
	const ScopedViewport2D viewport{ Rect{ 0, 0, SceneSize } };
	const ScopedQuadWarp2D warp{ Mat3x3{ 1, 0, 0.001f, 0, 1, 0.0005f, 0, 0, 1 }, ScopedQuadWarp2D::Target::Set };
	const auto actual = Capture([&]
	{
		texture.drawClipped(80, 60, RectF{ 96, 72, 32, 24 });
		CHECK(Graphics2D::GetScissorRect() == Optional<Rect>{ Rect{ 100, 60, 64, 64 } });
		CHECK(Graphics2D::GetViewport() == Optional<Rect>{ Rect{ 0, 0, SceneSize } });
		RectF{ 112, 100, 40, 20 }.draw(Palette::Red);
	});
	const auto expected = Capture([&]
	{
		texture(16, 12, 32, 24).draw(96, 72);
		RectF{ 112, 100, 40, 20 }.draw(Palette::Red);
	});
	CheckNear(actual.image, expected.image);
}
