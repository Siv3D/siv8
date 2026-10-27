# Documentation rules

- Store durable guides and development knowledge under `docs/<subject>/`, with
  a curated subject README linked from `docs/README.md`. Keep every document
  reachable from that index. Add subdirectories only when needed; root README
  and AGENTS remain entry points.
- Use descriptive lowercase kebab-case filenames and relative Markdown links.
  Link to repository files instead of copying API contracts or test programs.
- Public-header Doxygen owns API contracts; docs explains usage and design;
  Test owns fixtures, execution steps, and expected results. Root TODO.md owns
  unfinished work. Put unadopted designs in the subject's `proposals/` directory
  with explicit status and release scope; do not present them as implemented.
- Update affected docs with code changes. Link to canonical design and validation
  guidance instead of repeating it; omit transient counts, commit hashes,
  machine state, and redundant "last updated" dates.
- At feature completion, consolidate durable knowledge in existing guides and
  remove superseded galleries, completed handoffs, obsolete proposals, and
  temporary artifacts within scope. Keep one canonical gallery per feature;
  separate manual programs only for distinct validation needs. Use Git history
  instead of archival copies.
- Preserve executable programs in `Test/Manual/` when separating explanations.
  Put development tools in `tools/` and document their use in the subject guide.
- Update links when moving or removing documents. From the repository root, run
  `python3 tools/check_docs.py` and `git diff --check` after documentation changes.
