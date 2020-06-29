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
    using component_list = typename ECT::component_list;
public:

    entity(std::uint32_t id)
        : id_{id}
        , signatures_{false}
        , data_index_per_component_{}
    {
    }

    std::uint32_t id() const noexcept{ return id_; }

    template<typename T>
    void set_signature(bool enabled){
        static_assert(ECT::template is_component<T>(), "T is not a component");

        constexpr auto component_index = ECT::template component_index<T>();
        signatures_[component_index] = enabled;
    }

    template<typename T>
    [[nodiscard]] bool get_signature() const noexcept{
        static_assert(ECT::template is_component<T>(), "T is not a component");

        constexpr auto component_index = ECT::template component_index<T>();

        if constexpr(component_index == -1){
            return false;
        }
        else{
            return signatures_[component_index];
        }
    }

    bool check(component_signature_storage other_signature){
        return (signatures_ & other_signature) == other_signature;
    }

    template<typename T>
    void set_data_index(std::uint32_t data_index) noexcept{
        static_assert(ECT::template is_component<T>(), "T is not a component");

        constexpr auto component_index = ECT::template component_index<T>();

        data_index_per_component_[component_index] = data_index;
    }

    template<typename T>
    [[nodiscard]] std::uint32_t get_data_index() const noexcept{
        static_assert(ECT::template is_component<T>(), "T is not a component");
        constexpr auto component_index = ECT::template component_index<T>();
        return data_index_per_component_[component_index];
    }

private:
    std::uint32_t id_;
    component_signature_storage signatures_{false};
    std::array<std::uint32_t, ECT::component_count()> data_index_per_component_{};
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
    [[nodiscard]] auto& get_signature_bitset(){
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
    using entity_t = entity<ECT>;

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
    std::uint32_t add_component(T c){
        static_assert(ECT::template is_component<T>(), "T is not a component");   

        std::vector<T>& component_vector_ = std::get<std::vector<T>>(components_);

        auto vec_size_ = component_vector_.size();
        component_vector_.push_back(c);

        return vec_size_;
    }

    template<typename T>
    [[nodiscard]] T& get_component(std::uint32_t data_index) noexcept{
        static_assert(ECT::template is_component<T>(), "T is not a component");   

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

    /**
     * @brief Creates an entity
     * 
     * @return std::uint32_t Entity id
     */
    std::uint32_t create_entity() noexcept{
        std::uint32_t e_id_{0};

        entities_.push_back(std::move(entity_t{entity_count_}));

        e_id_ = entity_count_;

        ++entity_count_;

        return e_id_;
    }

    const entity_t& get_entity(std::uint32_t index) const noexcept{
        return entities_[index];
    }

    /**
     * @brief Returns the entity count
     * 
     * @return std::uint32_t 
     */
    std::uint32_t entity_count() const noexcept{
        return entity_count_;
    }
    
    /**
     * @brief Adds the \param c component to the entity
     * 
     * @tparam T Component Type
     * @param entity_id 
     * @param c 
     */
    template<typename T> 
    void add_component(std::uint32_t entity_id, T c) {
        static_assert(ECT::template is_component<T>(), "T is not a component");
        assert(!(entity_id > entity_count_));

        entity_t& entity_ = entities_[entity_id];
        entity_.set_signature<T>(true);
        auto data_index = components_.add_component<T>(std::move(c));
        entity_.set_data_index<T>(data_index);
    }

    /**
     * @brief Adds default constructed \tparam T component to the entity
     * 
     * @tparam T Component Type
     * @param entity_id 
     */
    template<typename T>
    void add_component(std::uint32_t entity_id){
        static_assert(ECT::template is_component<T>(), "T is not a component");
        static_assert(std::is_default_constructible_v<T>, "T is not default constructible");
        assert(!(entity_id > entity_count_));

        entity_t& entity_ = entities_[entity_id];
        entity_.set_signature<T>(true);
        auto data_index = components_.add_component<T>(T{});
        entity_.set_data_index<T>(data_index);
    }

    /**
     * @brief 
     * 
     * @tparam T Component Type
     * @tparam callable 
     * @param c 
     */
    template<typename T, class callable>
    void each(callable c){

        for (const entity_t& entity_ : entities_){
            if(entity_.get_signature<T>()){
                auto data_index_ = entity_.get_data_index<T>();
                if constexpr(std::is_invocable_v<callable, T&>){
                    auto& data_ = components_.get_component<T>(data_index_);
                    c(data_);
                }
                else if constexpr(std::is_invocable_v<callable, const T&>){
                    const auto& data_ = components_.get_component<T>(data_index_);
                    c(data_);
                }
            }
        }
    }

    template<typename... Ts, class callable>
    void for_matching_entities(callable c){
        auto sig_ = build_signature<Ts...>();
        for (entity_t& entity_ : entities_){
            if(entity_.check(sig_)){
                c( components_.get_component<Ts>(entity_.get_data_index<Ts>())... );
            }
        }
    }

    template<typename... Ts>
    static component_signature_storage_t build_signature(){
        component_signature_storage_t sig_{0};
        sig_.set(ECT::template component_index<Ts>()...);

        return sig_;
    }

private:



private:
    std::vector<entity_t> entities_{};
    component_storage_t components_{};
    signature_storage_t signatures_{};

    std::uint32_t entity_count_{0};
};


} // namespace yaecs