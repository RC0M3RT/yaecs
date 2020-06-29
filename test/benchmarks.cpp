#include <catch2/catch_all.hpp>

std::uint64_t Fibonacci(std::uint64_t number) {
    return number < 2 ? 1 : Fibonacci(number - 1) + Fibonacci(number - 2);
}


TEST_CASE("Fibonacci") {
    BENCHMARK("Fibonacci 20") {
        return Fibonacci(5);
    };
}