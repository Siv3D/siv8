# Texture and DynamicTexture Visual Test

This manual test compares the rendered output of `Texture` and
`DynamicTexture` with reference colors drawn using `Rect::draw()`.
It covers all uncompressed texture formats supported by the corresponding
`Texture::Create*()` and `DynamicTexture::Create*()` functions.

Each row contains three panels:

1. A regular `Texture`.
2. A `DynamicTexture`, updated with either `fill()` or `fillRegion()` where
   that operation is supported.
3. The expected output drawn with `Rect::draw()`.

All three panels should show the same four quadrants. A red `[FAILED]` label
indicates that texture creation, a dynamic update, or the resulting texture
format did not match the expected state.

The bottom-right quadrant is translucent. Siv3D's standard 2D rendering uses
premultiplied alpha, so texture data containing alpha is premultiplied before
upload. For the sRGB texture, the color is premultiplied in linear space and
then encoded with the sRGB curve.

To run this test, temporarily use the following code as the platform test
application's `Main.cpp`, build it, and launch the application without
`--test-only`. Do not commit the temporary `Main.cpp` replacement.

Future tests that require visual inspection or human input should be added as
separate Markdown files in this directory.

```cpp
# include <Siv3D.hpp> // Siv3D v0.8.0
# include "../Test/Siv3DTest.hpp"

namespace
{
	constexpr int32 PatternWidth = 16;
	constexpr int32 PatternHeight = 12;
	constexpr Size PatternSize{ PatternWidth, PatternHeight };

	enum class VisibleChannels
	{
		R,
		RG,
		RGB,
		RGBA,
	};

	struct TextureVisualCase
	{
		String name;
		String dynamicOperation;
		Texture texture;
		DynamicTexture dynamicTexture;
		VisibleChannels visibleChannels;
		bool updateSucceeded;
	};

	[[nodiscard]]
	const std::array<ColorF, 4>& GetTestColors()
	{
		static const std::array<ColorF, 4> colors
		{
			ColorF{ 1.0, 0.0, 0.0 },
			ColorF{ 0.0, 1.0, 0.0 },
			ColorF{ 0.0, 0.0, 1.0 },
			ColorF{ 1.0, 1.0, 1.0, (2.0 / 3.0) },
		};

		return colors;
	}

	template <class Pixel, class Converter>
	[[nodiscard]]
	Grid<Pixel> MakePattern(Converter converter)
	{
		Grid<Pixel> result{ PatternSize };
		const auto& colors = GetTestColors();

		for (int32 y = 0; y < PatternHeight; ++y)
		{
			for (int32 x = 0; x < PatternWidth; ++x)
			{
				const size_t index = ((x < (PatternWidth / 2)) ? 0 : 1)
					+ ((y < (PatternHeight / 2)) ? 0 : 2);
				result[y][x] = converter(colors[index]);
			}
		}

		return result;
	}

	[[nodiscard]]
	Image MakePatternImage()
	{
		Image result{ PatternSize };
		const auto& colors = GetTestColors();

		for (int32 y = 0; y < PatternHeight; ++y)
		{
			for (int32 x = 0; x < PatternWidth; ++x)
			{
				const size_t index = ((x < (PatternWidth / 2)) ? 0 : 1)
					+ ((y < (PatternHeight / 2)) ? 0 : 2);
				result[y][x] = colors[index];
			}
		}

		result.premultiplyAlpha();

		return result;
	}

	template <class Pixel, class Converter>
	bool FillQuadrants(DynamicTexture& texture, Converter converter)
	{
		constexpr Size regionSize{ (PatternWidth / 2), (PatternHeight / 2) };
		const auto& colors = GetTestColors();
		bool succeeded = true;

		for (int32 index = 0; index < 4; ++index)
		{
			const Point pos{ ((index % 2) * regionSize.x), ((index / 2) * regionSize.y) };
			const Grid<Pixel> region{ regionSize, converter(colors[index]) };
			succeeded = (texture.fillRegion(region, Rect{ pos, regionSize }) && succeeded);
		}

		return succeeded;
	}

	[[nodiscard]]
	bool FillColorQuadrants(DynamicTexture& texture)
	{
		constexpr Size regionSize{ (PatternWidth / 2), (PatternHeight / 2) };
		const auto& colors = GetTestColors();
		bool succeeded = true;

		for (int32 index = 0; index < 4; ++index)
		{
			const Point pos{ ((index % 2) * regionSize.x), ((index / 2) * regionSize.y) };
			const ColorF colorPMA = colors[index].premultiplied();
			succeeded = (texture.fillRegion(colorPMA, Rect{ pos, regionSize }) && succeeded);
		}

		return succeeded;
	}

	[[nodiscard]]
	ColorF GetExpectedColor(const ColorF& color, const VisibleChannels channels)
	{
		switch (channels)
		{
		case VisibleChannels::R:
			return ColorF{ color.r, 0.0, 0.0 };
		case VisibleChannels::RG:
			return ColorF{ color.r, color.g, 0.0 };
		case VisibleChannels::RGB:
			return ColorF{ color.r, color.g, color.b };
		case VisibleChannels::RGBA:
			return color;
		}

		return Palette::Magenta;
	}

	void DrawExpectedPattern(const Rect& rect, const VisibleChannels channels)
	{
		const int32 leftWidth = (rect.w / 2);
		const int32 topHeight = (rect.h / 2);
		const std::array<Rect, 4> regions
		{
			Rect{ rect.x, rect.y, leftWidth, topHeight },
			Rect{ (rect.x + leftWidth), rect.y, (rect.w - leftWidth), topHeight },
			Rect{ rect.x, (rect.y + topHeight), leftWidth, (rect.h - topHeight) },
			Rect{ (rect.x + leftWidth), (rect.y + topHeight), (rect.w - leftWidth), (rect.h - topHeight) },
		};

		const auto& colors = GetTestColors();

		for (size_t i = 0; i < regions.size(); ++i)
		{
			regions[i].draw(GetExpectedColor(colors[i], channels));
		}
	}

	[[nodiscard]]
	Array<TextureVisualCase> CreateTextureVisualCases()
	{
		Array<TextureVisualCase> cases;
		cases.reserve(15);

		{
			const auto data = MakePattern<uint8>([](const ColorF& color) { return color.toR8_Unorm(); });
			auto dynamicTexture = DynamicTexture::CreateR8_Unorm(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<uint8>(dynamicTexture, [](const ColorF& color) { return color.toR8_Unorm(); });
			cases.push_back({ U"R8_Unorm", U"fillRegion(Grid<uint8>)",
				Texture::CreateR8_Unorm(data), std::move(dynamicTexture), VisibleChannels::R, succeeded });
		}

		{
			const auto data = MakePattern<uint16>([](const ColorF& color) { return color.toR8G8_Unorm(); });
			auto dynamicTexture = DynamicTexture::CreateR8G8_Unorm(data);
			cases.push_back({ U"R8G8_Unorm", U"Create(Grid<uint16>)",
				Texture::CreateR8G8_Unorm(data), std::move(dynamicTexture), VisibleChannels::RG, true });
		}

		{
			const auto data = MakePattern<HalfFloat>([](const ColorF& color) { return color.toR16_Float(); });
			auto dynamicTexture = DynamicTexture::CreateR16_Float(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<HalfFloat>(dynamicTexture, [](const ColorF& color) { return color.toR16_Float(); });
			cases.push_back({ U"R16_Float", U"fillRegion(Grid<HalfFloat>)",
				Texture::CreateR16_Float(data), std::move(dynamicTexture), VisibleChannels::R, succeeded });
		}

		{
			const auto data = MakePattern<Color>([](const ColorF& color)
			{
				return color.premultiplied().toColor();
			});
			auto dynamicTexture = DynamicTexture::CreateR8G8B8A8_Unorm(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<Color>(dynamicTexture, [](const ColorF& color)
			{
				return color.premultiplied().toColor();
			});
			cases.push_back({ U"R8G8B8A8_Unorm", U"fillRegion(Grid<Color>)",
				Texture::CreateR8G8B8A8_Unorm(data), std::move(dynamicTexture), VisibleChannels::RGBA, succeeded });
		}

		{
			const auto data = MakePattern<Color>([](const ColorF& color)
			{
				return color.premultiplied().linearToSRGB().toColor();
			});
			auto dynamicTexture = DynamicTexture::CreateR8G8B8A8_Unorm_SRGB(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<Color>(dynamicTexture, [](const ColorF& color)
			{
				return color.premultiplied().linearToSRGB().toColor();
			});
			cases.push_back({ U"R8G8B8A8_Unorm_SRGB", U"fillRegion(Grid<Color>)",
				Texture::CreateR8G8B8A8_Unorm_SRGB(data), std::move(dynamicTexture), VisibleChannels::RGBA, succeeded });
		}

		{
			const auto data = MakePattern<uint32>([](const ColorF& color) { return color.toR16G16_Unorm(); });
			auto dynamicTexture = DynamicTexture::CreateR16G16_Unorm(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<std::pair<uint16, uint16>>(dynamicTexture, [](const ColorF& color)
			{
				return std::pair<uint16, uint16>
				{
					static_cast<uint16>(color.r * 65535.0),
					static_cast<uint16>(color.g * 65535.0)
				};
			});
			cases.push_back({ U"R16G16_Unorm", U"fillRegion(Grid<pair<uint16, uint16>>)",
				Texture::CreateR16G16_Unorm(data), std::move(dynamicTexture), VisibleChannels::RG, succeeded });
		}

		{
			const auto data = MakePattern<uint32>([](const ColorF& color) { return color.toR16G16_Float(); });
			auto dynamicTexture = DynamicTexture::CreateR16G16_Float(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<std::pair<HalfFloat, HalfFloat>>(dynamicTexture, [](const ColorF& color)
			{
				return std::pair<HalfFloat, HalfFloat>{ HalfFloat{ color.r }, HalfFloat{ color.g } };
			});
			cases.push_back({ U"R16G16_Float", U"fillRegion(Grid<pair<HalfFloat, HalfFloat>>)",
				Texture::CreateR16G16_Float(data), std::move(dynamicTexture), VisibleChannels::RG, succeeded });
		}

		{
			const auto data = MakePattern<float>([](const ColorF& color) { return color.toR32_Float(); });
			auto dynamicTexture = DynamicTexture::CreateR32_Float(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<float>(dynamicTexture, [](const ColorF& color) { return color.toR32_Float(); });
			cases.push_back({ U"R32_Float", U"fillRegion(Grid<float>)",
				Texture::CreateR32_Float(data), std::move(dynamicTexture), VisibleChannels::R, succeeded });
		}

		{
			const auto data = MakePattern<uint32>([](const ColorF& color)
			{
				return color.premultiplied().toR10G10B10A2_Unorm();
			});
			auto dynamicTexture = DynamicTexture::CreateR10G10B10A2_Unorm(data);
			cases.push_back({ U"R10G10B10A2_Unorm", U"Create(Grid<uint32>)",
				Texture::CreateR10G10B10A2_Unorm(data), std::move(dynamicTexture), VisibleChannels::RGBA, true });
		}

		{
			const auto data = MakePattern<uint32>([](const ColorF& color) { return color.toR11G11B10_UFloat(); });
			auto dynamicTexture = DynamicTexture::CreateR11G11B10_UFloat(data);
			cases.push_back({ U"R11G11B10_UFloat", U"Create(Grid<uint32>)",
				Texture::CreateR11G11B10_UFloat(data), std::move(dynamicTexture), VisibleChannels::RGB, true });
		}

		{
			const auto data = MakePattern<uint64>([](const ColorF& color)
			{
				return color.premultiplied().toR16G16B16A16_Float();
			});
			auto dynamicTexture = DynamicTexture::CreateR16G16B16A16_Float(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<std::array<HalfFloat, 4>>(dynamicTexture, [](const ColorF& color)
			{
				const ColorF colorPMA = color.premultiplied();
				return std::array<HalfFloat, 4>
				{
					HalfFloat{ colorPMA.r },
					HalfFloat{ colorPMA.g },
					HalfFloat{ colorPMA.b },
					HalfFloat{ colorPMA.a }
				};
			});
			cases.push_back({ U"R16G16B16A16_Float", U"fillRegion(Grid<std::array<HalfFloat, 4>>)",
				Texture::CreateR16G16B16A16_Float(data), std::move(dynamicTexture), VisibleChannels::RGBA, succeeded });
		}

		{
			const auto data = MakePattern<Float2>([](const ColorF& color) { return color.toR32G32_Float(); });
			auto dynamicTexture = DynamicTexture::CreateR32G32_Float(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<Float2>(dynamicTexture, [](const ColorF& color) { return color.toR32G32_Float(); });
			cases.push_back({ U"R32G32_Float", U"fillRegion(Grid<Float2>)",
				Texture::CreateR32G32_Float(data), std::move(dynamicTexture), VisibleChannels::RG, succeeded });
		}

		{
			const auto data = MakePattern<Float4>([](const ColorF& color)
			{
				return color.premultiplied().toR32G32B32A32_Float();
			});
			auto dynamicTexture = DynamicTexture::CreateR32G32B32A32_Float(PatternSize, Palette::Black);
			const bool succeeded = FillQuadrants<Float4>(dynamicTexture, [](const ColorF& color)
			{
				return color.premultiplied().toR32G32B32A32_Float();
			});
			cases.push_back({ U"R32G32B32A32_Float", U"fillRegion(Grid<Float4>)",
				Texture::CreateR32G32B32A32_Float(data), std::move(dynamicTexture), VisibleChannels::RGBA, succeeded });
		}

		{
			const Image image = MakePatternImage();
			DynamicTexture dynamicTexture{ PatternSize, Palette::Black };
			const bool succeeded = dynamicTexture.fill(image);
			cases.push_back({ U"Image constructor / DynamicTexture::fill()", U"fill(Image)",
				Texture{ image }, std::move(dynamicTexture), VisibleChannels::RGBA, succeeded });
		}

		{
			const Image image = MakePatternImage();
			DynamicTexture dynamicTexture{ PatternSize, Palette::Black };
			const bool succeeded = FillColorQuadrants(dynamicTexture);
			cases.push_back({ U"DynamicTexture::fillRegion(ColorF)", U"fillRegion(ColorF)",
				Texture{ image }, std::move(dynamicTexture), VisibleChannels::RGBA, succeeded });
		}

		return cases;
	}

	void DrawVisualCase(const TextureVisualCase& item, const Rect& card, const Font& titleFont, const Font& labelFont)
	{
		constexpr int32 CardPadding = 12;
		constexpr int32 SwatchGap = 10;
		constexpr int32 SwatchHeight = 58;
		const int32 swatchWidth = ((card.w - (CardPadding * 2) - (SwatchGap * 2)) / 3);
		const int32 swatchY = (card.y + 48);

		card.draw(ColorF{ 0.13 });
		card.drawFrame(1, ColorF{ 0.25 });

		titleFont(item.name).draw(card.pos + Point{ CardPadding, 4 }, ColorF{ 0.94 });

		const bool succeeded = (item.updateSucceeded
			&& (not item.texture.isEmpty())
			&& (not item.dynamicTexture.isEmpty())
			&& (item.texture.format() == item.dynamicTexture.format()));
		String status = item.dynamicOperation;
		status += (succeeded ? U"  [OK]" : U"  [FAILED]");
		const ColorF statusColor = (succeeded ? ColorF{ 0.45, 0.9, 0.55 } : ColorF{ 1.0, 0.25, 0.25 });
		labelFont(status).draw(Anchor::TopRight, (card.tr() + Point{ -CardPadding, 7 }), statusColor);

		const std::array<StringView, 3> labels
		{
			U"Texture",
			U"DynamicTexture",
			U"Rect::draw() expected",
		};

		std::array<Rect, 3> swatches;

		for (int32 i = 0; i < 3; ++i)
		{
			const int32 x = (card.x + CardPadding + (i * (swatchWidth + SwatchGap)));
			labelFont(labels[i]).draw(x, (card.y + 27), ColorF{ 0.65 });
			swatches[i] = Rect{ x, swatchY, swatchWidth, SwatchHeight };
			swatches[i].draw(ColorF{ 0.03 });
		}

		item.texture.resized(swatches[0].size).draw(swatches[0].pos);
		item.dynamicTexture.resized(swatches[1].size).draw(swatches[1].pos);
		DrawExpectedPattern(swatches[2], item.visibleChannels);

		for (const auto& swatch : swatches)
		{
			swatch.drawFrame(1, ColorF{ 0.55 });
		}
	}
}

void Main()
{
	const int32 exitCode = RunTest();

	if (System::GetCommandLineArgs().contains(U"--test-only"))
	{
		System::Exit(exitCode);
		return;
	}

	Window::Resize(1680, 1050);
	Window::SetTitle(U"Texture / DynamicTexture Visual Test");
	Scene::SetBackground(ColorF{ 0.07 });

	const Font headerFont{ 24, Typeface::Bold };
	const Font titleFont{ 17, Typeface::Bold };
	const Font labelFont{ 13 };
	const Array<TextureVisualCase> cases = CreateTextureVisualCases();

	while (System::Update())
	{
		headerFont(U"Texture / DynamicTexture visual test").draw(20, 10, ColorF{ 0.96 });
		labelFont(U"Each row must show the same four quadrants: Texture | DynamicTexture | Rect::draw() expected")
			.draw(20, 42, ColorF{ 0.65 });

		const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };

		constexpr int32 Left = 20;
		constexpr int32 Top = 66;
		constexpr int32 CardWidth = 812;
		constexpr int32 CardHeight = 116;
		constexpr int32 ColumnGap = 16;
		constexpr int32 RowGap = 6;

		for (size_t i = 0; i < cases.size(); ++i)
		{
			const int32 column = static_cast<int32>(i % 2);
			const int32 row = static_cast<int32>(i / 2);
			const Rect card
			{
				(Left + column * (CardWidth + ColumnGap)),
				(Top + row * (CardHeight + RowGap)),
				CardWidth,
				CardHeight
			};

			DrawVisualCase(cases[i], card, titleFont, labelFont);
		}
	}
}
```
