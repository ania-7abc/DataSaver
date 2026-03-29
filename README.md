# DataSaver

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Header-only](https://img.shields.io/badge/header--only-brightgreen.svg)](https://github.com/ania-7/DataSaver)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**DataSaver** – a minimalistic C++17 header‑only library that extends object lifetime and provides convenient access to heterogeneous data. It manages memory via `std::shared_ptr`, allows merging multiple value sets, and offers type‑safe access.

## Features

- ✅ Lifetime extension – capture all `shared_ptr`s in a lambda via `Construct()`
- ✅ Merge multiple `DataSaver` instances with `operator+`
- ✅ Intuitive access:
  - single type → `->` and `*` operators
  - multiple types → `operator[]` (returns `std::variant<Types*...>`) + compile‑time `get<N>()`
- ✅ Header‑only – no compilation needed
- ✅ C++17 standard (fold expressions, index_sequence)

## Installation

### Using CMake FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
  DataSaver
  GIT_REPOSITORY https://github.com/ania-7/DataSaver.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(DataSaver)

target_link_libraries(your_target PRIVATE DataSaver)
```

### Manual

Copy the `include/DataSaver` folder into your project and add the include path.

## Usage Example

```cpp
#include <DataSaver/DataSaver.hpp>
#include <iostream>

struct Dog {
    void bark() const { std::cout << "Woof!\n"; }
};

int main() {
    // Different objects
    DataSaver::DataSaver<int> i(42);
    DataSaver::DataSaver<std::string> s("hello");
    DataSaver::DataSaver<Dog> d(Dog{});

    // Single object access
    std::cout << *i << "\n";   // 42
    d->bark();                 // Woof!

    // Merge
    auto merged = i + s + d;   // DataSaver<int, std::string, Dog>

    // Runtime index access
    auto v0 = merged[0];
    std::visit([](auto* p) { std::cout << *p << "\n"; }, v0); // 42

    // Compile‑time index access
    std::cout << merged.get<1>() << "\n"; // hello
    merged.get<2>().bark();               // Woof!

    // Lifetime extension
    auto saver = merged.Construct();
    auto later = saver();    // new DataSaver owning the same data
    later[0];                // still accessible

    return 0;
}
```

## API Reference

### `template<typename... Types> class DataSaver`

| Method | Description |
|--------|-------------|
| `explicit DataSaver(Types... args)` | Stores copies (via move) of the passed values in `shared_ptr`. |
| `auto Construct() const` | Returns a lambda that captures copies of all `shared_ptr`s. When called, returns a new `DataSaver` holding the same data. |
| `operator->()`, `operator*()` | Access the single stored object (only when `sizeof...(Types) == 1`). |
| `operator[](size_t index) const` | Runtime index access. Returns `std::variant<Types*...>`. (For multiple types only). |
| `template<size_t I> auto& get() const` | Compile‑time index access. |

### `operator+(const DataSaver<A...>&, const DataSaver<B...>&)`

Merges two `DataSaver` instances into one containing all objects from both. The resulting type is `DataSaver<A..., B...>`.

## License

MIT © ania_7 (Anya Baykina Dmitrievna). See [LICENSE](LICENSE) for details.
