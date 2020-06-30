#pragma once

#include <array>
#include <bitset>

#include "../mpl/mpl.hpp"

namespace yaecs{


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

} // namespace yaecs