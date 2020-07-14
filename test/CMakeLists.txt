add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE project_warnings project_options yaecs
                                    CONAN_PKG::gtest)

add_test(NAME tests COMMAND tests)


add_executable(benchmarks benchmarks.cpp)
target_link_libraries(benchmarks PRIVATE project_warnings project_options yaecs CONAN_PKG::benchmark)

add_test(NAME benchmarks COMMAND benchmarks)
