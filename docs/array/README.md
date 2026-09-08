# Array

`Array` is an owning sequence. Its public contracts are documented in
[Array.hpp](../../Siv3D/include/Siv3D/Array.hpp); the bool specialization is in
[BoolArray.ipp](../../Siv3D/include/Siv3D/detail/BoolArray.ipp).

The [Array-like guide](array-like.md) applies these rules to geometry, grids,
images, strings, bytes, and audio, and describes their class-specific units.

## Choosing an operation

Use `take(n)` to copy a prefix and `drop(n)` to copy the remainder. Both clamp
`n` to the array size. `take_while(f)` and `drop_while(f)` split at the first
value for which the predicate is false. `slice(index)` copies through the end;
`slice(index, length)` requires the entire requested interval to exist.

`get_if(index)` and `find_if(f)` return a pointer or `nullptr`. `indexOf_if(f)`
returns an optional index, which can outlive the array. An index does not keep
an element alive or track it through later insertions, erasures, or sorting.

`filter(f)` preserves the order of matching elements. When called on a movable
array, it compacts the existing storage. Use a named array when the original
contents must remain available; use `std::move(array)` when the storage can be
consumed. An exception during compaction can leave some elements moved from.

## Ownership and borrowing

Element access on a mutable temporary returns a value. Fluent operations such
as `append`, `fill`, and `operator <<` preserve ownership when chained from a
temporary. Direct borrowing operations, including iterators, spans, and pointer
searches, require an lvalue. Accessors that would otherwise borrow from a const
temporary reject that call explicitly.

A view from a mutable temporary owns that array. A view from an lvalue borrows
from it. This distinction matters even though both can be used in a range-for
loop. Passing a temporary through an external function that returns a reference
can still hide its lifetime; ref-qualified members cannot prevent that.

`Array<bool>` stores addressable bool elements, rather than `std::vector<bool>`
proxies. Its borrowed pointers and spans follow the same lifetime rules.

## Allocators and reusable storage

Operations deriving an array of the same element type retain the source
allocator. Ordinary copy construction and assignment continue to follow
`allocator_traits`; they are not overridden by this rule.

`chunk` and `in_groups` retain the source allocator type and object in each
inner array. The outer array uses its default allocator. `flatten` accepts inner
arrays with different allocator types and produces an array with the default
allocator. `map` and `parallel_map` also use the default allocator for their
result element type.

`release()` returns owned dynamic storage to the allocator. A pool allocator
may retain that memory; this does not promise a decrease in process memory.
The bool specialization may retain inline storage. Prefer `clear()` when the
allocation will be reused.

## Comparisons and parallel work

Default sorting needs `<` to define a strict weak ordering. Consecutive duplicate
removal uses `==`. `sort_and_unique()` keeps that equality meaning: removing all
duplicates requires ordering equivalence and equality to agree. Stable duplicate
removal uses hashing and needs copyable keys, equality, and `std::hash` or an
ADL `hash_value`; equal keys must hash equally.

`parallel_map` on the general Array template preserves output order, but does not
promise invocation order. The bool specialization does not provide parallel
operations; a general Array can still produce bool results.
Workers share the callable and its captured state. Synchronization of mutable
shared state belongs to the caller. Started workers finish before an exception
is propagated. Results need construction and move construction; default
construction and assignment are not required. Cheap result types use direct
writes, while other types use per-worker construction followed by ordered
combination. Parallel execution is not automatically faster for small jobs.

## Migration within siv8

- Replace `Array::head(n)` with `take(n)`. `head_span` and `head_view` retain their
  distinct borrowing and view meanings.
- Bind an array to a local variable before retrieving an iterator, span, or
  element pointer. Mutators returning a borrowed iterator or element reference also
  require an lvalue.
- `partition` and `stable_partition` now return the boundary iterator specified
  by their documentation, rather than a subrange.
- Self copy append duplicates the values; self move append does nothing.
- `asArray()` adapters must return the destination Array type or its
  `container_type`, including the allocator type.

## Validation

[Array tests](../../Test/Test_Array.cpp) exercise value results, rejected borrowing
expressions, generic element types, allocator identity, storage reuse, and
parallel exceptions. Run the documented macOS workflow from the repository root:

```sh
./macOS/run-tests.sh '--test-case=Array*'
./macOS/run-tests.sh
```

The [test strategy and instrumentation guide](testing.md) describes exception
injection, deterministic model checks, bool storage boundaries, and isolated
ASan/UBSan and coverage runs.
