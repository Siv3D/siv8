# Documentation

Durable usage guides and development notes are organized by subject. API contracts
live in the public headers; executable tests and their expected results live in
Test. Unfinished work is tracked in [TODO](../TODO.md).

- [Development](development/README.md): repository workflow, documentation ownership, and validation.
- [Array](array/README.md): sequence operations, ownership, allocators, and migration.
- [Mesh3D](mesh3d/README.md): CPU modeling, assembly, export, and implementation guidance.

For documentation changes, follow the [documentation rules](AGENTS.md) and run
`python3 tools/check_docs.py` from the repository root. Add a subject when needed;
there is no requirement to pre-create a directory for every API class.
