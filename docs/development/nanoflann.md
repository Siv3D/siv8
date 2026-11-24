# nanoflann maintenance

Siv3D bundles the single upstream
[nanoflann.hpp](../../Siv3D/include/ThirdParty/nanoflann/nanoflann.hpp) header.
[ThirdParty.md](../../ThirdParty.md) records the release version.
[KDTree.hpp](../../Siv3D/include/Siv3D/KDTree.hpp) and
[KDTree.ipp](../../Siv3D/include/Siv3D/detail/KDTree.ipp) integrate its static
`KDTreeSingleIndexAdaptor` with `L2_Simple_Adaptor` and Siv3D result arrays.
The manifold metrics, Eigen adaptor, and incremental index are not used by
this wrapper.

## Updating

Compare the bundled header with the recorded upstream tag before replacing it.
The current header has no local patches. Keep LF line endings, embedded notices,
the existing include layout, and the wrapper's MSVC warning suppression.
Update `ThirdParty.md` and compare upstream's `COPYING` and header notices with
the nanoflann entry in
[LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp).

Check the wrapper's custom result adapters against upstream's search loop.
They rely on `DistanceType`, `IndexType`, `worstDist()`, and `sort()`, and on the
search loop filtering by `dist < worstDist()` before calling `addPoint()`.
Keep squared-distance results and avoid assuming a particular order for ties
or unsorted radius results. Preserve the explicit tree argument required by
`freeIndex()` and `usedMemory()`. The latter's accounting and retained memory
can change with upstream's node storage; releasing nodes does not imply a
reported memory usage of zero.

Major-version changes should be checked against this Euclidean integration
before selecting a fallback release. Removal of an unused upstream metric does
not by itself require changing Siv3D's public API. Rebuild all consumers after
replacing the header; internal node layout is not a compatibility guarantee.

## Validation

[Test_KDTree.cpp](../../Test/Test_KDTree.cpp) instantiates the public search
overloads with float and double coordinates and checks results against direct
distance calculations. Coverage includes radius sorting and boundaries, empty
datasets, result reuse, rebuilding after dataset changes, and memory lifecycle.
Tests provide the dataset adapter's `GetElement()` and `GetPointer()` functions.

Use `--test-case=KDTree.*` for focused iteration, then run the full suite with
the [host test workflow](README.md#build-and-test). These checks exercise Siv3D's
static Euclidean path; they do not establish correctness of every optional
upstream metric, incremental index, or another platform's compiler.
