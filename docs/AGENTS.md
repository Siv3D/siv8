# Documentation rules

- Organize durable documentation by subject. Start with a subject README and add
  subdirectories only when the existing collection needs them.
- Keep every document reachable from docs/README.md through normal Markdown
  links. Subject READMEs are curated entry points, not copies of file inventories.
- Use descriptive lowercase kebab-case filenames and relative Markdown links.
  Link to repository files rather than copying API contracts or test programs.
- Public-header Doxygen is authoritative for API contracts; docs explains usage
  and design; Test owns fixtures, execution steps, and expected results. TODO.md
  at the repository root owns unfinished work.
- Keep unadopted designs in the relevant subject's proposals directory. State
  their status and release scope; do not present them as implemented behavior.
- Update affected documentation with code changes. Avoid transient test counts,
  commit hashes, machine state, and redundant “last updated” dates.
- Preserve executable test programs in Test/Manual when separating explanations.
  Put development tools in tools and describe their usage in the subject docs.
- Run `python3 tools/check_docs.py` from the repository root after documentation
  changes. It checks local file destinations and docs index reachability, not
  external URLs or heading fragments. Run `git diff --check` as well.
