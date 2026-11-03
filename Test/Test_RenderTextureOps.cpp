//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("RenderTextureOps.copy_and_resample_regions")
{
	Grid<Color> pixels{ Size{ 8, 6 }, Color{ 255, 0, 255 } };
	for (int32 y = 1; y < 5; ++y)
	{
		for (int32 x = 2; x < 6; ++x)
		{
			pixels[y][x] = Color{ uint8(x * 30), uint8(y * 40), 51, 123 };
		}
	}
	const RenderTexture input{ pixels };
	const RenderTexture output{ Size{ 10, 8 }, Palette::Blue };
	RenderTextureOps::Copy(input, Rect{ 2, 1, 4, 4 }, output, Rect{ 3, 2, 4, 4 });
	Grid<Color> actual;
	REQUIRE(output.read(actual));
	for (int32 y = 0; y < 8; ++y)
	{
		for (int32 x = 0; x < 10; ++x)
		{
			CHECK(actual[y][x] == ((3 <= x && x < 7 && 2 <= y && y < 6) ? pixels[y - 1][x - 1] : Palette::Blue));
		}
	}
	const RenderTexture enlarged{ Size{ 8, 8 } };
	RenderTextureOps::Resample(input, Rect{ 2, 1, 4, 4 }, enlarged, Rect{ 8, 8 }, TextureFilter::Nearest);
	REQUIRE(enlarged.read(actual));
	for (int32 y = 0; y < 8; ++y)
	{
		for (int32 x = 0; x < 8; ++x)
		{
			CHECK(actual[y][x] == pixels[1 + y / 2][2 + x / 2]);
		}
	}
}

TEST_CASE("RenderTextureOps.filters_preserve_constant")
{
	RenderTextureOps::Workspace work;
	const Float4 color{ -0.5f, 2, 0.25f, 0.5f };
	const RenderTexture source{ Grid<Float4>{ Size{ 17, 11 }, color } };
	const RenderTexture destination{ source.size(), TextureFormat::R32G32B32A32_Float };
	const RenderTexture small{ Size{ 3, 2 }, TextureFormat::R32G32B32A32_Float };
	for (const double sigma : { 0.0, 0.5, 2.0, 32.0 })
	{
		RenderTextureOps::GaussianBlur(source, destination, sigma, work);
		Grid<Float4> actual;
		REQUIRE(destination.read(actual));
		for (const auto& pixel : actual)
		{
			CHECK(pixel.distanceFrom(color) < 0.00001);
		}
	}
	for (const double radius : { 0.0, 0.5, 2.0, 7.0, 256.0 })
	{
		RenderTextureOps::DualKawaseBlur(source, destination, radius, work);
		Grid<Float4> actual;
		REQUIRE(destination.read(actual));
		for (const auto& pixel : actual)
		{
			CHECK(pixel.distanceFrom(color) < 0.00001);
		}
	}
	RenderTextureOps::Downsample(source, small, work);
	Grid<Float4> actual;
	REQUIRE(small.read(actual));
	for (const auto& pixel : actual)
	{
		CHECK(pixel.distanceFrom(color) < 0.00001);
	}
}

namespace
{
	Grid<float> MakePattern(const Size size)
	{
		Grid<float> result{ size };
		for (int32 y = 0; y < size.y; ++y)
		{
			for (int32 x = 0; x < size.x; ++x)
			{
				result[y][x] = ((x * 7 + y * 13) % 19) * 0.125f;
			}
		}
		return result;
	}

	void CheckClose(const Grid<float>& actual, const Grid<float>& expected, const double tolerance = 0.0003)
	{
		REQUIRE(actual.size() == expected.size());
		double maximum = 0;
		for (size_t i = 0; i < static_cast<size_t>(actual.width() * actual.height()); ++i)
		{
			maximum = Max(maximum, std::abs(static_cast<double>(actual.data()[i]) - expected.data()[i]));
		}
		CHECK(maximum <= tolerance);
	}

	Grid<float> GaussianReference(const Grid<float>& source, const double sigma)
	{
		if (sigma == 0)
		{
			return source;
		}
		const int32 radius = static_cast<int32>(std::ceil(3 * sigma));
		Array<double> weights(2 * radius + 1);
		double sum = 0;
		for (int32 i = -radius; i <= radius; ++i)
		{
			weights[i + radius] = std::exp(-0.5 * ((i / sigma) * (i / sigma)));
			sum += weights[i + radius];
		}
		for (auto& weight : weights)
		{
			weight /= sum;
		}
		Grid<double> horizontal{ source.size(), 0.0 };
		Grid<float> result{ source.size() };
		for (int32 y = 0; y < source.height(); ++y)
		{
			for (int32 x = 0; x < source.width(); ++x)
			{
				for (int32 i = -radius; i <= radius; ++i)
				{
					horizontal[y][x] += source[y][Clamp(x + i, 0, source.width() - 1)] * weights[i + radius];
				}
			}
		}
		for (int32 y = 0; y < source.height(); ++y)
		{
			for (int32 x = 0; x < source.width(); ++x)
			{
				double value = 0;
				for (int32 i = -radius; i <= radius; ++i)
				{
					value += horizontal[Clamp(y + i, 0, source.height() - 1)][x] * weights[i + radius];
				}
				result[y][x] = static_cast<float>(value);
			}
		}
		return result;
	}
}

TEST_CASE("RenderTextureOps.copy_preserves_saved_texels_all_formats")
{
	for (uint8 i = FromEnum(TexturePixelFormat::R8_Unorm); i <= FromEnum(TexturePixelFormat::R32G32B32A32_Float); ++i)
	{
		const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
		CAPTURE(format);
		Array<Byte> bytes(7 * 5 * format.pixelSize());
		for (size_t j = 0; j < bytes.size(); ++j)
		{
			bytes[j] = static_cast<Byte>((j * 37) & 255);
		}
		const RenderTexture input{ Size{ 7, 5 }, std::span<const Byte>{ bytes }, format };
		const RenderTexture output{ input.size(), format };
		RenderTextureOps::Copy(input, output);
		Array<Byte> actual(bytes.size());
		REQUIRE(output.read(std::span{ actual }));
		CHECK(actual == bytes);
	}
}

TEST_CASE("RenderTextureOps.gaussian_matches_discrete_convolution")
{
	RenderTextureOps::Workspace work;
	for (const Size size : { Size{ 17, 11 }, Size{ 1, 7 }, Size{ 9, 1 }, Size{ 1, 1 }, Size{ 7, 5 } })
	{
		const auto pixels = MakePattern(size);
		const RenderTexture source{ pixels };
		const RenderTexture destination{ size, TextureFormat::R32_Float };
		for (const double sigma : { 0.0, 1e-100, 0.2, 0.75, 2.0, 4.5, 32.0 })
		{
			CAPTURE(size, sigma);
			RenderTextureOps::GaussianBlur(source, destination, sigma, work);
			Grid<float> actual;
			REQUIRE(destination.read(actual));
			// Pairing taps uses the hardware sampler's finite interpolation precision.
			CheckClose(actual, GaussianReference(pixels, sigma), 0.005);
		}
	}
}

TEST_CASE("RenderTextureOps.region_isolation_and_workspace_history")
{
	RenderTextureOps::Workspace reused;
	const RenderTexture poison{ Size{ 65, 47 }, ColorF{ 1000, 0, 0, 1 }, TextureFormat::R32_Float };
	const RenderTexture poisonResult{ poison.size(), TextureFormat::R32_Float };
	const RenderTexture reduced{ Size{ 8, 5 }, TextureFormat::R32_Float };
	for (const Size size : { Size{ 13, 9 }, Size{ 7, 5 }, Size{ 21, 17 }, Size{ 1, 9 } })
	{
		const auto pixels = MakePattern(size);
		Grid<float> atlas{ size + Size{ 8, 8 }, 1000.0f };
		for (int32 y = 0; y < size.y; ++y)
		{
			for (int32 x = 0; x < size.x; ++x)
			{
				atlas[y + 3][x + 2] = pixels[y][x];
			}
		}
		const RenderTexture source{ atlas }, tight{ pixels };
		const Rect input{ 2, 3, size }, output{ 3, 2, size };
		const RenderTexture expected{ size, TextureFormat::R32_Float };
		const RenderTexture actual{ atlas.size(), ColorF{ -77, 0, 0, 1 }, TextureFormat::R32_Float };
		for (int32 op = 0; op < 4; ++op)
		{
			CAPTURE(size, op);
			RenderTextureOps::Workspace fresh;
			if (op == 0)
			{
				RenderTextureOps::GaussianBlur(poison, poisonResult, 5, reused);
				RenderTextureOps::GaussianBlur(tight, expected, 2.25, fresh);
				RenderTextureOps::GaussianBlur(source(input).resized(800, 600), actual, output, 2.25, reused);
			}
			else if (op == 1 || op == 2)
			{
				const double radius = (op == 1) ? 0.75 : 12.5;
				RenderTextureOps::DualKawaseBlur(poison, poisonResult, 16, reused);
				RenderTextureOps::DualKawaseBlur(tight, expected, radius, fresh);
				RenderTextureOps::DualKawaseBlur(source(input).resized(1, 1), actual, output, radius, reused);
			}
			else
			{
				RenderTextureOps::Resample(tight, expected);
				RenderTextureOps::Resample(source(input).resized(100, 100), actual, output);
			}
			Grid<float> e, a;
			REQUIRE(expected.read(e));
			REQUIRE(actual.read(a));
			Grid<float> region{ size };
			for (int32 y = 0; y < a.height(); ++y)
			{
				for (int32 x = 0; x < a.width(); ++x)
				{
					if (((output.x <= x) && (x < output.x + output.w) && (output.y <= y) && (y < output.y + output.h)))
					{
						region[y - output.y][x - output.x] = a[y][x];
					}
					else
					{
						CHECK(a[y][x] == -77);
					}
				}
			}
			CheckClose(region, e);
		}
		// Multistage reduction uses its own retained levels after other operations.
		RenderTextureOps::Downsample(poison, reduced, reused);
		const Size small{ Max(1, size.x / 5), Max(1, size.y / 5) };
		const RenderTexture smallExpected{ small, TextureFormat::R32_Float }, smallActual{ small, TextureFormat::R32_Float };
		RenderTextureOps::Workspace fresh;
		RenderTextureOps::Downsample(tight, smallExpected, fresh);
		RenderTextureOps::Downsample(source(input), smallActual, Rect{ small }, reused);
		Grid<float> e, a;
		REQUIRE(smallExpected.read(e));
		REQUIRE(smallActual.read(a));
		CheckClose(a, e);
	}
}

TEST_CASE("RenderTextureOps.downsample_reduces_checkerboard")
{
	RenderTextureOps::Workspace work;
	Grid<float> checker{ Size{ 64, 32 } };
	for (int32 y = 0; y < checker.height(); ++y)
	{
		for (int32 x = 0; x < checker.width(); ++x)
		{
			checker[y][x] = static_cast<float>((x + y) & 1);
		}
	}
	const RenderTexture source{ checker };
	for (const Size size : { Size{ 32, 16 }, Size{ 8, 4 }, Size{ 1, 1 }, Size{ 64, 1 } })
	{
		const RenderTexture output{ size, TextureFormat::R32_Float };
		RenderTextureOps::Downsample(source, output, work);
		Grid<float> actual;
		REQUIRE(output.read(actual));
		CheckClose(actual, Grid<float>{ size, 0.5f });
	}
}

TEST_CASE("RenderTextureOps.aliasing_uses_the_original_input")
{
	for (int32 op = 0; op < 7; ++op)
	{
		CAPTURE(op);
		const auto pixels = MakePattern(Size{ 21, 17 });
		const RenderTexture source{ pixels }, inPlace{ pixels };
		const RenderTexture reference{ source.size(), TextureFormat::R32_Float };
		RenderTextureOps::Copy(source, reference);
		RenderTextureOps::Workspace work, second;
		const Rect input{ 1, 2, 13, 9 };
		const Rect output{ 4, 5, (op == 5) ? Size{ 5, 3 } : (op == 6) ? Size{ 7, 5 } : Size{ 13, 9 } };
		const auto run = [&](const Texture& sourceTexture, const RenderTexture& destinationTexture, RenderTextureOps::Workspace& workspace)
		{
			if (op < 2)
			{
				RenderTextureOps::GaussianBlur(sourceTexture, input, destinationTexture, output, (op == 0) ? 0 : 2, workspace);
			}
			else if (op < 5)
			{
				RenderTextureOps::DualKawaseBlur(sourceTexture, input, destinationTexture, output, (op == 2) ? 0 : (op == 3) ? 0.4 : 6, workspace);
			}
			else
			{
				RenderTextureOps::Downsample(sourceTexture, input, destinationTexture, output, workspace);
			}
		};
		run(source, reference, work);
		const RenderTexture shared = inPlace;
		run(shared, inPlace, second);
		Grid<float> e, a;
		REQUIRE(reference.read(e));
		REQUIRE(inPlace.read(a));
		CheckClose(a, e);
	}
}

TEST_CASE("RenderTextureOps.dual_kawase_strength_is_continuous")
{
	Grid<float> pixels{ Size{ 65, 65 }, 0.0f };
	pixels[32][32] = 1;
	const RenderTexture source{ pixels }, a{ source.size(), TextureFormat::R32_Float }, b{ source.size(), TextureFormat::R32_Float };
	RenderTextureOps::Workspace work;
	for (const double radius : { 0.0, 1.0, 3.0, 7.0, 15.0, 31.0, 63.0, 127.0, 255.0 })
	{
		CAPTURE(radius);
		RenderTextureOps::DualKawaseBlur(source, a, Max(0.0, radius - 1e-5), work);
		RenderTextureOps::DualKawaseBlur(source, b, radius + 1e-5, work);
		Grid<float> x, y;
		REQUIRE(a.read(x));
		REQUIRE(b.read(y));
		CheckClose(x, y, 0.00002);
		for (const auto value : y)
		{
			CHECK((0 <= value && value <= 1));
		}
	}
}

TEST_CASE("RenderTextureOps.validation_and_empty_regions_do_not_write")
{
	const RenderTexture source{ Size{ 7, 5 }, Palette::Red }, destination{ source.size(), Palette::Blue };
	RenderTextureOps::Workspace work;
	CHECK_THROWS_AS(RenderTextureOps::Copy(Texture{}, destination), Error);
	CHECK_THROWS_AS(RenderTextureOps::Copy(source, Rect{ -1, 0, 7, 5 }, destination, Rect{ 7, 5 }), Error);
	CHECK_THROWS_AS(RenderTextureOps::Copy(source, Rect{ 1, 0, 7, 5 }, destination, Rect{ 7, 5 }), Error);
	CHECK_THROWS_AS(RenderTextureOps::Resample(source, source), Error);
	CHECK_THROWS_AS(RenderTextureOps::Resample(source, destination, static_cast<TextureFilter>(255)), Error);
	CHECK_THROWS_AS(RenderTextureOps::Downsample(source, Rect{ 2, 2 }, destination, Rect{ 3, 3 }, work), Error);
	CHECK_THROWS_AS(RenderTextureOps::GaussianBlur(source, Rect{ 2, 2 }, destination, Rect{ 3, 3 }, 1, work), Error);
	CHECK_THROWS_AS(RenderTextureOps::DualKawaseBlur(source, Rect{ 2, 2 }, destination, Rect{ 3, 3 }, 1, work), Error);
	const RenderTexture shared = source;
	CHECK_NOTHROW(RenderTextureOps::Copy(source, shared));
	CHECK_THROWS_AS(RenderTextureOps::Copy(source, Rect{ 0, 0, 3, 3 }, shared, Rect{ 1, 1, 3, 3 }), Error);
	for (const double invalid : { -1.0, 257.0, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN() })
	{
		CHECK_THROWS_AS(RenderTextureOps::GaussianBlur(source, destination, invalid, work), Error);
		CHECK_THROWS_AS(RenderTextureOps::DualKawaseBlur(source, destination, invalid, work), Error);
	}
	CHECK_THROWS_AS(RenderTextureOps::GaussianBlur(source, destination, 32.1, work), Error);
	CHECK_NOTHROW(RenderTextureOps::Resample(source, Rect{ 0, 0, 0, 0 }, destination, Rect{ 7, 5, 0, 0 }));
	CHECK_NOTHROW(RenderTextureOps::GaussianBlur(source, Rect{ 0, 0, 0, 0 }, destination, Rect{ 7, 5, 0, 0 }, 1, work));
	CHECK_THROWS_AS(RenderTextureOps::GaussianBlur(source, Rect{ 0, 0, 0, 0 }, destination, Rect{ 7, 5, 0, 0 }, -1, work), Error);
	CHECK_THROWS_AS(RenderTextureOps::Resample(source, Rect{ 0, 0, 0, 1 }, destination, Rect{ 1, 1 }), Error);
	auto fractional = TextureRegion{ source };
	fractional.uvRect.left = 0.1f;
	CHECK_THROWS_AS(RenderTextureOps::Resample(fractional, destination, Rect{ destination.size() }), Error);
	CHECK_THROWS_AS(RenderTextureOps::Downsample(source.mirrored(), destination, Rect{ destination.size() }, work), Error);
	Grid<Color> actual;
	REQUIRE(destination.read(actual));
	for (const auto& color : actual)
	{
		CHECK(color == Palette::Blue);
	}
}

TEST_CASE("RenderTextureOps.preserves_mips_and_converts_linear_color")
{
	const RenderTexture input{ Size{ 8, 8 }, ColorF{ 0.25, 0.5, 0.75, 0.5 }, TextureFormat::R32G32B32A32_Float };
	const RenderTexture output{ Size{ 8, 8 }, Palette::Blue, TextureFormat::R8G8B8A8_Unorm_SRGB, TextureDesc::Mipmap };
	output.generateMips();
	RenderTextureOps::Workspace work;
	for (int32 op = 0; op < 4; ++op)
	{
		if (op == 0)
		{
			RenderTextureOps::Resample(input, output);
		}
		else if (op == 1)
		{
			RenderTextureOps::Downsample(input, output, work);
		}
		else if (op == 2)
		{
			RenderTextureOps::GaussianBlur(input, output, 2, work);
		}
		else
		{
			RenderTextureOps::DualKawaseBlur(input, output, 6, work);
		}
		Image actual, mip;
		REQUIRE(output.readAsImage(actual));
		REQUIRE(output.readAsImage(mip, 2));
		CHECK(mip == Image{ 2, 2, Palette::Blue });
		const Color expected = ColorF{ 0.25, 0.5, 0.75, 0.5 }.linearToSRGB().toColor();
		for (const auto p : actual)
		{
			CHECK(std::abs(int(p.r) - expected.r) <= 1);
			CHECK(std::abs(int(p.g) - expected.g) <= 1);
			CHECK(std::abs(int(p.b) - expected.b) <= 1);
			CHECK(std::abs(int(p.a) - expected.a) <= 1);
		}
	}
	const RenderTexture copied{ output.size(), Palette::Red, output.format(), TextureDesc::Mipmap };
	copied.generateMips();
	RenderTextureOps::Copy(output, copied);
	Image mip;
	REQUIRE(copied.readAsImage(mip, 2));
	CHECK(mip == Image{ 2, 2, Palette::Red });
}

TEST_CASE("RenderTextureOps.order_state_and_workspace_lifetime")
{
	const auto config = Scene::GetConfig();
	const ScopeExit restore{ [&] { Scene::Configure(config); } };
	Scene::Configure(SceneConfig::Fixed({ 64, 32 }));
	REQUIRE(System::Update());
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
	const ScopedColorMul2D mul{ ColorF{ 1.0 }, ScopedColorMul2D::Target::Set };
	const ScopedColorAdd2D add{ ColorF{ 0, 0 }, ScopedColorAdd2D::Target::Set };
	const ScopedViewport2D viewport{ none };
	const ScopedScissorRect2D scissor{ none };
	const ScopedRenderStates2D states{ BlendState::Opaque, SamplerState::ClampNearest };
	const RenderTexture canvas{ Size{ 32, 16 }, Palette::Black };
	const RenderTexture source{ Size{ 8, 8 }, Palette::Blue }, processed{ source.size() };
	const ScopedRenderTarget2D target{ canvas };
	for (int32 op = 0; op < 5; ++op)
	{
		CAPTURE(op);
		canvas.clear(Palette::Black);
		{
			const ScopedRenderTarget2D inputTarget{ source };
			Rect{ source.size() }.draw(Palette::White);
		}
		// Leave a preceding draw queued while every ordinary drawing state is nondefault.
		const ScopedColorMul2D color{ ColorF{ 0.25, 0.5, 0.75, 1 }, ScopedColorMul2D::Target::Set };
		const ScopedColorAdd2D addition{ ColorF{ 0.1, 0, 0, 0 }, ScopedColorAdd2D::Target::Set };
		const ScopedViewport2D offsetViewport{ Rect{ 1, 0, 30, 16 } };
		const ScopedScissorRect2D clip{ Rect{ 0, 0, 32, 8 } };
		const Transformer2D offset{ Mat3x2::Translate(2, 0) };
		Rect{ 0, 0, 8, 16 }.draw();
		{
			RenderTextureOps::Workspace work;
			if (op == 0)
			{
				RenderTextureOps::Copy(source, processed);
			}
			else if (op == 1)
			{
				RenderTextureOps::Resample(source, processed);
			}
			else if (op == 2)
			{
				RenderTextureOps::Downsample(source, processed, work);
			}
			else if (op == 3)
			{
				RenderTextureOps::GaussianBlur(source, processed, 3, work);
			}
			else
			{
				RenderTextureOps::DualKawaseBlur(source, processed, 2.5, work);
			}
			RenderTextureOps::Workspace moved{ std::move(work) };
			moved.release(); // Recorded GPU commands must retain their resources.
			work = std::move(moved);
		}
		processed.draw(16, 0);
		CHECK(Graphics2D::GetViewport() == Rect{ 1, 0, 30, 16 });
		CHECK(Graphics2D::GetScissorRect() == Rect{ 0, 0, 32, 8 });
		Image image, plain;
		REQUIRE(canvas.readAsImage(image));
		REQUIRE(processed.readAsImage(plain));
		CHECK(plain == Image{ 8, 8, Palette::White });
		CHECK(image[4][5] == image[4][21]);
		CHECK(image[4][21] != Palette::Black);
		CHECK(image[10][5] == Palette::Black);
		CHECK(image[10][21] == Palette::Black);
		CHECK(image[4][1] == Palette::Black);
	}
}

TEST_CASE("RenderTextureOps.multisample_input_uses_last_resolve")
{
	REQUIRE(System::Update());
	const MSRenderTexture source{ Size{ 8, 8 }, Palette::Red, MSAASampleCount::X4MSAA };
	REQUIRE(source);
	source.resolve();
	source.clear(Palette::Blue);
	const RenderTexture output{ source.size() };
	RenderTextureOps::Copy(source, output);
	Image actual;
	REQUIRE(output.readAsImage(actual));
	CHECK(actual == Image{ 8, 8, Palette::Red });
	source.resolve();
	RenderTextureOps::Resample(source, output);
	REQUIRE(output.readAsImage(actual));
	CHECK(actual == Image{ 8, 8, Palette::Blue });
}

TEST_CASE("RenderTextureOps.custom_shaders_constants_and_mrt_survive")
{
	REQUIRE(System::Update());
	const ScopedViewport2D viewport{ none };
	const ScopedScissorRect2D scissor{ none };
	const ScopedRenderStates2D states{ BlendState::Opaque, RasterizerState::Default2D };
	const std::string metal = R"(
#include <metal_stdlib>
using namespace metal;
struct Output { float4 a [[color(0)]]; float4 b [[color(1)]]; };
vertex float4 TestVS(uint id [[vertex_id]], constant float4& offset [[buffer(2)]])
{
    return float4(id == 2 ? 3.0 : -1.0, id == 1 ? 3.0 : -1.0, 0, 1) + offset;
}
fragment Output TestPS(constant float4& color [[buffer(3)]]) { return { color, color.bgra }; }
)";
	const std::string hlsl = R"(
cbuffer Offset : register(b2) { float4 offset; };
cbuffer Color : register(b3) { float4 color; };
struct Output { float4 a : SV_Target0; float4 b : SV_Target1; };
float4 TestVS(uint id : SV_VertexID) : SV_POSITION
{
    return float4(id == 2 ? 3.0 : -1.0, id == 1 ? 3.0 : -1.0, 0, 1) + offset;
}
Output TestPS() { Output o; o.a = color; o.b = color.bgra; return o; }
)";
# if SIV3D_PLATFORM(MACOS)
	const VertexShader vs = VertexShader::MSL(metal, U"TestVS");
	const PixelShader ps = PixelShader::MSL(metal, U"TestPS");
# else
	const VertexShader vs = VertexShader::HLSL(hlsl, U"TestVS");
	const PixelShader ps = PixelShader::HLSL(hlsl, U"TestPS");
# endif
	REQUIRE(vs);
	REQUIRE(ps);
	const ScopedCustomShader2D shaders{ vs, ps };
	ConstantBuffer<Float4> offset{ Float4{ 0, 0, 0, 0 } }, color{ Float4{ 1, 0, 0, 1 } };
	const ScopedVSConstantBuffer2D vsConstants{ 2, offset };
	const ScopedPSConstantBuffer2D psConstants{ 3, color };
	const RenderTexture a{ Size{ 8, 8 } }, b{ a.size() }, saved{ a.size() }, filtered{ a.size() };
	const ScopedRenderTarget2D targets{ a, b };
	Graphics2D::DrawTriangles(1);
	RenderTextureOps::Copy(a, saved);
	RenderTextureOps::Workspace work;
	RenderTextureOps::GaussianBlur(a, filtered, 2, work);
	a.clear(Palette::Black);
	b.clear(Palette::Black);
	Graphics2D::DrawTriangles(1);
	Image image;
	REQUIRE(a.readAsImage(image));
	CHECK(image == Image{ 8, 8, Palette::Red });
	REQUIRE(b.readAsImage(image));
	CHECK(image == Image{ 8, 8, Palette::Blue });
	REQUIRE(saved.readAsImage(image));
	CHECK(image == Image{ 8, 8, Palette::Red });
	REQUIRE(filtered.readAsImage(image));
	CHECK(image == Image{ 8, 8, Palette::Red });
}

TEST_CASE("RenderTextureOps.linear_resample_clamps_cropped_edges")
{
	const Grid<float> pixels{ { 100.0f, 0.0f, 1.0f, -100.0f } };
	const RenderTexture source{ pixels };
	const RenderTexture destination{ Size{ 7, 3 }, TextureFormat::R32_Float };
	RenderTextureOps::Resample(source, Rect{ 1, 0, 2, 1 }, destination, Rect{ destination.size() });
	Grid<float> actual;
	REQUIRE(destination.read(actual));
	for (int32 y = 0; y < 3; ++y)
	{
		for (int32 x = 0; x < 7; ++x)
		{
			const double expected = Clamp((x + 0.5) * 2 / 7 - 0.5, 0.0, 1.0);
			CHECK(std::abs(actual[y][x] - expected) < 0.002);
		}
	}
}

TEST_CASE("RenderTextureOps.zero_blur_converts_each_texel_without_mixing")
{
	Grid<Float4> pixels{ Size{ 19, 11 } };
	for (int32 y = 0; y < 11; ++y)
	{
		for (int32 x = 0; x < 19; ++x)
		{
			pixels[y][x] = ((x + y) & 1) ? Float4{ 0, 1, 0, 1 } : Float4{ 1, 0, 0, 1 };
		}
	}
	const RenderTexture source{ pixels }, output{ Size{ 15, 9 } };
	RenderTextureOps::Workspace work;
	for (int32 op = 0; op < 3; ++op)
	{
		if (op == 0)
		{
			RenderTextureOps::GaussianBlur(source, Rect{ 2, 1, 15, 9 }, output, Rect{ output.size() }, 0, work);
		}
		else if (op == 1)
		{
			RenderTextureOps::DualKawaseBlur(source, Rect{ 2, 1, 15, 9 }, output, Rect{ output.size() }, 0, work);
		}
		else
		{
			RenderTextureOps::Downsample(source, Rect{ 2, 1, 15, 9 }, output, Rect{ output.size() }, work);
		}
		Grid<Color> actual;
		REQUIRE(output.read(actual));
		for (int32 y = 0; y < 9; ++y)
		{
			for (int32 x = 0; x < 15; ++x)
			{
				CHECK(actual[y][x] == ((x + y + 3) & 1 ? Palette::Lime : Palette::Red));
			}
		}
	}
}

TEST_CASE("RenderTextureOps.dual_kawase_bright_points_remain_stable_at_odd_sizes")
{
	RenderTextureOps::Workspace work;
	for (const Size size : { Size{ 256, 256 }, Size{ 257, 259 }, Size{ 1, 257 } })
	{
		for (const int32 phase : { 0, 1 })
		{
			Grid<float> pixels{ size, 0.0f };
			pixels[size.y / 2 + phase][size.x / 2] = 1;
			const RenderTexture source{ pixels }, output{ size, TextureFormat::R32_Float };
			for (const double radius : { 3.0, 7.0, 15.0 })
			{
				CAPTURE(size, phase, radius);
				RenderTextureOps::DualKawaseBlur(source, output, radius, work);
				Grid<float> actual;
				REQUIRE(output.read(actual));
				double energy = 0;
				for (const float value : actual)
				{
					energy += value;
				}
				// Approximate low-pass reduction need not conserve energy exactly at odd
				// dimensions, but a one-pixel shift must not multiply its contribution.
				CHECK(std::abs(energy - 1) < 0.06);
			}
		}
	}
}
