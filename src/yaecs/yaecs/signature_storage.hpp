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

} // namespace yaecs