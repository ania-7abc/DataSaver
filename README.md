# DataSaver

A small library for C++17 that prevents dangling pointers when passing lambdas that use raw pointers obtained from `std::shared_ptr`.

## The Problem

If a lambda captures a raw pointer obtained via `.get()` from a `std::shared_ptr`, the pointer becomes dangling after the `shared_ptr` is destroyed.  
`DataSaver` solves this by binding the lambda together with the `shared_ptr`s themselves, extending the lifetime of the objects for the entire lifetime of the lambda.

## How It Works

`DataSaver<Types...>` stores a tuple of `std::shared_ptr<Types>`.  
Its `Construct(Func&&)` method returns a new lambda that captures **copies** of those pointers **and** your function. All arguments are forwarded to the original function.  
The `|` and `+` operators provide convenient syntax for building such bundles.

## Requirements

- C++17
- Standard library only

## Example

```cpp
#include "data_saver.hpp"
#include <ftxui/dom/elements.hpp>   // example third-party API
#include <memory>
#include <string>

auto make_safe_input() {
    using namespace data_saver;
    using namespace ftxui;

    auto content = std::make_shared<std::string>();
    auto option = InputOption::Default();
    // InputOption::content expects a raw pointer
    option.content = StringRef(content.get());

    // The lambda captures only option, not the shared_ptr.
    // If returned as is, content will die and option.content will be dangling.
    // DataSaver via operator| binds the lambda together with the shared_ptr:
    return [option]() { return Input(option); } | content;
}

int main() {
    auto safe_lambda = make_safe_input();
    // content is still alive (stored inside safe_lambda)
    auto element = safe_lambda();   // returns ftxui::Input
}
```

## Operators

### `operator|` – passing a function to DataSaver

```cpp
auto saver = DataSaver(ptr1, ptr2);
auto composed = saver | [](auto&&... args) { /* ... */ };

// Symmetric form: function on the left, shared_ptr on the right
auto composed = some_lambda | some_shared_ptr;
```

### `operator+` – combining pointers and DataSaver

```cpp
auto saver1 = ptr1 + ptr2;                // DataSaver<T1,T2>
auto saver2 = saver1 + ptr3;              // DataSaver<T1,T2,T3>
auto saver3 = ptr4 + saver2;              // DataSaver<T4,T1,T2,T3>
auto saver4 = saver2 + saver3;            // combining two DataSaver
```

## Class Interface

```cpp
template <typename... Types>
class DataSaver {
public:
    DataSaver(std::shared_ptr<Types>... ptrs);
    DataSaver() = default;

    // Return a lambda that returns a tuple of pointers (without user function)
    auto Construct() const;

    // Return a lambda that calls func and stores the pointers
    template <typename Func>
    auto Construct(Func&& func) const;

    template <size_t I>
    decltype(auto) get();

    auto operator[](size_t index) const;  // returns std::variant

    const auto& asTuple() const;
};
```

## When to Use

- You are working with an API that expects raw pointers (e.g., `StringRef`, `std::string_view`‑like types, legacy code).
- You need to return a lambda that uses such a raw pointer, but the original `shared_ptr` goes out of scope.
- You want a simple way to bundle `shared_ptr`s together with a function.
