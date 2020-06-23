project(yaecs CXX)

add_library(yaecs STATIC 
  "yaecs/entity.hpp"
)

target_include_directories(yaecs PUBLIC "${CMAKE_SOURCE_DIR}/src")

set_target_properties(yaecs PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(yaecs LINK_PRIVATE  
  project_options
  project_warnings
)
