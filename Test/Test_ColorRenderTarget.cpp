//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <type_traits>

namespace s3d
{
	static_assert(not std::is_base_of_v<Texture, ColorRenderTarget>);
	static_assert(std::is_convertible_v<RenderTexture, ColorRenderTarget>);
	static_assert(std::is_convertible_v<MSRenderTexture, ColorRenderTarget>);
	static_assert(not std::is_constructible_v<ColorRenderTarget, Texture>);
	static_assert(not std::is_constructible_v<ColorRenderTarget, DynamicTexture>);
	static_assert(not std::is_convertible_v<ColorRenderTarget, Texture>);
	static_assert(std::is_same_v<decltype(std::declval<const ColorRenderTarget&>().getTexture()), Texture>);
	static_assert(std::is_copy_constructible_v<ColorRenderTarget> && std::is_copy_assignable_v<ColorRenderTarget>);
	static_assert(std::is_move_constructible_v<ColorRenderTarget> && std::is_move_assignable_v<ColorRenderTarget>);

	static_assert(std::is_constructible_v<ScopedRenderTarget2D, RenderTexture>);
	static_assert(std::is_constructible_v<ScopedRenderTarget2D, MSRenderTexture>);
	static_assert(std::is_constructible_v<ScopedRenderTarget2D, ColorRenderTarget>);
	static_assert(std::is_constructible_v<ScopedRenderTarget2D, Optional<RenderTexture>>);
	static_assert(std::is_constructible_v<ScopedRenderTarget2D, Optional<MSRenderTexture>>);
	static_assert(std::is_constructible_v<ScopedRenderTarget2D, Optional<ColorRenderTarget>>);
	static_assert(std::is_constructible_v<ScopedRenderTarget2D, None_t>);
	static_assert(not std::is_constructible_v<ScopedRenderTarget2D, Texture>);
	static_assert(not std::is_constructible_v<ScopedRenderTarget2D, Optional<Texture>>);

	namespace
	{
		struct DrawState
		{
			const ScopedRenderTarget2D scene{ none };
			const ScopedViewport2D viewport{ none };
			const ScopedScissorRect2D scissor{ none };
			const ScopedCustomShader2D shaders{ VertexShader{}, PixelShader{} };
			const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
			const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
			const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
			const ScopedRenderStates2D blend{ BlendState::Opaque };
			const ScopedColorMul2D mul{ ColorF{ 1.0 }, ScopedColorMul2D::Target::Set };
			const ScopedColorAdd2D add{ ColorF{ 0.0, 0.0 }, ScopedColorAdd2D::Target::Set };
		};

		template <class T>
		Image Read(const T& resource, const uint32 mipLevel = 0)
		{
			Image image;
			REQUIRE(resource.readAsImage(image, mipLevel));
			return image;
		}

		void DrawInto(const ColorRenderTarget& target)
		{
			const ScopedRenderTarget2D scoped{ target };
			target.clear(Palette::Black);
			Rect{ target.size() }.draw(Palette::Red);
			Graphics2D::Flush();
			target.resolve();
			target.generateMips();
		}
	}

	TEST_CASE("ColorRenderTarget.empty_and_invalid_target")
	{
		const DrawState state;
		const RenderTexture single{ Size{ 8, 8 }, Palette::Black };
		const ScopedRenderTarget2D outer{ single };
		const ScopedViewport2D viewport{ Rect{ 1, 2, 4, 5 } };
		for (const ColorRenderTarget target : { ColorRenderTarget{}, ColorRenderTarget{ RenderTexture{} }, ColorRenderTarget{ MSRenderTexture{} } })
		{
			CHECK(target.isEmpty());
			CHECK_FALSE(target);
			CHECK(target.id() == Texture::IDType::Null());
			CHECK(target.getMSAASampleCount() == MSAASampleCount::NoMSAA);
			CHECK_FALSE(target.getTexture());
			CHECK(std::addressof(target.clear(Palette::Red)) == std::addressof(target));
			CHECK_NOTHROW(target.resolve());
			CHECK_NOTHROW(target.generateMips());
			CHECK_THROWS_AS(ScopedRenderTarget2D{ target }, Error);
			CHECK_THROWS_AS(ScopedRenderTarget2D{ Optional<ColorRenderTarget>{ target } }, Error);
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == single.id());
			CHECK(Graphics2D::GetViewport() == Rect{ 1, 2, 4, 5 });
		}
		CHECK(ColorRenderTarget{} == ColorRenderTarget{ RenderTexture{} });
		CHECK(Read(single) == Image{ 8, 8, Palette::Black });
	}

	TEST_CASE("ColorRenderTarget.metadata_sharing_copy_move_and_swap")
	{
		const DrawState state;
		RenderTexture single{ Size{ 8, 4 }, Palette::Red, TextureFormat::R8G8B8A8_Unorm_SRGB };
		MSRenderTexture ms{ Size{ 7, 3 }, Palette::Blue, MSAASampleCount::X2MSAA, TextureFormat::R16G16B16A16_Float };
		REQUIRE(single);
		REQUIRE(ms);
		ColorRenderTarget a = single, b = ms;
		CHECK(a.id() == single.id());
		CHECK(b.id() == ms.id());
		CHECK(a.size() == single.size());
		CHECK(b.size() == ms.size());
		CHECK(a.format() == single.format());
		CHECK(b.format() == ms.format());
		CHECK(a.getMSAASampleCount() == MSAASampleCount::NoMSAA);
		CHECK(b.getMSAASampleCount() == MSAASampleCount::X2MSAA);
		CHECK(a != b);
		const auto copy = a;
		ColorRenderTarget assigned;
		assigned = copy;
		ColorRenderTarget moved{ std::move(assigned) };
		CHECK(moved == a);
		assigned = std::move(moved);
		CHECK(assigned == copy);
		swap(a, b);
		CHECK(b == copy);
		CHECK(a.getMSAASampleCount() == MSAASampleCount::X2MSAA);
		CHECK(b.getMSAASampleCount() == MSAASampleCount::NoMSAA);
		a.clear(ColorF{ 2.0, 0.5, 4.0, 1.0 }).resolve();
		Grid<std::array<HalfFloat, 4>> pixels;
		REQUIRE(ms.read(pixels));
		CHECK(float(pixels[0][0][0]) == 2.0f);
		CHECK(float(pixels[0][0][2]) == 4.0f);
		const Texture sampled = b.getTexture();
		const auto msID = ms.id();
		single.release();
		ms.release();
		CHECK(a.id() == msID);
		CHECK(a.getTexture());
		b.release();
		CHECK_FALSE(b);
		CHECK(copy);
		CHECK(sampled);
		const RenderTexture output{ Size{ 8, 4 } };
		{
			const ScopedRenderTarget2D target{ output };
			sampled.draw();
		}
		CHECK(Read(output) == Image{ 8, 4, Palette::Red });
	}

	TEST_CASE("ColorRenderTarget.getTexture_retains_resource_after_all_targets_released")
	{
		const DrawState state;
		for (const bool multisample : { false, true })
		{
			CAPTURE(multisample);
			const Texture sampled = [&]() -> Texture
			{
				ColorRenderTarget target = multisample
					? ColorRenderTarget{ MSRenderTexture{ Size{ 8, 8 }, Palette::Blue } }
					: ColorRenderTarget{ RenderTexture{ Size{ 8, 8 }, Palette::Blue } };
				target.clear(Palette::Green).resolve();
				Texture result = target.getTexture();
				target.release();
				return result;
			}();
			const RenderTexture output{ Size{ 8, 8 }, Palette::Black };
			{
				const ScopedRenderTarget2D scope{ output };
				sampled.draw();
			}
			CHECK(Read(output) == Image{ 8, 8, Palette::Green });
		}
	}

	TEST_CASE("ColorRenderTarget.common_operations_and_resolved_mips")
	{
		const DrawState state;
		const auto check = [](const auto& resource)
		{
			const ColorRenderTarget target = resource;
			const bool ms = (target.getMSAASampleCount() != MSAASampleCount::NoMSAA);
			CAPTURE(ms, target.getMSAASampleCount());
			target.clear(Palette::Red);
			CHECK(Read(resource) == Image{ 8, 8, ms ? Palette::Blue : Palette::Red });
			CHECK(Read(resource, 1) == Image{ 4, 4, Palette::Blue });
			target.generateMips(); // Never resolves MSAA implicitly.
			CHECK(Read(resource, 1) == Image{ 4, 4, ms ? Palette::Blue : Palette::Red });
			target.resolve();
			CHECK(Read(resource) == Image{ 8, 8, Palette::Red });
			CHECK(Read(resource, 1) == Image{ 4, 4, ms ? Palette::Blue : Palette::Red });
			target.generateMips();
			CHECK(Read(resource, 1) == Image{ 4, 4, Palette::Red });
			{
				const ScopedRenderTarget2D sameTarget{ target };
				const ScopedViewport2D viewport{ Rect{ 1, 2, 3, 4 } };
				DrawInto(resource); // Includes a nested scope for the same output.
				CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == resource.id());
				CHECK(Graphics2D::GetViewport() == Rect{ 1, 2, 3, 4 });
			}
			CHECK(Read(resource) == Image{ 8, 8, Palette::Red });
			CHECK(Read(resource, 1) == Image{ 4, 4, Palette::Red });
		};
		check(RenderTexture{ Size{ 8, 8 }, Palette::Blue, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap });
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			check(MSRenderTexture{ Size{ 8, 8 }, Palette::Blue, samples, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap });
		}
	}

	TEST_CASE("ColorRenderTarget.non_ms_resolve_and_empty_operations_do_not_flush")
	{
		const DrawState state;
		const RenderTexture output{ Size{ 8, 8 }, Palette::Black };
		const ColorRenderTarget target = output;
		DynamicTexture source{ Image{ 8, 8, Palette::Red } };
		{
			const ScopedRenderTarget2D scope{ target };
			source.draw();
			target.resolve();
			const Texture sampled = target.getTexture();
			CHECK(sampled.id() == output.id());
			ColorRenderTarget{}.clear(Palette::Green).resolve();
			ColorRenderTarget{}.generateMips();
			target.generateMips(); // No mip chain: this is also a no-op.
			REQUIRE(source.fill(Palette::Blue));
		}
		// Any implicit Flush above would have sampled the earlier red upload.
		CHECK(Read(output) == Image{ 8, 8, Palette::Blue });
	}

	TEST_CASE("ColorRenderTarget.ms_resolve_requires_explicit_flush")
	{
		const DrawState state;
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			for (const bool flush : { false, true })
			{
				CAPTURE(samples, flush);
				const MSRenderTexture resource{ Size{ 8, 8 }, Palette::Blue, samples };
				const ColorRenderTarget target = resource;
				{
					const ScopedRenderTarget2D scope{ target };
					Rect{ target.size() }.draw(Palette::Red);
					if (flush) { Graphics2D::Flush(); }
					target.resolve();
				}
				CHECK(Read(resource) == Image{ 8, 8, flush ? Palette::Red : Palette::Blue });
				target.resolve();
				CHECK(Read(resource) == Image{ 8, 8, Palette::Red });
			}
		}
	}

	TEST_CASE("ColorRenderTarget.scope_conversions_nesting_and_feedback")
	{
		const DrawState state;
		const RenderTexture single{ Size{ 8, 8 }, Palette::Black };
		const MSRenderTexture ms{ Size{ 8, 8 }, Palette::Blue };
		const auto check = [&](const auto& argument, const Texture::IDType expected)
		{
			const ScopedRenderTarget2D scope{ argument };
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == expected);
			CHECK_FALSE(Graphics2D::GetViewport());
		};
		{
			ColorRenderTarget selected = single;
			const ScopedRenderTarget2D outer{ selected };
			selected.release();
			const ScopedViewport2D viewport{ Rect{ 1, 2, 4, 5 } };
			check(single, single.id());
			check(ms, ms.id());
			check(Optional<RenderTexture>{ single }, single.id());
			check(Optional<MSRenderTexture>{ ms }, ms.id());
			check(ColorRenderTarget{ ms }, ms.id());
			check(Optional<ColorRenderTarget>{ ms }, ms.id());
			const auto scene = [&](const auto& argument)
			{
				const ScopedRenderTarget2D scope{ argument };
				CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTarget());
			};
			scene(none);
			scene(Optional<RenderTexture>{});
			scene(Optional<MSRenderTexture>{});
			scene(Optional<ColorRenderTarget>{});
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == single.id());
			CHECK(Graphics2D::GetViewport() == Rect{ 1, 2, 4, 5 });
			{
				const ColorRenderTarget target = ms;
				ScopedRenderTarget2D scope{ target };
				CHECK_NOTHROW(target.getTexture().draw());
				single.draw(-16, 0);
				const ScopedRenderTarget2D moved{ std::move(scope) };
			}
			CHECK_THROWS_AS(ColorRenderTarget{ single }.getTexture().draw(), Error);
		}
		CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTarget());
		CHECK(Read(single) == Image{ 8, 8, Palette::Black });
	}
}
