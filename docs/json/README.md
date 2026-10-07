# JSON

[JSON.hpp](../../Siv3D/include/Siv3D/JSON.hpp) defines the public contract.
The class provides a DOM for configuration files and API payloads: objects,
arrays, strings, booleans, null, signed/unsigned 64-bit integers and double
precision numbers. It supports editing, iteration, text formatting, file I/O,
and BSON, CBOR and MessagePack conversion. Engine types use
[JSONSerialization.hpp](../../Siv3D/include/Siv3D/JSONSerialization.hpp).

## Reading application data

Keep the root alive while using its children. Check `isInvalid()` to detect
parse failure: a successfully parsed `null` also converts to `false`.
Check `contains()` before reading an optional key from a const object.
A mutable subscript inserts null for a missing key.

```cpp
const JSON config = JSON::Load(U"config.json");
if (config.isInvalid())
{
    throw Error{ U"Could not load config.json" };
}
if (config.isObject() && config.contains(U"title"))
{
    const String title = config[U"title"].getOr<String>(U"Untitled");
}
```

`getOpt<T>()` and `getOr<T>()` handle supported conversion failures; they are
not schema or numeric range validators. Validate application constraints before
using values as indices, sizes, identifiers, or counts. In particular, narrowing
an integer can wrap, and converting an out-of-range floating-point value to an
integer is not safe. Whole-document parse success does not validate those uses.

The parser accepts C/C++ comments and retains the last value of a duplicate key.
It therefore should not be used as a strict JSON syntax validator. The JSON
interchange baseline is [RFC 8259](https://www.rfc-editor.org/rfc/rfc8259).
Integer tokens beyond the stored integer ranges can become doubles, losing
precision; extremely large exponents fail to parse. Arbitrary precision decimal
numbers require a separate representation agreed with the data producer.

## Ownership and editing

Children returned by subscripting and iteration borrow storage. Copying a child
copies that reference; copying an owning root copies its data. Use `clone()`
when retaining a child independently, returning it from a function, or editing
its parent in a way that can invalidate references. Insertion and erasure in
objects can invalidate children just as array reallocation can.

```cpp
JSON readSettings()
{
    const JSON document = JSON::Load(U"settings.json", AllowExceptions::Yes);
    return document[U"settings"].clone();
}
```

A borrowed const child copied into a mutable variable detaches on mutable element
access or modification.
Do not retain a child of a temporary root without cloning it. `swap()` exchanges
the handles and their ownership/reference state, rather than writing through
two borrowed handles. For a same-object copy into a new key, write
`object[U"new"] = object[U"old"].clone();`: the right-hand-side clone must be
completed before insertion can reallocate the parent. Use iteration over objects
or arrays; scalar iteration has no object key and is unsuitable for the key/value loop interface.

## Representation and limits

- Objects preserve insertion order. Equality is also sensitive to object key
  order because the storage is `nlohmann::ordered_json`.
- Non-finite numbers constructed in memory format as `null`. Binary data is an
  extension, not a JSON text type; use binary formats when preserving it.
- `flatten()` / `unflatten()` do not preserve empty arrays and empty objects:
  they become null. Numeric object keys can also be interpreted as array indices.
- BSON requires an object root. This bundled implementation represents integers
  above `INT64_MAX` using BSON type 0x11, which the
  [BSON specification](https://bsonspec.org/spec.html) defines as Timestamp.
  Siv3D can round-trip this extension, but other consumers see a timestamp,
  not a generic unsigned integer. BSON can also introduce binary subtype 0
  metadata, so compare `getBinary()` when checking the payload alone.
- `JSONPointer.hpp` contains no usable public pointer implementation. JSON Patch,
  JSON Schema, SAX/streaming parsing, and parser resource limits are not exposed
  by this wrapper. These are optional capabilities beyond ordinary DOM editing.
- The full document is held in memory. Object lookup uses the ordered container's
  linear search, so very wide objects and repeated lookups need workload-specific
  performance evaluation. There is no depth/size budget for untrusted input.

Pending changes that need contract or lower-layer design decisions are tracked
in [TODO.md](../../TODO.md), including checked numeric conversion and write-error
reporting. The wrapper does not currently provide a single exception family for
all operations; `Parse`/`Load`, `get`, checked array access and const object
access translate documented failures to `Error`, while other delegated
operations can throw `JSON::json_base::exception`. Custom serializers and custom
readers may propagate their own exceptions.

## Validation

[Core JSON tests](../../Test/Test_JSON.cpp) cover grammar, Unicode, escapes,
embedded NUL, numeric boundaries, invalid input, conversion templates, editing,
borrowed values, iterators, file/reader failures, text and binary round trips,
and generated nested/wide data. [Serialization tests](../../Test/Test_JSONSerialization.cpp)
cover engine geometry types. The [fixture notes](../../Test/data/json/README.md)
describe byte-sensitive file inputs.

Run the host's documented [test workflow](../development/README.md#build-and-test).
On macOS, `./macOS/run-tests.sh '--test-case=JSON*'` runs the focused checks;
`./macOS/run-tests.sh` runs the full suite. These tests do not establish unlimited
input depth/size support or power-loss-safe disk persistence.
