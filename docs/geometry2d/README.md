# Geometry2D

API contracts live in the public headers, including
[Raycast](../../Siv3D/include/Siv3D/Geometry2D/Raycast.hpp),
[distance queries](../../Siv3D/include/Siv3D/Geometry2D/Distance.hpp), and
[boundary distance queries](../../Siv3D/include/Siv3D/Geometry2D/SignedDistance.hpp).

- [Continuing a ray after contact](proposals/raycast-continuation.md): investigated
  limitations and an unadopted search-range extension.
- [Raycast continuation diagnostic](../../Test/Manual/RaycastContinuation.md):
  reproducible reflection, sliding, corner, and numerical-scale cases.
