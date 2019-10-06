#include <iostream>
#include <fmt/core.h>
#include <chrono>
#include <string>

int main()
{
    auto tp = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(tp);
    fmt::print("Hello, World! {}", now_c);
}
