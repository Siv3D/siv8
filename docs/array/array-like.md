# Array-like owners and views

The ownership rules described in the [Array guide](README.md) also apply to
`LineString`, `MultiPolygon`, `Grid`, `Image`, `String`, `Blob`, and `Wave`.
Their public headers define the exact contracts; this guide explains how to
choose and migrate operations across the classes.

## Borrowing and method chains

Retrieve pointers, iterators, spans, and row or column views from a named owner.
These results follow the underlying storage's lifetime and invalidation rules.
An owning temporary can provide an element by value, but cannot provide a
borrowed pointer or reference. Const temporaries are explicitly rejected by
borrowing accessors.

```cpp
LineString line{ Vec2{ 0, 0 }, Vec2{ 10, 20 } };
const Vec2* point = line.get_if(1);
auto vertices = line.subspan(0, 2);

auto transformed = LineString{ Vec2{ 0, 0 }, Vec2{ 10, 20 } }
    .drop(1)
    .moveBy(5, 0); // owns its vertices
```

Fluent mutators return the original object by reference on lvalues and an owned
result on mutable rvalues. This includes sequence append/fill/assignment helpers,
LineString and MultiPolygon geometry mutators, Grid fill/paste helpers, and
String mutators. Ordinary assignment operators retain their conventional
reference result. APIs with a `void` result, such as `Image::fill` and
`Blob::append`, retain that result type.

`LineString::asArray`, `MultiPolygon::asArray`, `Blob::asArray`, and
`Grid::getContainer` transfer their array when called on a mutable rvalue.
`String::str` likewise transfers its underlying string. Bind the returned owner
to a variable before borrowing from it.

## Operations and units

| Class | Sequence operations and units |
| --- | --- |
| `LineString` | `get_if`, `find_if`, `indexOf_if`, `take`, `drop`, `drop_while`, `slice`, and `filter` operate on vertices in order. |
| `MultiPolygon` | The same operations work on whole polygons; consuming operations move polygons and preserve their internal storage. |
| `String` | Searches, `take`, `drop`, `drop_while`, and `filter` operate on UTF-32 code units. Use `substr` for intervals. These operations do not count grapheme clusters. |
| `Blob` | `get_if`, `take`, `drop`, and `slice` use byte positions and counts. |
| `Wave` | `get_if`, `take`, `drop`, and `slice` use stereo frames. Derived waves preserve the sample rate, including empty results. |
| `Grid`, `Image` | `get_if(Point)` and `get_if(y, x)` use two-dimensional coordinates and return `nullptr` outside the bounds. Row, column, and rectangular operations retain their spatial meaning. |

On mutable rvalues, sequence slicing and filtering reuse the owner's storage.
On lvalues they return an independent result. `take` and `drop` clamp their count;
`slice` requires the complete requested interval to exist. Predicate exceptions
propagate to the caller; an in-place operation may have partially changed the
contents before an exception.

No generic filtering or dropping operation is added to images or grids: the
existing row, column, and rectangle APIs describe their shape explicitly.
Wave filtering is also separate from slicing, since deleting frames changes
the signal's time axis.

## Allocators, dimensions, and release

Grid transformations producing the same element type retain the source's
allocator. Ordinary copies and assignments use the usual `allocator_traits`
rules. Array-based Grid construction and deduction also retain the allocator
type. `Grid::map` and `parallel_map` produce a Grid of the mapped element type
with its default allocator, preserving dimensions and row-major order.
`parallel_map` supports results that cannot be default constructed or assigned;
its callable-sharing and exception rules are the same as Array's.

Moving a Grid leaves its source with zero dimensions and no elements. Moving
its container out has the same effect. Self move assignment leaves it unchanged.

`release` clears the owner and returns dynamic storage to its allocator; this
is not a promise that the allocator returns memory to the operating system.
Grid and Image also reset their dimensions. String can retain its inline
storage. Use `clear` to retain capacity for reuse.

## StringView is a borrower

A StringView does not own its characters. A temporary **view** can safely return
pointers or split views while its backing string remains alive. Restrictions
on temporary owning strings do not apply to a temporary view object itself.

```cpp
const String text = U"alpha,beta";
const auto parts = StringView{ text }.splitView(U',');
// parts borrows text, so text must outlive every use of parts.
```

Implicit conversion from String to StringView, and construction from a
`std::u32string`, retain lifetime annotations. They support passing a temporary
owner to a function that consumes the view during the call. They do not extend
the owner's lifetime. In particular, do not keep the view after that full
expression ends. `String::subview`, `splitView`, and `splitLines` require a named
owner, including when the source is const.

## Migration and verification

- Replace owning `head(n)` calls on LineString, MultiPolygon, String, and
  StringView with `take(n)`.
- Bind owners before direct borrowing, including `Image[y]`, row/column views,
  and iterator-returning insert/erase operations.
- A LineString `asArray()` adapter must produce `Array<Vec2>` or its exact
  container type. Member-pointer callables use `std::invoke` semantics.
- LineString and MultiPolygon self copy append duplicates the sequence; their
  self move append leaves it unchanged. Blob can also append an overlapping byte
  interval from its own live storage. String retains its string-style append
  semantics for the source argument.

The shared [borrowing checks](../../Test/Test_ArrayLike.cpp) reject each unsafe
expression independently. Class tests cover boundaries, failures, storage reuse,
and the relevant allocator, dimension, or sample-rate invariants:
[LineString](../../Test/Test_LineString.cpp),
[MultiPolygon](../../Test/Test_MultiPolygon.cpp), [Grid](../../Test/Test_Grid.cpp),
[Image](../../Test/Test_Image.cpp), [String](../../Test/Test_String.cpp),
[StringView](../../Test/Test_StringView.cpp), [Blob](../../Test/Test_Blob.cpp),
and [Wave](../../Test/Test_Wave.cpp).
