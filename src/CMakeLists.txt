project(yaecs CXX)

add_library(yaecs INTERFACE 
  # "${CMAKE_CURRENT_LIST_DIR}/metal/metal.hpp"
  # "${CMAKE_CURRENT_LIST_DIR}/yaecs/entity.hpp"
)

target_include_directories(yaecs INTERFACE "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(yaecs INTERFACE  
  project_options
  project_warnings
)
