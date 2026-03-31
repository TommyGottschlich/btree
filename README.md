# btree

A generic B-tree implementation in C++, templated on both value type and tree order. Supports insert, delete, and search while maintaining all B-tree structural invariants.

## Design Notes

**Raw pointer ownership is intentional.** This project was written as an exercise in manual memory management — tracking ownership, preventing double-frees, and correctly handling pointer transfer during splits and merges. Production code would use `std::unique_ptr`.

**Both recursive and iterative insert are implemented.** The default path is iterative; the recursive path can be selected via `insert(value, true)`. Both maintain identical invariants — the two implementations exist to compare the approaches.

**`validate()` checks structural invariants** after any mutation: all leaves at the same depth, every non-root node meets minimum occupancy, and all keys are in sorted order within their bounds.

## Complexity

| Operation | Average | Worst |
|-----------|---------|-------|
| Search    | O(log n) | O(log n) |
| Insert    | O(log n) | O(log n) |
| Delete    | O(log n) | O(log n) |
| Space     | O(n)     | O(n)     |

## Usage

```cpp
ds::Btree<int, 3> tree;   // 2-3 tree
tree.insert(5);
tree.insert(10);
tree.contains(5);         // true
tree.remove(5);
tree.validate();          // check structural invariants
tree.size();              // number of keys
tree.height();
tree.print();
```

The `ORDER` template parameter must be >= 3. A `static_assert` enforces this at compile time.

## Build

```sh
./support/build.sh
```

```sh
./support/clean.sh
```

**Run:**
```sh
./build/arm64/btree/btree
```

**Test:**
```sh
ctest --test-dir build/arm64
```

Replace `arm64` with `x86_64` on Intel.
