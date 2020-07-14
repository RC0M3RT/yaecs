#pragma once

#include "ec_traits.hpp"
#include "entity.hpp"
#include "component_storage.hpp"

#include "../mpl/mpl.hpp"

#include <cassert>

namespace yaecs{

/**
 * @brief Entity component engine
 * 
 * @tparam ECT entity component traits
 */
template<typename ECT>
class ec_engine{
    using ec_traits_type = ECT;

    using component_signature_storage_t = typename ec_traits_type::component_signature_storage_t;

    using entity_t              = entity<ec_traits_type>;
    using component_storage_t   = component_storage<ec_traits_type>;

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

    template<typename Tag>
    inline void add_tag(std::uint32_t entity_id) noexcept{
        if(entity_id < entity_count_) [[likely]] {
            entity_t& e = entities_[entity_id];
            e.set_tag<Tag>();
        }
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

    /**
     * @brief Recurse entities
     * 
     * @tparam Tag 
     * @tparam Callable 
     * @tparam Components The interested components in entity if tag matches
     * @param c 
     */
    template<typename Tag, class Callable, typename... Components>
    void each_matching_tag(Callable c){
        static_assert(ECT::template is_tag<Tag>(), "tparam Tag is not a tag");
        constexpr auto tag_index = static_cast<int>(ECT::template tag_index<Tag>());
        auto sig_ = build_signature<Components...>();

        for (const entity_t& entity_ : entities_){
            if(entity_.tag() == tag_index && entity_.check(sig_)){
                c( components_.get_component<Components>(entity_.get_data_index<Components>())... );
            }
        }
    }

    /**
     * @brief 
     * 
     * @tparam Ts The components
     * @tparam callable 
     * @param c 
     */
    template<typename... Ts, class callable>
    void for_matching_entities(callable c){
        auto sig_ = build_signature<Ts...>();
        for (entity_t& entity_ : entities_){
            if(entity_.check(sig_)){
                c( components_.get_component<Ts>(entity_.get_data_index<Ts>())... );
            }
        }
    } 

    /**
     * @brief Builds a signature bitset
     * 
     * @tparam Ts The components
     * @return component_signature_storage_t 
     */
    template<typename... Ts>
    static component_signature_storage_t build_signature(){
        component_signature_storage_t sig_{false};
        (set_signature<Ts>(sig_) , ... );

        return sig_;
    }

private:

    template<typename T>
    static void set_signature(component_signature_storage_t& sig) {
        sig.set(ECT::template component_index<T>());
    }


private:
    std::vector<entity_t> entities_{};
    component_storage_t components_{};

    std::uint32_t entity_count_{0};
};


} // namespace yaecs