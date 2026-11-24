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
}

TEST_CASE("TextureCover.public_overloads")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const TextureRegion region{ texture };
	const RectF rect{ 80, 60, 96, 96 };
	for (const bool useRegion : { false, true })
	for (const bool explicitAnchor : { false, true })
	for (const bool tinted : { false, true })
	{
		CAPTURE(useRegion, explicitAnchor, tinted);
		const auto actual = Capture([&]
		{
			const auto draw = [&](const auto& source)
			{
				if (explicitAnchor)
				{
					return (tinted ? source.drawCover(rect, Anchor::Center, Tint) : source.drawCover(rect, Anchor::Center));
				}
				return (tinted ? source.drawCover(rect, Tint) : source.drawCover(rect));
			};
			CHECK((useRegion ? draw(region) : draw(texture)) == rect);
		});
		const auto expected = Capture([&]
		{
			// A 64x48 image covering a square retains the middle 48x48 pixels.
			texture(8, 0, 48, 48).resized(rect.size).draw(rect.pos, (tinted ? Tint : ColorF{ Palette::White }));
		});
		CHECK(actual.triangles == 2);
		CheckNear(actual.image, expected.image);
		CHECK(actual.image[100][100] != Palette::Black);
		CHECK(actual.image[59][100] == Palette::Black);
	}
}

TEST_CASE("TextureCover.anchors_and_aspect_ratios")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const Anchor anchors[3][3] = {
		{ Anchor::TopLeft, Anchor::TopCenter, Anchor::TopRight },
		{ Anchor::MiddleLeft, Anchor::Center, Anchor::MiddleRight },
		{ Anchor::BottomLeft, Anchor::BottomCenter, Anchor::BottomRight } };
	const RectF square{ 24, 24, 96, 96 }, wide{ 144, 24, 128, 48 }, small{ 144, 96, 32, 24 };
	for (int32 row = 0; row < 3; ++row)
	for (int32 col = 0; col < 3; ++col)
	{
		CAPTURE(row, col);
		const auto actual = Capture([&]
		{
			CHECK(texture.drawCover(square, anchors[row][col]) == square);
			CHECK(TextureRegion{ texture }.drawCover(wide, anchors[row][col]) == wide);
			CHECK(texture.drawCover(small, anchors[row][col]) == small);
		});
		const auto expected = Capture([&]
		{
			texture(col * 8, 0, 48, 48).resized(square.size).draw(square.pos);
			texture(0, row * 12, 64, 24).resized(wide.size).draw(wide.pos);
			texture.scaled(0.5).draw(small.pos);
		});
		CHECK(actual.triangles == 6);
		CheckNear(actual.image, expected.image);
	}
}

TEST_CASE("TextureCover.region_uv_flips_and_signed_sizes")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const auto original = texture(8, 4, 48, 40).resized(192, 80);
	const RectF narrow{ 40, 24, 96, 80 }, wide{ 40, 144, 192, 40 };
	for (const bool mirror : { false, true })
	for (const bool flip : { false, true })
	for (const SizeF scale : { SizeF{ 1, 1 }, SizeF{ -1, 1 }, SizeF{ 1, -1 }, SizeF{ -1, -1 } })
	for (const bool lastEdge : { false, true })
	{
		CAPTURE(mirror, flip, scale, lastEdge);
		const auto region = original.mirrored(mirror).flipped(flip).scaled(scale);
		const Anchor anchor = (lastEdge ? Anchor::BottomRight : Anchor::TopLeft);
		const auto actual = Capture([&]
		{
			CHECK(region.drawCover(narrow, anchor, Tint) == narrow);
			CHECK(region.drawCover(wide, anchor, Tint) == wide);
		});
		const auto expected = Capture([&]
		{
			// Independently orient the image, then retain a half of its displayed area.
			const auto oriented = original.mirrored(mirror != (scale.x < 0)).flipped(flip != (scale.y < 0));
			oriented(lastEdge ? 96 : 0, 0, 96, 80).draw(narrow.pos, Tint);
			oriented(0, lastEdge ? 40 : 0, 192, 40).draw(wide.pos, Tint);
		});
		CHECK(actual.triangles == 4);
		CheckNear(actual.image, expected.image);
	}
}

TEST_CASE("TextureCover.empty_regions")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const auto blank = Capture([] {});
	const auto actual = Capture([&]
	{
		for (const SizeF size : { SizeF{ 0, 48 }, SizeF{ 64, 0 }, SizeF{ 0, 0 }, SizeF{ -64, 48 }, SizeF{ 64, -48 } })
		{
			const RectF rect{ 40, 60, size };
			const RectF empty{ 40, 60, 0, 0 };
			CHECK(texture.drawCover(rect) == empty);
			CHECK(texture.drawCover(rect, Anchor::BottomRight, Tint) == empty);
			CHECK(TextureRegion{ texture }.drawCover(rect) == empty);
			CHECK(TextureRegion{ texture }.drawCover(rect, Anchor::TopLeft, Tint) == empty);
		}
		const RectF rect{ 40, 60, 64, 48 }, empty{ 40, 60, 0, 0 };
		for (const SizeF size : { SizeF{ 0, 48 }, SizeF{ 64, 0 }, SizeF{ 0, 0 } })
		{
			CHECK(texture.resized(size).drawCover(rect) == empty);
			CHECK(texture.resized(size).drawCover(rect, Anchor::TopLeft) == empty);
		}
		CHECK(TextureRegion{}.drawCover(rect) == empty);
	});
	CHECK(actual.triangles == 0);
	CHECK(actual.image == blank.image);
}

TEST_CASE("TextureCover.transforms_resolution_and_draw_state")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const RectF rect{ -20.25, -16.125, 40.5, 40.5 };
	const ScopedRenderStates2D sampler{ SamplerState::ClampLinear };
	const ScopedColorMul2D colorMul{ ColorF{ 0.5, 0.75, 1.0, 0.8 } };
	const ScopedColorAdd2D colorAdd{ ColorF{ 0.1, 0.0, 0.2, 0.0 } };
	for (const SceneResolution resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	for (const Mat3x2 local : { Mat3x2::Identity(), Mat3x2::Scale(1.5, 0.75).rotated(30_deg), Mat3x2::Scale(-2, 1.25) })
	{
		CAPTURE(resolution, local);
		Scene::Configure(SceneConfig::Fixed(SceneSize, resolution));
		const Transformer2D l{ local, Transformer2D::Target::SetLocal };
		const Transformer2D c{ Mat3x2::Scale(1.25).translated(160, 120), Transformer2D::Target::SetCamera };
		const ScopedViewport2D viewport{ Rect{ 0, 0, Scene::RenderResolution() } };
		const Rect clip{ 0, 0, Scene::RenderResolution().x / 2, Scene::RenderResolution().y };
		const ScopedScissorRect2D scissor{ clip };
		const ScopedQuadWarp2D warp{ Mat3x3{ 1, 0, 0.001f, 0, 1, 0.0005f, 0, 0, 1 }, ScopedQuadWarp2D::Target::Set };
		const auto actual = Capture([&]
		{
			CHECK(texture.drawCover(rect, Anchor::MiddleRight, Tint) == rect);
			CHECK(Graphics2D::GetScissorRect() == Optional<Rect>{ clip });
			CHECK(Graphics2D::GetViewport() == Optional<Rect>{ Rect{ 0, 0, Scene::RenderResolution() } });
			CHECK(Graphics2D::GetPSSamplerState() == SamplerState::ClampLinear);
			RectF{ -32, 32, 64, 12 }.draw(Palette::Orange);
		});
		const auto expected = Capture([&]
		{
			texture(16, 0, 48, 48).resized(rect.size).draw(rect.pos, Tint);
			RectF{ -32, 32, 64, 12 }.draw(Palette::Orange);
		});
		CHECK(actual.image.size() == Scene::RenderResolution());
		CHECK(actual.triangles == 4);
		CheckNear(actual.image, expected.image);
	}
}
