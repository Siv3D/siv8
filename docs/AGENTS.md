# Documentation rules

- Organize docs by subject with curated READMEs; keep every document reachable
  from docs/README.md. Add subdirectories only when needed.
- Use descriptive lowercase kebab-case filenames and relative Markdown links.
  Link to repository files rather than copying API contracts or test programs.
- Public-header Doxygen is authoritative for API contracts; docs explains usage
  and design; Test owns fixtures, execution steps, and expected results. TODO.md
  at the repository root owns unfinished work.
- Mark proposals with their status and release scope; do not describe them as
  implemented behavior.
- Update affected documentation with code changes. Avoid transient test counts,
  commit hashes, machine state, and redundant “last updated” dates.
- Preserve executable test programs in Test/Manual when separating explanations.
  Put development tools in tools and describe their usage in the subject docs.
- Validate documentation changes with `python3 tools/check_docs.py` and
  `git diff --check` from the repository root.
