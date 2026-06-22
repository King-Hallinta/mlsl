# mlsl (My Little Standard Library)

mlsl is a C++23 library providing memory-aware utilities, containers, ownership types, and allocator support.

## Features

* Allocator abstractions
* Arena and pool allocators
* Smart pointers such as `UniquePtr`, `SharedPtr`, `WeakPtr`, `IntrusivePtr`
* Utilities such as `Span`, `StaticBuffer`, `ObjectPool`
* Allocator-aware strings
* Containers including `Array`, `List`, `Deque`, `Queue`, `Stack`, `RingBuffer`, `Map`, `Set`, `HashMap`, and `HashSet`
* Fixed and inline container variants
* No exceptions 🥹

## Brief Example

```cpp
mlsl::StaticBuffer<1024> memory;
mlsl::LinearArena arena(memory.Data(), memory.Size());

mlsl::List<int> values(&arena);
values.Append(10);
values.Append(20);
```

## Requirements

* C++23
* CMake 3.25+
* MSVC
* Catch2 (optional, for tests)

## Building

Configure with Visual Studio:

```bash
cmake --preset vs2026
```

Build the debug configuration:

```bash
cmake --build --preset vs2026-debug
```

Configure without tests:

```bash
cmake --preset vs2026 -DMLSL_BUILD_TESTS=OFF
```

## License

mlsl is licensed under the Apache License 2.0. See [LICENSE](LICENSE.txt) for details.
