#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <variant>

namespace data_saver
{

template <typename... Types> class DataSaver;

namespace detail
{

template <typename... Ptrs> auto make_data_saver(Ptrs &&...ptrs)
{
    return DataSaver<typename std::decay_t<Ptrs>::element_type...>(std::forward<Ptrs>(ptrs)...);
}

} // namespace detail

template <typename... Types> class DataSaver
{
  public:
    DataSaver(std::shared_ptr<Types>... ptrs) : ptrs_(std::move(ptrs)...)
    {
    }

    DataSaver() = default;

    auto Construct() const
    {
        return [ptrs = ptrs_]() { return ptrs; };
    }

    template <typename Func> auto Construct(Func &&func) const
    {
        return [ptrs = ptrs_, func = std::forward<Func>(func)](auto &&...args) -> decltype(auto) {
            return func(std::forward<decltype(args)>(args)...);
        };
    }

    template <size_t I> decltype(auto) get()
    {
        return std::get<I>(ptrs_);
    }

    template <size_t I> decltype(auto) get() const
    {
        return std::get<I>(ptrs_);
    }

    auto operator[](size_t index) const
    {
        using Variant = std::variant<std::shared_ptr<Types>...>;
        if (index >= sizeof...(Types))
            throw std::out_of_range("DataSaver::operator[]: index out of range");
        return getVariant(index, std::index_sequence_for<Types...>{});
    }

    const auto &asTuple() const
    {
        return ptrs_;
    }

  private:
    std::tuple<std::shared_ptr<Types>...> ptrs_;

    template <size_t... Is> auto getVariant(size_t index, std::index_sequence<Is...>) const
    {
        using Variant = std::variant<std::shared_ptr<Types>...>;
        Variant result;
        ((index == Is ? (result = std::get<Is>(ptrs_)) : void()), ...);
        return result;
    }
};

template <typename Func, typename T> auto operator|(Func &&func, std::shared_ptr<T> ptr)
{
    return DataSaver(ptr).Construct(func);
}

template <typename Func, typename... Us> auto operator|(Func &&func, DataSaver<Us...> saver)
{
    return saver.Construct(func);
}

// operator+ overloads

// shared_ptr + shared_ptr
template <typename T, typename U> auto operator+(std::shared_ptr<T> lhs, std::shared_ptr<U> rhs)
{
    return detail::make_data_saver(std::move(lhs), std::move(rhs));
}

// shared_ptr + DataSaver
template <typename T, typename... Us> auto operator+(std::shared_ptr<T> lhs, const DataSaver<Us...> &rhs)
{
    return std::apply([&lhs](const auto &...rhs_ptrs) { return detail::make_data_saver(std::move(lhs), rhs_ptrs...); },
                      rhs.asTuple());
}

// DataSaver + shared_ptr
template <typename... Ts, typename U> auto operator+(const DataSaver<Ts...> &lhs, std::shared_ptr<U> rhs)
{
    return std::apply([&rhs](const auto &...lhs_ptrs) { return detail::make_data_saver(lhs_ptrs..., std::move(rhs)); },
                      lhs.asTuple());
}

// DataSaver + DataSaver
template <typename... Ts, typename... Us> auto operator+(const DataSaver<Ts...> &lhs, const DataSaver<Us...> &rhs)
{
    return std::apply([](const auto &...lhs_ptrs,
                         const auto &...rhs_ptrs) { return detail::make_data_saver(lhs_ptrs..., rhs_ptrs...); },
                      std::tuple_cat(lhs.asTuple(), rhs.asTuple()));
}

} // namespace data_saver
