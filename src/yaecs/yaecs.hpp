#pragma once

#include <cstdint>
#include <tuple>
#include <array>
#include <vector>
#include <bitset>
#include <type_traits>
#include <cassert>

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

    static constexpr int component_count(){
        return component_count_;
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
class entity{
    using component_signature_storage = typename ECT::component_signature_storage_t;
public:

    entity(std::uint32_t id)
        : id_{id}
        , signatures_{false}
    {
    }

    std::uint32_t id() const noexcept{ return id_; }

    template<typename T>
    void set_signature(bool enabled){
        constexpr auto component_index = ECT::template component_index<T>();
        signatures_[component_index] = enabled;
    }

    template<typename T>
    bool get_signature(){
        constexpr auto component_index = ECT::template component_index<T>();

        if constexpr(component_index == -1){
            return false;
        }
        else{
            return signatures_[component_index];
        }
    }

private:
    std::uint32_t id_;
    component_signature_storage signatures_{false};
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

/**
 * @brief Entity component engine
 * 
 * @tparam ECT entity component traits
 */
template<typename ECT>
class ec_engine{
    using ec_traits_type = ECT;

    using component_signature_storage_t = typename ec_traits_type::component_signature_storage_t;

    using entity_t = entity<ec_traits_type>;
    using component_storage_t = component_storage<ec_traits_type>;
    using signature_storage_t = signature_storage<ec_traits_type>;

public:

    std::uint32_t create_entity() noexcept{
        std::uint32_t e_id_{0};

        entities_.push_back(std::move(entity_t{entity_count_}));

        e_id_ = entity_count_;

        ++entity_count_;

        return e_id_;
    }

    const entity_t& get_entity(std::size_t index) noexcept{
        return entities_[index];
    }

    std::uint32_t entity_count() const noexcept{
        return entity_count_;
    }

private:


private:

    std::vector<entity_t> entities_{};
    component_storage_t components_{};
    signature_storage_t signatures_{};

    std::uint32_t entity_count_{0};
};


} // namespace yaecs