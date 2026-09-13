# Pattern collection

A 960 x 700 gallery with tabs for all eleven Pattern types and a Layers tab.
It contains 84 recipes, including fine and bold variations in light and dark
palettes. Pattern cards retain only their frame and name;
compact navigation and animation controls sit outside them.

The [original 32-card overview](PatternCollection32.md) is preserved separately,
including its complete program and original parameter values.

## Execution

1. Paste the complete program below into a separate Siv3D application built with
   this revision. Keep the repository's platform test entry point intact.
2. Run on Metal or D3D11 with the standard engine assets. No extra images are needed.
3. Click a Pattern tab or use Left/Right to change collections. Wave has two pages;
   use its footer arrows or PageUp/PageDown to switch pages.
4. Play/Pause or Space controls animation. Pausing holds the phase and changing
   tabs preserves it. Reset or R pauses and restores the initial pattern positions.
5. Optional command-line arguments:
   - `--collection=Truchet` opens the named tab (case-sensitive).
   - `--page=2` selects a one-based page, clamped to the selected collection.
   - `--animate` starts animation immediately.
   - `--capture` saves the selected page after three frames (90 when animating).
   - `--capture-all` captures all 13 pages, beginning with Halftone, and exits.
     Animation time resets between automatic captures to give each page an equal
     run-up. This option starts at the first collection regardless of selection.
   Captures are `pattern-collection-<collection>-<page>.png` in the application's
   working directory, with zero-based indices. Remove these retained images when
   no longer needed. Avoid interacting with the UI during automatic capture.

## Expected results

- The initial Halftone page retains all eight original recipes: Glacier, Carbon,
  Acid, Violet, Coral, Blueprint, Prism, and Champagne. Each uses its original
  420 x 104 coordinates at 1:1 scale.
- PolkaDot, Stripe, Grid, Checker, Triangle, and HexGrid each have six recipes,
  contrasting fine and large motifs, light and dark palettes, and angles.
- Truchet includes Orbit, a teal ring lattice using one Alternating pattern.
  The original two-PolkaDot recipe remains in the archived overview.
- Layers combines Woven Light (Weave + Halftone), Pearl Circuit, Tartan,
  Interference, Ember Mesh, Kaleidoscope, Solar Static, and Petal Cut. Aligned
  masks move together.
- New types use the same card dimensions to compare them as button backgrounds.
  Wave, Ripple, Weave, and the first seven Truchet recipes retain the parameter
  values from their focused samples; a shorter card reveals a smaller vertical portion of these infinite patterns. Zero/full
  width diagnostics remain in the focused samples, not in this collection.
- Pages hold up to eight cards in two columns. Collections with fewer than eight
  recipes leave the remaining space free. Wave's second page contains Echo and Straight.
- Ripple expands radially without moving its center. Other patterns translate;
  Halftone keeps its radius field fixed while the dot lattice moves through it.
  Truchet moves its existing layout without reshuffling seeds. Every frame derives
  paint from immutable presets; paused animation does not reset on tab changes.
- Strongly minified or fine overlapping patterns can alias. This gallery uses
  the existing shaders and does not alter their filtering or parameter payloads.

For editable parameters and geometric transformations use the focused samples:
[Pattern gallery](PatternGallery.md), [Wave](Wave.md), [Ripple](Ripple.md),
[Weave](Weave.md), and [Truchet](Truchet.md).

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>
# include <array>
# include <cmath>
# include <span>

namespace
{
	struct Preset
	{
		StringView name;
		ColorF text;
		PatternParameters paint;
		Optional<PatternParameters> overlay = none;
	};

	using Layout = Pattern::Truchet::Layout;

	// Recipes use 420 x 104 drawing coordinates.
	const std::array<Preset, 8> HalftonePresets{{
		{ U"Glacier", ColorF{ 0.15, 0.24, 0.38 },
			Pattern::Halftone{ ColorF{ 0.66, 0.81, 0.97 }, ColorF{ 0.95, 0.97, 1.0 },
				9, 0, 4.5, 45_deg, { 0, 0 }, { 0, 34 }, { 0, 110 } } },
		{ U"Carbon", ColorF{ 0.88, 0.94, 1.0 },
			Pattern::Halftone{ ColorF{ 0.18, 0.25, 0.34 }, ColorF{ 0.055, 0.075, 0.105 },
				7, 0.8, 2.7, 0_deg, { 2, 1 }, { 0, -80 }, { 420, 130 } } },
		{ U"Acid", ColorF{ 0.06, 0.12, 0.08 },
			Pattern::Halftone{ ColorF{ 0.09, 0.18, 0.12 }, ColorF{ 0.82, 0.98, 0.28 },
				16, 0, 8, 45_deg, { 0, 0 }, { 235, 5 }, { 325, 110 } } },
		{ U"Violet", ColorF{ 0.97, 0.92, 1.0 },
			Pattern::Halftone{ ColorF{ 0.63, 0.40, 0.98 }, ColorF{ 0.13, 0.075, 0.24 },
				11, 0, 5.5, 15_deg, { 12, 8 }, { 170, 15 }, { 410, 95 } } },
		{ U"Coral", ColorF{ 0.23, 0.06, 0.10 },
			Pattern::Halftone{ ColorF{ 1.0, 0.92, 0.84 }, ColorF{ 0.97, 0.40, 0.37 },
				18, 0, 9, 45_deg, { 4, 0 }, { 240, 5 }, { 25, 125 } } },
		{ U"Blueprint", ColorF{ 0.96, 0.98, 1.0 },
			Pattern::Halftone{ ColorF{ 0.37, 0.64, 0.94 }, ColorF{ 0.10, 0.28, 0.66 },
				10, 0.5, 2.4, 0_deg, { 5, 4 }, { 0, -35 }, { 0, 145 } } },
		{ U"Prism", ColorF{ 0.95, 0.98, 1.0 },
			Pattern::Halftone{ ColorF{ 0.20, 0.90, 0.93, 0.85 }, ColorF{ 0.07, 0.11, 0.20 },
				13, 0, 6.5, 45_deg, { 0, 0 }, { 160, 0 }, { 390, 120 } },
			Pattern::Halftone{
				ColorF{ 1.0, 0.32, 0.65, 0.70 }, ColorF{ 0, 0 },
				13, 0, 6.5, 45_deg, { 6.5, 0 }, { 220, -5 }, { -10, 125 } } },
		{ U"Champagne", ColorF{ 0.31, 0.22, 0.13 },
			Pattern::Halftone{ ColorF{ 0.71, 0.51, 0.25 }, ColorF{ 0.99, 0.96, 0.88 },
				12, 0, 6, 45_deg, { 0, 0 }, { 125, 0 }, { -45, 65 } },
			Pattern::Halftone{
				ColorF{ 0.71, 0.51, 0.25 }, ColorF{ 0, 0 },
				12, 0, 6, 45_deg, { 0, 0 }, { 295, 0 }, { 465, 65 } } },
	}};

	const std::array<Preset, 6> PolkaDotPresets{{
		{ U"Rose Dots", ColorF{ 0.34, 0.13, 0.20 },
			Pattern::PolkaDot{ ColorF{ 0.90, 0.63, 0.70 }, ColorF{ 1.0, 0.91, 0.93 }, 15, 0.70, 45_deg } },
		{ U"Noir Dots", ColorF{ 0.94, 0.92, 0.84 },
			Pattern::PolkaDot{ ColorF{ 0.36, 0.32, 0.24 }, ColorF{ 0.10, 0.10, 0.095 }, 12, 0.38, 0_deg } },
		{ U"Soda", ColorF{ 0.08, 0.29, 0.36 },
			Pattern::PolkaDot{ ColorF{ 0.62, 0.88, 0.92 }, ColorF{ 0.88, 0.98, 0.98 }, 30, 1.0, 30_deg } },
		{ U"Stardust", ColorF{ 0.92, 0.92, 1.0 },
			Pattern::PolkaDot{ ColorF{ 0.40, 0.38, 0.61 }, ColorF{ 0.09, 0.075, 0.16 }, 7, 0.34, 45_deg } },
		{ U"Apricot", ColorF{ 0.40, 0.22, 0.14 },
			Pattern::PolkaDot{ ColorF{ 0.92, 0.65, 0.42 }, ColorF{ 1.0, 0.93, 0.81 }, 48, 0.82, 15_deg } },
		{ U"Caviar", ColorF{ 0.92, 1.0, 0.94 },
			Pattern::PolkaDot{ ColorF{ 0.07, 0.20, 0.18 }, ColorF{ 0.23, 0.44, 0.35 }, 20, 1.30, 45_deg } },
	}};

	const std::array<Preset, 6> StripePresets{{
		{ U"Silk", ColorF{ 0.36, 0.23, 0.15 },
			Pattern::Stripe{ ColorF{ 0.88, 0.79, 0.65 }, ColorF{ 0.99, 0.94, 0.84 }, 13, 0.28, 32_deg } },
		{ U"Racing", ColorF{ 1.0, 0.95, 0.86 },
			Pattern::Stripe{ ColorF{ 0.74, 0.23, 0.15 }, ColorF{ 0.89, 0.34, 0.22 }, 38, 0.90, -35_deg } },
		{ U"Pinstripe", ColorF{ 0.92, 0.96, 1.0 },
			Pattern::Stripe{ ColorF{ 0.27, 0.38, 0.52 }, ColorF{ 0.09, 0.15, 0.23 }, 10, 0.14, 0_deg } },
		{ U"Deckchair", ColorF{ 0.10, 0.31, 0.34 },
			Pattern::Stripe{ ColorF{ 0.48, 0.77, 0.76 }, ColorF{ 0.91, 0.98, 0.93 }, 64, 1.0, 90_deg } },
		{ U"Mulberry", ColorF{ 1.0, 0.91, 0.96 },
			Pattern::Stripe{ ColorF{ 0.39, 0.18, 0.33 }, ColorF{ 0.16, 0.065, 0.16 }, 24, 0.55, -60_deg } },
		{ U"Etched Gold", ColorF{ 0.37, 0.28, 0.15 },
			Pattern::Stripe{ ColorF{ 0.78, 0.66, 0.43 }, ColorF{ 0.98, 0.95, 0.84 }, 6, 0.40, 18_deg } },
	}};

	const std::array<Preset, 6> GridPresets{{
		{ U"Graphite", ColorF{ 0.88, 0.94, 0.96 },
			Pattern::Grid{ ColorF{ 0.22, 0.29, 0.33 }, ColorF{ 0.07, 0.10, 0.12 }, 24, 0.09, 0_deg },
			Pattern::Grid{ ColorF{ 0.39, 0.50, 0.55, 0.30 }, ColorF{ 0, 0 }, 96, 0.06, 0_deg } },
		{ U"Mint Grid", ColorF{ 0.10, 0.32, 0.26 },
			Pattern::Grid{ ColorF{ 0.51, 0.78, 0.65 }, ColorF{ 0.87, 0.97, 0.90 }, 23, 0.12, 0_deg } },
		{ U"Amethyst", ColorF{ 0.97, 0.92, 1.0 },
			Pattern::Grid{ ColorF{ 0.45, 0.31, 0.60 }, ColorF{ 0.19, 0.12, 0.30 }, 30, 0.16, 45_deg } },
		{ U"Microchip", ColorF{ 0.82, 1.0, 0.95 },
			Pattern::Grid{ ColorF{ 0.13, 0.36, 0.30 }, ColorF{ 0.035, 0.13, 0.12 }, 9, 0.16, 0_deg } },
		{ U"Terracotta", ColorF{ 0.42, 0.23, 0.19 },
			Pattern::Grid{ ColorF{ 0.84, 0.62, 0.51 }, ColorF{ 0.99, 0.92, 0.83 }, 56, 0.28, 0_deg } },
		{ U"Ice Lattice", ColorF{ 0.14, 0.28, 0.43 },
			Pattern::Grid{ ColorF{ 0.66, 0.81, 0.94 }, ColorF{ 0.94, 0.98, 1.0 }, 38, 0.10, 30_deg } },
	}};

	const std::array<Preset, 6> CheckerPresets{{
		{ U"Gingham", ColorF{ 0.19, 0.29, 0.48 },
			Pattern::Checker{ ColorF{ 0.65, 0.76, 0.91 }, ColorF{ 0.95, 0.97, 1.0 }, 14, 0.5, 0.5 } },
		{ U"Matcha", ColorF{ 0.14, 0.24, 0.09 },
			Pattern::Checker{ ColorF{ 0.66, 0.76, 0.46 }, ColorF{ 0.83, 0.89, 0.65 }, 24, 1, 1, 45_deg } },
		{ U"Copper Weave", ColorF{ 1.0, 0.90, 0.76 },
			Pattern::Checker{ ColorF{ 0.47, 0.28, 0.19 }, ColorF{ 0.18, 0.12, 0.10 }, 8, 0.65, 0.35, 45_deg } },
		{ U"Piano", ColorF{ 0.94, 0.95, 1.0 },
			Pattern::Checker{ ColorF{ 0.27, 0.29, 0.35 }, ColorF{ 0.09, 0.10, 0.14 }, 48, 1, 1 } },
		{ U"Peach Twill", ColorF{ 0.43, 0.24, 0.24 },
			Pattern::Checker{ ColorF{ 0.85, 0.63, 0.59 }, ColorF{ 1.0, 0.92, 0.85 }, 6, 0.70, 0.45, 45_deg } },
		{ U"Lavender Plaid", ColorF{ 0.31, 0.24, 0.45 },
			Pattern::Checker{ ColorF{ 0.70, 0.64, 0.84 }, ColorF{ 0.96, 0.93, 1.0 }, 36, 0.85, 0.30, 15_deg } },
	}};

	const std::array<Preset, 6> TrianglePresets{{
		{ U"Origami", ColorF{ 0.32, 0.21, 0.40 },
			Pattern::Triangle{ ColorF{ 0.81, 0.70, 0.87 }, ColorF{ 0.95, 0.89, 0.98 }, 32, 15_deg } },
		{ U"Tidal", ColorF{ 0.90, 1.0, 0.98 },
			Pattern::Triangle{ ColorF{ 0.11, 0.42, 0.43 }, ColorF{ 0.07, 0.27, 0.32 }, 23, 30_deg } },
		{ U"Confetti", ColorF{ 0.44, 0.22, 0.26 },
			Pattern::Triangle{ ColorF{ 0.92, 0.66, 0.68 }, ColorF{ 1.0, 0.92, 0.88 }, 9, 0_deg } },
		{ U"Obsidian", ColorF{ 0.94, 0.93, 1.0 },
			Pattern::Triangle{ ColorF{ 0.27, 0.24, 0.36 }, ColorF{ 0.10, 0.085, 0.16 }, 64, -15_deg } },
		{ U"Saffron", ColorF{ 0.40, 0.27, 0.08 },
			Pattern::Triangle{ ColorF{ 0.91, 0.71, 0.29 }, ColorF{ 1.0, 0.95, 0.73 }, 44, 30_deg } },
		{ U"Frost", ColorF{ 0.19, 0.32, 0.44 },
			Pattern::Triangle{ ColorF{ 0.71, 0.83, 0.93 }, ColorF{ 0.95, 0.98, 1.0 }, 16, 45_deg } },
	}};

	const std::array<Preset, 6> HexGridPresets{{
		{ U"Honeycomb", ColorF{ 0.35, 0.22, 0.08 },
			Pattern::HexGrid{ ColorF{ 0.77, 0.53, 0.21 }, ColorF{ 1.0, 0.90, 0.62 }, 18, 0.20, 0_deg } },
		{ U"Ion", ColorF{ 0.86, 1.0, 0.94 },
			Pattern::HexGrid{ ColorF{ 0.20, 0.58, 0.44 }, ColorF{ 0.055, 0.14, 0.15 }, 17, 0.32, 30_deg } },
		{ U"Pollen", ColorF{ 0.40, 0.30, 0.12 },
			Pattern::HexGrid{ ColorF{ 0.85, 0.73, 0.43 }, ColorF{ 0.99, 0.96, 0.83 }, 8, 0.30, 15_deg } },
		{ U"Deep Space", ColorF{ 0.90, 0.92, 1.0 },
			Pattern::HexGrid{ ColorF{ 0.36, 0.30, 0.57 }, ColorF{ 0.09, 0.07, 0.17 }, 38, 0.08, 0_deg } },
		{ U"Clay Cells", ColorF{ 0.42, 0.24, 0.20 },
			Pattern::HexGrid{ ColorF{ 0.86, 0.64, 0.53 }, ColorF{ 1.0, 0.93, 0.85 }, 28, 0.48, 30_deg } },
		{ U"Sea Glass", ColorF{ 0.12, 0.32, 0.31 },
			Pattern::HexGrid{ ColorF{ 0.49, 0.75, 0.67 }, ColorF{ 0.89, 0.98, 0.93 }, 24, 0.16, -15_deg } },
	}};

	const std::array<Preset, 8> LayersPresets{{
		{ U"Woven Light", ColorF{ 0.92, 0.98, 1.0 },
			Pattern::Weave{ ColorF{ 0.20, 0.33, 0.43 }, ColorF{ 0.055, 0.10, 0.17 }, 22, 10, 2, 45_deg },
			Pattern::Halftone{ ColorF{ 0.48, 0.91, 0.80, 0.65 }, ColorF{ 0, 0 },
				11, 0, 5, 45_deg, { 0, 0 }, { 185, 0 }, { 410, 125 } } },
		{ U"Pearl Circuit", ColorF{ 0.35, 0.24, 0.25 },
			Pattern::Grid{ ColorF{ 0.80, 0.67, 0.66 }, ColorF{ 0.99, 0.93, 0.90 }, 28, 0.13 },
			Pattern::PolkaDot{ ColorF{ 0.65, 0.42, 0.43 }, ColorF{ 0, 0 }, 28, 0.28 } },
		{ U"Tartan", ColorF{ 0.99, 0.94, 0.79 },
			Pattern::Stripe{ ColorF{ 0.43, 0.16, 0.22 }, ColorF{ 0.09, 0.14, 0.20 }, 46, 1.0, 0_deg },
			Pattern::Stripe{ ColorF{ 0.84, 0.63, 0.35, 0.44 }, ColorF{ 0, 0 }, 29, 0.75, 90_deg } },
		{ U"Interference", ColorF{ 0.91, 0.97, 1.0 },
			Pattern::Stripe{ ColorF{ 0.14, 0.47, 0.51 }, ColorF{ 0.07, 0.12, 0.20 }, 9, 0.55, 30_deg },
			Pattern::Stripe{ ColorF{ 0.70, 0.36, 0.85, 0.72 }, ColorF{ 0, 0 }, 9.4, 0.50, 35_deg } },
		{ U"Ember Mesh", ColorF{ 1.0, 0.91, 0.77 },
			Pattern::HexGrid{ ColorF{ 0.38, 0.24, 0.22 }, ColorF{ 0.14, 0.09, 0.14 }, 24, 0.28, 0_deg },
			Pattern::Halftone{ ColorF{ 1.0, 0.47, 0.18, 0.85 }, ColorF{ 0, 0 },
				14, 0, 6, 30_deg, { 0, 0 }, { 170, 5 }, { 400, 110 } } },
		{ U"Kaleidoscope", ColorF{ 0.23, 0.22, 0.40 },
			Pattern::Triangle{ ColorF{ 0.80, 0.67, 0.93 }, ColorF{ 0.96, 0.91, 0.99 }, 31, 0_deg },
			Pattern::Triangle{ ColorF{ 0.20, 0.65, 0.73, 0.32 }, ColorF{ 0, 0 }, 31, 60_deg, { 0.5, 0.25 } } },
		{ U"Solar Static", ColorF{ 1.0, 0.95, 0.84 },
			Pattern::Halftone{ ColorF{ 0.99, 0.73, 0.27 }, ColorF{ 0.15, 0.13, 0.23 },
				17, 0, 8.5, 45_deg, { 0, 0 }, { 150, 0 }, { 395, 110 } },
			Pattern::Stripe{ ColorF{ 0.87, 0.77, 1.0, 0.24 }, ColorF{ 0, 0 }, 9, 0.25, -45_deg } },
		{ U"Petal Cut", ColorF{ 0.36, 0.16, 0.17 },
			Pattern::PolkaDot{ ColorF{ 0.91, 0.55, 0.47 }, ColorF{ 1.0, 0.91, 0.81 }, 28, 1.25, 0_deg },
			Pattern::Stripe{ ColorF{ 1.0, 0.91, 0.81 }, ColorF{ 0, 0 }, 28, 1.0, 90_deg, { 0.25, 0 } } },
	}};

	const std::array<Preset, 10> WavePresets{{
		{ U"Porcelain", ColorF{ 0.16, 0.30, 0.46 },
			Pattern::Wave{ ColorF{ 0.65, 0.79, 0.90 }, ColorF{ 0.96, 0.98, 1 }, 18, 2, 4, 110 } },
		{ U"Midnight", ColorF{ 0.9, 0.95, 1 },
			Pattern::Wave{ ColorF{ 0.16, 0.27, 0.42 }, ColorF{ 0.045, 0.07, 0.12 }, 27, 8, 9, 100, -20_deg } },
		{ U"Citrus", ColorF{ 0.15, 0.23, 0.08 },
			Pattern::Wave{ ColorF{ 0.67, 0.84, 0.19 }, ColorF{ 0.95, 0.98, 0.72 }, 38, 15, 14, 100, -15_deg } },
		{ U"Lilac", ColorF{ 0.32, 0.21, 0.46 },
			Pattern::Wave{ ColorF{ 0.72, 0.59, 0.88 }, ColorF{ 0.96, 0.92, 1 }, 22, 5, 7, 70, 30_deg } },
		{ U"Coral", ColorF{ 0.47, 0.16, 0.18 },
			Pattern::Wave{ ColorF{ 0.98, 0.50, 0.44 }, ColorF{ 1, 0.88, 0.80 }, 42, 18, 12, 125, 15_deg } },
		{ U"Lagoon", ColorF{ 0.85, 1, 0.97 },
			Pattern::Wave{ ColorF{ 0.12, 0.58, 0.52 }, ColorF{ 0.02, 0.22, 0.25 }, 30, 8, 10, 90, -35_deg } },
		{ U"Gold Thread", ColorF{ 1, 0.90, 0.62 },
			Pattern::Wave{ ColorF{ 0.60, 0.44, 0.20 }, ColorF{ 0.11, 0.095, 0.09 }, 14, 1.5, 3, 110, 12_deg } },
		{ U"Cross Current", ColorF{ 0.88, 0.95, 1 },
			Pattern::Wave{ ColorF{ 0.18, 0.32, 0.49 }, ColorF{ 0.055, 0.10, 0.18 }, 32, 7, 7, 100, 20_deg },
			Pattern::Wave{ ColorF{ 0.40, 0.69, 0.79, 0.5 }, ColorF{ 0, 0 }, 32, 3, 7, 100, -20_deg } },
		{ U"Echo", ColorF{ 0.34, 0.22, 0.44 },
			Pattern::Wave{ ColorF{ 0.75, 0.60, 0.81 }, ColorF{ 0.96, 0.92, 0.97 }, 36, 8, 9, 100 },
			Pattern::Wave{ ColorF{ 0.48, 0.32, 0.62, 0.65 }, ColorF{ 0, 0 }, 36, 2, 9, 100, 0_deg, { 0, 9 } } },
		{ U"Straight", ColorF{ 0.89, 0.94, 1 },
			Pattern::Wave{ ColorF{ 0.22, 0.33, 0.49 }, ColorF{ 0.10, 0.16, 0.27 }, 22, 5, 0, 80, -25_deg } },
	}};

	const std::array<Preset, 7> RipplePresets{{
		{ U"Porcelain", ColorF{ 0.16, 0.30, 0.46 },
			Pattern::Ripple{ ColorF{ 0.62, 0.77, 0.90 }, ColorF{ 0.96, 0.98, 1 }, 16, 2, { 430, 320 } } },
		{ U"Eclipse", ColorF{ 0.9, 0.95, 1 },
			Pattern::Ripple{ ColorF{ 0.20, 0.32, 0.49 }, ColorF{ 0.045, 0.07, 0.12 }, 28, 9, { 280, 55 }, 14 } },
		{ U"Sunrise", ColorF{ 0.30, 0.25, 0.08 },
			Pattern::Ripple{ ColorF{ 0.96, 0.69, 0.20 }, ColorF{ 1, 0.95, 0.74 }, 36, 16, { 190, 240 } } },
		{ U"Coral", ColorF{ 0.47, 0.16, 0.18 },
			Pattern::Ripple{ ColorF{ 0.98, 0.50, 0.44 }, ColorF{ 1, 0.88, 0.80 }, 42, 18, { -80, -90 } } },
		{ U"Orbit", ColorF{ 0.34, 0.22, 0.44 },
			Pattern::Ripple{ ColorF{ 0.73, 0.59, 0.84 }, ColorF{ 0.96, 0.92, 0.98 }, 32, 7, { 265, 68 } },
			Pattern::Ripple{ ColorF{ 0.48, 0.32, 0.62, 0.7 }, ColorF{ 0, 0 }, 32, 2, { 265, 68 }, 9 } },
		{ U"Sonar", ColorF{ 0.80, 1, 0.92 },
			Pattern::Ripple{ ColorF{ 0.13, 0.60, 0.42 }, ColorF{ 0.025, 0.13, 0.12 }, 18, 2, { 275, 65 } } },
		{ U"Interference", ColorF{ 0.86, 0.95, 1 },
			Pattern::Ripple{ ColorF{ 0.18, 0.32, 0.49 }, ColorF{ 0.055, 0.10, 0.18 }, 22, 5, { 55, 60 } },
			Pattern::Ripple{ ColorF{ 0.40, 0.69, 0.79, 0.5 }, ColorF{ 0, 0 }, 22, 3, { 315, 60 } } },
	}};

	const std::array<Preset, 7> WeavePresets{{
		{ U"Linen", ColorF{ 0.35, 0.29, 0.22 },
			Pattern::Weave{ ColorF{ 0.77, 0.70, 0.59 }, ColorF{ 0.96, 0.94, 0.88 }, 20, 7, 2 } },
		{ U"Cobalt", ColorF{ 0.89, 0.95, 1 },
			Pattern::Weave{ ColorF{ 0.23, 0.39, 0.62 }, ColorF{ 0.045, 0.09, 0.17 }, 30, 16, 3, 15_deg } },
		{ U"Basket", ColorF{ 0.39, 0.24, 0.09 },
			Pattern::Weave{ ColorF{ 0.79, 0.55, 0.26 }, ColorF{ 0.99, 0.90, 0.72 }, 36, 20, 8 } },
		{ U"Rose", ColorF{ 0.47, 0.20, 0.27 },
			Pattern::Weave{ ColorF{ 0.87, 0.62, 0.68 }, ColorF{ 1, 0.91, 0.91 }, 28, 14, 3, 45_deg } },
		{ U"Carbon", ColorF{ 0.88, 0.93, 0.98 },
			Pattern::Weave{ ColorF{ 0.22, 0.27, 0.34 }, ColorF{ 0.075, 0.095, 0.13 }, 20, 12, 2, 45_deg } },
		{ U"Oblique", ColorF{ 0.11, 0.33, 0.32 },
			Pattern::Weave{ ColorF{ 0.35, 0.66, 0.59 }, ColorF{ 0.88, 0.96, 0.89 }, 34, 18, 4, -25_deg } },
		{ U"Grid", ColorF{ 0.34, 0.23, 0.45 },
			Pattern::Weave{ ColorF{ 0.72, 0.59, 0.84 }, ColorF{ 0.96, 0.92, 0.98 }, 24, 6, 0, 12_deg } },
	}};

	const std::array<Preset, 8> TruchetPresets{{
		{ U"Porcelain", ColorF{ 0.16, 0.30, 0.46 },
			Pattern::Truchet{ ColorF{ 0.62, 0.77, 0.90 }, ColorF{ 0.96, 0.98, 1 }, 40, 3, 0_deg, {}, Layout::Uniform } },
		{ U"Midnight", ColorF{ 0.9, 0.95, 1 },
			Pattern::Truchet{ ColorF{ 0.22, 0.36, 0.55 }, ColorF{ 0.045, 0.07, 0.12 }, 42, 10, 15_deg, {}, Layout::Random, 1 } },
		{ U"Citrus", ColorF{ 0.19, 0.28, 0.07 },
			Pattern::Truchet{ ColorF{ 0.69, 0.84, 0.20 }, ColorF{ 0.97, 0.98, 0.74 }, 48, 16, 0_deg, {}, Layout::Random, 17 } },
		{ U"Loops", ColorF{ 0.47, 0.20, 0.27 },
			Pattern::Truchet{ ColorF{ 0.87, 0.58, 0.67 }, ColorF{ 1, 0.91, 0.91 }, 34, 7, 0_deg, {}, Layout::Alternating } },
		{ U"Labyrinth", ColorF{ 0.88, 0.93, 0.98 },
			Pattern::Truchet{ ColorF{ 0.25, 0.32, 0.40 }, ColorF{ 0.06, 0.085, 0.12 }, 24, 6, 45_deg, {}, Layout::Random, 0x7FC00001u } },
		{ U"Jade", ColorF{ 0.11, 0.33, 0.32 },
			Pattern::Truchet{ ColorF{ 0.35, 0.66, 0.59 }, ColorF{ 0.88, 0.96, 0.89 }, 54, 13, -25_deg, {}, Layout::Uniform } },
		{ U"Pearls", ColorF{ 0.34, 0.23, 0.45 },
			Pattern::Truchet{ ColorF{ 0.72, 0.59, 0.84 }, ColorF{ 0.96, 0.92, 0.98 }, 30, 12, 45_deg, {}, Layout::Alternating } },
		{ U"Orbit", ColorF{ 0.91, 0.98, 1.0 },
			Pattern::Truchet{ ColorF{ 0.24, 0.69, 0.78 }, ColorF{ 0.065, 0.15, 0.22 },
				18.4, 3.3, 0_deg, { 0, 18.4 }, Layout::Alternating } },
	}};

	PatternParameters ScrollPattern(PatternParameters paint, const Float2 displacement)
	{
		const auto& m = paint.uvTransform;
		const Float2 shift{
			m._11 * displacement.x + m._21 * displacement.y,
			m._12 * displacement.x + m._22 * displacement.y };
		paint.uvTransform._31 -= shift.x;
		paint.uvTransform._32 -= shift.y;
		if (paint.type == PatternType::Halftone)
		{
			// Keep the drawing-space radius field fixed while moving the lattice.
			paint.extraParams.z += (paint.extraParams.x * shift.x + paint.extraParams.y * shift.y);
		}
		return paint;
	}

	struct Collection
	{
		StringView name;
		std::span<const Preset> presets;
	};

	const std::array<Collection, 12> Collections{{
		{ U"Halftone", HalftonePresets },
		{ U"PolkaDot", PolkaDotPresets },
		{ U"Stripe", StripePresets },
		{ U"Grid", GridPresets },
		{ U"Checker", CheckerPresets },
		{ U"Triangle", TrianglePresets },
		{ U"HexGrid", HexGridPresets },
		{ U"Wave", WavePresets },
		{ U"Ripple", RipplePresets },
		{ U"Weave", WeavePresets },
		{ U"Truchet", TruchetPresets },
		{ U"Layers", LayersPresets },
	}};

	constexpr size_t CardsPerPage = 8;

	PatternParameters AnimatePattern(PatternParameters paint, const double time)
	{
		if (paint.type == PatternType::Ripple)
		{
			paint.param1 += static_cast<float>(std::fmod(time * 0.35, 1.0));
			return paint;
		}
		return ScrollPattern(paint, Float2{
			static_cast<float>(28.0 * std::sin(time * 0.55)),
			static_cast<float>(18.0 * std::sin(time * 0.31)) });
	}

	bool Button(const Font& font, const StringView text, const RectF& rect, const bool selected = false)
	{
		const bool hovered = rect.mouseOver();
		if (hovered) { Cursor::RequestStyle(CursorStyle::Hand); }
		rect.rounded(8).draw(selected ? ColorF{ 0.22, 0.35, 0.47 }
			: ColorF{ hovered ? 0.19 : 0.13, hovered ? 0.22 : 0.15, hovered ? 0.27 : 0.19 });
		font(text).drawAt(rect.center(), selected ? ColorF{ 0.96, 0.98, 1 } : ColorF{ 0.76, 0.82, 0.89 });
		return rect.leftClicked();
	}
}

void Main()
{
	Window::Resize(960, 700);
	Window::SetTitle(U"Pattern collection");
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(960, 700);
	Scene::SetBackground(ColorF{ 0.075, 0.085, 0.105 });
	const Font label{ 30, Typeface::Bold };
	const Font ui{ 16, Typeface::Medium };
	const auto shape = RectF{ 0, 0, 420, 104 }.rounded(20);
	const auto& args = System::GetCommandLineArgs();
	const bool captureAll = args.contains(U"--capture-all");
	const bool capture = (captureAll || args.contains(U"--capture"));
	bool animate = args.contains(U"--animate");
	double time = 0;
	size_t collectionIndex = 0, page = 0;
	int32 frames = 0;
	for (const auto& arg : args)
	{
		for (size_t i = 0; i < Collections.size(); ++i)
		{
			if (arg == Format(U"--collection=", Collections[i].name)) { collectionIndex = i; }
		}
		if (arg.starts_with(U"--page="))
		{
			if (const auto n = ParseOpt<uint32>(arg.substr(7)); n && (*n > 0)) { page = (*n - 1); }
		}
	}
	page = Min(page, (Collections[collectionIndex].presets.size() - 1) / CardsPerPage);
	if (captureAll) { collectionIndex = page = 0; }

	while (System::Update())
	{
		if (capture && ScreenCapture::HasNewFrame())
		{
			ScreenCapture::GetFrame().save(Format(U"pattern-collection-", collectionIndex, U"-", page, U".png"));
			if (not captureAll) { return; }
			++page;
			if (page * CardsPerPage >= Collections[collectionIndex].presets.size())
			{
				page = 0;
				if (++collectionIndex == Collections.size()) { return; }
			}
			frames = 0;
			time = 0;
		}

		if (KeyLeft.down()) { collectionIndex = (collectionIndex + Collections.size() - 1) % Collections.size(); page = 0; }
		if (KeyRight.down()) { collectionIndex = (collectionIndex + 1) % Collections.size(); page = 0; }
		for (size_t i = 0; i < Collections.size(); ++i)
		{
			if (Button(ui, Collections[i].name, RectF{ 36 + (i % 6) * 150, 20 + (i / 6) * 40, 138, 32 }, collectionIndex == i))
			{
				collectionIndex = i;
				page = 0;
			}
		}
		const auto& collection = Collections[collectionIndex];
		const size_t pageCount = (collection.presets.size() + CardsPerPage - 1) / CardsPerPage;
		if (pageCount > 1)
		{
			const bool previous = Button(ui, U"<", RectF{ 36, 644, 36, 32 });
			const bool next = Button(ui, U">", RectF{ 148, 644, 36, 32 });
			if (previous || KeyPageUp.down()) { page = (page + pageCount - 1) % pageCount; }
			if (next || KeyPageDown.down()) { page = (page + 1) % pageCount; }
			ui(page + 1, U" / ", pageCount).drawAt(Vec2{ 110, 660 }, ColorF{ 0.76, 0.82, 0.89 });
		}
		const bool toggle = Button(ui, animate ? U"Pause" : U"Play", RectF{ 736, 644, 88, 32 }, animate);
		const bool reset = Button(ui, U"Reset", RectF{ 836, 644, 88, 32 });
		if (toggle || KeySpace.down()) { animate = !animate; }
		if (reset || KeyR.down()) { animate = false; time = 0; }
		if (animate) { time += Scene::DeltaTime(); }

		const auto visible = collection.presets.subspan(page * CardsPerPage,
			Min(CardsPerPage, collection.presets.size() - page * CardsPerPage));
		for (size_t i = 0; i < visible.size(); ++i)
		{
			const auto& preset = visible[i];
			const Transformer2D transform{ Mat3x2::Translate(36 + (i % 2) * 468, 126 + (i / 2) * 120) };
			RectF{ 0, 5, 420, 104 }.rounded(20).draw(ColorF{ 0, 0.25 });
			shape.draw(AnimatePattern(preset.paint, time));
			if (preset.overlay) { shape.draw(AnimatePattern(*preset.overlay, time)); }
			shape.drawFrame(0, 1.5, ColorF{ 0.85, 0.92, 1, 0.7 });
			label(preset.name).draw(30, 30, preset.text);
		}
		if (capture && (++frames == (animate ? 90 : 3))) { ScreenCapture::RequestCurrentFrame(); }
	}
}

```
