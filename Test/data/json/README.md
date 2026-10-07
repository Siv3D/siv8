# JSON fixtures

These original fixtures are consumed by [Test_JSON.cpp](../../Test_JSON.cpp).
Run the JSON cases with the host's test runner; tests never modify this directory.

| File | Input and expected result |
| --- | --- |
| `sample.json` | Application configuration with nested records, arrays, Unicode, null, empty containers, escaped keys and integer boundaries; loads successfully. |
| `utf8-bom.json` | UTF-8 BOM, Japanese/emoji text, and CRLF; loads successfully. |
| `utf16le.json`, `utf16be.json` | The same document in BOM-prefixed UTF-16; loads successfully. |
| `invalid-cr-token.json` | `[1` + CR + `2]`; rejected rather than joined into `[12]`. |
| `invalid-cr-string.json` | A raw CR inside a string; rejected. |
| `invalid-nul.json` | `{}` + NUL + `trailing`; rejected rather than accepting a prefix. |
| `invalid-utf8.json` | Quotes containing the byte FF; rejected. |
| `invalid-truncated.json` | An incomplete object/array; rejected. |
| `invalid-utf16-odd.json` | BOM and an odd number of UTF-16 payload bytes; rejected. |
| `invalid-utf16-surrogate.json` | An unpaired high surrogate in a quoted UTF-16 string; rejected. |

The encoding and invalid-input files are marked binary in
[.gitattributes](.gitattributes) so Git does not normalize their BOMs, CR, NUL or
invalid code units. `sample.json` is ordinary LF text. Save tests write only to
`Test::OutputPath(U"json/...")`, which the shared runner cleans.
