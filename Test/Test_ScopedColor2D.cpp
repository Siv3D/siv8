//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "ImageDrawingTestHelper.hpp"
# include <type_traits>

namespace
{
	using Mul = ScopedColorMul2D;
	using Add = ScopedColorAdd2D;

	struct ColorReset
	{
		Mul mul{ Palette::White, Mul::Target::Set };
		Add add{ 0.0, Add::Target::Set };
	};
}

TEST_CASE("ScopedColor2D.nested_push_set_and_restore")
{
	const Float4 savedMul = Graphics2D::GetColorMul();
	const Float3 savedAdd = Graphics2D::GetColorAdd();
	{
		const Mul outerMul{ 0.5, 1.0, 0.25, 0.5, Mul::Target::Set };
		const Add outerAdd{ 0.125, -0.25, 0.5, Add::Target::Set };
		{
			const Mul innerMul{ ColorF{ 0.5, 0.25, 2.0, 0.5 } };
			const Add innerAdd{ ColorF{ 0.25, 0.5, -0.125, 0.75 } };
			CHECK(Graphics2D::GetColorMul() == Float4{ 0.25f, 0.25f, 0.5f, 0.25f });
			CHECK(Graphics2D::GetColorAdd() == Float3{ 0.375f, 0.25f, 0.375f });
			{
				const Mul leafMul{ 2.0, 2.0, 0.5, 0.5, Mul::Target::Push };
				const Add leafAdd{ 0.125, -0.5, 0.125, Add::Target::Push };
				CHECK(Graphics2D::GetColorMul() == Float4{ 0.5f, 0.5f, 0.25f, 0.125f });
				CHECK(Graphics2D::GetColorAdd() == Float3{ 0.5f, -0.25f, 0.5f });
				{
					// Neutral Push preserves the parent; Set isolates from it.
					const Mul neutralMul{ 1.0 };
					const Add neutralAdd{ 0.0 };
					CHECK(Graphics2D::GetColorMul() == Float4{ 0.5f, 0.5f, 0.25f, 0.125f });
					CHECK(Graphics2D::GetColorAdd() == Float3{ 0.5f, -0.25f, 0.5f });
					const Mul resetMul{ 1.0, Mul::Target::Set };
					CHECK(Graphics2D::GetColorAdd() == Float3{ 0.5f, -0.25f, 0.5f });
					const Add resetAdd{ 0.0, Add::Target::Set };
					CHECK(Graphics2D::GetColorMul() == Float4{ 1, 1, 1, 1 });
					CHECK(Graphics2D::GetColorAdd() == Float3{ 0, 0, 0 });
					const Mul pushedAfterSet{ 0.5, 0.5 };
					const Add addedAfterSet{ -0.25 };
					CHECK(Graphics2D::GetColorMul() == Float4{ 0.5f, 0.5f, 0.5f, 0.5f });
					CHECK(Graphics2D::GetColorAdd() == Float3{ -0.25f, -0.25f, -0.25f });
				}
				CHECK(Graphics2D::GetColorMul() == Float4{ 0.5f, 0.5f, 0.25f, 0.125f });
				CHECK(Graphics2D::GetColorAdd() == Float3{ 0.5f, -0.25f, 0.5f });
			}
			CHECK(Graphics2D::GetColorMul() == Float4{ 0.25f, 0.25f, 0.5f, 0.25f });
			CHECK(Graphics2D::GetColorAdd() == Float3{ 0.375f, 0.25f, 0.375f });
		}
		CHECK(Graphics2D::GetColorMul() == Float4{ 0.5f, 1, 0.25f, 0.5f });
		CHECK(Graphics2D::GetColorAdd() == Float3{ 0.125f, -0.25f, 0.5f });
	}
	CHECK(Graphics2D::GetColorMul() == savedMul);
	CHECK(Graphics2D::GetColorAdd() == savedAdd);
}

TEST_CASE("ScopedColor2D.constructor_targets")
{
	const Float4 baseMul{ 0.5f, 2, 0.25f, 0.5f };
	const Float3 baseAdd{ 0.125f, -0.25f, 0.5f };
	const Mul outerMul{ baseMul, Mul::Target::Set };
	const Add outerAdd{ baseAdd, Add::Target::Set };
	const auto checkMul = [&](const Float4 value, auto make)
	{
		for (const auto target : { Mul::Target::Push, Mul::Target::Set })
		{
			const auto scope = make(target);
			CHECK(Graphics2D::GetColorMul() == ((target == Mul::Target::Push) ? baseMul * value : value));
			CHECK(Graphics2D::GetColorAdd() == baseAdd);
		}
		CHECK(Graphics2D::GetColorMul() == baseMul);
	};
	const Float4 rgba{ 0.25f, 0.5f, 2, 0.25f };
	checkMul(rgba, [&](auto t) { return Mul{ ColorF{ rgba }, t }; });
	checkMul(rgba, [&](auto t) { return Mul{ rgba, t }; });
	checkMul(rgba, [&](auto t) { return Mul{ rgba.xyz(), 0.25, t }; });
	checkMul({ 0.25f, 0.5f, 2, 1 }, [&](auto t) { return Mul{ rgba.xyz(), t }; });
	checkMul({ 0.25f, 0.25f, 0.25f, 0.5f }, [](auto t) { return Mul{ 0.25, 0.5, t }; });
	checkMul({ 0.25f, 0.25f, 0.25f, 1 }, [](auto t) { return Mul{ 0.25, t }; });
	checkMul(rgba, [](auto t) { return Mul{ 0.25, 0.5, 2.0, 0.25, t }; });
	checkMul({ 0.25f, 0.5f, 2, 1 }, [](auto t) { return Mul{ 0.25, 0.5, 2.0, t }; });

	const auto checkAdd = [&](const Float3 value, auto make)
	{
		for (const auto target : { Add::Target::Push, Add::Target::Set })
		{
			const auto scope = make(target);
			CHECK(Graphics2D::GetColorAdd() == ((target == Add::Target::Push) ? baseAdd + value : value));
			CHECK(Graphics2D::GetColorMul() == baseMul);
		}
		CHECK(Graphics2D::GetColorAdd() == baseAdd);
	};
	checkAdd(rgba.xyz(), [&](auto t) { return Add{ ColorF{ rgba }, t }; });
	checkAdd(rgba.xyz(), [&](auto t) { return Add{ rgba, t }; });
	checkAdd(rgba.xyz(), [&](auto t) { return Add{ rgba.xyz(), t }; });
	checkAdd({ 0.25f, 0.25f, 0.25f }, [](auto t) { return Add{ 0.25, t }; });
	checkAdd(rgba.xyz(), [](auto t) { return Add{ 0.25, 0.5, 2.0, t }; });
}

TEST_CASE("ScopedColor2D.independent_order_and_boundaries")
{
	const ColorReset reset;
	for (const bool mulFirst : { false, true })
	{
		const auto drawScope = []
		{
			// Default vector overloads accumulate without scaling Add by Mul.
			const Mul mul{ Float3{ 2, 0.5f, 4 }, 0.5 };
			const Add add{ Float3{ -0.5f, 1, -0.25f } };
			CHECK(Graphics2D::GetColorMul() == Float4{ 1, 0.25f, 2, 0.25f });
			CHECK(Graphics2D::GetColorAdd() == Float3{ -0.25f, 1.25f, 0 });
		};
		if (mulFirst)
		{
			const Mul mul{ 0.5, 0.5 };
			const Add add{ 0.25 };
			drawScope();
		}
		else
		{
			const Add add{ 0.25 };
			const Mul mul{ 0.5, 0.5 };
			drawScope();
		}
	}
	const Float4 originalMul{ 0.3f, 0.7f, 1.1f, 0.4f };
	const Float3 originalAdd{ 0.3f, -0.7f, 1.1f };
	const Mul parentMul{ originalMul };
	const Add parentAdd{ originalAdd };
	{
		const Mul zero{ Float4{ 0, 0, 0, 0 } };
		const Add large{ Float4{ 1024, -1024, 0, 1 } };
		CHECK(Graphics2D::GetColorMul() == Float4{ 0, 0, 0, 0 });
		CHECK(Graphics2D::GetColorAdd().x > 1.0f);
		CHECK(Graphics2D::GetColorAdd().y < 0.0f);
	}
	// Restore the saved values exactly, including after multiplication by zero.
	CHECK(Graphics2D::GetColorMul() == originalMul);
	CHECK(Graphics2D::GetColorAdd() == originalAdd);
}

TEST_CASE("ScopedColor2D.move_and_unwind")
{
	static_assert(not std::is_copy_constructible_v<Mul>);
	static_assert(not std::is_copy_constructible_v<Add>);
	static_assert(std::is_nothrow_move_constructible_v<Mul>);
	static_assert(std::is_nothrow_move_constructible_v<Add>);
	const Mul parentMul{ 0.5, 0.5, Mul::Target::Set };
	const Add parentAdd{ 0.25, Add::Target::Set };
	Optional<Mul> mulOwner;
	Optional<Add> addOwner;
	{
		Mul mul{ 0.5, 0.5, 0.5 };
		Add add{ 0.25, 0.25, 0.25 };
		mulOwner.emplace(std::move(mul));
		addOwner.emplace(std::move(add));
	}
	CHECK(Graphics2D::GetColorMul() == Float4{ 0.25f, 0.25f, 0.25f, 0.5f });
	CHECK(Graphics2D::GetColorAdd() == Float3{ 0.5f, 0.5f, 0.5f });
	addOwner.reset();
	mulOwner.reset();
	{
		Mul inactiveMul;
		Add inactiveAdd;
		const Mul movedMul{ std::move(inactiveMul) };
		const Add movedAdd{ std::move(inactiveAdd) };
	}
	struct Unwind {};
	CHECK_THROWS_AS(([]
	{
		const ColorReset reset;
		throw Unwind{};
	}()), Unwind);
	CHECK(Graphics2D::GetColorMul() == Float4{ 0.5f, 0.5f, 0.5f, 0.5f });
	CHECK(Graphics2D::GetColorAdd() == Float3{ 0.25f, 0.25f, 0.25f });
}

TEST_CASE("ScopedColor2D.pma_drawing_and_restore")
{
	const SceneConfig oldConfig = Scene::GetConfig();
	const ColorF oldBackground = Scene::GetBackground();
	const ScopeExit restoreScene{ [&]
	{
		Scene::Configure(oldConfig);
		Scene::SetBackground(oldBackground);
	} };
	Scene::Configure(SceneConfig::Fixed({ 320, 240 }, SceneResolution::SceneSize));
	Scene::SetBackground(ColorF{ 0.125, 0.25, 0.375 });
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
	const ScopedViewport2D viewport{ none };
	const ScopedScissorRect2D scissor{ none };
	const ScopedRenderStates2D states{ BlendState::Default2D, RasterizerState::Default2D, SamplerState::ClampNearest };
	const ColorReset reset;
	Image pixels{ 2, 2, Color{ 128, 192, 64, 128 } };
	pixels[0][0] = Color{ 0, 0 };
	pixels.premultiplyAlpha();
	const Texture texture{ pixels, TextureDesc::NoMipmap };
	REQUIRE(texture);
	const auto draw = [&](const int32 column)
	{
		const int32 x = (10 + column * 50);
		texture.resized(32, 32).draw(x, 20, ColorF{ 0.5, 1.0, 0.75, 0.5 });
		Rect{ x, 70, 32, 32 }.draw(ColorF{ 0.5, 1.0, 0.75, 0.5 });
	};
	const auto capture = [&](auto render)
	{
		REQUIRE(System::Update());
		render();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	};
	// Flat explicit states provide the reference for the nested scopes below.
	const auto reference = capture([&]
	{
		const Float4 multipliers[]{ { 0.5f, 1, 0.25f, 0.5f }, { 0.25f, 0.5f, 0.25f, 0.25f },
			{ 1, 1, 1, 1 }, { 0.25f, 0.5f, 0.25f, 0.25f }, { 0.5f, 1, 0.25f, 0.5f }, { 1, 1, 1, 1 } };
		const Float3 additions[]{ { 0.125f, 0, -0.125f }, { 0.375f, -0.125f, 0 },
			{ 0, 0, 0 }, { 0.375f, -0.125f, 0 }, { 0.125f, 0, -0.125f }, { 0, 0, 0 } };
		for (int32 i = 0; i < 6; ++i)
		{
			const Mul mul{ multipliers[i], Mul::Target::Set };
			const Add add{ additions[i], Add::Target::Set };
			draw(i);
		}
	});
	REQUIRE(reference.size() == Size{ 320, 240 });
	CHECK(reference[80][120] != reference[0][0]);
	CHECK(reference[25][15] == reference[0][0]); // Transparent texels stay transparent under Add.
	for (const bool flush : { false, true })
	{
		const auto actual = capture([&]
		{
			{
				const Mul outerMul{ 0.5, 1.0, 0.25, 0.5 };
				const Add outerAdd{ 0.125, 0.0, -0.125 };
				draw(0);
				{
					const Add innerAdd{ 0.25, -0.125, 0.125 };
					const Mul innerMul{ 0.5, 0.5, 1.0, 0.5 };
					draw(1);
					if (flush) Graphics2D::Flush();
					{
						const ColorReset isolate;
						draw(2);
					}
					draw(3);
				}
				draw(4);
			}
			draw(5);
		});
		CHECK_THAT(actual, s3d::ImageDrawingTest::ImageEquals(reference, 1));
	}
}
