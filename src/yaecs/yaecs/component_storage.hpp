#pragma once

#include "../mpl/mpl.hpp"

#include <vector>

namespace yaecs{

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

        return static_cast<std::uint32_t>(vec_size_);
    }

    template<typename T>
    [[nodiscard]] T& get_component(std::uint32_t data_index) noexcept{
        static_assert(ECT::template is_component<T>(), "T is not a component");   

        return std::get<std::vector<T>>(components_)[data_index];
    }

private:
    storage_t<component_list> components_{}; 
};

} // namespace yaecs
