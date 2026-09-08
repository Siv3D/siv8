#!/bin/bash
# Validates project syntax and shared test registration in both test projects.
set -euo pipefail

readonly script_dir="$(
	cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
	pwd
)"
readonly repo_dir="$(cd -- "${script_dir}/.." && pwd)"

if command -v plutil >/dev/null 2>&1; then
	plutil -lint "${script_dir}/OpenSiv3D.xcodeproj/project.pbxproj"
else
	echo "warning: plutil not found; skipped validating OpenSiv3D.xcodeproj" >&2
fi

xmllint --noout \
	"${repo_dir}/WindowsDesktop/Siv3D.vcxproj" \
	"${repo_dir}/WindowsDesktop/Siv3D.vcxproj.filters" \
	"${repo_dir}/WindowsDesktop/Siv3D-Test.vcxproj" \
	"${repo_dir}/WindowsDesktop/Siv3D-Test.vcxproj.filters"

python3 "${repo_dir}/tools/check_test_projects.py"

echo "Project files OK."
