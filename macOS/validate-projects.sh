#!/bin/bash
# Validates the syntax of the macOS Xcode project and the Windows Visual Studio
# project files. Run this after adding, removing, or renaming files in them.
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

echo "Project files OK."
