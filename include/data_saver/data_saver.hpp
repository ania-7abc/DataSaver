// data_saver.hpp
#pragma once

#include <tuple>
#include <memory>
#include <utility>
#include <variant>
#include <stdexcept>
#include <type_traits>

namespace data_saver
{

    namespace detail
    {

        template <typename... Ts>
        decltype(auto) tuple_get_at(const std::tuple<std::shared_ptr<Ts>...> &tup, size_t index)
        {
            using result_t = std::variant<Ts *...>;
            result_t result;
            bool found = false;
            [&]<size_t... I>(std::index_sequence<I...>)
            {
                ((index == I ? (result = std::get<I>(tup).get(), found = true) : void()), ...);
            }(std::index_sequence_for<Ts...>{});
            if (!found)
                throw std::out_of_range("DataSaver::operator[]: index out of range");
            return result;
        }

    }

    template <typename... Types>
    class DataSaver
    {
    public:
        explicit DataSaver(Types... args)
            : data_(std::make_shared<Types>(std::move(args))...)
        {
        }

        explicit DataSaver(std::tuple<std::shared_ptr<Types>...> tup)
            : data_(std::move(tup))
        {
        }

        const std::tuple<std::shared_ptr<Types>...> &getTuple() const
        {
            return data_;
        }

        auto Construct() const
        {
            auto captured = data_;
            return [captured = std::move(captured)]()
            {
                return DataSaver<Types...>(captured);
            };
        }

        template <typename = std::enable_if_t<sizeof...(Types) == 1>>
        auto operator->() const
        {
            static_assert(sizeof...(Types) == 1, "operator-> requires exactly one type");
            return std::get<0>(data_).get();
        }

        template <typename = std::enable_if_t<sizeof...(Types) == 1>>
        auto &operator*() const
        {
            static_assert(sizeof...(Types) == 1, "operator* requires exactly one type");
            return *std::get<0>(data_);
        }

        template <typename = std::enable_if_t<sizeof...(Types) == 1>>
        operator Types *() const
        {
            static_assert(sizeof...(Types) == 1, "operator Types* requires exactly one type");
            return std::get<0>(data_).get();
        }

        template <typename = std::enable_if_t<sizeof...(Types) == 1>>
        Types *raw() const
        {
            static_assert(sizeof...(Types) == 1, "raw() requires exactly one type");
            return std::get<0>(data_).get();
        }

        template <typename = std::enable_if_t<sizeof...(Types) != 1>>
        auto operator[](size_t index) const
        {
            static_assert(sizeof...(Types) != 1, "operator[] is only for multiple types");
            return detail::tuple_get_at(data_, index);
        }

        template <typename = std::enable_if_t<sizeof...(Types) != 1>>
        auto raw(size_t index) const
        {
            static_assert(sizeof...(Types) != 1, "raw(index) is only for multiple types");
            return detail::tuple_get_at(data_, index);
        }

        template <size_t I>
        auto &get() const
        {
            static_assert(I < sizeof...(Types), "Index out of bounds");
            return *std::get<I>(data_);
        }

        template <size_t I>
        auto *raw() const
        {
            static_assert(I < sizeof...(Types), "Index out of bounds");
            return std::get<I>(data_).get();
        }

    private:
        std::tuple<std::shared_ptr<Types>...> data_;
    };

    template <typename... Types1, typename... Types2>
    auto operator+(const DataSaver<Types1...> &lhs, const DataSaver<Types2...> &rhs)
    {
        auto combined = std::tuple_cat(lhs.getTuple(), rhs.getTuple());
        return DataSaver<Types1..., Types2...>(std::move(combined));
    }

}
