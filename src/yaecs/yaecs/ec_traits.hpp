#pragma once

#include <bitset>

#include "../mpl/mpl.hpp"

namespace yaecs{

template<typename... Ts>
using signature =  yaecs::mpl::type_list<Ts...>;

template<typename... Ts>
using signature_list = yaecs::mpl::type_list<Ts...>;

template<typename... Ts>
using component_list = yaecs::mpl::type_list<Ts...>;

/**
 * @brief Entity component traits
 * 
 * @tparam CL component list
 * @tparam SL signature list
 */
template<typename CL, typename SL>
struct ec_traits
{
    using component_list                        = CL;
    using signature_list                        = SL;

    using component_signature_storage_t         = std::bitset< yaecs::mpl::size_v<component_list> >;

    template<typename T>
    static constexpr bool is_component(){
        return yaecs::mpl::contains_v<T, component_list>;
    }

    template<typename T>
    static constexpr bool is_signature(){
        return yaecs::mpl::contains_v<T, signature_list>;
    }

    template<typename T>
    static constexpr int component_index(){
        if constexpr(yaecs::mpl::contains_v<T, component_list>){
            return yaecs::mpl::index_v<T, component_list>;
        }
        else{
            return -1;
        }
    }

    template<typename T>
    static constexpr int signature_index(){
        if constexpr(yaecs::mpl::contains_v<T, signature_list>){
            return yaecs::mpl::index_v<T, signature_list>;
        }
        else{
            return -1;
        }
    }

    static constexpr int signature_count(){
        return signature_count_;
    }

    static constexpr int component_count(){
        return component_count_;
    }

    constexpr static int component_count_{yaecs::mpl::size_v<component_list>};
    constexpr static int signature_count_{yaecs::mpl::size_v<signature_list>};
};


} // namespace yaecs