# Public API documentation

Use these rules when adding or revising public-header Doxygen. Documentation
ownership and organization follow [docs/AGENTS.md](../AGENTS.md).

- Describe the adopted, observable contract: preconditions, return values,
  failure behavior, side effects, ownership, and non-obvious boundary cases.
  State when a result is empty or absent; do not call all such outcomes failures.
  Preserve guarantees about partial results and rollback of side effects.
- Keep shared behavior in one canonical contract and link to it from related
  types and overloads. Do not create a separate `@remark` for each tested case.
  Put usage guidance, geometric derivations, implementation choices, and detailed
  shape examples in subject guides. Omit incidental advice and assurances of
  expected behavior, such as "no extra X is needed" or Unicode support.
- Put each fact in its appropriate Doxygen field; do not repeat `@param` or
  `@return` information in `@remark`. Retain useful ordinary examples, preferably
  input/output pairs; improve incomplete examples rather than deleting them
  because the behavior can be inferred.
- Add platform notes only for differences in the public contract; combine
  identical behavior. Keep migration states, incomplete backend ports,
  implementation steps, fix history, and verification status out of API prose.
  Track temporary gaps in development docs, test instructions, and root TODO.md.
  Restoring an already documented contract does not itself need new prose.
- When simplifying documentation, check the implementation and tests. Do not
  silently strengthen guarantees, remove meaningful preconditions, or erase
  actual platform differences.
