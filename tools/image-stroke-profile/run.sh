#!/bin/bash
# Isolated stroke phases; no profiling hooks in the engine binary.
set -euo pipefail
if [[ "$(uname -s)" != Darwin ]]; then
  echo "This workflow requires macOS and the Xcode command-line tools." >&2
  exit 1
fi
readonly repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
readonly mode="${1:-timing}"
flags=(-O3 -DNDEBUG)
workload=strokes
case "${mode}" in
  timing) ;;
  frames-timing) workload=frames ;;
  frames-allocations) workload=frames; flags+=(-DSIV3D_STROKE_COUNT_ALLOCATIONS) ;;
  allocations) flags+=(-DSIV3D_STROKE_COUNT_ALLOCATIONS) ;;
  *) echo "Usage: $0 [timing|allocations|frames-timing|frames-allocations] [output-directory]" >&2; exit 2 ;;
esac
if [[ $# -ge 2 ]]; then
  mkdir -p -- "$2"
  output_dir="$(cd -- "$2" && pwd)"
else
  output_dir="$(mktemp -d "${TMPDIR:-/tmp}/siv8-stroke-profile.XXXXXX")"
fi
readonly output_dir
printf 'Artifacts: %s\n' "${output_dir}"
xcrun clang++ -std=c++23 "${flags[@]}" \
  -I "${repo_dir}/Siv3D/include" -I "${repo_dir}/Siv3D/include/ThirdParty" \
  -I "${repo_dir}/Siv3D/src" \
  "${repo_dir}/tools/image-stroke-profile/profile.cpp" \
  "${repo_dir}/Siv3D/src/Siv3D/ImagePixel/SivImagePixel.cpp" \
  -Wl,-dead_strip -o "${output_dir}/stroke-profile"
"${output_dir}/stroke-profile" "${workload}" > "${output_dir}/${mode}.csv"
printf 'Results: %s/%s.csv\n' "${output_dir}" "${mode}"
