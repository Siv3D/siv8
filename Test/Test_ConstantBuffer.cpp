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
# include <optional>

# if SIV3D_PLATFORM(WINDOWS)
# include "../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/D3D11Renderer2DCommandManager.hpp"
# include <Siv3D/ConstantBuffer/D3D11/ConstantBuffer_D3D11.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# elif SIV3D_PLATFORM(MACOS)
# include "../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer2D/Metal/MetalRenderer2DCommandManager.hpp"
# endif

namespace
{
# if SIV3D_PLATFORM(WINDOWS)
	using ConstantBufferCommandManager = D3D11Renderer2DCommandManager;
	using ConstantBufferCommandType = D3D11Renderer2DCommandType;
# elif SIV3D_PLATFORM(MACOS)
	using ConstantBufferCommandManager = MetalRenderer2DCommandManager;
	using ConstantBufferCommandType = MetalRenderer2DCommandType;
# endif

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

		std::array<std::array<uint32, Graphics::ConstantBufferSlotCount>, 2> previous{};

		ConstantBufferSceneScope()
		{
			Scene::SetBackground(Palette::Black);
			for (uint32 stage = 0; stage < previous.size(); ++stage)
			{
				for (uint32 slot = 2; slot < Graphics::ConstantBufferSlotCount; ++slot)
				{
					previous[stage][slot] = SIV3D_ENGINE(Renderer2D)->beginConstantBufferScope(static_cast<ShaderStage>(stage), slot, nullptr, 0);
				}
			}
		}

		~ConstantBufferSceneScope()
		{
			for (uint32 stage = 0; stage < previous.size(); ++stage)
			{
				for (uint32 slot = 2; slot < Graphics::ConstantBufferSlotCount; ++slot)
				{
					SIV3D_ENGINE(Renderer2D)->endConstantBufferScope(static_cast<ShaderStage>(stage), slot, previous[stage][slot]);
				}
			}
			Scene::SetBackground(background);
		}
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
		commands.push(ShaderStage::Pixel, 2, cb._data(), cb.size());
		commands.flush();
		CHECK(commands.size() == 1);
		cb->value = Float4{ 5, 6, 7, 8 };
		commands.push(ShaderStage::Vertex, 13, cb._data(), cb.size());
	}
	commands.flush();
	CHECK(commands.size() == 2);
	// Growing the snapshot table must not invalidate previous values.
	const std::array<uint32, 4096> integers{};
	commands.push(ShaderStage::Pixel, 3, integers.data(), sizeof(integers));
	commands.flush();
	Parameters first{}, second{};
	std::memcpy(&first, commands.data(commands.get(0)), sizeof(first));
	std::memcpy(&second, commands.data(commands.get(1)), sizeof(second));
	CHECK(first.value == Float4{ 1, 2, 3, 4 });
	CHECK(second.value == Float4{ 5, 6, 7, 8 });
	CHECK(commands.get(1).stage == ShaderStage::Vertex);
	CHECK(commands.get(1).slot == 13);
	commands.reset();
	CHECK(commands.size() == 3);
	// Vertex slots precede pixel slots in the replay.
	std::memcpy(&first, commands.data(commands.get(1)), sizeof(first));
	CHECK(first.value == Float4{ 1, 2, 3, 4 });
	commands.push(ShaderStage::Pixel, 2, &second, sizeof(second));
	commands.flush();
	CHECK(commands.size() == 4);
}

TEST_CASE("ConstantBuffer.invalid_slots")
{
	ConstantBuffer<Parameters> cb{};
	for (const uint32 slot : { 0u, 1u, 14u, 0xFFFFFFFFu })
	{
		CHECK_THROWS_AS((ScopedVSConstantBuffer2D{ slot, cb }), Error);
		CHECK_THROWS_AS((ScopedPSConstantBuffer2D{ slot, cb }), Error);
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
				const ScopedVSConstantBuffer2D offsetConstants{ 2, cb };
				cb->value = Float4{ 1, 0, 0, 1 };
				const ScopedPSConstantBuffer2D redConstants{ 2, cb };
				const ScopedCustomShader2D shader{ offset, shape };
				Rect{ 20, 20, 30, 30 }.draw();
				cb->value = Float4{ 0, 1, 0, 1 };
				const ScopedPSConstantBuffer2D greenConstants{ 2, cb };
				Rect{ 70, 20, 30, 30 }.draw();
				cb->value = Float4{ 0, 0, 1, 1 }; // The existing green snapshot remains.
				Rect{ 120, 20, 30, 30 }.draw();
			}
			{
				// The source buffer is destroyed before the queued texture draw.
				const ScopedPSConstantBuffer2D tint{ 2,
					ConstantBuffer<Parameters>{ Parameters{ Float4{ 0.5f, 1, 0.75f, 0.5f } } } };
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
		const ScopedPSConstantBuffer2D firstSlot{ 2, cb };
		(*cb)[0] = 192;
		const ScopedPSConstantBuffer2D lastSlot{ 13, cb };
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
			Rect{ 30, 160, 20, 20 }.draw(Palette::Red);
			Rect{ 70, 160, 20, 20 }.draw(Palette::Blue);
		});
		const ScopedCustomShader2D shader{ vs, ps };
		(*cb)[0] = (*cb)[Count - 1] = Float4{ 5, 0, 0, 0 };
		const ScopedVSConstantBuffer2D outerOffset{ 13, cb };
		(*cb)[0] = (*cb)[Count - 1] = Float4{ 0, 0, 1, 1 };
		const ScopedPSConstantBuffer2D outerTint{ 2, cb };
		for (int32 frame = 0; frame < 5; ++frame)
		{
			const auto actual = CaptureConstants([&]
			{
				for (int32 i = 0; i < 24; ++i)
				{
					// A second encoder/flush in the same frame must not recycle prior upload data.
					if (i == 12) { Graphics2D::Flush(); }
					(*cb)[0] = (*cb)[Count - 1] = Float4{ 5, 0, 0, 0 };
					const ScopedVSConstantBuffer2D offset{ 13, cb };
					(*cb)[0] = (*cb)[Count - 1] = ((i % 2) ? Float4{ 0, 0, 1, 1 } : Float4{ 1, 0, 0, 1 });
					const ScopedPSConstantBuffer2D tint{ 2, cb };
					Rect{ (20 + i % 8 * 30), (20 + i / 8 * 30), 20, 20 }.draw();
				}
				{
					(*cb)[0] = (*cb)[Count - 1] = Float4{ 1, 0, 0, 1 };
					const ScopedPSConstantBuffer2D inner{ 2, cb };
					Graphics2D::Flush();
					Rect{ 20, 160, 20, 20 }.draw();
				}
				Graphics2D::Flush();
				// Both outer snapshots survive the flush without reconstructing their scopes.
				Rect{ 60, 160, 20, 20 }.draw();
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

TEST_CASE("ConstantBuffer.saved_values_and_storage_reuse")
{
	ConstantBuffer2DCommands commands;
	const Parameters red{ Float4{ 1, 0, 0, 1 } }, green{ Float4{ 0, 1, 0, 1 } };
	const uint32 unset = commands.save(ShaderStage::Pixel, 2);
	CHECK(unset == 0);
	commands.push(ShaderStage::Pixel, 2, &red, sizeof(red));
	const uint32 saved = commands.save(ShaderStage::Pixel, 2);
	commands.flush();
	const void* redStorage = commands.data(commands.get(0));
	commands.push(ShaderStage::Pixel, 2, &green, sizeof(green));
	commands.flush();
	for (int32 i = 0; i < 4; ++i)
	{
		commands.reset();
		CHECK(commands.size() == 1);
		Parameters value;
		std::memcpy(&value, commands.data(commands.get(0)), sizeof(value));
		CHECK(value.value == green.value);
	}
	const uint32 restored = static_cast<uint32>(commands.size());
	commands.restore(ShaderStage::Pixel, 2, saved);
	commands.flush();
	CHECK(commands.data(commands.get(restored)) == redStorage);
	Parameters value;
	std::memcpy(&value, commands.data(commands.get(restored)), sizeof(value));
	CHECK(value.value == red.value);
	const uint32 cleared = static_cast<uint32>(commands.size());
	commands.restore(ShaderStage::Pixel, 2, unset);
	commands.flush();
	CHECK(commands.get(cleared).size == 0);
	CHECK(commands.data(commands.get(cleared)) == nullptr);
	commands.reset();
	CHECK(commands.size() == 0);

	// Steady-state Set/flush reuses two blocks, without growing per frame.
	std::array<uint32, 2> snapshots{};
	for (int32 i = 0; i < 100; ++i)
	{
		const uint32 index = static_cast<uint32>(commands.size());
		commands.push(ShaderStage::Vertex, 13, &red, sizeof(red));
		commands.flush();
		const uint32 snapshot = commands.get(index).snapshot;
		if (i < 2) { snapshots[i] = snapshot; }
		else { CHECK((snapshot == snapshots[0] || snapshot == snapshots[1])); }
		commands.reset();
	}
}

TEST_CASE("ConstantBuffer.persists_across_flush_and_frames")
{
	const ConstantBufferSceneScope scope;
	const PixelShader ps = LoadConstantPS(U"PS_TintShape");
	const VertexShader vs = HLSL{ U"example/shader/hlsl/constant_buffer.hlsl", U"VS_Offset" }
		| MSL{ U"example/shader/msl/constant_buffer.metal", U"VS_Offset" };
	REQUIRE(vs);
	REQUIRE(ps);
	const auto oldVS = Graphics2D::GetCustomVertexShader();
	const auto oldPS = Graphics2D::GetCustomPixelShader();
	REQUIRE(System::Update());
	const auto reference = CaptureConstants([]
	{
		Rect{ 30, 20, 30, 30 }.draw(Palette::Red);
		Rect{ 80, 20, 30, 30 }.draw(Palette::Red);
	});
	const ScopedVSConstantBuffer2D offset{ 2,
		ConstantBuffer<Parameters>{ Parameters{ Float4{ 10, 0, 0, 0 } } } };
	const ScopedPSConstantBuffer2D tint{ 2,
		ConstantBuffer<Parameters>{ Parameters{ Float4{ 1, 0, 0, 1 } } } };
	{
		const ScopedCustomShader2D shader{ vs, ps };
		for (int32 frame = 0; frame < 5; ++frame)
		{
			const auto actual = CaptureConstants([&]
			{
				Rect{ 20, 20, 30, 30 }.draw();
				Graphics2D::Flush();
				Graphics2D::Flush(); // No draw between flushes.
				CHECK(Graphics2D::GetCustomVertexShader()->id() == vs.id());
				CHECK(Graphics2D::GetCustomPixelShader()->id() == ps.id());
				Rect{ 70, 20, 30, 30 }.draw();
			});
			CheckImages(actual, reference);
			CHECK(Graphics2D::GetCustomVertexShader()->id() == vs.id());
			CHECK(Graphics2D::GetCustomPixelShader()->id() == ps.id());
		}
	}
	CHECK(Graphics2D::GetCustomVertexShader() == oldVS);
	CHECK(Graphics2D::GetCustomPixelShader() == oldPS);
}

TEST_CASE("ConstantBuffer.scopes_restore_and_move")
{
	STATIC_REQUIRE(std::is_nothrow_move_constructible_v<ScopedPSConstantBuffer2D>);
	STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<ScopedPSConstantBuffer2D>);
	const ConstantBufferSceneScope scope;
	const PixelShader ps = LoadConstantPS(U"PS_TintShape");
	const VertexShader vs = HLSL{ U"example/shader/hlsl/constant_buffer.hlsl", U"VS_Offset" }
		| MSL{ U"example/shader/msl/constant_buffer.metal", U"VS_Offset" };
	REQUIRE(ps);
	REQUIRE(vs);
	REQUIRE(System::Update());
	const auto reference = CaptureConstants([]
	{
		Rect{ 30, 20, 30, 30 }.draw(Palette::Red);
		Rect{ 90, 20, 30, 30 }.draw(Palette::Lime);
		Rect{ 130, 20, 30, 30 }.draw(Palette::Lime);
		Rect{ 180, 20, 30, 30 }.draw(Palette::Blue);
		Rect{ 230, 20, 30, 30 }.draw(Palette::Red);
	});
	const auto actual = CaptureConstants([&]
	{
		ConstantBuffer<Parameters> cb{ Parameters{ Float4{ 10, 0, 0, 0 } } };
		const ScopedCustomShader2D shader{ vs, ps };
		const ScopedVSConstantBuffer2D offset{ 2, cb };
		cb->value = Float4{ 1, 0, 0, 1 };
		const ScopedPSConstantBuffer2D tint{ 2, cb };
		{
			const ScopedVSConstantBuffer2D inactiveVS;
			ScopedPSConstantBuffer2D inactivePS;
			const ScopedPSConstantBuffer2D movedInactive{ std::move(inactivePS) };
		}
		Rect{ 20, 20, 30, 30 }.draw();
		{
			std::optional<ScopedPSConstantBuffer2D> moved;
			{
				cb->value = Float4{ 0, 1, 0, 1 };
				ScopedPSConstantBuffer2D inner{ 2, cb };
				moved.emplace(std::move(inner));
			}
			{
				cb->value = Float4{ 20, 0, 0, 0 };
				const ScopedVSConstantBuffer2D innerOffset{ 2, cb };
				Rect{ 70, 20, 30, 30 }.draw();
			}
			Graphics2D::Flush();
			Rect{ 120, 20, 30, 30 }.draw();
			{
				cb->value = Float4{ 0, 0, 1, 1 };
				const ScopedPSConstantBuffer2D blue{ 2, cb };
				Rect{ 170, 20, 30, 30 }.draw();
				Graphics2D::Flush();
			}
			moved.reset();
			Graphics2D::Flush();
		}
		Rect{ 220, 20, 30, 30 }.draw();
	});
	CheckImages(actual, reference);
	// Both outer scopes restore the originally unset slots, including after a flush.
	for (const ShaderStage stage : { ShaderStage::Vertex, ShaderStage::Pixel })
	{
		const uint32 previous = SIV3D_ENGINE(Renderer2D)->beginConstantBufferScope(stage, 2, nullptr, 0);
		CHECK(previous == 0);
		SIV3D_ENGINE(Renderer2D)->endConstantBufferScope(stage, 2, previous);
	}
	Graphics2D::Flush();
}

TEST_CASE("ConstantBuffer.scope_spans_frames_and_restores_snapshot")
{
	const ConstantBufferSceneScope scope;
	const PixelShader ps = LoadConstantPS(U"PS_TintShape");
	REQUIRE(ps);
	REQUIRE(System::Update());
	const auto red = CaptureConstants([] { Rect{ 20, 20, 30, 30 }.draw(Palette::Red); });
	const auto green = CaptureConstants([] { Rect{ 20, 20, 30, 30 }.draw(Palette::Lime); });
	ConstantBuffer<Parameters> cb{ Parameters{ Float4{ 1, 0, 0, 1 } } };
	const ScopedCustomShader2D shader{ ps };
	const ScopedPSConstantBuffer2D outer{ 2, cb };
	{
		cb->value = Float4{ 0, 1, 0, 1 };
		const ScopedPSConstantBuffer2D inner{ 2, cb };
		cb->value = Float4{ 0, 0, 1, 1 };
		for (int32 frame = 0; frame < 4; ++frame)
		{
			CheckImages(CaptureConstants([] { Rect{ 20, 20, 30, 30 }.draw(); }), green);
		}
	}
	CheckImages(CaptureConstants([] { Rect{ 20, 20, 30, 30 }.draw(); }), red);
}

# if SIV3D_PLATFORM(WINDOWS) || SIV3D_PLATFORM(MACOS)

TEST_CASE("ConstantBuffer.commands_repeated_scopes")
{
	for (const ShaderStage stage : { ShaderStage::Vertex, ShaderStage::Pixel })
	{
		for (const bool bound : { false, true })
		{
			ConstantBufferCommandManager manager;
			const Parameters outer{ Float4{ -1, 0, 0, 0 } };
			if (bound) { manager.pushConstantBuffer(stage, 2, &outer, sizeof(outer)); }
			manager.flush();
			const size_t firstCommand = manager.getCommands().size();
			for (int32 i = 0; i < 100; ++i)
			{
				const Parameters value{ Float4{ static_cast<float>(i), 0, 0, 0 } };
				const uint32 previous = manager.beginConstantBufferScope(stage, 2, &value, sizeof(value));
				manager.pushDraw(6);
				manager.endConstantBufferScope(stage, 2, previous);
			}
			manager.flush();

			uint32 updates = 0, restores = 0, draws = 0;
			float current = -1;
			for (size_t i = firstCommand; i < manager.getCommands().size(); ++i)
			{
				const auto& command = manager.getCommands()[i];
				if (command.type == ConstantBufferCommandType::SetConstantBuffer)
				{
					const auto& buffers = manager.getConstantBuffers();
					const auto& cb = buffers.get(command.index);
					REQUIRE(cb.stage == stage);
					REQUIRE(cb.slot == 2);
					if (cb.size)
					{
						Parameters value;
						std::memcpy(&value, buffers.data(cb), sizeof(value));
						current = value.value.x;
					}
					else { current = -1; }
					if (current < 0) { ++restores; }
					else { ++updates; }
				}
				else if (command.type == ConstantBufferCommandType::Draw)
				{
					CHECK(current == static_cast<float>(draws));
					CHECK(manager.getDraw(command.index).indexCount == 6);
					++draws;
				}
			}
			CHECK(draws == 100);
			CHECK(updates == 100);
			// Only the final restoration is needed; none belongs between iterations.
			CHECK(restores == 1);
		}
	}
}

TEST_CASE("ConstantBuffer.commands_empty_scopes_preserve_batch")
{
	for (const ShaderStage stage : { ShaderStage::Vertex, ShaderStage::Pixel })
	{
		for (const bool bound : { false, true })
		{
			ConstantBufferCommandManager manager;
			const Parameters outer{ Float4{ 1, 0, 0, 1 } }, inner{ Float4{ 0, 1, 0, 1 } };
			if (bound) { manager.pushConstantBuffer(stage, 2, &outer, sizeof(outer)); }
			manager.pushDraw(6);
			for (int32 i = 0; i < 100; ++i)
			{
				const uint32 previous = manager.beginConstantBufferScope(stage, 2, &inner, sizeof(inner));
				manager.endConstantBufferScope(stage, 2, previous);
				manager.pushDraw(6);
			}
			manager.flush();
			uint32 draws = 0;
			for (const auto& command : manager.getCommands())
			{
				if (command.type == ConstantBufferCommandType::Draw)
				{
					++draws;
					CHECK(manager.getDraw(command.index).indexCount == 606);
				}
			}
			CHECK(draws == 1);
			CHECK(manager.getConstantBuffers().size() == (bound ? 1 : 0));
		}
	}
}

TEST_CASE("ConstantBuffer.commands_independent_pending_slots_and_reset")
{
	ConstantBufferCommandManager manager;
	const Parameters red{ Float4{ 1, 0, 0, 1 } }, green{ Float4{ 0, 1, 0, 1 } };
	manager.pushConstantBuffer(ShaderStage::Vertex, 2, &red, sizeof(red));
	manager.pushConstantBuffer(ShaderStage::Pixel, 2, &red, sizeof(red));
	manager.pushConstantBuffer(ShaderStage::Pixel, 13, &red, sizeof(red));
	manager.pushDraw(6);
	// Cancelling one slot's change must leave other pending slots intact.
	const uint32 previous = manager.beginConstantBufferScope(ShaderStage::Vertex, 2, &green, sizeof(green));
	manager.pushConstantBuffer(ShaderStage::Pixel, 13, &green, sizeof(green));
	manager.endConstantBufferScope(ShaderStage::Vertex, 2, previous);
	manager.pushDraw(6);
	manager.pushConstantBuffer(ShaderStage::Pixel, 2, nullptr, 0);
	manager.pushDraw(6);
	manager.flush();

	std::array<std::array<Float4, Graphics::ConstantBufferSlotCount>, 2> values{};
	uint32 draws = 0, clears = 0;
	for (const auto& command : manager.getCommands())
	{
		if (command.type == ConstantBufferCommandType::SetConstantBuffer)
		{
			const auto& buffers = manager.getConstantBuffers();
			const auto& cb = buffers.get(command.index);
			Parameters value{};
			if (cb.size) { std::memcpy(&value, buffers.data(cb), sizeof(value)); }
			else { ++clears; }
			values[FromEnum(cb.stage)][cb.slot] = value.value;
		}
		else if (command.type == ConstantBufferCommandType::Draw)
		{
			CHECK(values[FromEnum(ShaderStage::Vertex)][2] == red.value);
			CHECK(values[FromEnum(ShaderStage::Pixel)][2] == ((draws < 2) ? red.value : Float4{}));
			CHECK(values[FromEnum(ShaderStage::Pixel)][13] == ((draws == 0) ? red.value : green.value));
			++draws;
		}
	}
	CHECK(draws == 3);
	CHECK(clears == 1);
	CHECK(manager.getConstantBuffers().size() == 5);

	// An explicit command flush commits a reset even without a following draw.
	manager.pushConstantBuffer(ShaderStage::Vertex, 2, nullptr, 0);
	manager.flush();
	CHECK(manager.getConstantBuffers().size() == 6);
	CHECK(manager.getConstantBuffers().get(5).size == 0);
	manager.reset();
	// Only PS slot 13 is still bound after the execution boundary.
	REQUIRE(manager.getConstantBuffers().size() == 1);
	CHECK(manager.getConstantBuffers().get(0).stage == ShaderStage::Pixel);
	CHECK(manager.getConstantBuffers().get(0).slot == 13);
	manager.pushDraw(6);
	manager.flush();
	CHECK(manager.getConstantBuffers().size() == 1);
}

# endif

TEST_CASE("ConstantBuffer.repeated_scopes_render_snapshots")
{
	const ConstantBufferSceneScope scope;
	const PixelShader ps = LoadConstantPS(U"PS_TintShape");
	const VertexShader vs = HLSL{ U"example/shader/hlsl/constant_buffer.hlsl", U"VS_Offset" }
		| MSL{ U"example/shader/msl/constant_buffer.metal", U"VS_Offset" };
	REQUIRE(vs);
	REQUIRE(ps);
	REQUIRE(System::Update());
	const auto reference = CaptureConstants([]
	{
		for (int32 i = 0; i < 24; ++i)
		{
			Rect{ (20 + i % 8 * 30 + i % 3), (20 + i / 8 * 30), 20, 20 }.draw((i % 2) ? Palette::Lime : Palette::Red);
		}
		Rect{ 20, 130, 20, 20 }.draw(Palette::Blue);
	});
	ConstantBuffer<Parameters> cb;
	for (int32 frame = 0; frame < 4; ++frame)
	{
		const auto actual = CaptureConstants([&]
		{
			for (int32 i = 0; i < 24; ++i)
			{
				const ScopedCustomShader2D shader{ vs, ps };
				cb->value = Float4{ static_cast<float>(i % 3), 0, 0, 0 };
				const ScopedVSConstantBuffer2D vsConstants{ 2, cb };
				cb->value = ((i % 2) ? Float4{ 0, 1, 0, 1 } : Float4{ 1, 0, 0, 1 });
				const ScopedPSConstantBuffer2D psConstants{ 2, cb };
				cb->value = Float4{ 0, 0, 1, 1 }; // Updating the source alone must not affect either snapshot.
				Rect{ (20 + i % 8 * 30), (20 + i / 8 * 30), 20, 20 }.draw();
			}
			Rect{ 20, 130, 20, 20 }.draw(Palette::Blue);
		});
		CheckImages(actual, reference);
	}
}

# if SIV3D_PLATFORM(WINDOWS)

namespace
{
	struct D3D11ConstantBufferBindings
	{
		std::array<ID3D11Buffer*, Graphics::ConstantBufferSlotCount> vs{}, ps{};

		explicit D3D11ConstantBufferBindings(ID3D11DeviceContext* context)
		{
			context->VSGetConstantBuffers(0, Graphics::ConstantBufferSlotCount, vs.data());
			context->PSGetConstantBuffers(0, Graphics::ConstantBufferSlotCount, ps.data());
		}

		~D3D11ConstantBufferBindings()
		{
			for (auto* buffer : vs) { if (buffer) { buffer->Release(); } }
			for (auto* buffer : ps) { if (buffer) { buffer->Release(); } }
		}

		D3D11ConstantBufferBindings(const D3D11ConstantBufferBindings&) = delete;
		D3D11ConstantBufferBindings& operator =(const D3D11ConstantBufferBindings&) = delete;

		void restore(ID3D11DeviceContext* context) const
		{
			context->VSSetConstantBuffers(0, Graphics::ConstantBufferSlotCount, vs.data());
			context->PSSetConstantBuffers(0, Graphics::ConstantBufferSlotCount, ps.data());
		}
	};
}

TEST_CASE("ConstantBuffer.external_D3D11_bindings")
{
	const ConstantBufferSceneScope scope;
	const PixelShader ps = LoadConstantPS(U"PS_TintShape");
	const VertexShader vs = HLSL{ U"example/shader/hlsl/constant_buffer.hlsl", U"VS_Offset" };
	REQUIRE(vs);
	REQUIRE(ps);
	REQUIRE(System::Update());
	const auto reference = CaptureConstants([]
	{
		Rect{ 30, 20, 30, 30 }.draw(Palette::Red);
		Rect{ 80, 20, 30, 30 }.draw(Palette::Red);
		Rect{ 20, 80, 100, 50 }.draw(Pattern::PolkaDot{});
	});
	Graphics2D::Flush();
	auto* context = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getDevice().getContext();
	const D3D11ConstantBufferBindings original{ context };
	const ScopeExit restoreNative{ [&] { original.restore(context); } };

	ConstantBuffer<std::array<Float4, 4096>> foreign;
	foreign->fill(Float4{ 17, 19, 23, 29 });
	REQUIRE(foreign._update_if_dirty());
	auto* foreignBuffer = static_cast<ConstantBuffer_D3D11*>(foreign._base())->getBuffer();
	REQUIRE(foreignBuffer);
	std::array<ID3D11Buffer*, Graphics::ConstantBufferSlotCount> foreignBindings;
	foreignBindings.fill(foreignBuffer);

	std::array<std::optional<ScopedVSConstantBuffer2D>, Graphics::ConstantBufferSlotCount> vsConstants;
	std::array<std::optional<ScopedPSConstantBuffer2D>, Graphics::ConstantBufferSlotCount> psConstants;
	{
		ConstantBuffer<Parameters> cb;
		for (uint32 slot = 2; slot < Graphics::ConstantBufferSlotCount; ++slot)
		{
			if (slot == 5) { continue; } // A slot that has never been set by this renderer.
			cb->value = Float4{ 10, 0, 0, 0 };
			vsConstants[slot].emplace(slot, cb);
			cb->value = Float4{ 1, 0, 0, 1 };
			psConstants[slot].emplace(slot, cb);
		}
	} // Only the renderer's snapshots remain.

	for (uint32 phase = 0; phase < 3; ++phase)
	{
		// Cover persistent scopes, ended scopes, and an entirely unset user range.
		for (uint32 slot = 2; slot < Graphics::ConstantBufferSlotCount; ++slot)
		{
			if ((phase == 2) || ((phase == 1) && (slot % 2)))
			{
				vsConstants[slot].reset();
				psConstants[slot].reset();
			}
		}
		Graphics2D::Flush();
		const D3D11ConstantBufferBindings expected{ context };
		REQUIRE(expected.vs[0]);
		REQUIRE(expected.ps[0]);
		REQUIRE(expected.ps[1]);
		CHECK(expected.vs[1] == nullptr);

		for (int32 frame = 0; frame < 3; ++frame)
		{
			for (int32 pass = 0; pass < 2; ++pass)
			{
				INFO("phase=" << phase << ", frame=" << frame << ", pass=" << pass);
				// Simulate another renderer without informing Renderer2D or its state tracker.
				context->VSSetConstantBuffers(0, Graphics::ConstantBufferSlotCount, foreignBindings.data());
				context->PSSetConstantBuffers(0, Graphics::ConstantBufferSlotCount, foreignBindings.data());
				// No scope owns slot 5; the execution boundary must clear its foreign binding.

				if (phase < 2)
				{
					const ScopedCustomShader2D shader{ vs, ps };
					Rect{ (20 + pass * 50), 20, 30, 30 }.draw();
				}
				else
				{
					Rect{ (30 + pass * 50), 20, 30, 30 }.draw(Palette::Red);
				}
				if (pass == 1) { Rect{ 20, 80, 100, 50 }.draw(Pattern::PolkaDot{}); }
				Graphics2D::Flush();

				const D3D11ConstantBufferBindings actual{ context };
				for (uint32 slot = 0; slot < Graphics::ConstantBufferSlotCount; ++slot)
				{
					INFO("slot=" << slot);
					CHECK(actual.vs[slot] == expected.vs[slot]);
					CHECK(actual.ps[slot] == expected.ps[slot]);
					if ((slot >= 2) && ((slot == 5) || (phase == 2) || ((phase == 1) && (slot % 2))))
					{
						CHECK(actual.vs[slot] == nullptr);
						CHECK(actual.ps[slot] == nullptr);
					}
				}
			}
			ScreenCapture::RequestCurrentFrame();
			REQUIRE(System::Update());
			REQUIRE(ScreenCapture::HasNewFrame());
			CheckImages(ScreenCapture::GetFrame(), reference);
		}
	}
}

# endif
