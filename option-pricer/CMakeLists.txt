cmake_minimum_required(VERSION 3.16)
project(option_pricer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(OPTION_PRICER_BUILD_TESTS "Build unit tests" ON)

add_library(option_pricer
    src/types.cpp
    src/math.cpp
    src/black_scholes.cpp
    src/binomial.cpp
    src/monte_carlo.cpp
)
add_library(option_pricer::option_pricer ALIAS option_pricer)

target_include_directories(option_pricer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(option_pricer PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(pricer examples/price.cpp)
target_link_libraries(pricer PRIVATE option_pricer)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(pricer PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if(OPTION_PRICER_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.4
    )
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

    enable_testing()
    include(CTest)
    include(Catch)

    add_executable(option_pricer_tests
        tests/test_black_scholes.cpp
        tests/test_binomial.cpp
        tests/test_monte_carlo.cpp
        tests/test_validation.cpp
    )
    target_link_libraries(option_pricer_tests PRIVATE option_pricer Catch2::Catch2WithMain)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(option_pricer_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    catch_discover_tests(option_pricer_tests)
endif()
