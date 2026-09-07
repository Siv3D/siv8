#!/usr/bin/env python3
"""Check local Markdown file links and reachability from docs/README.md.

Intentionally small: no network requests, heading-fragment validation, or
third-party dependencies. Check repository-owned prose, not fenced examples.
"""

import argparse
from pathlib import Path
import re
import tempfile
from urllib.parse import unquote, urlsplit


INLINE_LINK = re.compile(
    r"!?\[[^\]\n]*\]\(\s*(?:<([^>\n]+)>|([^\s)]+))"
    r"(?:\s+[\"'][^\"']*[\"'])?\s*\)"
)
REFERENCE = re.compile(r"^ {0,3}\[[^\]\n]+\]:\s*(?:<([^>\n]+)>|(\S+))")
FENCE = re.compile(r"^ {0,3}(`{3,}|~{3,})")


def destinations(text):
    """Yield (line number, destination) for inline links and reference definitions."""
    fence = None
    for number, line in enumerate(text.splitlines(), 1):
        marker = FENCE.match(line)
        if fence:
            if marker and marker[1][0] == fence[0] and len(marker[1]) >= len(fence):
                if not line[marker.end():].strip():
                    fence = None
            continue
        if marker:
            fence = marker[1]
            continue
        line = re.sub(r"(`+).*?\1", "", line)
        matches = list(INLINE_LINK.finditer(line))
        reference = REFERENCE.match(line)
        if reference:
            matches.append(reference)
        for match in matches:
            yield number, match[1] or match[2]


def check(root):
    root = root.resolve()
    docs = root / "docs"
    documents = set(root.glob("*.md")) | set(docs.rglob("*.md"))
    documents |= set((root / "Test" / "Manual").rglob("*.md"))
    documents = {path.resolve() for path in documents}
    errors = []
    graph = {path: set() for path in documents}
    for source in sorted(documents):
        for number, href in destinations(source.read_text(encoding="utf-8-sig")):
            url = urlsplit(href)
            if url.scheme or url.netloc or not url.path:
                continue
            path = unquote(url.path)
            target = ((root / path.lstrip("/")) if path.startswith("/")
                      else (source.parent / path)).resolve()
            if not target.is_relative_to(root) or not target.exists():
                errors.append(f"{source.relative_to(root)}:{number}: missing local destination: {href}")
            elif target in graph:
                graph[source].add(target)
    start = docs / "README.md"
    if start not in graph:
        errors.append("docs/README.md: missing documentation entry point")
        return errors
    reachable = set()
    pending = [start]
    while pending:
        source = pending.pop()
        if source not in reachable:
            reachable.add(source)
            pending.extend(graph[source] - reachable)
    for path in sorted(documents):
        if path.is_relative_to(docs) and path not in reachable:
            errors.append(f"{path.relative_to(root)}: not reachable from docs/README.md")
    return errors


def self_test():
    with tempfile.TemporaryDirectory() as directory:
        root = Path(directory)
        (root / "docs" / "topic").mkdir(parents=True)
        (root / "Test" / "Manual").mkdir(parents=True)
        index = root / "docs" / "README.md"
        index.write_text('[Topic](topic/README.md)\n[Rules](AGENTS.md)\n')
        (root / "docs" / "AGENTS.md").write_text("Rules\n")
        topic = root / "docs" / "topic" / "README.md"
        topic.write_text('[Details](<a note.md>#section)\n[encoded](a%20note.md "title")\n')
        (topic.parent / "a note.md").write_text(
            "# Notes\n```md\n[not a link](missing.md)\n```\n"
            "~~~\n[example](missing.md)\n~~~\n`[code](missing.md)`\n"
            "[web](https://example.invalid/no-network)\n[local heading](#heading)\n"
            "[ref]: ../README.md\n"
        )
        assert check(root) == [], check(root)
        topic.write_text(topic.read_text() + "[missing](absent.md)\n")
        assert any("missing local destination" in error for error in check(root))
        topic.write_text("# Topic\n")
        assert any("a note.md: not reachable" in error for error in check(root))
        (root / "Test" / "Manual" / "Check.md").write_text("[bad](missing.md)\n")
        assert any("Test/Manual/Check.md:1" in error for error in check(root))
        topic.write_text("[escape](../../../outside.md)\n")
        assert any("outside.md" in error for error in check(root))
        index.unlink()
        assert any("missing documentation entry point" in error for error in check(root))
    print("Documentation checker self-tests passed.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if args.self_test:
        self_test()
        return 0
    errors = check(Path(__file__).resolve().parents[1])
    if errors:
        print("\n".join(errors))
        return 1
    print("Documentation file links and index reachability OK.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
