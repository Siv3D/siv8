//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Renderer2DCommon.hpp"
# include <type_traits>

namespace
{
	struct WarpSceneScope
	{
		ColorF background = Scene::GetBackground();
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D colorMul{ Palette::White };
		ScopedColorAdd2D colorAdd{ 0.0 };
		ScopedRenderStates2D blend{ BlendState::Default2D };

		WarpSceneScope() { Scene::SetBackground(Palette::Black); }
		~WarpSceneScope() { Scene::SetBackground(background); }
	};

	struct CapturedWarpDraw
	{
		Image image;
		FrameMetrics metrics;
	};

	template <class Draw>
	CapturedWarpDraw CaptureWarpDraw(Draw&& draw)
	{
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return { ScreenCapture::GetFrame(), Profiler::GetFrameMetrics() };
	}
}

TEST_CASE("QuadWarp.constants_and_scopes")
{
	static_assert(not std::is_copy_constructible_v<ScopedQuadWarp2D>);
	static_assert(std::is_nothrow_move_constructible_v<ScopedQuadWarp2D>);
	static_assert(sizeof(VSConstants2D) == 64);
	static_assert(offsetof(VSConstants2D, colorMul) == 48);
	VSConstants2D constants;
	const Mat3x3 matrix{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	constants.setTransform(matrix);
	CHECK(constants.transform[0] == Float4{ 1, 2, 0, 3 });
	CHECK(constants.transform[1] == Float4{ 4, 5, 0, 6 });
	CHECK(constants.transform[2] == Float4{ 7, 8, 0, 9 });

	const WarpSceneScope scene;
	const Mat3x3 outer{ Mat3x2::Translate(40, 20) };
	const Mat3x3 inner{ 1, 0, 0.001f, 0, 1, 0.002f, 0, 0, 1 };
	{
		ScopedQuadWarp2D scope{ outer };
		CHECK(Graphics2D::GetQuadWarpTransform() == outer);
		{
			const ScopedQuadWarp2D nested{ inner };
			CHECK(Graphics2D::GetQuadWarpTransform() == inner * outer);
			{
				const ScopedQuadWarp2D disable{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
				CHECK(Graphics2D::GetQuadWarpTransform() == Mat3x3::Identity());
			}
			CHECK(Graphics2D::GetQuadWarpTransform() == inner * outer);
		}
		CHECK(Graphics2D::GetQuadWarpTransform() == outer);
		{
			const ScopedQuadWarp2D moved{ std::move(scope) };
			const ScopedQuadWarp2D inactive;
			const Transformer2D local{ Mat3x2::Scale(2), Transformer2D::Target::SetLocal };
			const Transformer2D camera{ Mat3x2::Scale(3), Transformer2D::Target::SetCamera };
			CHECK(Graphics2D::GetQuadWarpTransform() == outer);
			CHECK(Graphics2D::GetRMSScaling() == 6.0f);
		}
		CHECK(Graphics2D::GetQuadWarpTransform() == Mat3x3::Identity());
	}
	CHECK(Graphics2D::GetQuadWarpTransform() == Mat3x3::Identity());
}

TEST_CASE("QuadWarp.draw_state_and_batching")
{
	const WarpSceneScope scene;
	REQUIRE(System::Update());
	const Mat3x3 warp{ Mat3x2::Translate(100, 0) };
	const auto reference = CaptureWarpDraw([]
	{
		RectF{ 120, 20, 30, 30 }.draw(Palette::Red);
		RectF{ 180, 20, 30, 30 }.draw(Palette::Blue);
		RectF{ 20, 80, 30, 30 }.draw(Palette::Lime);
	});
	for (int32 frame = 0; frame < 4; ++frame)
	{
		const auto actual = CaptureWarpDraw([&]
		{
			{
				const ScopedQuadWarp2D scoped{ warp };
				RectF{ 20, 20, 30, 30 }.draw(Palette::Red);
				{
					const ScopedQuadWarp2D empty{ Mat3x3{ Mat3x2::Scale(2) } };
				}
				RectF{ 80, 20, 30, 30 }.draw(Palette::Blue);
			}
			RectF{ 20, 80, 30, 30 }.draw(Palette::Lime);
		});
		CHECK(actual.image == reference.image);
		CHECK(actual.metrics.drawCalls == 2);
		CHECK(actual.metrics.triangleCount == 6);
	}

	// A live scope survives frame boundaries and explicit Flush().
	{
		const ScopedQuadWarp2D scoped{ warp };
		for (int32 frame = 0; frame < 4; ++frame)
		{
			const auto actual = CaptureWarpDraw([]
			{
				RectF{ 20, 20, 30, 30 }.draw(Palette::Red);
				Graphics2D::Flush();
				RectF{ 80, 20, 30, 30 }.draw(Palette::Blue);
				const ScopedQuadWarp2D disable{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
				RectF{ 20, 80, 30, 30 }.draw(Palette::Lime);
			});
			CHECK(actual.image == reference.image);
			CHECK(Graphics2D::GetQuadWarpTransform() == warp);
		}
	}
}

TEST_CASE("QuadWarp.texture_perspective")
{
	const WarpSceneScope scene;
	Image image{ 128, 128 };
	for (int32 y = 0; y < 128; ++y)
	{
		for (int32 x = 0; x < 128; ++x)
		{
			image[y][x] = Color{ static_cast<uint8>(x * 2), static_cast<uint8>(y * 2), 0 };
		}
	}
	const Texture texture{ image };
	REQUIRE(texture);
	const RectF source{ 0, 0, 128, 128 };
	const Quad target{ 40, 40, 300, 65, 235, 270, 85, 235 };
	const auto h = Mat3x3::TryHomography(source, target);
	REQUIRE(h);
	const auto inverse = Mat3x3::TryHomography(target, source);
	REQUIRE(inverse);
	REQUIRE(System::Update());
	const auto frame = CaptureWarpDraw([&]
	{
		const ScopedQuadWarp2D warp{ *h };
		texture.draw();
	});
	REQUIRE(frame.image.width() > 300);
	REQUIRE(frame.image.height() > 270);
	// Samples on both triangles, including either side of their shared diagonal.
	for (const Vec2 uv : { Vec2{ 24, 24 }, Vec2{ 90, 25 }, Vec2{ 25, 90 }, Vec2{ 65, 63 }, Vec2{ 63, 65 }, Vec2{ 100, 100 } })
	{
		const Point pixel = h->transformPoint(uv).asPoint();
		const Vec2 original = inverse->transformPoint(Vec2{ pixel } + Vec2{ 0.5, 0.5 });
		const Color actual = frame.image[pixel.y][pixel.x];
		CHECK(Abs(int32(actual.r) - int32(original.x * 2 - 1)) <= 3);
		CHECK(Abs(int32(actual.g) - int32(original.y * 2 - 1)) <= 3);
	}
	CHECK(frame.image[20][20] == Palette::Black);

	// Local/Camera always precede Warp, independent of scope construction order.
	const auto withAffine = [&](const bool warpFirst)
	{
		return CaptureWarpDraw([&]
		{
			if (warpFirst)
			{
				const ScopedQuadWarp2D warp{ *h };
				const Transformer2D local{ Mat3x2::Scale(2) };
				const Transformer2D camera{ Mat3x2::Translate(10, 15), Transformer2D::Target::SetCamera };
				texture.resized(40, 40).draw();
			}
			else
			{
				const Transformer2D local{ Mat3x2::Scale(2) };
				const Transformer2D camera{ Mat3x2::Translate(10, 15), Transformer2D::Target::SetCamera };
				const ScopedQuadWarp2D warp{ *h };
				texture.resized(40, 40).draw();
			}
		});
	};
	CHECK(withAffine(true).image == withAffine(false).image);
}

TEST_CASE("QuadWarp.viewport_scissor_and_geometry")
{
	const WarpSceneScope scene;
	REQUIRE(System::Update());
	const auto draw = []
	{
		RectF{ 0, 0, 200, 120 }.draw(Palette::Red);
		Circle{ 100, 60, 20 }.draw(Palette::Blue);
	};
	const auto reference = CaptureWarpDraw([&]
	{
		const ScopedViewport2D viewport{ Rect{ 70, 50, 250, 200 } };
		const ScopedScissorRect2D scissor{ Rect{ 100, 75, 120, 70 } };
		const Transformer2D transform{ Mat3x2::Translate(20, 20) };
		draw();
	});
	const auto actual = CaptureWarpDraw([&]
	{
		const ScopedQuadWarp2D warp{ Mat3x3{ Mat3x2::Translate(20, 20) } };
		const ScopedViewport2D viewport{ Rect{ 70, 50, 250, 200 } };
		const ScopedScissorRect2D scissor{ Rect{ 100, 75, 120, 70 } };
		draw();
		CHECK(Graphics2D::GetViewport() == Optional<Rect>{ Rect{ 70, 50, 250, 200 } });
		CHECK(Graphics2D::GetScissorRect() == Optional<Rect>{ Rect{ 100, 75, 120, 70 } });
	});
	CHECK(actual.image == reference.image);
	CHECK(actual.image[80][110] == Palette::Red);
	CHECK(actual.image[80][95] == Palette::Black);
}

TEST_CASE("QuadWarp.custom_stages_and_vertex_colors")
{
	const WarpSceneScope scene;
# if SIV3D_PLATFORM(WINDOWS)
	const std::string source = R"(
struct Vertex { float2 position : POSITION; float2 uv : TEXCOORD0; float4 color : COLOR0; };
struct Varying { float4 position : SV_POSITION; float4 colorPMA : COLOR0; float2 uv : TEXCOORD0; };
cbuffer Constants : register(b0) { row_major float3x4 transform; float4 colorMul; };
Varying VS(Vertex input)
{
    Varying result;
    const float4 clip = mul(float3(input.position, 1), transform);
    result.position = float4(clip.xy, 0, clip.w);
    const float4 color = input.color * colorMul;
    result.colorPMA = float4(color.rgb * color.a, color.a);
    result.uv = input.uv;
    return result;
}
float4 PS(Varying input) : SV_TARGET { return input.colorPMA; }
)";
	const VertexShader vs = VertexShader::HLSL(source, U"VS");
	const PixelShader ps = PixelShader::HLSL(source, U"PS");
# else
	const std::string source = R"(
#include <metal_stdlib>
using namespace metal;
struct Vertex { float2 position; float2 uv; float4 color; };
struct Varying { float4 position [[position]]; float4 colorPMA; float2 uv; };
struct Constants { float3x4 transform; float4 colorMul; };
vertex Varying VS(uint id [[vertex_id]], constant Vertex* vertices [[buffer(0)]],
                  constant Constants& c [[buffer(1)]])
{
    const Vertex input = vertices[id];
    Varying result;
    const float4 clip = c.transform * float3(input.position, 1);
    result.position = float4(clip.xy, 0, clip.w);
    const float4 color = input.color * c.colorMul;
    result.colorPMA = float4(color.rgb * color.a, color.a);
    result.uv = input.uv;
    return result;
}
fragment float4 PS(Varying input [[stage_in]]) { return input.colorPMA; }
)";
	const VertexShader vs = VertexShader::MSL(source, U"VS");
	const PixelShader ps = PixelShader::MSL(source, U"PS");
# endif
	REQUIRE(vs);
	REQUIRE(ps);
	const Texture texture{ Image{ 4, 4, Palette::White } };
	REQUIRE(texture);
	const auto h = Mat3x3::TryHomography(RectF{ 0, 0, 200, 100 }, Quad{ 30, 30, 360, 65, 290, 270, 70, 240 });
	REQUIRE(h);
	const auto draw = [&]
	{
		texture(1, 1, 2, 2).resized(200, 100).mirrored().draw(0, 0,
			ColorF{ 1, 0, 0, 0.8 }, ColorF{ 0, 1, 0, 0.7 }, ColorF{ 0, 0, 1, 0.6 }, ColorF{ 1, 1, 1, 0.5 });
	};
	REQUIRE(System::Update());
	const auto reference = CaptureWarpDraw([&]
	{
		{ const ScopedQuadWarp2D warp{ *h }; draw(); }
		RectF{ 20, 300, 60, 30 }.draw(Palette::Orange);
	});
	for (const int32 stages : { 1, 2, 3 })
	{
		const auto actual = CaptureWarpDraw([&]
		{
			{
				const ScopedQuadWarp2D warp{ *h };
				if (stages == 1) { const ScopedCustomShader2D custom{ vs }; draw(); }
				if (stages == 2) { const ScopedCustomShader2D custom{ ps }; draw(); }
				if (stages == 3) { const ScopedCustomShader2D custom{ vs, ps }; draw(); }
			}
			RectF{ 20, 300, 60, 30 }.draw(Palette::Orange);
		});
		CHECK(actual.image == reference.image);
	}
}

TEST_CASE("QuadWarp.pattern_curves_and_text")
{
	const WarpSceneScope scene;
	const Font font{ FontMethod::MSDF, 24 };
	REQUIRE(font);
	const auto draw = [&]
	{
		RectF{ 20, 20, 110, 90 }.draw(Pattern::Checker{});
		Circle{ 180, 65, 35 }.draw(Palette::Orange);
		font(U"Warp").draw(20, 140, Palette::White);
		font(U"Effect").draw(20, 170, TextEffect::QuadWarp{ 0.1, -0.1 });
	};
	const RectF source{ 0, 0, 240, 200 };
	const Quad target{ 40, 40, 360, 70, 310, 310, 60, 270 };
	const auto h = Mat3x3::TryHomography(source, target);
	REQUIRE(h);
	REQUIRE(System::Update());
	const auto reference = CaptureWarpDraw([&] { const ScopedQuadWarp2D warp{ *h }; draw(); });
	CHECK(reference.metrics.triangleCount > 6);
	for (const int32 count : { 16382, 16384 })
	{
		const auto actual = CaptureWarpDraw([&]
		{
			for (int32 i = 0; i < count; ++i) { RectF{ -10, -10, 1, 1 }.draw(); }
			const ScopedQuadWarp2D warp{ *h };
			draw();
		});
		CHECK(actual.image == reference.image);
		CHECK(actual.metrics.triangleCount == reference.metrics.triangleCount + count * 2);
	}
	// Use the rasterized glyph as an ordinary texture: the effect must keep the outer warp.
	const auto glyphs = font.getGlyphs(U"A");
	REQUIRE(glyphs.size() == 1);
	const auto& glyph = glyphs.front();
	REQUIRE(glyph.texture.texture);
	REQUIRE(glyph.texture.size.x > 0);
	REQUIRE(glyph.texture.size.y > 0);
	const GlyphContext context{ .pos = { 30, 40 } };
	const TextEffect::QuadWarp effect{ 0.2, -0.1, Palette::White };
	const RectF rect{ context.pos, glyph.texture.size };
	Quad quad = rect.asQuad();
	quad.p0.y -= glyph.texture.size.x * 0.2;
	quad.p1.y -= glyph.texture.size.x * -0.1;
	const auto glyphWarp = Mat3x3::TryHomography(rect, quad);
	REQUIRE(glyphWarp);
	for (const Mat3x2 affine : { Mat3x2::Identity(), Mat3x2::ShearX(0.25).scaled(1.5, 0.8).translated(20, 10) })
	{
		const auto effectDraw = CaptureWarpDraw([&]
		{
			const Transformer2D local{ affine };
			const ScopedQuadWarp2D warp{ *h };
			effect.draw(glyph.texture, context);
		});
		const auto explicitDraw = CaptureWarpDraw([&]
		{
			// Raw glyph deformation, then affine transform, then the outer projective transform.
			const ScopedQuadWarp2D warp{ *glyphWarp * Mat3x3{ affine } * *h };
			glyph.texture.draw(context.pos);
		});
		REQUIRE(effectDraw.image.size() == explicitDraw.image.size());
		int32 largestDifference = 0;
		for (size_t i = 0; i < effectDraw.image.pixelCount(); ++i)
		{
			const Color a = effectDraw.image.data()[i], b = explicitDraw.image.data()[i];
			largestDifference = Max(largestDifference, Max(Abs(int32(a.r) - b.r), Abs(int32(a.g) - b.g), Abs(int32(a.b) - b.b)));
		}
		// Equivalent homographies can differ by float rounding at antialiased edges.
		CHECK(largestDifference <= 1);
	}
	const auto invalid = CaptureWarpDraw([&]
	{
		const Transformer2D collapse{ Mat3x2::Scale(0) };
		const ScopedQuadWarp2D warp{ *h };
		effect.draw(glyph.texture, context);
	});
	CHECK(invalid.metrics.triangleCount == 0);
}
