#pragma once

#include <tuple>
#include <memory>
#include <utility>
#include <type_traits>
#include "detail/tuple_utils.hpp"

namespace DataSaver
{

    template <typename... Types>
    class DataSaver
    {
    public:
        explicit DataSaver(Types... args);

        explicit DataSaver(std::tuple<std::shared_ptr<Types>...> tup);

        const std::tuple<std::shared_ptr<Types>...> &getTuple() const;

        auto Construct() const;

        template <typename = std::enable_if_t<sizeof...(Types) == 1>>
        auto operator->() const;

        template <typename = std::enable_if_t<sizeof...(Types) == 1>>
        auto &operator*() const;

        template <typename = std::enable_if_t<sizeof...(Types) != 1>>
        auto operator[](size_t index) const;

        template <size_t I>
        auto &get() const;

    private:
        std::tuple<std::shared_ptr<Types>...> data_;
    };

    template <typename... Types1, typename... Types2>
    auto operator+(const DataSaver<Types1...> &lhs, const DataSaver<Types2...> &rhs);

} // namespace DataSaver

#include "data_saver.inl"