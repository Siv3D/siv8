#!/bin/bash
set -euo pipefail

readonly script_dir="$(
	cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
	pwd
)"
readonly configuration="${CONFIGURATION:-Debug}"
readonly app_dir="${script_dir}/App"
readonly executable="${app_dir}/Siv3D-Test.app/Contents/MacOS/Siv3D-Test"

xcodebuild \
	-quiet \
	-project "${script_dir}/OpenSiv3D.xcodeproj" \
	-scheme Siv3D-Test \
	-configuration "${configuration}" \
	-destination "platform=macOS,arch=$(uname -m)" \
	CODE_SIGNING_ALLOWED=NO \
	build

cd "${app_dir}"
exec "${executable}" --test-only "$@"
