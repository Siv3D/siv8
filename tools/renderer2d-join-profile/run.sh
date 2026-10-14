#!/bin/bash
set -euo pipefail
if [[ "$(uname -s)" != Darwin ]]; then
  echo "This workflow requires macOS and Xcode command-line tools." >&2
  exit 1
fi
readonly repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
readonly mode="${1:-timing}"
flags=(-O3 -DNDEBUG)
case "${mode}" in
  timing) ;;
  allocations) flags+=(-DSIV3D_JOIN_COUNT_ALLOCATIONS) ;;
  debug) flags=(-O0 -g -DDEBUG=1 -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG) ;;
  *) echo "Usage: $0 [timing|allocations|debug] [output-directory]" >&2; exit 2 ;;
esac
if [[ $# -ge 2 ]]; then
  mkdir -p -- "$2"
  output_dir="$(cd -- "$2" && pwd)"
else
  output_dir="$(mktemp -d "${TMPDIR:-/tmp}/siv8-join-profile.XXXXXX")"
fi
readonly output_dir
printf 'Artifacts: %s\n' "${output_dir}"
xcrun clang++ -std=c++23 "${flags[@]}" \
  -I "${repo_dir}/Siv3D/include" -I "${repo_dir}/Siv3D/include/ThirdParty" \
  -I "${repo_dir}/Siv3D/src" -isystem "${repo_dir}/Dependencies/boost_1_88_0" \
  "${repo_dir}/tools/renderer2d-join-profile/profile.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder_Rounded.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder_StraightEdged.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/Quad/SivQuad.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/Line/SivLine.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/RectF/SivRectF.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/Geometry2D/SivGeometry2D_IntersectsAt.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/Polygon/GeometryCommon.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/Geometry2D/SivGeometry2D_Intersects.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/PolynomialSolver/SivPolynomialSolver.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/Polygon/SivPolygon.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/Polygon/PolygonDetail.cpp" \
  -Wl,-dead_strip -o "${output_dir}/join-profile"
if [[ "${mode}" == debug ]]; then
  "${output_dir}/join-profile" scan 500 > "${output_dir}/debug.csv" 2> "${output_dir}/debug-counts.csv"
else
  "${output_dir}/join-profile" > "${output_dir}/${mode}.csv" 2> "${output_dir}/${mode}-counts.csv"
fi
printf 'Results: %s/%s.csv\n' "${output_dir}" "${mode}"
