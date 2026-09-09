# TextFileReader benchmark

The opt-in `TextFileReader.benchmark.UTF8` case in
[Test_TextFileReader.cpp](../../Test/Test_TextFileReader.cpp) measures the current
implementation through its public API. It is skipped by the normal automated
suite and does not require changing `SIV3D_RUN_BENCHMARK`.

## Run

Build and run on Windows from the repository root:

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=TextFileReader.benchmark.UTF8', '--no-skip'
```

For repeated measurements of the same build, add `-SkipBuild`. Use Release,
run on an idle machine, and collect at least three reports for each revision.
The runner retains reports under `WindowsDesktop/Intermediate/TestReports/`.
The benchmark's Markdown tables and CSV rows are included in the doctest report.
Do not use `-SkipBuild` after changing implementation or test sources until the
new executable has been built.

On macOS, select Release explicitly (the host's runner defaults to Debug):

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=TextFileReader.benchmark.UTF8' '--no-skip'
```

For correctness checks, run the normal full suite described in the
[development guide](README.md#build-and-test). The non-benchmark TextFileReader
cases cover BOMs, CR removal, embedded NULs, invalid UTF-8, output reuse, mixed
read methods, short reads, exceptions, and buffer boundaries (including split
UTF-8 sequences, CRLF, and reopening a reader). CR removal is checked before
decoding, including CRs between code units and invalid input after normalization.

## Workloads and timing

Inputs are deterministic synthetic application-shaped workloads, not captured
production files. No shader compiler is invoked.

| Workload | Content |
| --- | --- |
| `config-json` | A small JSON configuration with 64 named items and Japanese text |
| `shader-ascii-lf` | 1024 HLSL-style function definitions, ASCII with LF |
| `dialogue-utf8-bom-crlf` | 1024 numbered dialogue lines with Japanese, English, an emoji, a UTF-8 BOM, and CRLF |
| `long-lines-1mib` | Approximately 1 MiB in 32 long lines with multilingual suffixes and CRLF |
| `blank-lines-crlf` | 32768 empty CRLF lines, exercising dense CR removal |
| `blank-lines-lf` | 32768 empty LF lines, separating empty-line processing from CR removal |
| `mixed-empty-lines` | Repeated empty, one-character, multilingual, and 4097-character lines with LF and CRLF |

Every workload runs through both `MemoryViewReader` and `BinaryFileReader`.
Memory input is a non-owning view: creating a reader does not copy the fixture.
File fixtures are generated under `Test::OutputPath()` before timing, and the
test runner owns their cleanup. Fixture generation and full-content verification
are outside timed regions. File timings reflect **warm OS caches**, including
open, BOM detection, reading, and close; they do not measure cold storage.

Each method creates a new TextFileReader per iteration, except
`readAll/utf32-reader-reuse`, which reopens the same reader and reuses its input
buffer as well as the destination. Its initial reads are verified before timing;
reopening (including closing the previous file and detecting the BOM) is timed.
`open-close` measures opening, BOM detection, and closing without consuming the
text; compare elapsed time per operation rather than its byte throughput.
Whole-file UTF-8 and UTF-32 output are each measured as a newly returned string
and with a reused destination. Line-by-line output reuses caller storage;
`readLine/utf32-value` calls the actual value-returning overload. Bulk line
reading measures both UTF-8 and UTF-32 output and reuses the outer array, but
its existing string elements are destroyed by the API. `readChar/utf32` consumes every decoded character without building an
output string. `readLine+readAll/utf8-reuse` reads the first line, then reads the
remaining text into the same UTF-8 destination; this exercises the transition
from buffered sequential reads to bulk reads. `JSON.Load` also includes JSON
parsing and destruction.

nanobench uses 11 epochs, a minimum 2 ms per epoch, and a warmup iteration.
Result sinks are inside timing; full-content assertions are outside it. The CSV
`batch` field records input bytes including BOM and CR, and `elapsed`
is the epoch median in seconds per complete workload. Compare matching workload, input backend,
and method names between builds. Use the median across repeated process runs,
and inspect variation before interpreting small changes. Workload order is
fixed, so repeat baseline/candidate measurements in reverse order when a
conclusion depends on a small difference.

There are no timing thresholds in correctness tests. A speedup in a memory
microbenchmark does not establish the same improvement in JSON parsing,
shader compilation, cold loading, or frame time. UTF-16 performance and
concurrent loading need separate measurements.
