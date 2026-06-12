# TOML test fixtures

## Success cases

- `valid_basic.toml`
  - Basic tables, scalar values, arrays, and arrays of tables.
- `valid_all_types.toml`
  - Bool, int, float, string, multiline string, local date/time/datetime, and offset datetime.
- `valid_nested.toml`
  - Nested tables, inline table, nested arrays, and arrays of tables.
- `valid_unicode.toml`
  - UTF-8 without BOM, Japanese text, symbols, and emoji.
- `valid_utf8_bom.toml`
  - UTF-8 with BOM. Intended for `Load()` / `LoadResult()` file-reading tests.
- `valid_empty.toml`
  - Empty TOML document. Expected to parse as an empty root table.
- `valid_comments_whitespace.toml`
  - Comments, whitespace, trailing comma in array.
- `valid_type_mismatch_fixture.toml`
  - Valid TOML intended for strict `getOpt<T>()`, `get<T>()`, `getOr<T>()`, and `getArray<T>()` tests.

## Failure cases

- `invalid_unclosed_array.toml`
  - Syntax error: unclosed array.
- `invalid_duplicate_key.toml`
  - Duplicate key.
- `invalid_duplicate_table.toml`
  - Duplicate table declaration.
- `invalid_invalid_date.toml`
  - Invalid calendar date.
- `invalid_invalid_number.toml`
  - Invalid numeric literal.
- `invalid_non_utf8_latin1.toml`
  - Latin-1 encoded file. Intended to fail Siv3D-side UTF-8 text reading before TOML parsing.
