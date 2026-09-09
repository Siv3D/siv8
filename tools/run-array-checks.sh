#!/bin/bash
# Isolated Array exception/model checks, using the same sources as the engine suite.
set -euo pipefail

if [[ "$(uname -s)" != Darwin ]]; then
	echo "This workflow requires macOS and the Xcode command-line tools." >&2
	exit 1
fi
readonly mode="${1:-asan}"
case "${mode}" in
	asan|coverage) ;;
	*) echo "Usage: $0 [asan|coverage] [output-directory]" >&2; exit 2 ;;
esac
readonly repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
if [[ $# -ge 2 ]]; then
	mkdir -p -- "$2"
	output_dir="$(cd -- "$2" && pwd)"
else
	output_dir="$(mktemp -d "${TMPDIR:-/tmp}/siv8-array-${mode}.XXXXXX")"
fi
readonly output_dir
printf 'Artifacts: %s\n' "${output_dir}"

flags=(-std=c++23 -g -O0 -fno-omit-frame-pointer)
if [[ "${mode}" == asan ]]; then
	flags+=(-fsanitize=address,undefined -fno-sanitize-recover=all)
else
	flags+=(-fprofile-instr-generate -fcoverage-mapping)
fi
xcrun clang++ "${flags[@]}" \
	-I "${repo_dir}/Siv3D/include" -I "${repo_dir}/Siv3D/include/ThirdParty" \
	"${repo_dir}/Test/ThirdParty/Catch2/catch_amalgamated.cpp" \
	"${repo_dir}/Test/Test_ArrayException.cpp" \
	"${repo_dir}/Test/Test_ArrayModel.cpp" \
	"${repo_dir}/Siv3D/src/Siv3D/Array/SivArray.cpp" \
	-o "${output_dir}/array-checks"

if [[ "${mode}" == asan ]]; then
	"${output_dir}/array-checks"
else
	LLVM_PROFILE_FILE="${output_dir}/array.profraw" "${output_dir}/array-checks"
	xcrun llvm-profdata merge -sparse "${output_dir}/array.profraw" -o "${output_dir}/array.profdata"
	sources=(
		"${repo_dir}/Siv3D/include/Siv3D/Array.hpp"
		"${repo_dir}/Siv3D/include/Siv3D/detail/Array.ipp"
		"${repo_dir}/Siv3D/include/Siv3D/detail/ArrayAlgorithm.ipp"
		"${repo_dir}/Siv3D/include/Siv3D/detail/BoolArray.ipp"
		"${repo_dir}/Siv3D/src/Siv3D/Array/SivArray.cpp"
	)
	xcrun llvm-cov report "${output_dir}/array-checks" \
		-instr-profile="${output_dir}/array.profdata" "${sources[@]}" | tee "${output_dir}/coverage.txt"
	xcrun llvm-cov show "${output_dir}/array-checks" \
		-instr-profile="${output_dir}/array.profdata" "${sources[@]}" \
		-format=html -output-dir="${output_dir}/html" -show-line-counts-or-regions
	printf 'Coverage report: %s/html/index.html\n' "${output_dir}"
fi
