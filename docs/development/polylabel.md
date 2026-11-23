# polylabel maintenance

Siv3D bundles the C++ header from
[polylabel v2.0.1](https://github.com/mapbox/polylabel/tree/v2.0.1).
Upstream [removed the unmaintained C++ implementation](https://github.com/mapbox/polylabel/pull/124)
after that release because it had fallen behind the JavaScript implementation.
The v2.1.0 release contains JavaScript only. Its fixes and performance claims
do not describe the bundled C++ implementation.

Keep [ThirdParty.md](../../ThirdParty.md) linked to the release containing the
C++ source. Do not advance its version solely because the JavaScript package
has a newer release. The v2.0.1 C++ header matches the last upstream copy before
removal. Porting later JavaScript changes or choosing another C++ implementation
requires a separate implementation review and validation.

## Local integration

The only local changes in
[polylabel.hpp](../../Siv3D/src/ThirdParty/mapbox/polylabel.hpp) are its four
geometry includes, which use `<ThirdParty/mapbox/geometry/...>` instead of
`<mapbox/geometry/...>`. Preserve these paths when comparing or replacing the
header. The adjacent geometry headers belong to the separately versioned
geometry.hpp dependency.

[SivGeometry2D_PoleOfInaccessibility.cpp](../../Siv3D/src/Siv3D/Geometry2D/SivGeometry2D_PoleOfInaccessibility.cpp)
copies the supplied points into one outer ring and calls `mapbox::polylabel<double>`.
It does not pass holes or enable upstream debug output. Preserve the ISC notice
and the 2016 Mapbox attribution in
[LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp).

## Validation

Documentation-only updates need source/license comparison and the repository's
documentation checks. If the algorithm is replaced, add focused tests for
analytic shapes, concave polygons, reversed vertex order, translation, scaling,
and positive precision values, then run the [host test workflow](README.md#build-and-test).
Check distance to the outline when several equally good result coordinates
are possible. Review empty-input and precision contracts separately instead of
silently introducing new runtime guards during a dependency update.
