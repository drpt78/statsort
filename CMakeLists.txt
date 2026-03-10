cmake_minimum_required(VERSION 3.15)
project(statsort VERSION 0.1.0 LANGUAGES CXX)

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
# Example executable
# -----------------------------------------------------------------------------
option(STATSORT_BUILD_EXAMPLES "Build example programs" ON)

if(STATSORT_BUILD_EXAMPLES)
    add_executable(statsort_example statsort_example.cpp)
    target_link_libraries(statsort_example PRIVATE statsort)
    target_include_directories(statsort_example PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------
option(STATSORT_BUILD_TESTS "Build test suite" ON)

if(STATSORT_BUILD_TESTS)
    enable_testing()

    # Try to find Boost.Test; fall back to the hand-rolled harness if absent
    find_package(Boost QUIET COMPONENTS unit_test_framework)

    add_executable(statsort_test statsort_test.cpp)
    target_include_directories(statsort_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    if(Boost_FOUND)
        target_link_libraries(statsort_test PRIVATE statsort Boost::unit_test_framework)
        target_compile_definitions(statsort_test PRIVATE BOOST_TEST_DYN_LINK)
        message(STATUS "statsort: using Boost.Test")
    else()
        target_link_libraries(statsort_test PRIVATE statsort)
        message(STATUS "statsort: Boost.Test not found, using built-in harness")
    endif()

    add_test(NAME statsort_test COMMAND statsort_test)
endif()

# -----------------------------------------------------------------------------
# Install rules (header-only)
# -----------------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(FILES statsort.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/boost/algorithm
)

install(TARGETS statsort
    EXPORT statsortTargets
)

install(EXPORT statsortTargets
    FILE statsortConfig.cmake
    NAMESPACE boost::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/statsort
)
