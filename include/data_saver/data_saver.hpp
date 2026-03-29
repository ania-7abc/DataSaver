// data_saver.hpp
#pragma once

#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace data_saver
{

namespace detail
{
template <typename... Ts> decltype(auto) tuple_get_at(const std::tuple<std::shared_ptr<Ts>...> &tup, size_t index)
{
    using result_t = std::variant<Ts *...>;
    result_t result;
    bool found = false;
    [&]<size_t... I>(std::index_sequence<I...>) {
        ((index == I ? (result = std::get<I>(tup).get(), found = true) : void()), ...);
    }(std::index_sequence_for<Ts...>{});
    if (!found)
        throw std::out_of_range("DataSaver::operator[]: index out of range");
    return result;
}
} // namespace detail

/**
 * @brief Holds a tuple of shared_ptr<Types>
 * @tparam Types List of managed types
 *
 * The Construct() method returns a lambda that captures the tuple of shared_ptrs,
 * keeping the managed objects alive until the lambda dies
 */
template <typename... Types> class DataSaver
{
  public:
    // common

    /// @brief Construct from an explicit tuple of shared_ptrs.
    explicit DataSaver(std::tuple<std::shared_ptr<Types>...> tup) : data_(std::move(tup))
    {
    }

    /**
     * @brief Returns a lambda that captures the shared_ptrs (keeps objects alive)
     * @return Lambda: []() -> DataSaver<Types...>
     *
     * Calling the lambda rebuilds a DataSaver from the captured tuple
     */
    auto Construct() const
    {
        auto captured = data_;
        return [captured = std::move(captured)]() { return DataSaver<Types...>(captured); };
    }

    /// @brief Concatenate two DataSavers with different type packs
    template <typename... Types1, typename... Types2>
    friend auto operator+(const DataSaver<Types1...> &lhs, const DataSaver<Types2...> &rhs)
    {
        auto combined = std::tuple_cat(lhs.getTuple(), rhs.getTuple());
        return DataSaver<Types1..., Types2...>(std::move(combined));
    }

    // single type

    /// @brief Forward arguments to constructor of the single type
    template <typename... Args, typename = std::enable_if_t<sizeof...(Types) == 1>>
    explicit DataSaver(Args &&...args)
        : data_(std::make_shared<std::tuple_element_t<0, std::tuple<Types...>>>(std::forward<Args>(args)...))
    {
    }

    /// @brief Pointer‑like member access
    template <typename = std::enable_if_t<sizeof...(Types) == 1>> auto operator->() const
    {
        static_assert(sizeof...(Types) == 1, "operator-> requires exactly one type");
        return std::get<0>(data_).get();
    }

    /// @brief Dereference
    template <typename = std::enable_if_t<sizeof...(Types) == 1>> auto &operator*() const
    {
        static_assert(sizeof...(Types) == 1, "operator* requires exactly one type");
        return *std::get<0>(data_);
    }

    /// @brief Implicit conversion to raw pointer
    template <typename = std::enable_if_t<sizeof...(Types) == 1>>
    operator std::tuple_element_t<0, std::tuple<Types...>> *() const
    {
        static_assert(sizeof...(Types) == 1, "conversion to Types* requires exactly one type");
        return std::get<0>(data_).get();
    }

    // multiple types

    /// @brief Construct from one value per type, moved into shared_ptrs
    template <typename = std::enable_if_t<sizeof...(Types) != 1>>
    explicit DataSaver(Types... args) : data_(std::make_shared<Types>(std::move(args))...)
    {
    }

    /**
     * @brief Indexed access, returns variant of raw pointers
     * @param index Position
     * @throw std::out_of_range
     */
    template <typename = std::enable_if_t<sizeof...(Types) != 1>> auto operator[](size_t index) const
    {
        static_assert(sizeof...(Types) != 1, "operator[] is only for multiple types");
        return detail::tuple_get_at(data_, index);
    }

    /**
     * @brief Compile‑time multi‑get, returns tuple of raw pointers
     * @tparam Is Indices to retrieve
     */
    template <size_t... Is> auto get() const
    {
        static_assert(sizeof...(Types) != 1, "get for multiple indices is only for multiple types");
        return std::forward_as_tuple(std::get<Is>(data_).get()...);
    }

  private:
    std::tuple<std::shared_ptr<Types>...> data_;

    const auto &getTuple() const
    {
        return data_;
    }
};

} // namespace data_saver
