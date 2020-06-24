#include <catch2/catch.hpp>

#include <yaecs/yaecs.hpp>

#include <type_traits>
#include <vector>
#include <string>


TEST_CASE("Entity component traits tests", "[ec_trait]")
{
  // Components
  struct C1{};
  struct C2{};
  struct C3{};

  // Signatures
  struct S1{};
  struct S2{};

  struct nothing_at_all{};

  using components = yaecs::component_list<C1, C2, C3>;
  using signatures = yaecs::signature_list<S1, S2>;

  using ec_traits_t = yaecs::ec_traits<components, signatures>;

  ec_traits_t ec_traits_1{};

  REQUIRE(ec_traits_1.is_component<C1>() == true);
  REQUIRE(ec_traits_1.is_component<C2>() == true);
  REQUIRE(ec_traits_1.is_component<C3>() == true);

  REQUIRE(ec_traits_1.is_component<S1>() == false);
  REQUIRE(ec_traits_1.is_component<S2>() == false);

  REQUIRE(ec_traits_1.is_signature<C1>() == false);
  REQUIRE(ec_traits_1.is_signature<C2>() == false);
  REQUIRE(ec_traits_1.is_signature<C3>() == false);

  REQUIRE(ec_traits_1.is_signature<S1>() == true);
  REQUIRE(ec_traits_1.is_signature<S2>() == true);

  REQUIRE(ec_traits_1.component_count_ == 3);
  REQUIRE(ec_traits_1.signature_count_ == 2);

  REQUIRE(ec_traits_1.component_index<C1>() == 0);
  REQUIRE(ec_traits_1.component_index<C2>() == 1);
  REQUIRE(ec_traits_1.component_index<C3>() == 2);
  REQUIRE(ec_traits_1.component_index<nothing_at_all>() == -1);

  REQUIRE(ec_traits_1.signature_index<S1>() == 0);
  REQUIRE(ec_traits_1.signature_index<S2>() == 1);
  REQUIRE(ec_traits_1.signature_index<nothing_at_all>() == -1);

  using component_signature_storage_type = typename ec_traits_t::component_signature_storage_t;

  // we have 3 components
  REQUIRE(std::is_same_v<component_signature_storage_type, std::bitset<3>> == true);

}

TEST_CASE("Signature storage tests", "[signature_storage]")
{
  // Components
  struct C1{};
  struct C2{};
  struct C3{};

  // Signatures
  struct S1{};
  struct S2{};

  struct nothing_at_all{};

  using components = yaecs::component_list<C1, C2, C3>;
  using signatures = yaecs::signature_list<S1, S2>;

  using ec_traits_t = yaecs::ec_traits<components, signatures>;

  using signature_storage_t = yaecs::signature_storage<ec_traits_t>;

  signature_storage_t ss_{};

  auto sb_ = ss_.get_signature_bitset<S1>();

}