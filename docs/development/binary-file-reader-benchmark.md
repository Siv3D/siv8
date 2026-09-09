# Windows BinaryFileReader benchmark

The [tool](../../tools/binary-file-reader-benchmark/benchmark.cpp) compares the
current Windows `BinaryFileReader`, the previous `std::ifstream` implementation,
and experimental Win32 buffer sizes. Each backend freshly compiles its public
and private reader implementations. The historical baseline is exported from a
fixed local Git revision selected by
[build.ps1](../../tools/binary-file-reader-benchmark/build.ps1); it remains
reproducible after the production backend changes. Experimental adapters use
that baseline's open, resource and public read/lookahead logic. The production
variant uses the current sources, including native size queries and failure
handling. Generated sources stay under `tools/binary-file-reader-benchmark/out/`.

## Build and run

Use Windows x64 with Visual Studio C++ tools and a matching **Release x64**
`Siv3D.lib`. Build `WindowsDesktop/Siv3D.vcxproj` with those settings first if the
library is missing or out of date. The scripts discover MSVC with `vswhere`.
Run from the repository root in PowerShell 7:

```powershell
./tools/binary-file-reader-benchmark/build.ps1
python tools/binary-file-reader-benchmark/run.py
```

This console program does not initialize the graphics engine or alter
`WindowsDesktop/Main.cpp`. It freshly compiles the public and private reader
implementation files for every backend and checks linker maps to confirm their
symbols come from the new objects. Other engine code, including compression and
text parsing, comes
from the supporting Release library. Build flags and library/source hashes are
saved in `out/build.json`. The runner verifies executable hashes against that
completed build manifest before preparation; interrupted builds must be rebuilt.

The baseline revision must be available in local Git history. For a shallow
clone, fetch the required history first. `build.ps1 -BaselineRef <ref>` can select
another compatible revision containing the previous `ifstream` implementation;
the exported revision and source hashes are recorded in the build manifest.

Useful options:

```powershell
python tools/binary-file-reader-benchmark/run.py --verify-only
python tools/binary-file-reader-benchmark/run.py --rounds 9 --seed 42
python tools/binary-file-reader-benchmark/run.py --variants ifstream production win32-16k
python tools/binary-file-reader-benchmark/run.py --scenario sequential
```

The runner prepares deterministic fixtures under `out/fixtures/` and checks each
backend before timing it. These are development-tool outputs, separate from the
engine test runner's `Test/output/`. Repository example and engine assets are
read-only. Each run keeps its raw per-round CSVs, `summary.csv`, `summary.md` and
`run.json`, plus a snapshot of the fixture manifest, in a distinct `out/results-.../` directory. Remove `out/` manually when
the retained fixtures, executables, linker maps and results are no longer needed.
Do not run concurrent benchmark sessions in the same output directory.

## Compared backends

| Name | User buffer | Behavior |
| --- | ---: | --- |
| `production` | 16 KiB | Current Win32 implementation, rebuilt from source |
| `ifstream` | MSVC's default | Historical implementation exported from Git |
| `win32-raw` | None | Synchronous `ReadFile`, native seeks; OS caching remains enabled |
| `win32-4k` | 4 KiB | Reusable buffer and logical seeks |
| `win32-16k` | 16 KiB | Reusable buffer and logical seeks |
| `win32-64k` | 64 KiB | Reusable buffer and logical seeks |
| `win32-64k-sequential` | 64 KiB | Same candidate with `FILE_FLAG_SEQUENTIAL_SCAN` |

The production reader allocates its buffer on the first read smaller than
16 KiB. It reuses that storage across close/reopen, invalidates cached contents
on close, and releases storage when the reader is destroyed. Large reads go
directly into the caller's destination, after any overlapping cached bytes are
consumed. A reader that only performs large reads needs no user buffer.

The cached window, the native file pointer and the caller's logical position are
tracked separately. A seek within cached data only changes the logical position;
a seek outside it checks the native operation immediately. Lookahead leaves the
logical position unchanged, and `readExact` commits a new position only after
complete success, even if a later native seek would fail. A failed prefetch is
retried for just the requested bytes, so a locked range beyond the request does
not prevent reading unlocked bytes. Open-time size queries use the opened handle.

The experimental buffered adapters use the same allocation and bulk-read
principles, but defer native seeks and retain the historical public lookahead
and exact-read policy. They are measurement controls, not alternate supported
production backends. The raw control exposes the cost of frequent API calls and
native seeks.

All Win32 variants use synchronous, ordinary cached file I/O, with read/write
sharing matching the MSVC CRT's deny-none mode. “Raw” here does **not** mean
`FILE_FLAG_NO_BUFFERING`, which has alignment requirements incompatible with
arbitrary byte reads. See Microsoft's [ReadFile documentation](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile)
and [file buffering documentation](https://learn.microsoft.com/en-us/windows/win32/fileio/file-buffering).
The sequential-scan hint is a separate experimental variant because benefits
depend on the access pattern; see Microsoft's [discussion of read-ahead timing](https://devblogs.microsoft.com/oldnewthing/20221130-00/?p=107505).

## Workloads and interpretation

The harness pairs controlled I/O patterns with real consumers. Large sequential
transfers expose unnecessary copies; small fields and text reading expose
per-call overhead; mixed headers, lookahead and clustered/random seeks exercise
buffer reuse. Small-file and real-asset batches include opening and closing.
Compressed-font expansion includes the actual `Compression::DecompressFile`
work, so decompression can dominate any reader improvement.

| Scenario | One iteration | Timed lifetime / processing |
| --- | --- | --- |
| `sequential_1m` | 64 MiB, 1 MiB per read | Already-open reader, reused destination |
| `stream_4k` | 64 MiB, 4 KiB per read | Already-open reader, reused destination |
| `scalar_4b` | 1 MiB, one uint32 per read | Already-open reader, accumulate values |
| `mixed_records` | 4096 records: peek 16 bytes, read 16, skip 240 | Already-open reader; 128 KiB delivered including peeks |
| `random_4k` | 4096 random 4 KiB reads within 64 MiB | Precomputed offsets, already-open reader |
| `clustered_4k` | 4096 reads in groups of 32 within 64 KiB neighborhoods | Precomputed offsets, already-open reader |
| `small_files` | 128 generated files, 1–64 KiB each | Construct, open, bulk read, close, destroy each reader |
| `real_shaders` | Four repository shader binaries | Whole-file bytes; includes open/close, excludes shader creation |
| `real_png` | Three repository PNG files | Whole-file bytes; includes open/close, excludes image decoding |
| `real_fonts` | Two repository compressed-font files | Whole-file bytes; includes open/close, excludes decompression |
| `text_utf8_lines` | 1024 multilingual dialogue lines, 64 KiB UTF-8 | Actual TextFileReader, encoding detection, line decoding, open/close |
| `text_utf16_lines` | The same dialogue encoded UTF-16LE with BOM | Actual TextFileReader; caller's line storage reused |
| `font_stream_128k` | NotoSansJP-Medium compressed font, 128 KiB requests | Input-only counterpart to decompression, includes open/close |
| `decompress_font` | Actual NotoSansJP-Medium decompression | Context/input/output scratch allocation included; output Blob capacity reused |

The manifest records the exact asset paths, sizes and hashes. The input-only
font scenario matches the current bundled `ZSTD_CStreamInSize()` of 128 KiB.
If updating Zstd or the compression implementation, review that pairing.

Untimed verification compares all fixture and asset bytes, runs 12,000 mixed
operations against an in-memory position/content model, and checks buffer
boundaries, partial final buffers, exact-read failure, invalid arguments,
move/reopen behavior and sparse-file offsets across 4 GiB. Each timed scenario
also runs a full-content checked warmup. Sparse checks report a skip if the
filesystem does not support the fixture. This verification does not exercise
single requests larger than `MAXDWORD` or artificially injected OS failures.

The experiment measures **warm OS cache** behavior. Fixture generation and
warmups occur before measurement, and reopening a file does not make it cold.
It does not evict the system cache or estimate storage-device cold-load speed.
Backend process order is shuffled reproducibly on each round; each executable
warms and calibrates its own batches. The runner rejects mismatched scenario
sets, byte counts or per-iteration checksums instead of comparing unequal work.

`median_ms` is the median per-workload duration across rounds; min/max describe
variation in **batch averages**, not individual-read latency percentiles.
`speedup = baseline median / candidate median`; above 1 is faster. Keep the
per-round data and examine variation before interpreting a small difference.
Use an idle machine and repeat with a different order seed. Timing includes
return-value checks and a small result sink; full content validation belongs to
the untimed verification pass. Do not infer frame-rate improvements from an I/O
microbenchmark or from one machine's warm-cache results.

Cold loading on representative storage, concurrent asset loading, and actual
game loading paths need separate measurements. Budget up to 16 KiB of retained
buffer capacity per reader that has performed a small file read, including
closed readers kept for reuse. Embedded-resource I/O is unchanged and is not a
performance target of this comparison.

## Regression tests

The [engine tests](../../Test/Test_BinaryFileReader.cpp) cover buffer boundaries,
partial final buffers, mixed read/seek/lookahead operations, lifetime and reopen
behavior. Windows cases additionally exercise real byte-range locks and recovery,
file/resource transitions, and sparse-file positions across 4 GiB. Run them
through the [Windows test runner](../../WindowsDesktop/run-tests.ps1):

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=*BinaryFileReader*'
```

Engine test fixtures belong to `Test/output/` and are cleaned up by the runner.
