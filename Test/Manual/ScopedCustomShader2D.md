# ScopedCustomShader2D

This program compares file-based custom shaders with built-in 2D rendering on
Windows (D3D11) and macOS (Metal). Pages 1–3 use the standard vertex/pixel constant
buffers; page 4 also supplies custom constants at slot 2. Textures and samplers
use slot 0. Shader objects and textures are created
once, before the drawing loop. The generated diagnostic image is explicitly
premultiplied before upload; constructing a texture from an Image does not
perform that conversion.

## Shader files

Each shader is self-contained. Load the HLSL or MSL file at runtime using
`HLSL{ path, entryPoint } | MSL{ path, entryPoint }`.

| Example | HLSL | MSL | Entry points |
| --- | --- | --- | --- |
| Standard template | [default2d.hlsl](../../WindowsDesktop/App/example/shader/hlsl/default2d.hlsl) | [default2d.metal](../../macOS/App/example/shader/msl/default2d.metal) | `VS_Shape`, `PS_Shape`, `PS_Texture` |
| Grayscale | [grayscale.hlsl](../../WindowsDesktop/App/example/shader/hlsl/grayscale.hlsl) | [grayscale.metal](../../macOS/App/example/shader/msl/grayscale.metal) | `PS_Grayscale` |
| Red/blue exchange | [rgb_to_bgr.hlsl](../../WindowsDesktop/App/example/shader/hlsl/rgb_to_bgr.hlsl) | [rgb_to_bgr.metal](../../macOS/App/example/shader/msl/rgb_to_bgr.metal) | `PS_RGBToBGR` |
| Posterization | [posterize.hlsl](../../WindowsDesktop/App/example/shader/hlsl/posterize.hlsl) | [posterize.metal](../../macOS/App/example/shader/msl/posterize.metal) | `PS_Posterize` |
| Custom constants | [constant_buffer.hlsl](../../WindowsDesktop/App/example/shader/hlsl/constant_buffer.hlsl) | [constant_buffer.metal](../../macOS/App/example/shader/msl/constant_buffer.metal) | `VS_Offset`, `PS_TintShape`, `PS_TintTexture` |
| Horizontal UV flip | [uv_flip.hlsl](../../WindowsDesktop/App/example/shader/hlsl/uv_flip.hlsl) | [uv_flip.metal](../../macOS/App/example/shader/msl/uv_flip.metal) | `VS_UVFlip` |

The standard VS applies the drawing transform and color multiplication, then
premultiplies vertex RGB by alpha. The texture PS multiplies the premultiplied
texture and vertex colors, then adds the standard additive color scaled by the
resulting alpha. The three color effects operate on that result, preserving alpha.
Posterization divides RGB by alpha, rounds to the nearest quarter (ties upward),
and premultiplies again; fully transparent pixels skip the division. RGB in
[0, 1] has five possible levels. HDR/additive values are not clamped.

HLSL uses `b0` separately for VS and PS. Metal uses vertex buffer 0 for vertices,
vertex buffer 1 for the standard VS constants, and fragment buffer 0 for the
standard PS constants. Keep the full standard buffer layouts, including fields
unused by these examples; offsets must match the engine.

The texture effects are for ordinary texture draws. `VS_UVFlip` maps `u` to
`1 - u` across the whole texture (UV range 0..1); it is not a cropped-region or
atlas-mirroring shader. Patterns, dashed lines, MSDF fonts, and quad-warp drawing
have different shader requirements. Draw labels and other such content outside
these custom shader scopes. The default template's `PS_Shape` is for solid shapes.

## Execution

1. Use a separate Siv3D v0.8 application built against this revision, with its
   normal engine resources/build settings, and paste the complete code below
   into its Main.cpp. Keep the repository's platform Main.cpp files and their
   test-only entry points intact.
2. Use the corresponding platform App directory as the asset root. On Windows,
   place the executable there, or configure the IDE working directory there.
   Alternatively, copy `example/windmill.png` and the appropriate
   `example/shader/hlsl/` or `example/shader/msl/` directory into your application's
   asset directory. The program loads `.hlsl`/`.metal` source files; the sample
   Metal files do not need to be added to the default Metal library.
3. Run on Windows (D3D11) and macOS (Metal). Press 1, 2, 3, or 4 to select a page.
   Press Space on page 3 to animate/pause rotation. Escape closes the application.
4. Optionally launch with `--capture` to save all four stationary pages and exit.
   Output is `Screenshot/ScopedCustomShader2D/page-1.png` through `page-4.png`,
   relative to the asset working directory. Repeating capture replaces these
   files. Delete that directory when the captures are no longer needed.

A loading error identifies the shader basename and entry point, or the texture.
A successful load alone does not establish correct rendering: inspect the pages
below. Capture mode records images; it does not automatically judge them.

## Expected results

- **Page 1 — effects:** Built-in and Default 2D are identical. Grayscale has no
  color; RGB to BGR exchanges red and blue. Posterize produces bands in the
  gradient, while the orange alpha ramp fades smoothly. UV flip mirrors each
  whole image. Transparent pixels reveal the checkerboard without bright fringes.
  The photo uses the same tint, additive color, and opacity in every tile; the
  diagnostic is drawn without those extra states. The generated diagnostic has
  asymmetric color bars, an RGB gradient, an alpha ramp (including alpha zero),
  and a fully transparent bottom strip.
- **Page 2 — scope restoration:** Read each row left to right using its caption.
  The first/last panels agree, and the second/fourth panels agree. The middle
  panel replaces just the selected stage: the other outer stage remains active.
  Nested pixel shaders replace each other; they do not form a sequence of image
  effects. All transitions are queued in one frame without explicit flushes.
- **Page 3 — standard states:** All three top panels agree, including the solid
  rectangle/circle. All three bottom panels agree: both constructor argument
  orders match built-in texture mirroring. Rotation/nonuniform scaling, vertex
  tint, color multiplication/addition, and alpha are applied in every panel.
  Space animates the transforms; the matches should persist across frames.
  The solid shapes below the bottom-row textures are drawn after each custom
  scope and should remain ordinary, unmirrored shapes.
- **Page 4 — custom constants:** The top three panels agree: a built-in tint and
  translation, persistent VS/PS constants with explicit flushes, and temporary
  constant buffers destroyed before drawing. The persistent constants are bound
  once before the main loop, so switching pages and returning must retain them.
  The bottom three panels agree: built-in red/green/red tints, repeated Set calls
  using one object for both stages, and nested scopes that restore the outer red
  tint. Flushes inside these scopes must not affect the images. Changing the
  original buffer without Set must not change the inner green tint.
  Opacity preserves the checkerboard and transparent diagnostic strip.
  ScopedCustomShader2D restores shaders; constant-buffer scopes independently
  restore their specified slots. The automated tests also cover shader scopes
  across frame boundaries, initially unset slots, moved scopes, slots 2/13,
  integer payloads, and 4096/4112/65536-byte buffers.
- The footer's colored shapes, photo, and white text remain unchanged on every
  page, demonstrating restoration before subsequent built-in draws.

Run each backend on its own host; D3D11 results do not establish Metal execution
coverage. For the repository's automated checks and host-specific build commands,
see the [development guide](../../docs/development/README.md#build-and-test).

## Complete code

```cpp
# include <Siv3D.hpp>
# include <cmath>

namespace
{
	VertexShader LoadVS(const StringView name, const StringView entryPoint)
	{
		const VertexShader shader = HLSL{ U"example/shader/hlsl/{}.hlsl"_fmt(name), String{ entryPoint } }
			| MSL{ U"example/shader/msl/{}.metal"_fmt(name), String{ entryPoint } };
		if (not shader)
		{
			throw Error{ U"Could not load vertex shader: {} / {}"_fmt(name, entryPoint) };
		}
		return shader;
	}

	PixelShader LoadPS(const StringView name, const StringView entryPoint)
	{
		const PixelShader shader = HLSL{ U"example/shader/hlsl/{}.hlsl"_fmt(name), String{ entryPoint } }
			| MSL{ U"example/shader/msl/{}.metal"_fmt(name), String{ entryPoint } };
		if (not shader)
		{
			throw Error{ U"Could not load pixel shader: {} / {}"_fmt(name, entryPoint) };
		}
		return shader;
	}

	struct Shaders
	{
		VertexShader shapeVS = LoadVS(U"default2d", U"VS_Shape");
		VertexShader offsetVS = LoadVS(U"constant_buffer", U"VS_Offset");
		PixelShader tintPS = LoadPS(U"constant_buffer", U"PS_TintTexture");
		VertexShader flipVS = LoadVS(U"uv_flip", U"VS_UVFlip");
		PixelShader shapePS = LoadPS(U"default2d", U"PS_Shape");
		PixelShader texturePS = LoadPS(U"default2d", U"PS_Texture");
		PixelShader grayscale = LoadPS(U"grayscale", U"PS_Grayscale");
		PixelShader bgr = LoadPS(U"rgb_to_bgr", U"PS_RGBToBGR");
		PixelShader posterize = LoadPS(U"posterize", U"PS_Posterize");
	};

	Image MakeDiagnostic()
	{
		Image image{ 240, 160, Color{ 255, 128, 64, 0 } };
		const std::array<Color, 4> bars{ Palette::Red, Palette::Lime, Palette::Blue, Palette::Yellow };
		for (int32 y = 0; y < 144; ++y)
		{
			for (int32 x = 0; x < 240; ++x)
			{
				const double t = (x / 239.0);
				if (y < 56)
				{
					image[y][x] = bars[x / 60];
				}
				else if (y < 100)
				{
					image[y][x] = ColorF{ t, (1.0 - t), (0.2 + 0.6 * t) }.toColor();
				}
				else
				{
					image[y][x] = ColorF{ 1.0, 0.5, 0.25, t }.toColor();
				}
			}
		}
		// Texture(Image) uploads the pixels as supplied. Convert straight RGBA to PMA.
		image.premultiplyAlpha();
		return image;
	}

	void DrawChecker(const RectF& rect)
	{
		// Local coordinates give every comparison panel the same checker phase.
		const Transformer2D transform{ Mat3x2::Translate(rect.pos) };
		RectF{ 0, 0, rect.w, rect.h }.draw(Pattern::Checker{ ColorF{ 0.22 }, ColorF{ 0.34 }, 12 });
	}

	Vec2 TilePosition(const int32 index)
	{
		return Vec2{ (24 + (index % 3) * 352), (132 + (index / 3) * 276) };
	}

	void DrawTile(const Font& font, const StringView title, const StringView caption)
	{
		RectF{ 0, 0, 336, 250 }.draw(ColorF{ 0.16 });
		font(title).draw(18, Vec2{ 12, 12 });
		DrawChecker(RectF{ 12, 46, 312, 176 });
		font(caption).draw(13, Vec2{ 12, 229 }, ColorF{ 0.8 });
	}

	void DrawEffects(const Font& font, const Shaders& shaders, const Texture& photo, const Texture& diagnostic)
	{
		const std::array<StringView, 6> titles{
			U"Built-in", U"Default 2D", U"Grayscale", U"RGB to BGR", U"Posterize", U"UV flip (VS only)" };
		const std::array<StringView, 6> captions{
			U"Reference", U"Must match Built-in", U"Color effect after tint/add",
			U"Red and blue exchanged", U"Five RGB levels; continuous alpha", U"Both images mirrored horizontally" };
		for (int32 i = 0; i < 6; ++i)
		{
			const Transformer2D tile{ Mat3x2::Translate(TilePosition(i)) };
			DrawTile(font, titles[i], captions[i]);
			const auto draw = [&]
			{
				{
					const ScopedColorMul2D colorMul{ ColorF{ 0.8, 1.0, 0.7, 0.75 } };
					const ScopedColorAdd2D colorAdd{ 0.05, 0.02, 0.08 };
					photo.resized(146, 176).draw(12, 46);
				}
				diagnostic.resized(154, 176).draw(170, 46);
			};
			switch (i)
			{
			case 0:
				draw();
				break;
			case 1:
				{
					const ScopedCustomShader2D shader{ shaders.shapeVS, shaders.texturePS };
					draw();
					break;
				}
			case 5:
				{
					const ScopedCustomShader2D shader{ shaders.flipVS };
					draw();
					break;
				}
			default:
				{
					const PixelShader& ps = ((i == 2) ? shaders.grayscale : ((i == 3) ? shaders.bgr : shaders.posterize));
					const ScopedCustomShader2D shader{ ps };
					draw();
					break;
				}
			}
		}
	}

	void DrawNesting(const Font& font, const Shaders& shaders, const Texture& diagnostic)
	{
		const std::array<StringView, 4> captions{
			U"PS nesting: Normal  >  Grayscale  >  BGR  >  Grayscale  >  Normal",
			U"VS nesting: Normal  >  Flip  >  Default VS  >  Flip  >  Normal",
			U"Outer VS / inner PS: Normal  >  Flip  >  Flip + BGR  >  Flip  >  Normal",
			U"Outer PS / inner VS: Normal  >  BGR  >  BGR + Flip  >  BGR  >  Normal" };
		for (int32 row = 0; row < 4; ++row)
		{
			const int32 y = (132 + row * 140);
			font(captions[row]).draw(16, Vec2{ 24, (y - 25) });
			for (int32 column = 0; column < 5; ++column)
			{
				DrawChecker(RectF{ (24 + column * 212), y, 192, 96 });
			}
			const auto draw = [&](const int32 column)
			{
				diagnostic.resized(192, 96).draw((24 + column * 212), y);
			};
			draw(0);
			if (row == 0)
			{
				const ScopedCustomShader2D outer{ shaders.grayscale };
				draw(1);
				{
					const ScopedCustomShader2D inner{ shaders.bgr };
					draw(2);
				}
				draw(3);
			}
			else if (row == 1)
			{
				const ScopedCustomShader2D outer{ shaders.flipVS };
				draw(1);
				{
					const ScopedCustomShader2D inner{ shaders.shapeVS };
					draw(2);
				}
				draw(3);
			}
			else if (row == 2)
			{
				const ScopedCustomShader2D outer{ shaders.flipVS };
				draw(1);
				{
					const ScopedCustomShader2D inner{ shaders.bgr };
					draw(2);
				}
				draw(3);
			}
			else
			{
				const ScopedCustomShader2D outer{ shaders.bgr };
				draw(1);
				{
					const ScopedCustomShader2D inner{ shaders.flipVS };
					draw(2);
				}
				draw(3);
			}
			draw(4);
		}
	}

	void DrawStateContent(const Shaders& shaders, const Texture& diagnostic, const int32 mode, const double angle)
	{
		const ScopedColorMul2D colorMul{ ColorF{ 0.65, 1.0, 0.8, 0.65 } };
		const ScopedColorAdd2D colorAdd{ 0.12, 0.03, 0.07 };
		const auto drawTexture = [&](const bool mirror = false)
		{
			const Transformer2D transform{ (Mat3x2::Scale(0.94, 0.78) * Mat3x2::Rotate(angle)).translated(168, 126) };
			diagnostic.resized(258, 148).mirrored(mirror).drawAt(0, 0, ColorF{ 1.0, 0.8, 0.6, 0.8 });
		};
		switch (mode)
		{
		case 0:
			drawTexture();
			break;
		case 1:
			{
				const ScopedCustomShader2D shader{ shaders.shapeVS };
				drawTexture();
				break;
			}
		case 2:
			{
				const ScopedCustomShader2D shader{ shaders.shapeVS, shaders.texturePS };
				drawTexture();
				break;
			}
		case 3:
			{
				const ScopedCustomShader2D shader{ shaders.flipVS, shaders.texturePS };
				drawTexture();
				break;
			}
		case 4:
			{
				const ScopedCustomShader2D shader{ shaders.texturePS, shaders.flipVS };
				drawTexture();
				break;
			}
		case 5:
			drawTexture(true);
			break;
		}
		const auto drawShapes = []
		{
			RectF{ 40, 194, 108, 20 }.draw(ColorF{ 1.0, 0.2, 0.1, 0.6 });
			Circle{ 260, 204, 12 }.draw(ColorF{ 0.1, 0.4, 1.0, 0.7 });
		};
		if (mode == 1)
		{
			const ScopedCustomShader2D shader{ shaders.shapeVS };
			drawShapes();
		}
		else if (mode == 2)
		{
			const ScopedCustomShader2D shader{ shaders.shapeVS, shaders.shapePS };
			drawShapes();
		}
		else
		{
			drawShapes();
		}
	}


	struct CustomParameters
	{
		Float4 value;
	};

	void DrawConstants(const Font& font, const Shaders& shaders, const Texture& diagnostic)
	{
		const std::array<StringView, 6> titles{
			U"Built-in tint + translation", U"Persistent constants + flush", U"Temporary buffer lifetime",
			U"Built-in red / green / red", U"One buffer, both stages", U"Nested constants + flush" };
		const ColorF tint{ 0.5, 1.0, 0.75, 0.65 };
		const std::array<Float4, 3> colors{
			Float4{ 1, 0.25f, 0.25f, 0.75f }, Float4{ 0.25f, 1, 0.25f, 0.75f }, Float4{ 1, 0.25f, 0.25f, 0.75f } };
		for (int32 i = 0; i < 6; ++i)
		{
			const Transformer2D tile{ Mat3x2::Translate(TilePosition(i)) };
			DrawTile(font, titles[i], ((i < 3) ? U"Top row: all three must agree" : U"Bottom row: all three must agree"));
			if (i == 0)
			{
				diagnostic.resized(276, 148).draw(30, 60, tint);
			}
			else if (i == 1)
			{
				// Uses constants set once in Main(), across every frame and page change.
				const ScopedCustomShader2D shader{ shaders.offsetVS, shaders.tintPS };
				Graphics2D::Flush();
				Graphics2D::Flush();
				diagnostic.resized(276, 148).draw(12, 46);
			}
			else if (i == 2)
			{
				// The temporary source objects die at the ends of these declarations.
				const ScopedVSConstantBuffer2D offset{ 2,
					ConstantBuffer<CustomParameters>{ CustomParameters{ Float4{ 18, 14, 0, 0 } } } };
				const ScopedPSConstantBuffer2D color{ 2,
					ConstantBuffer<CustomParameters>{ CustomParameters{ Float4{ 0.5f, 1, 0.75f, 0.65f } } } };
				const ScopedCustomShader2D shader{ shaders.offsetVS, shaders.tintPS };
				diagnostic.resized(276, 148).draw(12, 46);
			}
			else if (i == 3)
			{
				for (int32 column = 0; column < 3; ++column)
				{
					diagnostic.resized(96, 176).draw((12 + column * 104), 46, ColorF{ colors[column] });
				}
			}
			else
			{
				ConstantBuffer<CustomParameters> cb{ CustomParameters{ Float4{ 0, 0, 0, 0 } } };
				const ScopedVSConstantBuffer2D offset{ 2, cb };
				cb->value = colors[0];
				const ScopedPSConstantBuffer2D outer{ 2, cb };
				const ScopedCustomShader2D shader{ shaders.offsetVS, shaders.tintPS };
				for (int32 column = 0; column < 3; ++column)
				{
					const int32 x = (12 + column * 104);
					if (i == 4)
					{
						cb->value = colors[column];
						Graphics2D::SetPSConstantBuffer(2, cb);
						diagnostic.resized(96, 176).draw(x, 46);
					}
					else if (column == 1)
					{
						cb->value = colors[1];
						const ScopedPSConstantBuffer2D inner{ 2, cb };
						cb->value = Float4{ 0, 0, 1, 1 }; // No Set: green is still active.
						Graphics2D::Flush();
						diagnostic.resized(96, 176).draw(x, 46);
					}
					else
					{
						Graphics2D::Flush();
						diagnostic.resized(96, 176).draw(x, 46);
					}
				}
			}
		}
	}

	void DrawStates(const Font& font, const Shaders& shaders, const Texture& diagnostic, const double angle)
	{
		const std::array<StringView, 6> titles{
			U"Built-in reference", U"Default VS only", U"Default VS + PS",
			U"{ flipVS, texturePS }", U"{ texturePS, flipVS }", U"Built-in mirrored() reference" };
		for (int32 i = 0; i < 6; ++i)
		{
			const Transformer2D tile{ Mat3x2::Translate(TilePosition(i)) };
			DrawTile(font, titles[i], ((i < 3) ? U"Top row: all three must agree" : U"Bottom row: all three must agree"));
			DrawStateContent(shaders, diagnostic, i, angle);
		}
	}
}

void Main()
{
	Window::Resize(1100, 800);
	Scene::SetBackground(ColorF{ 0.09, 0.1, 0.12 });
	const Shaders shaders;
	const Texture photo{ U"example/windmill.png" };
	const Texture diagnostic{ MakeDiagnostic() };
	if ((not photo) || (not diagnostic))
	{
		throw Error{ U"Could not load example/windmill.png or create the diagnostic texture" };
	}
	const Font font{ 20 };
	const bool capture = System::GetCommandLineArgs().contains(U"--capture");
	if (capture)
	{
		ScreenCapture::SetScreenshotDirectory(U"Screenshot/ScopedCustomShader2D/");
	}
	// Bind once; local scopes in DrawConstants restore these values.
	const ScopedVSConstantBuffer2D persistentOffset{ 2,
		ConstantBuffer<CustomParameters>{ CustomParameters{ Float4{ 18, 14, 0, 0 } } } };
	const ScopedPSConstantBuffer2D persistentTint{ 2,
		ConstantBuffer<CustomParameters>{ CustomParameters{ Float4{ 0.5f, 1, 0.75f, 0.65f } } } };
	int32 page = 0;
	int32 frame = 0;
	bool animate = false;
	double elapsed = 0.0;
	const std::array<StringView, 4> titles{ U"Effects", U"Nested scopes", U"Standard drawing states", U"Custom constant buffers" };
	while (System::Update())
	{
		if (capture)
		{
			page = (frame / 8);
		}
		else
		{
			if (Key1.down()) { page = 0; }
			if (Key2.down()) { page = 1; }
			if (Key3.down()) { page = 2; }
			if (Key4.down()) { page = 3; }
			if (KeySpace.down()) { animate = not animate; }
			if (animate) { elapsed += Scene::DeltaTime(); }
		}
		font(U"ScopedCustomShader2D / {}. {}"_fmt((page + 1), titles[page])).draw(28, Vec2{ 24, 18 });
		font(U"1: Effects   2: Nested scopes   3: Drawing states   4: Constants   Space: animate   Esc: exit")
			.draw(16, Vec2{ 24, 60 }, ColorF{ 0.75 });
		switch (page)
		{
		case 0:
			font(U"Photo: tint + additive color + opacity / Diagnostic: original input colors").draw(15, Vec2{ 24, 102 });
			DrawEffects(font, shaders, photo, diagnostic);
			break;
		case 1:
			DrawNesting(font, shaders, diagnostic);
			break;
		case 2:
			DrawStates(font, shaders, diagnostic, (0.12 + std::sin(elapsed) * 0.12));
			break;
		case 3:
			DrawConstants(font, shaders, diagnostic);
			break;
		}

		// All custom shader, transform, and color scopes have ended here.
		RectF{ 24, 714, 40, 40 }.draw(Palette::Red);
		Circle{ 94, 734, 20 }.draw(Palette::Lime);
		photo.resized(40, 40).draw(124, 714);
		font(U"Restored: red rectangle / green circle / original photo / white text")
			.draw(17, Vec2{ 180, 722 });
		if (capture && ((frame % 8) == 6))
		{
			ScreenCapture::SaveCurrentFrame(U"page-{}.png"_fmt(page + 1));
		}
		if (capture && (++frame == 32))
		{
			break; // The last requested capture was consumed by System::Update().
		}
	}
}
```
