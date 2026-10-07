#!/bin/bash
# Run the real test app with file-specific stdio fault injection.
set -euo pipefail
if [[ "$(uname -s)" != Darwin ]]; then
	echo "This workflow requires macOS." >&2
	exit 1
fi
readonly repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
readonly executable="${repo_dir}/macOS/App/Siv3D-Test.app/Contents/MacOS/Siv3D-Test"
if [[ ! -x "${executable}" ]]; then
	echo "Build the current test app with ./macOS/run-tests.sh first." >&2
	exit 1
fi
if [[ $# -ge 1 ]]; then
	mkdir -p -- "$1"
	output_dir="$(cd -- "$1" && pwd)"
else
	output_dir="$(mktemp -d "${TMPDIR:-/tmp}/siv8-writer-checks.XXXXXX")"
fi
readonly output_dir
printf 'Artifacts: %s\n' "${output_dir}"
xcrun clang++ -std=c++23 -Wall -Wextra -Werror -dynamiclib \
	"${repo_dir}/tools/binary-writer-checks/Interpose.cpp" \
	-o "${output_dir}/writer-faults.dylib"
cd "${repo_dir}/macOS/App"
DYLD_INSERT_LIBRARIES="${output_dir}/writer-faults.dylib" \
	"${executable}" --test-only '--test-case=BinaryFileWriter.injected_*' \
	2>&1 | tee "${output_dir}/results.txt"
