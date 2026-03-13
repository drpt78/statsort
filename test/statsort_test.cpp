// statsort_test.cpp
//
// Copyright (c) Peter Taraba 2025
// Distributed under the Boost Software License, Version 1.0.
//
// Unit tests for boost::algorithm::statsort (plain + projection overloads)

#define BOOST_TEST_MODULE statsort
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
#include <string>
#include <vector>

#include <boost/algorithm/statsort.hpp>  // NOLINT: path resolved via -I

// ── helpers ───────────────────────────────────────────────────────────────────

template <typename T>
static bool sorted(const std::vector<T>& v) {
    return std::is_sorted(v.begin(), v.end());
}

// ── plain arithmetic tests ────────────────────────────────────────────────────

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
    boost::algorithm::statsort(v.begin(), v.end());
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

// ── projection overload tests ─────────────────────────────────────────────────

// Rainer's exact use case from the mailing list
BOOST_AUTO_TEST_CASE(test_proj_struct_by_int_field) {
    struct my_complex_type {
        std::string name;
        int z;
    };

    std::vector<my_complex_type> v = {
        {"charlie", 30}, {"alice", 10}, {"bob", 20}, {"dave", 5}
    };

    boost::algorithm::statsort(v, [](const my_complex_type& x) { return x.z; });

    BOOST_CHECK(std::is_sorted(v.begin(), v.end(),
        [](const my_complex_type& a, const my_complex_type& b) {
            return a.z < b.z;
        }));
    BOOST_CHECK_EQUAL(v[0].name, "dave");
    BOOST_CHECK_EQUAL(v[1].name, "alice");
    BOOST_CHECK_EQUAL(v[2].name, "bob");
    BOOST_CHECK_EQUAL(v[3].name, "charlie");
}

BOOST_AUTO_TEST_CASE(test_proj_struct_by_double_field) {
    struct Particle { std::string name; double energy; };

    std::vector<Particle> ps = {
        {"gamma", 9.9}, {"alpha", 1.1}, {"beta", 5.5}
    };

    boost::algorithm::statsort(ps, [](const Particle& p) { return p.energy; });

    BOOST_CHECK_EQUAL(ps[0].name, "alpha");
    BOOST_CHECK_EQUAL(ps[1].name, "beta");
    BOOST_CHECK_EQUAL(ps[2].name, "gamma");
}

BOOST_AUTO_TEST_CASE(test_proj_member_pointer) {
    struct Point { int x; int y; };
    std::vector<Point> pts = { {3, 0}, {1, 0}, {4, 0}, {2, 0} };

    // Member pointer syntax via lambda (member pointers need explicit invoke)
    boost::algorithm::statsort(pts, [](const Point& p) { return p.x; });

    for (std::size_t i = 0; i + 1 < pts.size(); ++i)
        BOOST_CHECK(pts[i].x <= pts[i + 1].x);
}

BOOST_AUTO_TEST_CASE(test_proj_iterator_interface) {
    struct Item { int id; float score; };

    std::vector<Item> items = { {1, 9.5f}, {2, 3.2f}, {3, 7.1f}, {4, 0.5f} };

    boost::algorithm::statsort(items.begin(), items.end(),
                               [](const Item& i) { return i.score; });

    BOOST_CHECK_EQUAL(items[0].id, 4);
    BOOST_CHECK_EQUAL(items[1].id, 2);
    BOOST_CHECK_EQUAL(items[2].id, 3);
    BOOST_CHECK_EQUAL(items[3].id, 1);
}

BOOST_AUTO_TEST_CASE(test_proj_large_uniform) {
    struct Wrapper { double val; };

    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1e6);
    std::vector<Wrapper> v(10000);
    std::generate(v.begin(), v.end(), [&]{ return Wrapper{dist(rng)}; });

    auto expected = v;
    std::sort(expected.begin(), expected.end(),
              [](const Wrapper& a, const Wrapper& b) { return a.val < b.val; });

    boost::algorithm::statsort(v, [](const Wrapper& w) { return w.val; });

    for (std::size_t i = 0; i < v.size(); ++i)
        BOOST_CHECK_EQUAL(v[i].val, expected[i].val);
}

BOOST_AUTO_TEST_CASE(test_proj_all_keys_equal) {
    struct S { int key; std::string data; };
    std::vector<S> v = { {5, "a"}, {5, "b"}, {5, "c"} };
    boost::algorithm::statsort(v, [](const S& s) { return s.key; });
    BOOST_CHECK_EQUAL(v.size(), 3u);
}

BOOST_AUTO_TEST_CASE(test_proj_single_element) {
    struct S { double x; };
    std::vector<S> v = { {3.14} };
    boost::algorithm::statsort(v, [](const S& s) { return s.x; });
    BOOST_CHECK_EQUAL(v.size(), 1u);
}

BOOST_AUTO_TEST_CASE(test_proj_negative_keys) {
    struct S { int key; };
    std::vector<S> v = { {-5}, {-1}, {-3}, {0}, {-4}, {2} };
    boost::algorithm::statsort(v, [](const S& s) { return s.key; });
    BOOST_CHECK(std::is_sorted(v.begin(), v.end(),
        [](const S& a, const S& b) { return a.key < b.key; }));
}

// ── adversarial / fallback tests ─────────────────────────────────────────────
// These exercise the imbalance guard: inputs that would cause one bucket to
// swallow almost all elements, triggering the std::sort fallback path.

BOOST_AUTO_TEST_CASE(test_adversarial_spike) {
    // All values the same except one outlier — one bucket gets n-1 elements.
    std::vector<double> v(1000, 1.0);
    v.back() = 1e9;  // single outlier forces extreme skew
    boost::algorithm::statsort(v);
    BOOST_CHECK(sorted(v));
    BOOST_CHECK_EQUAL(v.front(), 1.0);
    BOOST_CHECK_EQUAL(v.back(),  1e9);
}

BOOST_AUTO_TEST_CASE(test_adversarial_two_clusters) {
    // Two tight clusters far apart — bucket 0 gets half, last bucket gets half.
    std::vector<double> v;
    v.reserve(2000);
    for (int i = 0; i < 1000; ++i) v.push_back(1.0 + i * 0.0001);
    for (int i = 0; i < 1000; ++i) v.push_back(1e9 + i * 0.0001);
    auto expected = v;
    std::sort(expected.begin(), expected.end());
    boost::algorithm::statsort(v);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(),
                                  expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_adversarial_geometric) {
    // Geometric sequence: most mass near min, extreme skew in bucket distribution.
    std::vector<double> v;
    double x = 1.0;
    for (int i = 0; i < 2000; ++i, x *= 1.01) v.push_back(x);
    // Shuffle to defeat early-exit paths
    std::mt19937 rng(77);
    std::shuffle(v.begin(), v.end(), rng);
    auto expected = v;
    std::sort(expected.begin(), expected.end());
    boost::algorithm::statsort(v);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(),
                                  expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_adversarial_proj_spike) {
    // Projection variant: same spike pattern but on complex objects.
    struct S { double key; int id; };
    std::vector<S> v;
    for (int i = 0; i < 999; ++i) v.push_back({1.0, i});
    v.push_back({1e9, 999});

    boost::algorithm::statsort(v, [](const S& s) { return s.key; });

    BOOST_CHECK(std::is_sorted(v.begin(), v.end(),
        [](const S& a, const S& b) { return a.key < b.key; }));
    BOOST_CHECK_EQUAL(v.back().id, 999);
}

// ── hand-rolled main ──────────────────────────────────────────────────────────

#ifndef USING_BOOST_TEST
int main() {
    std::cout << "Running boost::algorithm::statsort tests...\n\n";

    #define RUN(fn) do { fn(); std::cout << "  [PASS] " #fn "\n"; } while(0)

    // Plain arithmetic tests
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

    // Projection overload tests
    RUN(test_proj_struct_by_int_field);
    RUN(test_proj_struct_by_double_field);
    RUN(test_proj_member_pointer);
    RUN(test_proj_iterator_interface);
    RUN(test_proj_large_uniform);
    RUN(test_proj_all_keys_equal);
    RUN(test_proj_single_element);
    RUN(test_proj_negative_keys);

    // Adversarial / fallback tests
    RUN(test_adversarial_spike);
    RUN(test_adversarial_two_clusters);
    RUN(test_adversarial_geometric);
    RUN(test_adversarial_proj_spike);

    std::cout << "\nAll tests passed!\n";
    return 0;
}
#endif
