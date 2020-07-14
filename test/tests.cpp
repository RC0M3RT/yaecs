#include "gtest/gtest.h"

#include <yaecs/yaecs.hpp>
#include <yaecs/mpl/mpl.hpp>

#include <type_traits>
#include <vector>
#include <string>
#include <iostream>

TEST(Entitytests, entity)
{
  // Components
  struct C1{}; // 0
  struct C2{}; // 1
  struct C3{}; // 2

  // Tags
  struct tag1{}; // 0
  struct tag2{}; // 1
  struct tag3{}; // 2
  struct tag4{}; // 3
  struct tag5{}; // 4

  struct nothing_at_all{};

  using components = yaecs::component_list<C1, C2, C3>;
  using tags = yaecs::tag_list<tag1, tag2, tag3, tag4, tag5>;

  using ec_traits_t = yaecs::ec_traits<components, tags>;

  using entity_t = yaecs::entity<ec_traits_t>;

  entity_t e1{10};

  e1.set_signature<C1>(true);
  e1.set_signature<C2>(false);
  e1.set_signature<C3>(true);

  EXPECT_EQ(e1.get_signature<C1>(), true);
  EXPECT_EQ(e1.get_signature<C2>(), false);
  EXPECT_EQ(e1.get_signature<C3>(), true);

  e1.set_tag<tag4>();

  EXPECT_EQ(e1.tag(), 3);

}

TEST(Entity_component_traits_tests, ec_trait)
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
  using tags = yaecs::tag_list<S1, S2>;

  using ec_traits_t = yaecs::ec_traits<components, tags>;

  ec_traits_t ec_traits_1{};

  EXPECT_EQ(ec_traits_1.is_component<C1>(), true);
  EXPECT_EQ(ec_traits_1.is_component<C2>(), true);
  EXPECT_EQ(ec_traits_1.is_component<C3>(), true);

  EXPECT_EQ(ec_traits_1.is_component<S1>(), false);
  EXPECT_EQ(ec_traits_1.is_component<S2>(), false);

  EXPECT_EQ(ec_traits_1.is_tag<C1>(), false);
  EXPECT_EQ(ec_traits_1.is_tag<C2>(), false);
  EXPECT_EQ(ec_traits_1.is_tag<C3>(), false);

  EXPECT_EQ(ec_traits_1.is_tag<S1>(), true);
  EXPECT_EQ(ec_traits_1.is_tag<S2>(), true);

  EXPECT_EQ(ec_traits_1.component_count_, 3);
  EXPECT_EQ(ec_traits_1.tag_count_, 2);

  EXPECT_EQ(ec_traits_1.component_index<C1>(), 0);
  EXPECT_EQ(ec_traits_1.component_index<C2>(), 1);
  EXPECT_EQ(ec_traits_1.component_index<C3>(), 2);
  EXPECT_EQ(ec_traits_1.component_index<nothing_at_all>(), -1);

  EXPECT_EQ(ec_traits_1.tag_index<S1>(), 0);
  EXPECT_EQ(ec_traits_1.tag_index<S2>(), 1);
  EXPECT_EQ(ec_traits_1.tag_index<nothing_at_all>(), -1);

  using component_signature_storage_type = typename ec_traits_t::component_signature_storage_t;

  // we have 3 components
  auto csst = std::is_same_v<component_signature_storage_type, std::bitset<3>>;
  EXPECT_EQ(csst, true);

}

TEST(Component_storage_tests, component_storage)
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
  using tags = yaecs::tag_list<S1, S2>;

  using ec_traits_t = yaecs::ec_traits<components, tags>;

  using component_storage_t = yaecs::component_storage<ec_traits_t>;

  component_storage_t cs_{};

  C1 c1_{211, "asdasd"};

  cs_.add_component<C1>(c1_);
  auto c1p_ = cs_.get_component<C1>(0);

  EXPECT_EQ(c1p_.i1, 211);
  EXPECT_EQ(c1p_.s1, "asdasd");

  C2 c2_{3.10f, "qqwewe"};

  cs_.add_component<C2>(c2_);
  auto c2p_ = cs_.get_component<C2>(0);

  EXPECT_EQ(c2p_.f2, 3.10f);
  EXPECT_EQ(c2p_.s2, "qqwewe");

  C3 c3_1{"111", "222"};
  C3 c3_2{"3333", "4444"};
  C3 c3_3{"55555", "66666"};

  auto c3_1_data_index = cs_.add_component<C3>(c3_1);
  auto c3_2_data_index = cs_.add_component<C3>(c3_2);
  auto c3_3_data_index = cs_.add_component<C3>(c3_3);

  auto c3_1_data_ = cs_.get_component<C3>(c3_1_data_index);
  auto c3_2_data_ = cs_.get_component<C3>(c3_2_data_index);
  auto c3_3_data_ = cs_.get_component<C3>(c3_3_data_index);

  EXPECT_EQ(c3_1_data_.s3_1, "111");
  EXPECT_EQ(c3_1_data_.s3_2, "222");

  EXPECT_EQ(c3_2_data_.s3_1, "3333");
  EXPECT_EQ(c3_2_data_.s3_2, "4444");

  EXPECT_EQ(c3_3_data_.s3_1, "55555");
  EXPECT_EQ(c3_3_data_.s3_2, "66666");

}

TEST(mpl_tests, mpl_tt)
{
  struct T1{};
  struct T2{};
  struct T3{};

  struct nothing_at_all{};

  using tl = yaecs::mpl::type_list<T1, T2, T3>;

  auto s1 = yaecs::mpl::size_v<tl>;
  EXPECT_EQ(s1, static_cast<std::size_t>(3));

  auto i1 = yaecs::mpl::index_v<T1, tl>;
  EXPECT_EQ(i1, static_cast<std::size_t>(0));
  auto i2 = yaecs::mpl::index_v<T2, tl>;
  EXPECT_EQ(i2, static_cast<std::size_t>(1));
  auto i3 = yaecs::mpl::index_v<T3, tl>;
  EXPECT_EQ(i3, static_cast<std::size_t>(2));

  auto c1 = yaecs::mpl::contains_v<T1, tl>;
  EXPECT_EQ(c1, true);
  auto c2 = yaecs::mpl::contains_v<T2, tl>;
  EXPECT_EQ(c2, true);
  auto c3 = yaecs::mpl::contains_v<T3, tl>;
  EXPECT_EQ(c3, true);

  auto c4 = yaecs::mpl::contains_v<nothing_at_all, tl>;
  EXPECT_EQ(c4, false);

}


TEST(ec_engine_entity_tests, ec_engine_entity)
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
  using tags = yaecs::tag_list<S1, S2>;

  using ec_traits_t = yaecs::ec_traits<components, tags>;

  using ec_engine_type_t = yaecs::ec_engine<ec_traits_t>;

  ec_engine_type_t engine_{};

  auto e1_id = engine_.create_entity();

  EXPECT_EQ(e1_id, engine_.get_entity(0).id());

  EXPECT_EQ(engine_.entity_count(), static_cast<std::uint32_t>(1));

  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  engine_.create_entity();
  
  EXPECT_EQ(engine_.entity_count(), static_cast<std::uint32_t>(9));
}

TEST(ec_engine_system_tests, ec_engine_system)
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

  // Tags
  struct particle_tag{};
  struct follow_camera_tag{};

  struct nothing_at_all{};

  using components = yaecs::component_list<position, color, shader>;
  using tags = yaecs::tag_list<particle_tag, follow_camera_tag>;

  using ec_traits_t = yaecs::ec_traits<components, tags>;
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

  engine_.add_tag<particle_tag>(something_2);

  //--------------------------------------------------

  // In component list
  // position index: 0 
  // color index: 1
  // shader index: 2 

  std::bitset<ec_traits_t::component_count()> some_system_signature_ = engine_.build_signature<position, color>();

  EXPECT_EQ(some_system_signature_[0], true);

  std::bitset<ec_traits_t::component_count()> some_system_signature2_ = engine_.build_signature<color>();

  EXPECT_EQ(some_system_signature2_[0], false);
  EXPECT_EQ(some_system_signature2_[1], true);

  //---------------------------------------------

  engine_.for_matching_entities<position, color>([&](auto& pos, auto& col){
    pos.x = 1.0f;
    col.r = 6.0f;
  });

  engine_.for_matching_entities<position, color>([&](auto& pos, auto& col){
    EXPECT_EQ(pos.x, 1.0f);
    EXPECT_EQ(col.r, 6.0f);
  });

  engine_.for_matching_entities<position, shader>([&](auto& pos, auto& s){
    pos.x = 1.0f;
    s.program_id = 12121;
  });

  engine_.for_matching_entities<position, shader>([&](auto& pos, auto& s){
    EXPECT_EQ(pos.x, 1.0f);
    EXPECT_EQ(s.program_id, static_cast<std::uint32_t>(12121));
  });

  auto particle_handle = [&](position& pos, shader& s){
    pos.y = 12.0f;
    s.program_id = 32;
  };

  engine_.each_matching_tag<particle_tag, decltype(particle_handle), position, shader>(particle_handle);

  auto particle_handle2 = [&](position& pos, shader& s){
    EXPECT_EQ(pos.y, 12.0f);
    EXPECT_EQ(s.program_id, static_cast<std::uint32_t>(32));
  };

  engine_.each_matching_tag<particle_tag, decltype(particle_handle2), position, shader>(particle_handle2);

}