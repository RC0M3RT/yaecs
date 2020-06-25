#pragma once

namespace yaecs{
namespace mpl{

/**
 * @brief 
 * 
 * @tparam Ts Types
 */
template<typename... Ts>
struct type_list {};

namespace detail{
    template <typename seq, typename T>
    struct prepend;

    template <typename... Ts, typename T>
    struct prepend<T, type_list<Ts...>>
    {
        using type = type_list<T, Ts...>;
    };
}

template <typename seq, typename T>
using prepend_t = typename detail::prepend<T, seq>::type;

} // namespace mpl
} // namespace yaecs