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
# include "../Siv3D/src/Siv3D/Renderer2D/ConstantBuffer2DCommands.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/IRenderer2D.hpp"
# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include <cstring>

namespace
{
	struct Parameters
	{
		Float4 value;
	};

	struct ConstantBufferSceneScope
	{
		ColorF background = Scene::GetBackground();
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		ScopedColorMul2D mul{ Palette::White };
		ScopedColorAdd2D add{ 0.0 };
		ScopedRenderStates2D blend{ BlendState::Default2D };

		ConstantBufferSceneScope() { Scene::SetBackground(Palette::Black); }
		~ConstantBufferSceneScope() { Scene::SetBackground(background); }
	};

	template <class Draw>
	Image CaptureConstants(Draw&& draw)
	{
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}

	PixelShader LoadConstantPS(const StringView entry)
	{
		return HLSL{ U"example/shader/hlsl/constant_buffer.hlsl", String{ entry } }
			| MSL{ U"example/shader/msl/constant_buffer.metal", String{ entry } };
	}

	void CheckImages(const Image& actual, const Image& expected)
	{
		REQUIRE(actual.size() == expected.size());
		int32 maximumDifference = 0;
		for (size_t i = 0; i < (actual.size_bytes() / sizeof(Color)); ++i)
		{
			const auto a = actual.data()[i], b = expected.data()[i];
			maximumDifference = Max(maximumDifference, Max(Abs(int32(a.r) - b.r), Abs(int32(a.g) - b.g), Abs(int32(a.b) - b.b)), Abs(int32(a.a) - b.a));
		}
		CHECK(maximumDifference <= 1);
	}
}

TEST_CASE("ConstantBuffer.storage_and_dirty")
{
	struct Small { uint32 bits[5]; };
	struct alignas(32) Aligned { uint32 bits[8]; };
	STATIC_REQUIRE(ConstantBuffer<Small>::Size == 32);
	STATIC_REQUIRE(ConstantBuffer<Small>::NumVectors == 2);
	STATIC_REQUIRE(ConstantBuffer<Aligned>::Alignment == 32);
	STATIC_REQUIRE(ConstantBuffer<std::array<Float4, 4096>>::Size == 65536);
	ConstantBuffer<Small> cb;
	CHECK(cb.isDirty());
	CHECK(std::as_const(cb)->bits[0] == 0);
	CHECK(cb._update_if_dirty());
	CHECK_FALSE(cb.isDirty());
	CHECK_FALSE(cb._update_if_dirty());
	cb->bits[4] = 0xFEDCBA98u;
	CHECK(cb.isDirty());
	CHECK(cb._update_if_dirty());
	CHECK_FALSE(cb.isDirty());
	cb = Small{ { 1, 2, 3, 4, 5 } };
	CHECK(cb.isDirty());
	const void* data = cb._data();
	ConstantBuffer<Small> moved{ std::move(cb) };
	CHECK(moved._data() == data);
	CHECK(std::as_const(moved)->bits[4] == 5);
	ConstantBuffer<Aligned> aligned;
	CHECK((reinterpret_cast<uintptr_t>(aligned._data()) % 32) == 0);
}

TEST_CASE("ConstantBuffer.snapshot_owns_bytes")
{
	ConstantBuffer2DCommands commands;
	{
		ConstantBuffer<Parameters> cb{ Parameters{ Float4{ 1, 2, 3, 4 } } };
		CHECK(commands.push(ShaderStage::Pixel, 2, cb._data(), cb.size()) == 0);
		cb->value = Float4{ 5, 6, 7, 8 };
		CHECK(commands.push(ShaderStage::Vertex, 13, cb._data(), cb.size()) == 1);
	}
	// Subsequent insertions grow the arena; records store offsets, not pointers.
	const std::array<uint32, 4096> integers{};
	commands.push(ShaderStage::Pixel, 3, integers.data(), sizeof(integers));
	Parameters first{}, second{};
	std::memcpy(&first, commands.data(commands.get(0)), sizeof(first));
	std::memcpy(&second, commands.data(commands.get(1)), sizeof(second));
	CHECK(first.value == Float4{ 1, 2, 3, 4 });
	CHECK(second.value == Float4{ 5, 6, 7, 8 });
	CHECK(commands.get(1).stage == ShaderStage::Vertex);
	CHECK(commands.get(1).slot == 13);
	commands.clear();
	CHECK(commands.push(ShaderStage::Pixel, 2, &second, sizeof(second)) == 0);
	CHECK(commands.get(0).offset == 0);
}

TEST_CASE("ConstantBuffer.invalid_slots")
{
	ConstantBuffer<Parameters> cb{};
	for (const uint32 slot : { 0u, 1u, 14u, 0xFFFFFFFFu })
	{
		CHECK_THROWS_AS(Graphics2D::SetPSConstantBuffer(slot, cb), Error);
		CHECK_THROWS_AS(Graphics2D::SetVSConstantBuffer(slot, cb), Error);
	}
}

TEST_CASE("ConstantBuffer.sample_shaders_snapshot_and_PMA")
{
	const ConstantBufferSceneScope scope;
	const PixelShader shape = LoadConstantPS(U"PS_TintShape");
	const PixelShader texture = LoadConstantPS(U"PS_TintTexture");
	const VertexShader offset = HLSL{ U"example/shader/hlsl/constant_buffer.hlsl", U"VS_Offset" }
		| MSL{ U"example/shader/msl/constant_buffer.metal", U"VS_Offset" };
	REQUIRE(shape);
	REQUIRE(texture);
	REQUIRE(offset);
	Image pixels{ 8, 8, Color{ 200, 100, 50, 128 } };
	pixels.premultiplyAlpha();
	const Texture image{ pixels };
	REQUIRE(image);
	REQUIRE(System::Update());
	const auto reference = CaptureConstants([&]
	{
		Rect{ 30, 20, 30, 30 }.draw(Palette::Red);
		Rect{ 80, 20, 30, 30 }.draw(Palette::Lime);
		Rect{ 130, 20, 30, 30 }.draw(Palette::Lime);
		image.resized(40, 40).draw(20, 80, ColorF{ 0.5, 1.0, 0.75, 0.5 });
		Rect{ 20, 140, 30, 30 }.draw(Palette::Blue);
	});
	for (int32 frame = 0; frame < 5; ++frame)
	{
		const auto actual = CaptureConstants([&]
		{
			{
				ConstantBuffer<Parameters> cb{ Parameters{ Float4{ 10, 0, 0, 0 } } };
				// One object supplies both stages, with independent snapshots.
				Graphics2D::SetVSConstantBuffer(2, cb);
				cb->value = Float4{ 1, 0, 0, 1 };
				Graphics2D::SetPSConstantBuffer(2, cb);
				const ScopedCustomShader2D shader{ offset, shape };
				Rect{ 20, 20, 30, 30 }.draw();
				cb->value = Float4{ 0, 1, 0, 1 };
				Graphics2D::SetPSConstantBuffer(2, cb);
				Rect{ 70, 20, 30, 30 }.draw();
				cb->value = Float4{ 0, 0, 1, 1 }; // No Set: the existing green snapshot remains.
				Rect{ 120, 20, 30, 30 }.draw();
			}
			{
				ConstantBuffer<Parameters> cb{ Parameters{ Float4{ 0.5f, 1, 0.75f, 0.5f } } };
				Graphics2D::SetPSConstantBuffer(2, cb);
			}
			{
				const ScopedCustomShader2D shader{ texture };
				image.resized(40, 40).draw(20, 80);
			}
			Rect{ 20, 140, 30, 30 }.draw(Palette::Blue);
		});
		CheckImages(actual, reference);
	}
}

TEST_CASE("ConstantBuffer.independent_slots_and_integer_payload")
{
	const ConstantBufferSceneScope scope;
# if SIV3D_PLATFORM(WINDOWS)
	const PixelShader ps = PixelShader::HLSL(std::string{ R"(
cbuffer A : register(b2) { uint4 a; }
cbuffer B : register(b13) { uint4 b; }
float4 PS() : SV_TARGET { return float4(float(a.x) / 255.0, float(b.x) / 255.0, 0, 1); }
)" }, U"PS");
# else
	const PixelShader ps = PixelShader::MSL(std::string{ R"(
#include <metal_stdlib>
using namespace metal;
fragment float4 PS(constant uint4& a [[buffer(2)]], constant uint4& b [[buffer(13)]])
{ return float4(float(a.x) / 255.0, float(b.x) / 255.0, 0, 1); }
)" }, U"PS");
# endif
	REQUIRE(ps);
	REQUIRE(System::Update());
	const auto reference = CaptureConstants([] { Rect{ 20, 20, 30, 30 }.draw(Color{ 64, 192, 0 }); });
	const auto actual = CaptureConstants([&]
	{
		ConstantBuffer<std::array<uint32, 4>> cb;
		(*cb)[0] = 64;
		Graphics2D::SetPSConstantBuffer(2, cb);
		(*cb)[0] = 192;
		Graphics2D::SetPSConstantBuffer(13, cb);
		const ScopedCustomShader2D shader{ ps };
		Rect{ 20, 20, 30, 30 }.draw();
	});
	CheckImages(actual, reference);
}

namespace
{
	template <size_t Count>
	void CheckLargeConstantBuffers()
	{
		const ConstantBufferSceneScope scope;
# if SIV3D_PLATFORM(WINDOWS)
		const std::string source = "cbuffer C : register(b2) { float4 values[" + std::to_string(Count)
			+ "]; } float4 PS() : SV_TARGET { return (values[0] + values[" + std::to_string(Count - 1) + "]) * 0.5; }";
		const PixelShader ps = PixelShader::HLSL(source, U"PS");
# else
		const std::string source = "#include <metal_stdlib>\nusing namespace metal;\nstruct C { float4 values[" + std::to_string(Count)
			+ "]; }; fragment float4 PS(constant C& c [[buffer(2)]]) { return (c.values[0] + c.values[" + std::to_string(Count - 1) + "]) * 0.5; }";
		const PixelShader ps = PixelShader::MSL(source, U"PS");
# endif
# if SIV3D_PLATFORM(WINDOWS)
		const std::string vertexSource = "cbuffer C : register(b13) { float4 values[" + std::to_string(Count) + R"(]; }
cbuffer V : register(b0) { row_major float2x4 transform; float4 colorMul; }
struct Input { float2 pos : POSITION; float2 uv : TEXCOORD0; float4 color : COLOR0; };
struct Output { float4 pos : SV_POSITION; float4 color : COLOR0; float2 uv : TEXCOORD0; };
Output VS(Input input)
{
    float2 p = input.pos + values[0].xy + values[)" + std::to_string(Count - 1) + R"(].xy;
    Output result;
    result.pos = float4(transform._13_14 + p.x * transform._11_12 + p.y * transform._21_22, 0, 1);
    result.color = input.color * colorMul;
    result.color.rgb *= result.color.a;
    result.uv = input.uv;
    return result;
})";
		const VertexShader vs = VertexShader::HLSL(vertexSource, U"VS");
# else
		const std::string vertexSource = "#include <metal_stdlib>\nusing namespace metal;\nstruct C { float4 values[" + std::to_string(Count) + R"(]; };
struct V { float2x4 transform; float4 colorMul; };
struct Input { float2 pos; float2 uv; float4 color; };
struct Output { float4 pos [[position]]; float4 color; float2 uv; };
vertex Output VS(uint id [[vertex_id]], constant Input* vertices [[buffer(0)]],
                 constant V& v [[buffer(1)]], constant C& c [[buffer(13)]])
{
    float2 p = vertices[id].pos + c.values[0].xy + c.values[)" + std::to_string(Count - 1) + R"(].xy;
    Output result;
    result.pos = float4(v.transform[0].zw + p.x * v.transform[0].xy + p.y * v.transform[1].xy, 0, 1);
    result.color = vertices[id].color * v.colorMul;
    result.color.rgb *= result.color.a;
    result.uv = vertices[id].uv;
    return result;
})";
		const VertexShader vs = VertexShader::MSL(vertexSource, U"VS");
# endif
		REQUIRE(vs);
		REQUIRE(ps);
		ConstantBuffer<std::array<Float4, Count>> cb;
		REQUIRE(System::Update());
		const auto reference = CaptureConstants([]
		{
			for (int32 i = 0; i < 24; ++i)
			{
				Rect{ (30 + i % 8 * 30), (20 + i / 8 * 30), 20, 20 }.draw((i % 2) ? Palette::Blue : Palette::Red);
			}
		});
		for (int32 frame = 0; frame < 5; ++frame)
		{
			const auto actual = CaptureConstants([&]
			{
				for (int32 i = 0; i < 24; ++i)
				{
					// A second encoder/flush in the same frame must not recycle prior upload data.
					if (i == 12) { SIV3D_ENGINE(Renderer2D)->flush(); }
					(*cb)[0] = (*cb)[Count - 1] = Float4{ 5, 0, 0, 0 };
					Graphics2D::SetVSConstantBuffer(13, cb);
					(*cb)[0] = (*cb)[Count - 1] = ((i % 2) ? Float4{ 0, 0, 1, 1 } : Float4{ 1, 0, 0, 1 });
					Graphics2D::SetPSConstantBuffer(2, cb);
					const ScopedCustomShader2D shader{ vs, ps };
					Rect{ (20 + i % 8 * 30), (20 + i / 8 * 30), 20, 20 }.draw();
				}
			});
			CheckImages(actual, reference);
		}
	}
}

TEST_CASE("ConstantBuffer.size_boundaries_and_flush")
{
	SECTION("4096 bytes") { CheckLargeConstantBuffers<256>(); }
	SECTION("4112 bytes") { CheckLargeConstantBuffers<257>(); }
	SECTION("65536 bytes") { CheckLargeConstantBuffers<4096>(); }
}
