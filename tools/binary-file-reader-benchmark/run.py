"""Run verified Windows reader variants in shuffled rounds and retain raw measurements."""
from __future__ import annotations

import argparse
import csv
import datetime as dt
import json
import hashlib
import math
import os
from pathlib import Path
import random
import statistics
import subprocess
import sys

ROOT = Path(__file__).resolve().parent
VARIANTS = ['production', 'ifstream', 'win32-raw', 'win32-4k', 'win32-16k', 'win32-64k', 'win32-64k-sequential']


def run(arguments: list[str | Path]) -> None:
    subprocess.run([str(arg) for arg in arguments], check=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rounds', type=int, default=7, help='independent shuffled rounds (default: 7)')
    parser.add_argument('--seed', type=int, default=20260909)
    parser.add_argument('--scenario', default='', help='scenario name filter passed to the harness')
    parser.add_argument('--variants', nargs='+', choices=VARIANTS, default=VARIANTS)
    parser.add_argument('--verify-only', action='store_true')
    args = parser.parse_args()
    if os.name != 'nt':
        parser.error('this benchmark requires Windows')
    if args.rounds < 3:
        parser.error('--rounds must be at least 3')
    if 'ifstream' not in args.variants or len(set(args.variants)) != len(args.variants):
        parser.error('--variants must include ifstream, with no duplicates')

    out = ROOT / 'out'
    binaries = {name: out / name / 'benchmark.exe' for name in args.variants}
    for binary in binaries.values():
        if not binary.is_file():
            parser.error(f'build first: missing {binary}')
    manifest_path = out / 'build.json'
    if not manifest_path.is_file():
        parser.error('no completed build manifest; rerun build.ps1')
    build = json.loads(manifest_path.read_text(encoding='utf-8-sig'))
    hashes = {name: hashlib.sha256(path.read_bytes()).hexdigest() for name, path in binaries.items()}
    if any(build.get('executable_sha256', {}).get(name) != digest for name, digest in hashes.items()):
        parser.error('executable/build manifest mismatch; rerun build.ps1')
    # Each invocation owns a new result directory, while the deterministic fixtures
    # remain reusable. Preparation is outside every timing interval.
    stamp = dt.datetime.now(dt.timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
    results = out / f'results-{stamp}'
    results.mkdir(parents=True)
    run([binaries['ifstream'], '--prepare', out, ROOT.parent.parent])
    (results / 'fixtures-manifest.txt').write_bytes((out / 'fixtures/manifest.txt').read_bytes())
    for name, binary in binaries.items():
        print(f'Verify {name}', flush=True)
        run([binary, '--verify', out])
    if args.verify_only:
        print('All requested variants passed verification.')
        return

    rng = random.Random(args.seed)
    rows: list[dict] = []
    orders = []
    reference = {}
    for round_index in range(args.rounds):
        order = list(binaries)
        rng.shuffle(order)
        orders.append(order)
        for name in order:
            path = results / f'{round_index + 1:02d}-{name}.csv'
            print(f'Round {round_index + 1}/{args.rounds}: {name}', flush=True)
            arguments = [binaries[name], '--measure', out, path]
            if args.scenario:
                arguments.append(args.scenario)
            run(arguments)
            with path.open(newline='', encoding='utf-8-sig') as file:
                measured = list(csv.DictReader(file))
            if not measured:
                raise RuntimeError(f'No scenarios measured: {name}')
            signature = {}
            for row in measured:
                signature[row['scenario']] = (row['bytes_per_iteration'], row['checksum'])
                seconds = float(row['seconds'])
                iterations = int(row['iterations'])
                if not math.isfinite(seconds) or seconds <= 0 or iterations <= 0:
                    raise RuntimeError(f'Invalid timing: {row}')
                rows.append(dict(row, backend=name, round=round_index + 1,
                                 ms=seconds * 1000 / iterations))
            if reference and signature != reference:
                raise RuntimeError(f'Workload/checksum mismatch: {name}; refusing to summarize timings')
            reference = signature

    metadata = dict(arguments=vars(args), orders=orders, cache='warm, no OS cache eviction',
                    build=build,
                    executable_sha256={})
    metadata['executable_sha256'] = {name: hashlib.sha256(path.read_bytes()).hexdigest()
                                     for name, path in binaries.items()}
    if metadata['executable_sha256'] != hashes:
        raise RuntimeError('Executables changed during measurement; discard this run')
    (results / 'run.json').write_text(json.dumps(metadata, indent=2) + '\n', encoding='utf-8')

    summary = []
    for scenario in reference:
        baseline = statistics.median(row['ms'] for row in rows
                                     if row['scenario'] == scenario and row['backend'] == 'ifstream')
        for name in args.variants:
            samples = [row['ms'] for row in rows if row['scenario'] == scenario and row['backend'] == name]
            median = statistics.median(samples)
            summary.append(dict(scenario=scenario, backend=name, rounds=len(samples),
                                median_ms=median, min_ms=min(samples), max_ms=max(samples),
                                speedup=baseline / median))
    with (results / 'summary.csv').open('w', newline='', encoding='utf-8') as file:
        writer = csv.DictWriter(file, fieldnames=list(summary[0]))
        writer.writeheader()
        writer.writerows(summary)
    report = ['# BinaryFileReader warm-cache comparison', '',
              'Times are milliseconds per workload iteration, including the costs described in the guide. '
              'Ranges are min/max of batch averages across rounds, not individual-read tail latency.', '',
              '| Scenario | Backend | Median ms | Range ms | Speedup vs ifstream |',
              '| --- | --- | ---: | ---: | ---: |']
    for row in summary:
        report.append(f"| {row['scenario']} | {row['backend']} | {row['median_ms']:.3f} | "
                      f"{row['min_ms']:.3f}–{row['max_ms']:.3f} | {row['speedup']:.2f}x |")
    (results / 'summary.md').write_text('\n'.join(report) + '\n', encoding='utf-8')
    print(f'Results: {results / "summary.md"}', flush=True)


if __name__ == '__main__':
    main()
