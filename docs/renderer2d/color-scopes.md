# Scoped color multiplication and addition

[ScopedColorMul2D](../../Siv3D/include/Siv3D/ScopedColorMul2D.hpp) and
[ScopedColorAdd2D](../../Siv3D/include/Siv3D/ScopedColorAdd2D.hpp) let a drawing
function add its own tint, fade, or highlight while retaining the caller's color
settings. The public headers own their contracts.

## Nesting and independent accumulation

Both scopes default to `Target::Push`. Multiplication accumulates component-wise
in RGBA; addition accumulates component-wise in RGB. For example, a panel fade
and an icon fade can be specified independently:

```cpp
const ScopedColorMul2D panelFade{ 1.0, 0.5 };
{
    const ScopedColorMul2D iconFade{ 1.0, 0.5 };
    texture.draw(); // Effective alpha multiplier: 0.25
}
texture.draw(); // Effective alpha multiplier: 0.5
```

Mul and Add accumulate independently. Their relative declaration order does not
change the resulting color settings. In particular, a surrounding Mul does not
scale an inner Add:

```cpp
const ScopedColorMul2D dim{ 0.5 };
const ScopedColorAdd2D highlight{ 0.2 };
Rect{ 100, 100 }.draw(ColorF{ 0.4 }); // RGB: 0.4 * 0.5 + 0.2 = 0.4
```

The standard shape and texture shaders apply multiplication before addition.
The scopes aggregate those two settings; they do not build an ordered chain of
color filters. Intermediate scope values are not clamped, so negative additions
and coefficients above one retain their meaning. `Graphics2D::GetColorMul()` and
`GetColorAdd()` return the effective settings, including the active scopes.

## Replacing and isolating settings

Use `Target::Set` to replace the current setting until the scope ends. It changes
only that kind of color setting. To isolate a drawing function from both:

```cpp
{
    const ScopedColorMul2D mul{
        Palette::White, ScopedColorMul2D::Target::Set };
    const ScopedColorAdd2D add{
        0.0, ScopedColorAdd2D::Target::Set };

    texture.draw();
} // Both previous settings are restored.
```

A Push inside this block starts from the replaced settings. White multiplication
and zero addition without `Target::Set` retain the parent's effects. Numeric and
`Float3` Mul constructors also accept `Target` without an explicit alpha, using
an alpha multiplier of one.

Keep each scope alive while drawing and end scopes in reverse construction
order. Default-constructed scopes do not change state. Moving a scope transfers
its restoration responsibility. Restoration uses the saved setting, so a zero
multiplier does not prevent the parent from being restored.

## Premultiplied alpha and drawing coverage

Pass ordinary, unpremultiplied color values. The standard shaders perform the
PMA conversion. Add leaves drawing alpha unchanged and ignores the alpha
component of `ColorF` and `Float4`; its RGB contribution is weighted by the
drawn fragment's alpha.

An alpha multiplier applies to individual draws. It is not the opacity of a
flattened group: overlapping translucent draws can remain visible through one
another. Custom shaders must consume the effective color constants themselves.
MSDF outline and shadow colors also receive ColorMul, so a scoped alpha fade
affects the text fill and its decorations together. See
[MSDF color composition](msdf-font.md#scoped-color-multiplication) for style colors
and shader-side multiplication.

## Migration from v0.6

- Nested scopes previously replaced the same kind of setting. Add `Target::Set`
  where an absolute value or a temporary reset is intended.
- Remove manual accumulation such as
  `ScopedColorMul2D{ Graphics2D::GetColorMul() * local }`; pass `local` directly
  with Push. Apply the same change to manually summed Add values.
- Add now changes RGB only. Old numeric alpha arguments are no longer accepted,
  and a `ColorF` alpha is ignored. In particular, `ScopedColorAdd2D{ 0.2 }` and
  `ScopedColorAdd2D{ ColorF{ 0.2 } }` now have the same effect.

## Implementation and validation

Scopes combine their arguments with the saved current values on the CPU. The
renderer setters still replace the effective state, and destruction restores
the saved value directly. The backend command format, constant-buffer layout,
and shaders do not need a per-scope stack. State changes can still split draws
when nesting changes the effective value.

[Test_ScopedColor2D.cpp](../../Test/Test_ScopedColor2D.cpp) checks nested Push and
Set, independent ordering, constructor forms, neutral and zero values, negative
and above-one additions, move ownership, exception unwinding, and exact state
restoration. GPU captures compare nested scopes with explicit effective states
for shapes and a translucent PMA texture, including transparent texels, resets,
and a flush inside a scope. Existing scene-isolation helpers use explicit Set.
Run the focused `ScopedColor2D.*` cases and the full host suite using the
[development guide](../development/README.md#build-and-test).
