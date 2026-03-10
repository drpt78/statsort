// test/statsort_test.cpp
//
// Copyright (c) Peter Taraba 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  https://www.boost.org/LICENSE_1_0.txt)
//
// Boost.Test unit tests for boost::algorithm::statsort

#define BOOST_TEST_MODULE statsort
// Use header-only Boost.Test if available, otherwise fall back to a
// lightweight hand-rolled harness so the tests compile without a full
// Boost installation.
#if __has_include(<boost/test/included/unit_test.hpp>)
#  include <boost/test/included/unit_test.hpp>
#  define USING_BOOST_TEST 1
#else
#  include <cassert>
#  include <iostream>
#  define BOOST_AUTO_TEST_CASE(name) void name()
#  define BOOST_CHECK(expr)         assert(expr)
#  define BOOST_CHECK_EQUAL(a,b)    assert((a)==(b))
#  define BOOST_CHECK_EQUAL_COLLECTIONS(b1,e1,b2,e2) \
     assert(std::equal(b1,e1,b2))
#endif

#include <algorithm>
#include <array>
#include <numeric>
#include <random>
#include <vector>

#include <boost/algorithm/statsort.hpp>

// ── helpers ───────────────────────────────────────────────────────────────────

template <typename T>
static bool sorted(const std::vector<T>& v) {
    return std::is_sorted(v.begin(), v.end());
}

// ── correctness tests ─────────────────────────────────────────────────────────

BOOST_AUTO_TEST_CASE(test_empty) {
    std::vector<double> v;
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_single) {
    std::vector<double> v = {42.0};
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_two_elements) {
    std::vector<double> v = {9.0, 1.0};
    boost::algorithm::statsort(v);
    BOOST_CHECK_EQUAL(v[0], 1.0);
    BOOST_CHECK_EQUAL(v[1], 9.0);
}

BOOST_AUTO_TEST_CASE(test_all_equal) {
    std::vector<double> v(200, 3.14);
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_already_sorted) {
    std::vector<int> v(1000);
    std::iota(v.begin(), v.end(), 0);
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_reverse_sorted) {
    std::vector<int> v(1000);
    std::iota(v.rbegin(), v.rend(), 0);
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_negative_values) {
    std::vector<double> v = {-5.0, -1.0, -3.0, 0.0, 2.0, -4.0};
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_mixed_sign) {
    std::vector<int> v = {-100, 50, -30, 0, 80, -10, 20};
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_uniform_double) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1e6);
    std::vector<double> v(10000);
    std::generate(v.begin(), v.end(), [&]{ return dist(rng); });
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_gaussian_double) {
    std::mt19937 rng(123);
    std::normal_distribution<double> dist(500.0, 100.0);
    std::vector<double> v(10000);
    std::generate(v.begin(), v.end(), [&]{ return dist(rng); });
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_integer_type) {
    std::mt19937 rng(7);
    std::uniform_int_distribution<int> dist(-50000, 50000);
    std::vector<int> v(10000);
    std::generate(v.begin(), v.end(), [&]{ return dist(rng); });

    auto expected = v;
    std::sort(expected.begin(), expected.end());
    boost::algorithm::statsort(v);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(),
                                  expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_float_type) {
    std::mt19937 rng(55);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::vector<float> v(5000);
    std::generate(v.begin(), v.end(), [&]{ return dist(rng); });
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
}

BOOST_AUTO_TEST_CASE(test_matches_std_sort_large) {
    std::mt19937 rng(99);
    std::uniform_real_distribution<double> dist(0.0, 1e6);
    std::vector<double> v(100000);
    std::generate(v.begin(), v.end(), [&]{ return dist(rng); });

    auto expected = v;
    std::sort(expected.begin(), expected.end());
    boost::algorithm::statsort(v);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(),
                                  expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_iterator_interface) {
    std::mt19937 rng(11);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::vector<double> v(5000);
    std::generate(v.begin(), v.end(), [&]{ return dist(rng); });

    auto expected = v;
    std::sort(expected.begin(), expected.end());
    boost::algorithm::statsort(v.begin(), v.end()); // iterator overload
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(),
                                  expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_large_n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1e6);
    std::vector<double> v(500000);
    std::generate(v.begin(), v.end(), [&]{ return dist(rng); });

    auto expected = v;
    std::sort(expected.begin(), expected.end());
    boost::algorithm::statsort(v);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(),
                                  expected.begin(), expected.end());
}

// ── hand-rolled main (only used when Boost.Test is not available) ─────────────

#ifndef USING_BOOST_TEST
int main() {
    std::cout << "Running boost::algorithm::statsort tests...\n\n";

    #define RUN(fn) do { fn(); std::cout << "  [PASS] " #fn "\n"; } while(0)

    RUN(test_empty);
    RUN(test_single);
    RUN(test_two_elements);
    RUN(test_all_equal);
    RUN(test_already_sorted);
    RUN(test_reverse_sorted);
    RUN(test_negative_values);
    RUN(test_mixed_sign);
    RUN(test_uniform_double);
    RUN(test_gaussian_double);
    RUN(test_integer_type);
    RUN(test_float_type);
    RUN(test_matches_std_sort_large);
    RUN(test_iterator_interface);
    RUN(test_large_n);

    std::cout << "\nAll tests passed!\n";
    return 0;
}
#endif
