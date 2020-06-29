#include <catch2/catch_all.hpp>

#include <yaecs/yaecs.hpp>
#include <yaecs/mpl/mpl.hpp>


#include <type_traits>
#include <vector>
#include <string>
#include <iostream>

TEST_CASE("Entity tests", "[entity]")
{
  // Components
  struct C1{}; // 0
  struct C2{}; // 1
  struct C3{}; // 2

  // Signatures
  struct S1{}; // 0
  struct S2{}; // 1

  struct nothing_at_all{};

  using components = yaecs::component_list<C1, C2, C3>;
  using signatures = yaecs::signature_list<S1, S2>;

  using ec_traits_t = yaecs::ec_traits<components, signatures>;

  using entity_t = yaecs::entity<ec_traits_t>;

  entity_t e1{10};

  e1.set_signature<C1>(true);
  e1.set_signature<C2>(false);
  e1.set_signature<C3>(true);

  REQUIRE(e1.get_signature<C1>() == true);
  REQUIRE(e1.get_signature<C2>() == false);
  REQUIRE(e1.get_signature<C3>() == true);

}

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

  sb_.set();
}

TEST_CASE("Component storage tests", "[component_storage]")
{
  // Components
  struct C1{
    int i1;
    std::string s1;
  };
  struct C2{
    float f2;
    std::string s2;
  };
  struct C3{
    std::string s3_1;
    std::string s3_2;
  };

  // Signatures
  struct S1{};
  struct S2{};

  struct nothing_at_all{};

  using components = yaecs::component_list<C1, C2, C3>;
  using signatures = yaecs::signature_list<S1, S2>;

  using ec_traits_t = yaecs::ec_traits<components, signatures>;

  using component_storage_t = yaecs::component_storage<ec_traits_t>;

  component_storage_t cs_{};

  C1 c1_{211, "asdasd"};

  cs_.add_component<C1>(c1_);
  auto c1p_ = cs_.get_component<C1>(0);

  REQUIRE(c1p_.i1 == 211);
  REQUIRE(c1p_.s1 == "asdasd");

  C2 c2_{3.10f, "qqwewe"};

  cs_.add_component<C2>(c2_);
  auto c2p_ = cs_.get_component<C2>(0);

  REQUIRE(c2p_.f2 == 3.10f);
  REQUIRE(c2p_.s2 == "qqwewe");

  C3 c3_1{"111", "222"};
  C3 c3_2{"3333", "4444"};
  C3 c3_3{"55555", "66666"};

  auto c3_1_data_index = cs_.add_component<C3>(c3_1);
  auto c3_2_data_index = cs_.add_component<C3>(c3_2);
  auto c3_3_data_index = cs_.add_component<C3>(c3_3);

  auto c3_1_data_ = cs_.get_component<C3>(c3_1_data_index);
  auto c3_2_data_ = cs_.get_component<C3>(c3_2_data_index);
  auto c3_3_data_ = cs_.get_component<C3>(c3_3_data_index);

  REQUIRE(c3_1_data_.s3_1 == "111");
  REQUIRE(c3_1_data_.s3_2 == "222");

  REQUIRE(c3_2_data_.s3_1 == "3333");
  REQUIRE(c3_2_data_.s3_2 == "4444");

  REQUIRE(c3_3_data_.s3_1 == "55555");
  REQUIRE(c3_3_data_.s3_2 == "66666");

}

TEST_CASE("mpl tests", "[mpl]")
{
  struct T1{};
  struct T2{};
  struct T3{};

  struct nothing_at_all{};

  using tl = yaecs::mpl::type_list<T1, T2, T3>;

  REQUIRE(yaecs::mpl::size_v<tl> == 3);

  REQUIRE(yaecs::mpl::index_v<T1, tl> == 0);
  REQUIRE(yaecs::mpl::index_v<T2, tl> == 1);
  REQUIRE(yaecs::mpl::index_v<T3, tl> == 2);

  REQUIRE(yaecs::mpl::contains_v<T1, tl> == true);
  REQUIRE(yaecs::mpl::contains_v<T2, tl> == true);
  REQUIRE(yaecs::mpl::contains_v<T3, tl> == true);

  REQUIRE(yaecs::mpl::contains_v<nothing_at_all, tl> == false);

}


TEST_CASE("ec_engine entity tests", "[ec_engine.entity]")
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

  using ec_engine_type_t = yaecs::ec_engine<ec_traits_t>;

  ec_engine_type_t engine_{};

  auto e1_id = engine_.create_entity();

  REQUIRE(e1_id == engine_.get_entity(0).id());

  REQUIRE(engine_.entity_count() == 1);

  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  
  REQUIRE(engine_.entity_count() == 9);
}

TEST_CASE("ec_engine system tests", "[ec_engine.system]")
{
  struct position{
    float x;
    float y;
    float z;
  };

  struct color{
    float r{0.0f};
    float g;
    float b;
    float a;
  };

  struct shader{
    std::uint32_t program_id;
  };

  // Signatures
  struct S1{};
  struct S2{};

  struct nothing_at_all{};

  using components = yaecs::component_list<position, color, shader>;
  using signatures = yaecs::signature_list<S1, S2>;

  using ec_traits_t = yaecs::ec_traits<components, signatures>;
  using component_signature_storage_t = typename ec_traits_t::component_signature_storage_t;

  using ec_engine_type_t = yaecs::ec_engine<ec_traits_t>;

  ec_engine_type_t engine_{};

  //-----------------------------------------
  auto something_1 = engine_.create_entity();
  auto something_2 = engine_.create_entity();
  auto something_3 = engine_.create_entity();
  position pos_1{-1.0f, 0.5f, 0.0f};

  engine_.add_component<position>(something_1, pos_1);
  engine_.add_component<color>(something_1);
  engine_.add_component<shader>(something_1, shader{});

  engine_.add_component<position>(something_2, pos_1);
  engine_.add_component<shader>(something_2, shader{1});

  engine_.add_component<position>(something_3, pos_1);

  //--------------------------------------------------

  // In component list
  // position index: 0 
  // color index: 1
  // shader index: 2 

  std::bitset<ec_traits_t::component_count()> some_system_signature_ = engine_.build_signature<position, color>();

  REQUIRE(some_system_signature_[0] == true);

  std::bitset<ec_traits_t::component_count()> some_system_signature2_ = engine_.build_signature<color>();

  REQUIRE(some_system_signature2_[0] == false);
  REQUIRE(some_system_signature2_[1] == true);

  //---------------------------------------------

  engine_.for_matching_entities<position, color>([&](auto& pos, auto& col){
    pos.x = 1.0f;
    col.r = 6.0f;
  });

  engine_.for_matching_entities<position, color>([&](auto& pos, auto& col){
    REQUIRE(pos.x == 1.0f);
    REQUIRE(col.r == 6.0f);
  });

  engine_.for_matching_entities<position, shader>([&](auto& pos, auto& s){
    pos.x = 1.0f;
    s.program_id = 12121;
  });

  engine_.for_matching_entities<position, shader>([&](auto& pos, auto& s){
    REQUIRE(pos.x == 1.0f);
    REQUIRE(s.program_id == 12121);
  });

}