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
# include "../Siv3D/src/Siv3D/Renderer2D/Renderer2DCommon.hpp"
# include <cstddef>

TEST_CASE("PatternParameters.packing")
{
	constexpr PatternParameters pattern{
		.primaryColor = { 0.9f, 0.8f, 0.7f, 0.6f },
		.backgroundColor = { 0.1f, 0.2f, 0.3f, 0.4f },
		.uvTransform = { 2, 3, 4, 5, 6, 7 },
		.param0 = 0.25f,
		.param1 = 0.75f,
		.type = PatternType::Checker,
		.extraParams = { -2.0f, 0.125f, 0.5f, 4.0f },
	};
	constexpr auto packed = pattern.toFloat4Array();
	static_assert(noexcept(pattern.toFloat4Array()));
	static_assert(packed[0] == Float4{ 2, 3, 6, 7 });
	static_assert(packed[1] == Float4{ 4, 5, 0.25f, 0.75f });
	static_assert(packed.size() == 4);
	static_assert(sizeof(packed) == 64);
	static_assert(packed[3] == pattern.extraParams);
	CHECK(packed[2] == pattern.backgroundColor);

	// Reconstruct the shader's affine mapping, including its translation.
	const Float2 point{ 11, 13 };
	const Float2 uv = (packed[0].zw() + point.x * packed[0].xy() + point.y * packed[1].xy());
	CHECK(uv == pattern.uvTransform.transformPoint(point));

	constexpr auto defaults = PatternParameters{}.toFloat4Array();
	CHECK(defaults[0] == Float4{ 1, 0, 0, 0 });
	CHECK(defaults[1] == Float4{ 0, 1, 0, 0 });
	CHECK(defaults[2] == Float4{ 0, 0, 0, 0 });
	CHECK(defaults[3] == Float4{ 0, 0, 0, 0 });

	// Singular and reflected UV transforms are packed without scale compensation.
	for (const Mat3x2 transform : { Mat3x2::Scale(0), Mat3x2::Scale(-2, 3).translated(5, -7) })
	{
		auto p = pattern;
		p.uvTransform = transform;
		const auto data = p.toFloat4Array();
		CHECK(data[0].zw() + point.x * data[0].xy() + point.y * data[1].xy()
			== transform.transformPoint(point));
		CHECK(data[1].zw() == packed[1].zw());
		CHECK(data[2] == packed[2]);
		CHECK(data[3] == packed[3]);
	}
}

TEST_CASE("PatternParameters.effect_constants")
{
	static_assert(sizeof(PSEffectConstants2D) == 128);
	static_assert(offsetof(PSEffectConstants2D, patternUVTransform) == 0);
	static_assert(offsetof(PSEffectConstants2D, patternBackgroundColor) == 32);
	static_assert(offsetof(PSEffectConstants2D, patternExtraParams) == 48);
	static_assert(offsetof(PSEffectConstants2D, quadWarpInvHomography) == 64);
	static_assert(offsetof(PSEffectConstants2D, quadWarpUVTransform) == 112);

	PSEffectConstants2D constants{};
	CHECK(constants.patternExtraParams == Float4{ 0, 0, 0, 0 });
	const Mat3x3 homography{ 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	const Float4 quadUV{ 0.25f, 0.5f, 0.75f, 1.0f };
	constants.setQuadWarp(homography, quadUV);
	const auto quadState = constants;
	const PatternParameters pattern{
		.backgroundColor = { 0.2f, 0.3f, 0.4f, 0.5f },
		.uvTransform = { 2, 3, 4, 5, 6, 7 },
		.param0 = 8,
		.param1 = 9,
		.extraParams = { -1, 2, -3, 4 },
	};
	const auto packed = pattern.toFloat4Array();
	constants.setPattern(packed);
	CHECK(constants.patternUVTransform[0] == packed[0]);
	CHECK(constants.patternUVTransform[1] == packed[1]);
	CHECK(constants.patternBackgroundColor == packed[2]);
	CHECK(constants.patternExtraParams == packed[3]);
	for (size_t i = 0; i < 3; ++i)
	{
		CHECK(constants.quadWarpInvHomography[i] == quadState.quadWarpInvHomography[i]);
	}
	CHECK(constants.quadWarpUVTransform == quadState.quadWarpUVTransform);

	constants.setQuadWarp(Mat3x3::Identity(), Float4{ 1, 1, 0, 0 });
	CHECK(constants.patternUVTransform[0] == packed[0]);
	CHECK(constants.patternUVTransform[1] == packed[1]);
	CHECK(constants.patternBackgroundColor == packed[2]);
	CHECK(constants.patternExtraParams == packed[3]);
	constants.setPattern(PatternParameters{}.toFloat4Array());
	CHECK(constants.patternExtraParams == Float4{ 0, 0, 0, 0 });
}

# if SIV3D_PLATFORM(WINDOWS) || SIV3D_PLATFORM(MACOS)

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

	struct PatternFrame
	{
		Image image;
		FrameMetrics metrics;
	};

	template <class Draw>
	PatternFrame CapturePatternDraw(Draw&& draw)
	{
		const ColorF background = Scene::GetBackground();
		const ScopeExit restore{ [&] { Scene::SetBackground(background); } };
		Scene::SetBackground(Palette::Black);
		const Transformer2D cameraScope{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const Transformer2D localScope{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const ScopedViewport2D viewport{ none };
		const ScopedScissorRect2D scissor{ none };
		const ScopedColorMul2D colorMul{ Palette::White };
		const ScopedColorAdd2D colorAdd{ 0.0 };
		const ScopedRenderStates2D states{ BlendState::Default2D, RasterizerState::Default2D };
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		Image image = ScreenCapture::GetFrame();
		REQUIRE(image.width() >= 520);
		REQUIRE(image.height() >= 360);
		return { std::move(image), Profiler::GetFrameMetrics() };
	}

	Image CapturePattern(const PatternParameters& pattern, const Mat3x2& local,
		const Mat3x2& camera = Mat3x2::Identity())
	{
		return CapturePatternDraw([&]
		{
			const Transformer2D cameraScope{ camera, Transformer2D::Target::SetCamera };
			const Transformer2D localScope{ local, Transformer2D::Target::SetLocal };
			RectF{ 0, 0, 72, 64 }.draw(pattern);
		}).image;
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

TEST_CASE("Pattern.drawing_coordinates")
{
	const Mat3x2 transforms[]{
		Mat3x2::Translate(233, 97),
		Mat3x2{ 0, 1, -1, 0, 250, 120 }, // Exact quarter turn.
		Mat3x2::Scale(3).translated(240, 110),
		Mat3x2::Scale(3, 1).translated(240, 110),
		Mat3x2::Scale(-1, 1).translated(240, 110),
		Mat3x2::ShearX(0.5f).translated(240, 110),
		Mat3x2::Rotate(31_deg).scaled(2).translated(240, 110),
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

TEST_CASE("Pattern.viewport_and_continuity")
{
	for (const PatternType type : { PatternType::PolkaDot, PatternType::Stripe,
		PatternType::Grid, PatternType::Checker, PatternType::Triangle, PatternType::HexGrid })
	{
		INFO(static_cast<int32>(type));
		const auto pattern = MakeTestPattern(type);
		const auto draw = [&](const bool viewport, const bool split)
		{
			RectF{ 20, 180, 20, 20 }.draw(Palette::Red);
			{
				const ScopedViewport2D vp{ viewport ? Optional<Rect>{ Rect{ 137, 81, 190, 110 } } : none };
				const Transformer2D transform{ Mat3x2::Scale(2, 1).translated(
					viewport ? 7 : 144, viewport ? 9 : 90) };
				if (split)
				{
					// The seam is deliberately unrelated to the pattern period.
					RectF{ 0, 0, 29, 64 }.draw(pattern);
					Triangle{ 29, 0, 72, 0, 29, 64 }.draw(pattern);
					Triangle{ 72, 0, 72, 64, 29, 64 }.draw(pattern);
				}
				else
				{
					RectF{ 0, 0, 72, 64 }.draw(pattern);
				}
			}
			RectF{ 60, 180, 20, 20 }.draw(Palette::Blue);
		};
		const auto reference = CapturePatternDraw([&] { draw(false, false); });
		CHECK(reference.image[190][30] == Palette::Red);
		CHECK(reference.image[190][70] == Palette::Blue);
		for (const bool split : { false, true })
		{
			INFO(split);
			const auto actual = CapturePatternDraw([&] { draw(true, split); });
			CHECK(actual.image == reference.image);
			// Splitting unchanged pattern state must not split the GPU draw.
			CHECK(actual.metrics.drawCalls == reference.metrics.drawCalls);
		}
		// Geometry extends beyond a viewport with a nonzero origin. Clipping must
		// crop the pattern, not restart it at the viewport or leak into its neighbors.
		const auto unclipped = CapturePatternDraw([&]
		{
			const Transformer2D transform{ Mat3x2::Translate(137, 81) };
			RectF{ -20, -20, 200, 160 }.draw(pattern);
		});
		const auto clipped = CapturePatternDraw([&]
		{
			const ScopedViewport2D viewport{ 137, 81, 100, 80 };
			RectF{ -20, -20, 200, 160 }.draw(pattern);
		});
		int32 clippingMismatches = 0;
		String firstDifference;
		for (int32 y = 60; y < 190; ++y)
		{
			for (int32 x = 115; x < 280; ++x)
			{
				const bool inside = (137 <= x && x < 237 && 81 <= y && y < 161);
				const Color expected = inside ? unclipped.image[y][x] : Palette::Black;
				const Color actual = clipped.image[y][x];
				const int32 error = Max(Max(Abs(int32(actual.r) - int32(expected.r)), Abs(int32(actual.g) - int32(expected.g))),
					Max(Abs(int32(actual.b) - int32(expected.b)), Abs(int32(actual.a) - int32(expected.a))));
				// Different viewport projection sizes can round filtered pattern colors
				// by one 8-bit unit. Outside the viewport, require exact background.
				if (error > (inside ? 1 : 0))
				{
					++clippingMismatches;
					if (firstDifference.isEmpty())
					{
						firstDifference = Format(Point{ x, y }, U" expected ", expected, U" actual ", actual);
					}
				}
			}
		}
		INFO(firstDifference);
		CHECK(clippingMismatches == 0);
	}
}

TEST_CASE("Pattern.pattern_and_color_state_restore")
{
	const auto pattern = MakeTestPattern(PatternType::PolkaDot);
	const auto draw = [&](const bool scoped)
	{
		for (int32 i = 0; i < 3; ++i)
		{
			const Transformer2D transform{ Mat3x2::Translate(30 + i * 100, 40) };
			if ((i == 1) && scoped)
			{
				const ScopedColorMul2D mul{ ColorF{ 0.8, 0.5, 0.25, 0.75 } };
				const ScopedColorAdd2D add{ ColorF{ 0.1, 0.2, 0.3 } };
				RectF{ 0, 0, 72, 64 }.draw(pattern);
			}
			else
			{
				auto p = pattern;
				if (i == 1)
				{
					p.primaryColor = Float4{ 0.9f, 0.7f, 0.55f, 0.75f };
					p.backgroundColor = Float4{ 0.1f, 0.2f, 0.3f, 0.75f };
				}
				RectF{ 0, 0, 72, 64 }.draw(p);
			}
		}
		// Empty geometry must not prevent the following valid draw from restoring state.
		{
			const Transformer2D collapsed{ Mat3x2::Scale(0) };
			RectF{ 0, 0, 72, 64 }.draw(MakeTestPattern(PatternType::Grid));
		}
		RectF{ 340, 40, 40, 40 }.draw(Palette::Green);
	};
	const auto reference = CapturePatternDraw([&] { draw(false); });
	for (int32 frame = 0; frame < 4; ++frame)
	{
		const auto actual = CapturePatternDraw([&] { draw(true); });
		// Equivalent color arithmetic can round differently by one 8-bit unit.
		int32 differences = 0;
		for (int32 y = 40; y < 104; ++y)
		{
			for (int32 x = 30; x < 380; ++x)
			{
				const Color a = actual.image[y][x], b = reference.image[y][x];
				differences += (Abs(int32(a.r) - int32(b.r)) > 1
					|| Abs(int32(a.g) - int32(b.g)) > 1 || Abs(int32(a.b) - int32(b.b)) > 1);
			}
		}
		CHECK(differences == 0);
		CHECK(actual.image[50][350] == Palette::Green);
	}
}

TEST_CASE("Pattern.batch_boundaries_and_mixed_shaders")
{
	Image pixels{ 2, 2, Palette::Red };
	pixels[0][1] = Palette::Blue;
	pixels[1][0] = Palette::Green;
	pixels[1][1] = Palette::Yellow;
	const Texture texture{ pixels };
	REQUIRE(texture);
	const auto draw = [&]
	{
		const auto pattern = MakeTestPattern(PatternType::Stripe);
		const Transformer2D transform{ Mat3x2::Translate(33, 27) };
		RectF{ 0, 0, 72, 64 }.draw(pattern);
		texture.resized(40, 40).draw(100, 0);
		Circle{ 190, 32, 28 }.draw(pattern);
		RectF{ 240, 0, 40, 40 }.draw(Palette::Blue);
		const Quad quad{ 300, 0, 360, 10, 350, 64, 310, 56 };
		REQUIRE(texture.drawQuadWarp(quad));
		Circle{ 40, 120, 28 }.drawArc(LineCap::Round, 0, 270_deg, 8, 8, pattern);
		LineString{ Vec2{ 100, 100 }, Vec2{ 160, 140 }, Vec2{ 220, 100 } }
			.draw(LineCap::Round, 16, pattern);
		RectF{ 270, 100, 72, 64 }.draw(MakeTestPattern(PatternType::Checker));
	};
	const auto reference = CapturePatternDraw(draw);
	CHECK(reference.image[40][280] == Palette::Blue);
	for (const int32 rectangles : { 16380, 16381, 16382, 16383, 16384, 32765 })
	{
		INFO(rectangles);
		const auto actual = CapturePatternDraw([&]
		{
			for (int32 i = 0; i < rectangles; ++i)
			{
				RectF{ -10, -10, 1, 1 }.draw();
			}
			draw();
		});
		CHECK(actual.image == reference.image);
		CHECK(actual.metrics.triangleCount == reference.metrics.triangleCount + rectangles * 2);
	}

	// Equal pattern state batches; alternating with solid/texture/quad warp requires
	// one draw per state, with no extra draw introduced by the dedicated Pattern VS.
	for (const bool mixed : { false, true })
	{
		const auto frame = CapturePatternDraw([&]
		{
			const auto pattern = MakeTestPattern(PatternType::Checker);
			for (int32 i = 0; i < 8; ++i)
			{
				const double y = 20 + i * 36;
				RectF{ 20, y, 32, 32 }.draw(pattern);
				if (mixed)
				{
					RectF{ 60, y, 32, 32 }.draw(Palette::Red);
					texture.resized(32, 32).draw(100, y);
					REQUIRE(texture.drawQuadWarp(Quad{ 140, y, 172, y, 172, y + 32, 140, y + 32 }));
				}
			}
		});
		CHECK(frame.metrics.drawCalls == (mixed ? 32 : 1));
		CHECK(frame.metrics.triangleCount == (mixed ? 64 : 16));
	}
}

TEST_CASE("Pattern.extra_parameters")
{
# if SIV3D_PLATFORM(WINDOWS)
	const std::string source = R"(
cbuffer Effects : register(b1) { float4 padding[3]; float4 extra; };
float4 ReadExtra() : SV_TARGET
{
    return float4(extra.xyz * extra.w, 1);
}
)";
	const PixelShader ps = PixelShader::HLSL(source, U"ReadExtra");
# else
	const std::string source = R"(
#include <metal_stdlib>
using namespace metal;
struct Varying { float4 position [[position]]; float4 colorPMA; float2 uv; };
fragment float4 ReadExtra(Varying input [[stage_in]], constant float4* effects [[buffer(1)]])
{
    const float4 extra = effects[3];
    return float4(extra.xyz * extra.w, 1);
}
)";
	const PixelShader ps = PixelShader::MSL(source, U"ReadExtra");
# endif
	REQUIRE(ps);
	const Texture texture{ Image{ 2, 2, Palette::White } };
	REQUIRE(texture);
	const Float4 a{ 1.0f, 0.5f, 0.25f, 0.5f };
	const Float4 b{ 0.25f, 0.5f, 1.0f, 0.75f };
	const std::array<Float4, 6> values{ Float4{ 0, 0, 0, 0 }, a, a, b, a, Float4{ 0, 0, 0, 0 } };
	const auto checkColor = [](const Color color, const Float4 value)
	{
		const Color expected = ColorF{ value.x * value.w, value.y * value.w, value.z * value.w }.toColor();
		CHECK(Abs(int32(color.r) - expected.r) <= 1);
		CHECK(Abs(int32(color.g) - expected.g) <= 1);
		CHECK(Abs(int32(color.b) - expected.b) <= 1);
		CHECK(color.a == 255);
	};

	for (int32 frameIndex = 0; frameIndex < 2; ++frameIndex)
	{
		const auto frame = CapturePatternDraw([&]
		{
			auto pattern = MakeTestPattern(PatternType::PolkaDot);
			{
				const ScopedCustomShader2D shader{ ps };
				for (size_t i = 0; i < values.size(); ++i)
				{
					pattern.extraParams = values[i];
					RectF{ 20.0 + i * 70, 20, 60, 60 }.draw(pattern);
				}
			}
			REQUIRE(texture.drawQuadWarp(Quad{ 20, 130, 80, 135, 75, 185, 25, 180 }));
			const ScopedCustomShader2D shader{ ps };
			pattern.extraParams = a;
			RectF{ 100, 130, 60, 60 }.draw(pattern);
		});
		for (size_t i = 0; i < values.size(); ++i)
		{
			checkColor(frame.image[40][40 + i * 70], values[i]);
		}
		CHECK(frame.image[150][50] == Palette::White);
		checkColor(frame.image[150][120], a);
		// The equal A states batch, but changing only the fourth vector splits a draw.
		CHECK(frame.metrics.drawCalls == 7);
		CHECK(frame.metrics.triangleCount == 16);
	}

	const auto builtIns = [&](const bool extra)
	{
		return CapturePatternDraw([&]
		{
			for (int32 i = 0; i < 6; ++i)
			{
				auto pattern = MakeTestPattern(static_cast<PatternType>(i));
				if (extra)
				{
					pattern.extraParams = ((i % 2) ? a : b);
				}
				RectF{ 20.0 + (i % 3) * 120, 20.0 + (i / 3) * 120, 72, 64 }.draw(pattern);
			}
		});
	};
	const auto reference = builtIns(false);
	const auto actual = builtIns(true);
	CHECK(actual.image == reference.image);
	CHECK(actual.metrics.drawCalls == reference.metrics.drawCalls);
}

TEST_CASE("Pattern.custom_shader_contract")
{
# if SIV3D_PLATFORM(WINDOWS)
	const std::string source = R"(
struct Vertex { float2 position : POSITION; float2 uv : TEXCOORD0; float4 color : COLOR0; };
cbuffer Constants : register(b0) { row_major float2x4 transform; float4 colorMul; };
struct Varying { float4 position : SV_POSITION; float4 colorPMA : COLOR0; float2 uv : TEXCOORD0; };
Varying ShiftPattern(Vertex input)
{
    const float2 pos = input.position + float2(23, 0);
    Varying result;
    result.position = float4(transform[0].zw + pos.x * transform[0].xy + pos.y * transform[1].xy, 0, 1);
    result.colorPMA = input.color * colorMul;
    result.colorPMA.rgb *= result.colorPMA.a;
    result.uv = input.position;
    return result;
}
float4 Coordinates(Varying input) : SV_TARGET
{
    return float4(frac(input.uv / 32.0f), 0, 1);
}
)";
	const VertexShader vs = VertexShader::HLSL(source, U"ShiftPattern");
	const PixelShader ps = PixelShader::HLSL(source, U"Coordinates");
# else
	const std::string source = R"(
#include <metal_stdlib>
using namespace metal;
struct Vertex { float2 pos; float2 uv; float4 color; };
struct Constants { float2x4 transform; float4 colorMul; };
struct Varying { float4 position [[position]]; float4 colorPMA; float2 uv; };
vertex Varying ShiftPattern(uint id [[vertex_id]], constant Vertex* vertices [[buffer(0)]],
                           constant Constants& c [[buffer(1)]])
{
    const float2 pos = vertices[id].pos + float2(23, 0);
    Varying result;
    result.position = float4(c.transform[0].zw + pos.x * c.transform[0].xy + pos.y * c.transform[1].xy, 0, 1);
    result.colorPMA = vertices[id].color * c.colorMul;
    result.colorPMA.rgb *= result.colorPMA.a;
    result.uv = vertices[id].pos;
    return result;
}
fragment float4 Coordinates(Varying input [[stage_in]])
{
    return float4(fract(input.uv / 32.0f), 0, 1);
}
)";
	const VertexShader vs = VertexShader::MSL(source, U"ShiftPattern");
	const PixelShader ps = PixelShader::MSL(source, U"Coordinates");
# endif
	REQUIRE(vs);
	REQUIRE(ps);
	const auto pattern = MakeTestPattern(PatternType::Triangle);
	const auto draw = [&](const bool custom)
	{
		for (int32 i = 0; i < 3; ++i)
		{
			const Transformer2D transform{ Mat3x2::Translate(30 + i * 120, 40) };
			if (custom && i == 1)
			{
				const ScopedCustomShader2D shader{ vs };
				RectF{ 0, 0, 72, 64 }.draw(pattern);
			}
			else
			{
				const Transformer2D shift{ Mat3x2::Translate(i == 1 ? 23 : 0, 0) };
				RectF{ 0, 0, 72, 64 }.draw(pattern);
			}
		}
	};
	const auto reference = CapturePatternDraw([&] { draw(false); });
	for (int32 frame = 0; frame < 4; ++frame)
	{
		const auto actual = CapturePatternDraw([&] { draw(true); });
		CHECK(actual.image == reference.image);
	}
	const auto diagnostic = CapturePatternDraw([&]
	{
		{
			const Transformer2D transform{ Mat3x2::Translate(30, 40) };
			const ScopedCustomShader2D shader{ ps };
			RectF{ 0, 0, 72, 64 }.draw(pattern);
		}
		{
			const Transformer2D transform{ Mat3x2::Translate(150, 40) };
			const ScopedCustomShader2D shader{ vs, ps };
			RectF{ 0, 0, 72, 64 }.draw(pattern);
		}
		const Transformer2D transform{ Mat3x2::Translate(270, 40) };
		RectF{ 0, 0, 72, 64 }.draw(pattern);
	});
	// Pixel centers (10.5, 20.5) map to (10.5/32, 20.5/32), independently of
	// which VS supplies the coordinates. A normal texture UV would fail this check.
	for (const int32 origin : { 30, 173 })
	{
		const Color color = diagnostic.image[60][origin + 10];
		CHECK(Abs(int32(color.r) - 84) <= 1);
		CHECK(Abs(int32(color.g) - 163) <= 1);
		CHECK(color.b == 0);
	}
	for (int32 y = 40; y < 104; ++y)
	{
		for (int32 x = 270; x < 342; ++x)
		{
			REQUIRE(diagnostic.image[y][x] == reference.image[y][x]);
		}
	}
}

namespace
{
	// Exercise distinct shape/geometry builders using the same paint argument for
	// an ordinary white coverage mask and the actual pattern.
	void DrawPatternShape(const int32 shape, const auto& paint)
	{
		const RectF rect{ 8, 8, 56, 48 };
		const Circle circle{ 36, 32, 25 };
		const Ellipse ellipse{ 36, 32, 28, 22 };
		switch (shape)
		{
		case 0: Triangle{ 8, 8, 64, 8, 36, 56 }.draw(paint); break;
		case 1: rect.draw(paint); break;
		case 2: rect.drawFrame(10, paint); break;
		case 3: circle.draw(paint); break;
		case 4: circle.drawFrame(10, paint); break;
		case 5: circle.drawPie(15_deg, 270_deg, paint); break;
		case 6: circle.drawArc(LineCap::Round, 15_deg, 270_deg, 8, 4, paint); break;
		case 7: circle.drawSegment(90_deg, 30, paint); break;
		case 8: ellipse.draw(paint); break;
		case 9: ellipse.drawFrame(10, paint); break;
		case 10: ellipse.drawPie(15_deg, 270_deg, paint); break;
		case 11: SuperEllipse{ 36, 32, 28, 22, 4 }.draw(paint); break;
		case 12: Quad{ 8, 8, 64, 12, 60, 56, 12, 52 }.draw(paint); break;
		case 13: rect.rounded(10).draw(paint); break;
		case 14: rect.rounded(10).drawFrame(10, paint); break;
		case 15:
			Polygon{ { Vec2{ 8, 8 }, Vec2{ 64, 8 }, Vec2{ 52, 56 }, Vec2{ 12, 52 } } }
				.draw(Vec2{ 3, -2 }, paint);
			break;
		case 16:
			Polygon{ { Vec2{ -24, -20 }, Vec2{ 24, -20 }, Vec2{ 24, 20 }, Vec2{ -24, 20 } } }
				.drawTransformed(17_deg, Vec2{ 36, 32 }, paint);
			break;
		case 17:
			Shape2D{ { Float2{ 8, 8 }, Float2{ 64, 8 }, Float2{ 36, 56 } }, { TriangleIndex{ 0, 1, 2 } } }
				.drawFrame(10, paint);
			break;
		case 18:
			LineString{ Vec2{ 8, 16 }, Vec2{ 36, 48 }, Vec2{ 64, 16 } }.draw(LineCap::Round, 14, paint);
			break;
		default:
			// Rounded joins use the triangulated closed-line path.
			Quad{ 8, 8, 64, 8, 64, 56, 8, 56 }.drawFrame(10, paint, JoinStyle::Round);
			break;
		}
	}
}

TEST_CASE("Pattern.shape_paths")
{
	const auto pattern = MakeTestPattern(PatternType::Checker);
	const auto draw = [&](const int32 kind)
	{
		for (int32 i = 0; i < 20; ++i)
		{
			const Transformer2D transform{ Mat3x2::Scale(1.1, 1.2)
				.translated(20 + (i % 5) * 100, 20 + (i / 5) * 110) };
			if (kind == 0)
			{
				RectF{ 0, 0, 80, 72 }.draw(pattern);
			}
			else if (kind == 1)
			{
				DrawPatternShape(i, ColorF{ Palette::White });
			}
			else
			{
				DrawPatternShape(i, pattern);
			}
		}
	};
	const auto reference = CapturePatternDraw([&] { draw(0); });
	const auto mask = CapturePatternDraw([&] { draw(1); });
	const auto actual = CapturePatternDraw([&] { draw(2); });
	REQUIRE(actual.image.height() >= 440);
	for (int32 shape = 0; shape < 20; ++shape)
	{
		INFO(shape);
		int32 whiteSamples = 0, blackSamples = 0, mismatches = 0;
		for (int32 y = 22 + (shape / 5) * 110; y < 103 + (shape / 5) * 110; ++y)
		{
			for (int32 x = 22 + (shape % 5) * 100; x < 107 + (shape % 5) * 100; ++x)
			{
				bool interior = true;
				const Color expected = reference.image[y][x];
				if ((expected != Palette::White) && (expected != Palette::Black))
				{
					continue;
				}
				for (int32 dy = -1; dy <= 1; ++dy)
				{
					for (int32 dx = -1; dx <= 1; ++dx)
					{
						interior &= (mask.image[y + dy][x + dx] == Palette::White
							&& reference.image[y + dy][x + dx] == expected);
					}
				}
				if (interior)
				{
					(expected == Palette::White) ? ++whiteSamples : ++blackSamples;
					mismatches += (actual.image[y][x] != expected);
				}
			}
		}
		CHECK(whiteSamples > 5);
		CHECK(blackSamples > 5);
		CHECK(mismatches == 0);
	}
}

# endif
