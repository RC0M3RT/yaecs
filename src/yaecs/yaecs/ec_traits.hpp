#pragma once

#include <bitset>

#include "../mpl/mpl.hpp"

namespace yaecs{

template<typename... Ts>
using tag_list = yaecs::mpl::type_list<Ts...>;

template<typename... Ts>
using component_list = yaecs::mpl::type_list<Ts...>;

/**
 * @brief Entity component traits
 * 
 * @tparam CL component list
 * @tparam TL tag list
 */
template<typename CL, typename TL>
struct ec_traits
{
    using component_list                  = CL;
    using tag_list                        = TL;

    using component_signature_storage_t         = std::bitset< yaecs::mpl::size_v<component_list> >;
    using tag_storage_per_entity_t              = std::bitset< yaecs::mpl::size_v<tag_list> >;

    template<typename T>
    static constexpr bool is_component(){
        return yaecs::mpl::contains_v<T, component_list>;
    }

    template<typename T>
    static constexpr bool is_tag(){
        return yaecs::mpl::contains_v<T, tag_list>;
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
    static constexpr int tag_index(){
        if constexpr(yaecs::mpl::contains_v<T, tag_list>){
            return yaecs::mpl::index_v<T, tag_list>;
        }
        else{
            return -1;
        }
    }

    static constexpr int tag_count(){
        return tag_count_;
    }

    static constexpr int component_count(){
        return component_count_;
    }

    constexpr static int component_count_{yaecs::mpl::size_v<component_list>};
    constexpr static int tag_count_{yaecs::mpl::size_v<tag_list>};
};


} // namespace yaecs