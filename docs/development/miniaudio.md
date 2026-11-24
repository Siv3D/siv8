# miniaudio maintenance

The [vendored sources](../../Siv3D/src/ThirdParty/miniaudio) come from
[miniaudio](https://github.com/mackron/miniaudio).
[ThirdParty.md](../../ThirdParty.md) records the release version.
The header contains the implementation and embedded decoders; `miniaudio.c`
defines `MINIAUDIO_IMPLEMENTATION` and includes it. Both platform projects
compile that wrapper as C, with no local miniaudio feature or backend overrides.

[CAudio](../../Siv3D/src/Siv3D/Audio/CAudio.cpp) owns the native audio context.
[System::EnumerateMicrophones](../../Siv3D/src/Siv3D/MicrophoneInfo/SivMicrophoneInfo.cpp)
uses that context for capture-device enumeration. These are the current engine
integration points; Wave container and codec tests are not miniaudio coverage.

## Updating

Compare both files against the recorded upstream release before updating, and
preserve any local changes. The current sources have no functional patches;
retain LF line endings and remove newly introduced trailing whitespace when
required by `git diff --check`.
Keep the single implementation wrapper and rebuild all consumers together:
upstream does not guarantee ABI compatibility even between patch releases.

Update `ThirdParty.md` and the miniaudio entry in
[LicenseList.hpp](../../Siv3D/src/Siv3D/LicenseManager/LicenseList.hpp) from the
license footer of the bundled header. Siv3D displays the MIT No Attribution
(MIT-0) option. Check the header's copyright year directly; the upstream
repository's separate `LICENSE` file can lag behind it. MIT-0 must not be
replaced with the ordinary MIT license's attribution condition.

## Validation

[Test_Miniaudio.cpp](../../Test/Test_Miniaudio.cpp) checks null-backend context
lifecycle and enumeration, channel-map queries and stereo conversion, and
multichannel volume smoothing across processing-block boundaries. These tests
use in-memory samples and do not open playback or capture devices.

Use `--test-case=Miniaudio*` for focused iteration, then run the full suite with
the [host test workflow](README.md#build-and-test). Engine startup also initializes
the host's native audio context. These checks do not establish audible playback,
recording, hotplug behavior, or correctness of every asynchronous loading path;
report those separately if exercised.
