cmake_minimum_required(VERSION 3.20)
project(OptiForge VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(optiforge
    src/ir.cpp
    src/optimizer.cpp
    src/analysis.cpp
    src/riscv_emitter.cpp
)
target_include_directories(optiforge PUBLIC include)
target_compile_options(optiforge PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

add_executable(optiforge-cli src/main.cpp)
target_link_libraries(optiforge-cli PRIVATE optiforge)

add_executable(optiforge-benchmark src/benchmark.cpp)
target_link_libraries(optiforge-benchmark PRIVATE optiforge)

enable_testing()
add_executable(optiforge-tests tests/optimizer_tests.cpp)
target_link_libraries(optiforge-tests PRIVATE optiforge)
add_test(NAME optimizer_tests COMMAND optiforge-tests)
