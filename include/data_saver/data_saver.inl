#pragma once

#include "data_saver.hpp"

namespace DataSaver
{

    template <typename... Types>
    DataSaver<Types...>::DataSaver(Types... args)
        : data_(std::make_shared<Types>(std::move(args))...)
    {
    }

    template <typename... Types>
    DataSaver<Types...>::DataSaver(std::tuple<std::shared_ptr<Types>...> tup)
        : data_(std::move(tup))
    {
    }

    template <typename... Types>
    const std::tuple<std::shared_ptr<Types>...> &DataSaver<Types...>::getTuple() const
    {
        return data_;
    }

    template <typename... Types>
    auto DataSaver<Types...>::Construct() const
    {
        auto captured = data_;
        return [captured = std::move(captured)]()
        {
            return DataSaver<Types...>(captured);
        };
    }

    template <typename... Types>
    template <typename>
    auto DataSaver<Types...>::operator->() const
    {
        static_assert(sizeof...(Types) == 1, "operator-> requires exactly one type");
        return std::get<0>(data_).get();
    }

    template <typename... Types>
    template <typename>
    auto &DataSaver<Types...>::operator*() const
    {
        static_assert(sizeof...(Types) == 1, "operator* requires exactly one type");
        return *std::get<0>(data_);
    }

    template <typename... Types>
    template <typename>
    auto DataSaver<Types...>::operator[](size_t index) const
    {
        static_assert(sizeof...(Types) != 1, "operator[] is only for multiple types");
        return detail::tuple_get_at(data_, index);
    }

    template <typename... Types>
    template <size_t I>
    auto &DataSaver<Types...>::get() const
    {
        return *std::get<I>(data_);
    }

    template <typename... Types1, typename... Types2>
    auto operator+(const DataSaver<Types1...> &lhs, const DataSaver<Types2...> &rhs)
    {
        auto combined = std::tuple_cat(lhs.getTuple(), rhs.getTuple());
        return DataSaver<Types1..., Types2...>(std::move(combined));
    }

} // namespace DataSaver
