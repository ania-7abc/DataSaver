# DataSaver

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Header-only](https://img.shields.io/badge/header--only-brightgreen.svg)](https://github.com/ania-7abc/DataSaver)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**DataSaver** – a minimalistic C++17 header‑only library that extends object lifetime and provides convenient access to heterogeneous data. It manages memory via `std::shared_ptr`, allows merging multiple value sets, and offers type‑safe raw pointer access.

## Features

- ✅ Lifetime extension – capture all `shared_ptr`s in a lambda via `Construct()`
- ✅ Merge multiple `DataSaver` instances with `operator+`
- ✅ Intuitive access:
  - single type → `->`, `*`, implicit conversion to `T*`
  - multiple types → runtime `operator[]` (returns `std::variant<Types*...>`), compile‑time `get<Is>()` (returns `std::tuple<Types*...>`)
- ✅ Header‑only – no compilation needed
- ✅ C++17 standard (fold expressions, index_sequence)

## Installation

### Using CMake FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
  DataSaver
  GIT_REPOSITORY https://github.com/ania-7/DataSaver.git
  GIT_TAG        v2.0.0
)

FetchContent_MakeAvailable(DataSaver)

target_link_libraries(your_target PRIVATE DataSaver)
```

### Manual

Copy `data_saver.hpp` into your project and include it.

## Usage Example

```cpp
#include "data_saver.hpp"
#include <iostream>
#include <variant>

struct Dog {
    void bark() const { std::cout << "Woof!\n"; }
};

int main() {
    using namespace data_saver;

    // Single object
    DataSaver<int> i(42);
    DataSaver<std::string> s("hello");
    DataSaver<Dog> d(Dog{});

    std::cout << *i << "\n";   // 42
    d->bark();                 // Woof!

    int* raw_i = i;            // implicit conversion

    // Merge
    auto merged = i + s + d;   // DataSaver<int, std::string, Dog>

    // Runtime index access
    auto v0 = merged[0];
    std::visit([](auto* p) { std::cout << *p << "\n"; }, v0); // 42

    // Compile‑time access (tuple of raw pointers)
    auto [intPtr, strPtr, dogPtr] = merged.get<0, 1, 2>();
    std::cout << *strPtr << "\n";  // hello
    dogPtr->bark();                // Woof!

    // Lifetime extension
    auto saver = merged.Construct();
    auto later = saver();          // new DataSaver owning the same data
    later[0];                      // still accessible

    return 0;
}
```

## API Reference

### `template<typename... Types> class DataSaver`

| Method | Description |
|--------|-------------|
| **Construction** | |
| `explicit DataSaver(Args&&... args)` | (single type only) Forwards arguments to construct the single object inside a `shared_ptr`. |
| `explicit DataSaver(Types... args)` | (multiple types only) Moves each argument into a separate `shared_ptr`. |
| `explicit DataSaver(std::tuple<std::shared_ptr<Types>...> tup)` | Constructs from an existing tuple of `shared_ptr`s. |
| **Lifetime** | |
| `auto Construct() const` | Returns a lambda that captures copies of all `shared_ptr`s. When called, returns a new `DataSaver` with the same data. |
| **Access – single type only** | |
| `operator->()` | Member access via raw pointer. |
| `operator*()` | Dereference to the stored object. |
| `operator Types*()` | Implicit conversion to raw pointer. |
| **Access – multiple types only** | |
| `auto operator[](size_t index) const` | Runtime index access. Returns `std::variant<Types*...>`. Throws `std::out_of_range`. |
| `template<size_t... Is> auto get() const` | Compile‑time multi‑get. Returns `std::tuple<Types*...>` of raw pointers. |
| **Merging** | |
| `friend operator+(const DataSaver<A...>&, const DataSaver<B...>&)` | Merges two `DataSaver` instances into one containing all objects from both. Result type: `DataSaver<A..., B...>`. |

> **Note:** `getTuple()` is a private member – not part of the public API.

## License

MIT © ania_7 (Anya Baykina Dmitrievna). See [LICENSE](LICENSE) for details.
