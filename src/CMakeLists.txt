project(yaecs CXX)

add_library(yaecs INTERFACE)

target_include_directories(yaecs INTERFACE "${CMAKE_SOURCE_DIR}/src")

# target_compile_definitions(yaecs INTERFACE 
#   _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
# )

target_link_libraries(yaecs INTERFACE  
  project_options
  project_warnings
  CONAN_PKG::boost
)
