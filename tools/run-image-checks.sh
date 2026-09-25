#!/bin/bash
# Instrument the real Image implementation without starting the engine.
set -euo pipefail

if [[ "$(uname -s)" != Darwin ]]; then
	echo "This workflow requires macOS and the Xcode command-line tools." >&2
	exit 1
fi
readonly repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
readonly architecture="${2:-$(uname -m)}"
case "${architecture}" in
	arm64|x86_64) ;;
	*) echo "Unsupported macOS architecture: ${architecture}" >&2; exit 2 ;;
esac
if [[ $# -ge 1 ]]; then
	mkdir -p -- "$1"
	output_dir="$(cd -- "$1" && pwd)"
else
	output_dir="$(mktemp -d "${TMPDIR:-/tmp}/siv8-image-checks.XXXXXX")"
fi
readonly output_dir
printf 'Artifacts: %s\n' "${output_dir}"

flags=(-arch "${architecture}" -std=c++23 -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=all)
if [[ "${architecture}" == x86_64 ]]; then
	flags+=(-msse4.2)
fi
xcrun clang++ "${flags[@]}" -DSIV3D_IMAGE_ALLOCATION_TEST \
	-I "${repo_dir}/Siv3D/include" -I "${repo_dir}/Siv3D/include/ThirdParty" \
	"${repo_dir}/Test/ThirdParty/Catch2/catch_amalgamated.cpp" \
	"${repo_dir}/Test/Test_Image.cpp" \
	"${repo_dir}/Siv3D/src/Siv3D/Image/SivImage.cpp" \
	-Wl,-dead_strip -o "${output_dir}/image-checks"

"${output_dir}/image-checks"
