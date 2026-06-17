# mlsl (My Little Standard Library)

mlsl is a C++23 library providing memory-aware utilities, containers, ownership types, and allocator support.

## Features

* Allocator abstractions
* Arena and pool allocators
* Smart pointers such as `UniquePtr`, `SharedPtr`, `WeakPtr`, `IntrusivePtr`
* Utilities such as `Span`, `StaticBuffer`, `ObjectPool`
* Allocator-aware strings
* Containers including `Array`, `List`, `Deque`, `Queue`, `Stack`, `RingBuffer`, `HashMap`, and `HashSet`
* Fixed and inline container variants
* No exceptions 🥹

## Brief Example

```cpp
mlsl::StaticBuffer<1024> memory;
mlsl::LinearArena arena(memory.Data(), memory.Size());

mlsl::List<int> values(&arena);
values.AddBack(10);
values.AddBack(20);
```

## Requirements

* C++23
* Premake
* MSVC
* Catch2 (optionally, for tests)

## Building

Generate project files:

```bash
premake5 vs2022
```

Open the generated solution in Visual Studio and build the library.

## License

mlsl is licensed under the Apache License 2.0. See [LICENSE](LICENSE.txt) for details.
