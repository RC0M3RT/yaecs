#pragma once

#include <cstdint>
#include <tuple>
#include <array>
#include <vector>
#include <bitset>
#include <type_traits>

#include "mpl/mpl.hpp"

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

    constexpr static int component_count_{yaecs::mpl::size_v<component_list>};
    constexpr static int signature_count_{yaecs::mpl::size_v<signature_list>};
};


////---------------------------------------------------------------

/**
 * @brief 
 * 
 * @tparam ECT entity component traits
 */
template<typename ECT>
struct entity{
    
    using component_signature_storage = typename ECT::component_signature_storage_t;

    std::uint32_t id;
    component_signature_storage signatures;
};

/**
 * @brief 
 * 
 * @tparam ECT entity component traits
 */
template<typename ECT>
class signature_storage{
    using signature_list                    = typename ECT::signature_list;
    using component_signature_storage_t     = typename ECT::component_signature_storage_t;

public:

    signature_storage(){
        for(std::size_t i=0; i< signature_count_; i++){
            signature_storage_[i] = component_signature_storage_t{false};
        }
    }

    template<typename T>
    auto& get_signature_bitset(){
        static_assert(ECT::template is_signature<T>(), "Requested signature bitset T is not a signature");
        return signature_storage_[ECT::template signature_index<T>()];
    }

private:
    static constexpr int signature_count_{ECT::signature_count()};
    std::array<component_signature_storage_t, signature_count_> signature_storage_;
};

/**
 * @brief 
 * 
 * @tparam ECT entity component traits
 */
template<typename ECT>
class component_storage{
    using component_list = typename ECT::component_list;

    template<typename... Args>
    static std::tuple<std::vector<Args>...> to_tuple(yaecs::mpl::type_list<Args...>);

    template <typename seq>
    struct type_list_to_tuple {
        using type = decltype(to_tuple(std::declval<seq>()));
    };

    template <typename seq>
    using type_list_to_tuple_t = typename type_list_to_tuple<seq>::type;

    template<typename... Ts>
    using storage_t = type_list_to_tuple_t<component_list>;

public:

    template<typename T>
    void add_component(T& c){
        std::vector<T>& component_vector_ = std::get<std::vector<T>>(components_);
        component_vector_.push_back(c);
    }

    template<typename T>
    T& get_component(std::uint32_t data_index) noexcept{
        return std::get<std::vector<T>>(components_)[data_index];
    }

private:
    storage_t<component_list> components_{}; 
};




} // namespace yaecs