cmake_minimum_required(VERSION 3.15)
project(statsort VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------------
# Header-only library target
# -----------------------------------------------------------------------------
add_library(statsort INTERFACE)
add_library(boost::statsort ALIAS statsort)

target_include_directories(statsort INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(statsort INTERFACE cxx_std_17)

# -----------------------------------------------------------------------------
# Example
# -----------------------------------------------------------------------------
option(STATSORT_BUILD_EXAMPLES "Build example programs" ON)

if(STATSORT_BUILD_EXAMPLES)
    add_executable(statsort_example example/statsort_example.cpp)
    target_link_libraries(statsort_example PRIVATE statsort)
endif()

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------
option(STATSORT_BUILD_TESTS "Build test suite" ON)

if(STATSORT_BUILD_TESTS)
    enable_testing()
    find_package(Boost QUIET COMPONENTS unit_test_framework)

    add_executable(statsort_test test/statsort_test.cpp)
    target_link_libraries(statsort_test PRIVATE statsort)

    if(Boost_FOUND)
        target_link_libraries(statsort_test PRIVATE Boost::unit_test_framework)
        target_compile_definitions(statsort_test PRIVATE BOOST_TEST_DYN_LINK)
        message(STATUS "statsort: using Boost.Test")
    else()
        message(STATUS "statsort: Boost.Test not found — using built-in harness")
    endif()

    add_test(NAME statsort_test COMMAND statsort_test)
endif()

# -----------------------------------------------------------------------------
# Benchmarks
# -----------------------------------------------------------------------------
option(STATSORT_BUILD_BENCHMARKS "Build benchmark programs" OFF)

if(STATSORT_BUILD_BENCHMARKS)
    add_executable(statsort_bench benchmark/statsort_bench.cpp)
    target_link_libraries(statsort_bench PRIVATE statsort)
    target_compile_options(statsort_bench PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-O3>
        $<$<CXX_COMPILER_ID:MSVC>:/O2>
    )
endif()

# -----------------------------------------------------------------------------
# Install rules (header-only)
# -----------------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS statsort
    EXPORT statsortTargets
)

install(EXPORT statsortTargets
    FILE statsortConfig.cmake
    NAMESPACE boost::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/statsort
)
