project(yaecs CXX)

add_library(yaecs INTERFACE)

target_include_directories(yaecs INTERFACE "${CMAKE_SOURCE_DIR}/src")

target_compile_definitions(yaecs INTERFACE CATCH_CONFIG_ENABLE_BENCHMARKING)

target_link_libraries(yaecs INTERFACE  
  project_options
  project_warnings
)
