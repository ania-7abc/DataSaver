# DataSaver

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Header-only](https://img.shields.io/badge/header--only-brightgreen.svg)](https://github.com/ania-7abc/DataSaver)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**DataSaver** – a minimalistic C++17 header‑only library that extends object lifetime and provides convenient access to heterogeneous data. It manages memory via `std::shared_ptr`, allows merging multiple value sets, and offers type‑safe raw pointer access.

## Features

- ✅ Lifetime extension – capture all `shared_ptr`s in a lambda via `Construct()`
- ✅ Merge multiple `DataSaver` instances with `operator+`
- ✅ Intuitive access:
  - single type → `->`, `*`, implicit conversion to `T*`, and `raw()`
  - multiple types → `operator[]` (returns `std::variant<Types*...>`), `raw(index)`, compile‑time `get<N>()` and `raw<N>()`
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

Copy `data_saver.hpp` into your project and include it.

## Usage Example

```cpp
#include "data_saver.hpp"
#include <iostream>

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
    int* raw_i2 = i.raw();

    // Merge
    auto merged = i + s + d;   // DataSaver<int, std::string, Dog>

    // Runtime index access
    auto v0 = merged[0];
    std::visit([](auto* p) { std::cout << *p << "\n"; }, v0); // 42

    auto v1 = merged.raw(1);   // std::variant<std::string*, Dog*>

    // Compile‑time access
    std::cout << merged.get<1>() << "\n"; // hello
    merged.get<2>().bark();               // Woof!
    std::string* sp = merged.raw<1>();

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
| `auto Construct() const` | Returns a lambda capturing copies of all `shared_ptr`s. When called, returns a new `DataSaver` with the same data. |
| **Single type only** | |
| `operator->()`, `operator*()` | Access the stored object. |
| `operator Types*()` | Implicit conversion to raw pointer. |
| `Types* raw() const` | Explicit raw pointer access. |
| **Multiple types only** | |
| `operator[](size_t index) const` | Runtime index access. Returns `std::variant<Types*...>`. |
| `auto raw(size_t index) const` | Same as `operator[]`. |
| `template<size_t I> auto& get() const` | Compile‑time reference access. |
| `template<size_t I> auto* raw() const` | Compile‑time raw pointer access. |
| **Both** | |
| `const std::tuple<std::shared_ptr<Types>...>& getTuple() const` | Returns the internal tuple of `shared_ptr`. |

### `operator+(const DataSaver<A...>&, const DataSaver<B...>&)`

Merges two `DataSaver` instances into one containing all objects from both. Result type: `DataSaver<A..., B...>`.

## License

MIT © ania_7 (Anya Baykina Dmitrievna). See [LICENSE](LICENSE) for details.
