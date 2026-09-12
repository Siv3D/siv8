//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
//
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
//
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("PatternParameters.packing")
{
	constexpr PatternParameters pattern{
		.primaryColor = { 0.9f, 0.8f, 0.7f, 0.6f },
		.backgroundColor = { 0.1f, 0.2f, 0.3f, 0.4f },
		.uvTransform = { 2, 3, 4, 5, 6, 7 },
		.param0 = 0.25f,
		.param1 = 0.75f,
		.type = PatternType::Checker,
	};
	constexpr auto packed = pattern.toFloat4Array();
	static_assert(noexcept(pattern.toFloat4Array()));
	static_assert(packed[0] == Float4{ 2, 3, 6, 7 });
	static_assert(packed[1] == Float4{ 4, 5, 0.25f, 0.75f });
	CHECK(packed[2] == pattern.backgroundColor);

	// Reconstruct the shader's affine mapping, including its translation.
	const Float2 point{ 11, 13 };
	const Float2 uv = (packed[0].zw() + point.x * packed[0].xy() + point.y * packed[1].xy());
	CHECK(uv == pattern.uvTransform.transformPoint(point));

	// The existing D3D11 packing path still scales all six matrix elements only.
	for (const float factor : { 0.0f, 0.5f, 1.0f, 2.0f })
	{
		const auto legacy = pattern.toFloat4Array(factor);
		CHECK(legacy[0] == packed[0] * factor);
		CHECK(legacy[1].xy() == packed[1].xy() * factor);
		CHECK(legacy[1].zw() == packed[1].zw());
		CHECK(legacy[2] == packed[2]);
	}
	CHECK(PatternParameters{}.toFloat4Array()[0] == Float4{ 1, 0, 0, 0 });
}

// D3D11 keeps the previous coordinate model until the backend port.
# if SIV3D_PLATFORM(MACOS)

namespace
{
	PatternParameters MakeTestPattern(const PatternType type)
	{
		const ColorF primary{ Palette::White };
		const ColorF background{ Palette::Black };
		const Vec2 offset{ 0.13, 0.23 };
		switch (type)
		{
		case PatternType::PolkaDot:
			return Pattern::PolkaDot{ primary, background, 24, 0.6, 17_deg, offset };
		case PatternType::Stripe:
			return Pattern::Stripe{ primary, background, 24, 0.5, 17_deg, offset };
		case PatternType::Grid:
			return Pattern::Grid{ primary, background, 24, 0.4, 17_deg, offset };
		case PatternType::Checker:
			return Pattern::Checker{ primary, background, 24, 1, 1, 17_deg, offset };
		case PatternType::Triangle:
			return Pattern::Triangle{ primary, background, 24, 17_deg, offset };
		default:
			return Pattern::HexGrid{ primary, background, 24, 0.5, 17_deg, offset };
		}
	}

	Image CapturePattern(const PatternParameters& pattern, const Mat3x2& local,
		const Mat3x2& camera = Mat3x2::Identity())
	{
		const ColorF background = Scene::GetBackground();
		const ScopeExit restore{ [&] { Scene::SetBackground(background); } };
		Scene::SetBackground(Palette::Black);
		const Transformer2D cameraScope{ camera, Transformer2D::Target::SetCamera };
		const Transformer2D localScope{ local, Transformer2D::Target::SetLocal };
		const ScopedColorMul2D colorMul{ Palette::White };
		const ScopedColorAdd2D colorAdd{ 0.0 };
		REQUIRE(System::Update());
		RectF{ 0, 0, 72, 64 }.draw(pattern);
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		const Image image = ScreenCapture::GetFrame();
		REQUIRE(image.width() >= 520);
		REQUIRE(image.height() >= 360);
		return image;
	}

	void CheckCorrespondingInterior(const Image& reference, const Image& actual,
		const Mat3x2& transform)
	{
		int32 whiteCount = 0;
		int32 blackCount = 0;
		int32 mismatches = 0;
		for (int32 y = 3; y < 61; ++y)
		{
			for (int32 x = 3; x < 69; ++x)
			{
				const Color expected = reference[y + 20][x + 20];
				if ((expected != Palette::White) && (expected != Palette::Black))
				{
					continue;
				}
				bool interior = true;
				for (int32 dy = -1; dy <= 1; ++dy)
				{
					for (int32 dx = -1; dx <= 1; ++dx)
					{
						interior &= (reference[y + 20 + dy][x + 20 + dx] == expected);
					}
				}
				if (not interior)
				{
					continue;
				}
				(expected == Palette::White) ? ++whiteCount : ++blackCount;
				const Float2 pixel = transform.transformPoint(Float2{ x + 0.5f, y + 0.5f });
				const Color color = actual[static_cast<int32>(pixel.y)][static_cast<int32>(pixel.x)];
				// Sampling/filtering at transformed edges need not be byte-identical.
				if ((Abs(int32(color.r) - int32(expected.r)) > 8)
					|| (Abs(int32(color.g) - int32(expected.g)) > 8)
					|| (Abs(int32(color.b) - int32(expected.b)) > 8))
				{
					++mismatches;
				}
			}
		}
		CHECK(whiteCount > 20);
		CHECK(blackCount > 20);
		CHECK(mismatches == 0);
	}
}

TEST_CASE("Pattern.Metal.drawing_coordinates")
{
	const Mat3x2 transforms[]{
		Mat3x2::Translate(233, 97),
		Mat3x2{ 0, 1, -1, 0, 250, 120 }, // Exact quarter turn.
		Mat3x2::Scale(3).translated(240, 110),
		Mat3x2::Scale(3, 1).translated(240, 110),
	};
	for (const PatternType type : { PatternType::PolkaDot, PatternType::Stripe,
		PatternType::Grid, PatternType::Checker, PatternType::Triangle, PatternType::HexGrid })
	{
		INFO(static_cast<int32>(type));
		const auto pattern = MakeTestPattern(type);
		const Image reference = CapturePattern(pattern, Mat3x2::Translate(20, 20));
		for (const Mat3x2& transform : transforms)
		{
			INFO(transform);
			CheckCorrespondingInterior(reference, CapturePattern(pattern, transform), transform);
		}
		// The combined local/camera mapping must carry the same pattern coordinates.
		const Mat3x2 local = Mat3x2::Scale(3, 1);
		const Mat3x2 camera{ 0, 1, -1, 0, 250, 100 };
		CheckCorrespondingInterior(reference, CapturePattern(pattern, local, camera), local * camera);
	}
}

# endif
