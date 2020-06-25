#pragma once

#include <type_traits>
#include <cstdint>

namespace yaecs{
namespace mpl{

/**
 * @brief type list
 * 
 * @tparam Ts Types
 */
template<typename... Ts>
struct type_list {};

namespace detail{

    template<typename seq>
    struct size_impl;

    template<template <typename...> class seq, typename... Ts>
    struct size_impl<seq<Ts...>>{
        using type = std::integral_constant<std::size_t, sizeof...(Ts)>;
    };

    template<typename T, typename seq>
    struct index_impl;

    template<typename T, template <typename...> class seq, typename... Ts>
    struct index_impl<T, seq<T, Ts...>> : std::integral_constant<std::size_t, 0> {};

    template<typename T, typename U, template <typename...> class seq, typename... Ts>
    struct index_impl<T, seq<U, Ts...>> : std::integral_constant<std::size_t, 1 + index_impl<T, seq<Ts...>>::value> {};

    template<typename T, typename seq>
    struct contains_impl;

    template<typename T, template <typename...> class seq, typename... Ts>
    struct contains_impl<T, seq<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};

}

/**
 * @brief Gives true if \tparam seq contains \tparam T , else false
 * 
 * @tparam T 
 * @tparam seq type_list<...>
 */
template<typename T, typename seq>
constexpr bool contains_v = detail::contains_impl<T, seq>::value;

/**
 * @brief Gives the index of \tparam T in \tparam seq
 * 
 * @tparam T   
 * @tparam seq type_list<...>
 */
template<typename T, typename seq>
constexpr std::size_t index_v = detail::index_impl<T, seq>::value;

/**
 * @brief Gives the size type of given type list
 * 
 * @tparam seq type_list<...>
 */
template<typename seq>
using size_type = typename detail::size_impl<seq>::type;

/**
 * @brief Gives the size of given type list
 * 
 * @tparam seq type_list<...>
 */
template<typename seq>
constexpr std::size_t size_v = detail::size_impl<seq>::type::value;

} // namespace mpl
} // namespace yaecs