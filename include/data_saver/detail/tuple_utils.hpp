#pragma once

#include <tuple>
#include <variant>
#include <memory>
#include <stdexcept>
#include <utility>

namespace DataSaver::detail
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

} // namespace DataSaver::detail
