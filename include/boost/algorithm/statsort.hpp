// boost/algorithm/statsort.hpp
//
// Copyright (c) Peter Taraba 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  https://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------------------------
// Statistical Sort — O(n log log n) average complexity
// -----------------------------------------------------------------------------
//
// ALGORITHM REFERENCE
//   Peter Taraba, "Why would you sort when you know where things approximately
//   belong?", March 2025.
//   https://github.com/peta78/Sorting/tree/main/sorting_publication
//
// ALGORITHM SUMMARY
//   1. Find min and max of the input range — O(n).
//   2. Classify each element into one of sqrt(n) buckets by linear
//      interpolation of its value into [min, max] — O(n).
//   3. Recurse into each bucket using the same algorithm.
//   4. If the largest bucket holds more than IMBALANCE_RATIO of n,
//      fall back to std::sort for that bucket (adversarial-input guard).
//   5. Fall back to insertion sort when bucket size <= THRESHOLD (default 16).
//   6. Reassemble buckets in order.
//
// REQUIREMENTS
//   - C++17 or later
//   - ValueType must be arithmetic (integral or floating-point), OR
//     a projection function must be supplied mapping each element to an
//     arithmetic key (see projection overloads below)
//   - Range must be a contiguous container exposing .data() and .size()
//
// SYNOPSIS
//   // Sort a vector of arithmetic values
//   std::vector<double> v = { ... };
//   boost::algorithm::statsort(v);
//
//   // Iterator interface
//   boost::algorithm::statsort(v.begin(), v.end());
//
//   // Projection overload — sort complex objects by a numeric key
//   struct Particle { std::string name; double energy; };
//   std::vector<Particle> ps = { ... };
//   boost::algorithm::statsort(ps, [](const Particle& p) { return p.energy; });
//
//   // Member pointer syntax also works
//   struct Point { int x, y; };
//   std::vector<Point> pts = { ... };
//   boost::algorithm::statsort(pts, &Point::x);
//
//   // Projection with iterator interface
//   boost::algorithm::statsort(ps.begin(), ps.end(),
//                              [](const Particle& p) { return p.energy; });
//
// -----------------------------------------------------------------------------

#ifndef BOOST_ALGORITHM_STATSORT_HPP
#define BOOST_ALGORITHM_STATSORT_HPP

#include <boost/sort/spreadsort/spreadsort.hpp>

#include <algorithm>    // std::sort, std::min_element, std::max_element, std::copy
#include <cmath>        // std::sqrt
#include <cstddef>      // std::size_t
#include <functional>   // std::invoke
#include <iterator>     // std::iterator_traits, std::distance
#include <limits>       // std::numeric_limits
#include <type_traits>  // std::is_arithmetic_v, std::enable_if_t, std::invoke_result_t
#include <vector>       // std::vector

namespace boost {
namespace algorithm {

namespace detail {

// ── Tuneable constants ─────────────────────────────────────────────────────

/// Buckets smaller than this are sorted by insertion sort.
static constexpr std::size_t STATSORT_THRESHOLD = 1000000;

/// If any single bucket receives more than this fraction of n elements,
/// the bucket is considered pathologically skewed and falls back to
/// std::sort.  A value of 0.5 means: if any bucket holds more than half
/// the input, stop recursing and use std::sort on that bucket.
static constexpr double STATSORT_IMBALANCE_RATIO = 0.5;


// ── Insertion sorts ────────────────────────────────────────────────────────

/// Plain arithmetic insertion sort.
template <typename T>
inline void statsort_insertion(T* data, std::size_t n) noexcept
{
    for (std::size_t i = 1; i < n; ++i) {
        T key = data[i];
        std::size_t j = i;
        while (j > 0 && data[j - 1] > key) {
            data[j] = data[j - 1];
            --j;
        }
        data[j] = key;
    }
}

/// Insertion sort for complex objects ordered by proj(element).
template <typename T, typename Proj>
inline void statsort_insertion_proj(T* data, std::size_t n, Proj proj) noexcept
{
    for (std::size_t i = 1; i < n; ++i) {
        T key = data[i];
        auto key_val = proj(key);
        std::size_t j = i;
        while (j > 0 && proj(data[j - 1]) > key_val) {
            data[j] = data[j - 1];
            --j;
        }
        data[j] = key;
    }
}


// ── Recursive cores ────────────────────────────────────────────────────────

/// Recursive core for plain arithmetic types.
///
/// Imbalance guard: before recursing, this function checks whether any
/// single bucket has captured an unreasonably large share of n.  If so,
/// std::sort is invoked on that bucket directly.  This prevents O(n^2)
/// degradation on adversarial or highly skewed inputs while preserving
/// the O(n log log n) expected behaviour on smooth distributions.
template <typename T>
void statsort_impl(T* data, std::size_t n,
                   double min_val, double max_val,
                   T* scratch)
{
    if (n <= STATSORT_THRESHOLD) { boost::sort::spreadsort::spreadsort(data, data + n); return; }

    const std::size_t m     = static_cast<std::size_t>(std::sqrt(static_cast<double>(n)));
    const double      range = max_val - min_val;
    const double      scale = static_cast<double>(static_cast<std::size_t>(m)) / range;

    // Pass 1 — count elements per bucket
    std::vector<std::size_t> cnt(m, 0);
    for (std::size_t i = 0; i < n; ++i) {
        std::size_t b = static_cast<std::size_t>((static_cast<double>(data[i]) - min_val) * scale);
        if (b >= m) b = m - 1;
        ++cnt[b];
    }

    // Imbalance check — if the heaviest bucket holds > IMBALANCE_RATIO of n,
    // fall back to std::sort for the whole subrange.
    {
        const std::size_t max_allowed =
            static_cast<std::size_t>(STATSORT_IMBALANCE_RATIO * static_cast<double>(n)) + 1;
        for (std::size_t b = 0; b < m; ++b) {
            if (cnt[b] > max_allowed) {
                std::sort(data, data + n);
                return;
            }
        }
    }

    // Pass 2 — prefix sums → bucket start offsets
    std::vector<std::size_t> off(m + 1, 0);
    for (std::size_t i = 0; i < m; ++i)
        off[i + 1] = off[i] + cnt[i];

    // Pass 3 — scatter into scratch
    {
        std::vector<std::size_t> pos(off.begin(), off.begin() + m);
        for (std::size_t i = 0; i < n; ++i) {
            std::size_t b = static_cast<std::size_t>((static_cast<double>(data[i]) - min_val) * scale);
            if (b >= m) b = m - 1;
            scratch[pos[b]++] = data[i];
        }
    }

    // Pass 4 — recurse per bucket
    for (std::size_t b = 0; b < m; ++b) {
        const std::size_t bstart = off[b];
        const std::size_t bsize  = cnt[b];
        if (bsize == 0) continue;

        const double bmin = min_val + static_cast<double>(b)     * range / static_cast<double>(m);
        const double bmax = min_val + static_cast<double>(b + 1) * range / static_cast<double>(m);

        if (bsize <= STATSORT_THRESHOLD)
            boost::sort::spreadsort::spreadsort(scratch + bstart, scratch + bstart + bsize);
        else
            statsort_impl(scratch + bstart, bsize, bmin, bmax, data + bstart);
    }

    // Pass 5 — copy back
    std::copy(scratch, scratch + n, data);
}


/// Recursive core for projected (complex object) types.
///
/// Bucket placement uses the arithmetic key returned by proj(element).
/// Includes the same imbalance guard as the plain-type overload.
///
/// @param data     Objects to sort (input/output)
/// @param n        Number of elements
/// @param min_val  Lower bound of key range for this subproblem
/// @param max_val  Upper bound of key range (exclusive)
/// @param scratch  Workspace of size n
/// @param proj     Callable: (const T&) -> arithmetic key
template <typename T, typename Proj>
void statsort_impl_proj(T* data, std::size_t n,
                        double min_val, double max_val,
                        T* scratch,
                        Proj proj)
{
    if (n <= STATSORT_THRESHOLD) { boost::sort::spreadsort::spreadsort(data, data + n); return; }

    const std::size_t m     = static_cast<std::size_t>(std::sqrt(static_cast<double>(n)));
    const double      range = max_val - min_val;
    const double      scale = static_cast<double>(m) / range;

    // Pass 1 — count
    std::vector<std::size_t> cnt(m, 0);
    for (std::size_t i = 0; i < n; ++i) {
        std::size_t b = static_cast<std::size_t>((static_cast<double>(proj(data[i])) - min_val) * scale);
        if (b >= m) b = m - 1;
        ++cnt[b];
    }

    // Imbalance guard — same policy as the plain-type overload
    {
        const std::size_t max_allowed =
            static_cast<std::size_t>(STATSORT_IMBALANCE_RATIO * static_cast<double>(n)) + 1;
        for (std::size_t b = 0; b < m; ++b) {
            if (cnt[b] > max_allowed) {
                std::sort(data, data + n, [&](const T& a, const T& b_elem) {
                    return proj(a) < proj(b_elem);
                });
                return;
            }
        }
    }

    // Pass 2 — prefix sums
    std::vector<std::size_t> off(m + 1, 0);
    for (std::size_t i = 0; i < m; ++i)
        off[i + 1] = off[i] + cnt[i];

    // Pass 3 — scatter
    {
        std::vector<std::size_t> pos(off.begin(), off.begin() + m);
        for (std::size_t i = 0; i < n; ++i) {
            std::size_t b = static_cast<std::size_t>((static_cast<double>(proj(data[i])) - min_val) * scale);
            if (b >= m) b = m - 1;
            scratch[pos[b]++] = data[i];
        }
    }

    // Pass 4 — recurse per bucket
    for (std::size_t b = 0; b < m; ++b) {
        const std::size_t bstart = off[b];
        const std::size_t bsize  = cnt[b];
        if (bsize == 0) continue;

        const double bmin = min_val + static_cast<double>(b)     * range / static_cast<double>(m);
        const double bmax = min_val + static_cast<double>(b + 1) * range / static_cast<double>(m);

        if (bsize <= STATSORT_THRESHOLD)
            boost::sort::spreadsort::spreadsort(scratch + bstart, scratch + bstart + bsize);
        else
            statsort_impl_proj(scratch + bstart, bsize, bmin, bmax, data + bstart, proj);
    }

    // Pass 5 — copy back
    std::copy(scratch, scratch + n, data);
}

} // namespace detail


// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

/// boost::algorithm::statsort — container overload (arithmetic types)
///
/// Sorts a contiguous container of arithmetic values in ascending order.
/// Average O(n log log n) on smooth distributions; falls back to O(n log n)
/// std::sort on pathologically skewed inputs.
///
/// @tparam Container  Contiguous container whose value_type is arithmetic
///                    (e.g. std::vector<double>, std::array<int, N>)
/// @param  c          Container to sort in-place
///
/// Example:
///   std::vector<double> v = {3.1, 1.4, 2.7};
///   boost::algorithm::statsort(v);   // v == {1.4, 2.7, 3.1}
template <
    typename Container,
    typename T = typename Container::value_type,
    typename   = std::enable_if_t<std::is_arithmetic_v<T>>
>
void statsort(Container& c)
{
    const std::size_t n = c.size();
    if (n <= 1) return;

    T* data = c.data();
    T min_val = *std::min_element(data, data + n);
    T max_val = *std::max_element(data, data + n);
    if (min_val >= max_val) return;  // all elements equal

    const double mind = static_cast<double>(min_val);
    const double maxd = static_cast<double>(max_val)
                      + 0.0001 * (static_cast<double>(max_val) - mind);

    std::vector<T> scratch(n);
    detail::statsort_impl(data, n, mind, maxd, scratch.data());
}


/// boost::algorithm::statsort — container overload with projection
///
/// Sorts a contiguous container of objects in ascending order of a numeric
/// key extracted by a projection function.  The projection must return an
/// arithmetic type (int, float, double, etc.).
///
/// This overload covers the same use case as passing a comparator to
/// std::sort, but expresses intent more clearly by extracting a key.
///
/// Falls back to std::sort (via comparator) on pathologically skewed
/// key distributions.
///
/// @tparam Container  Contiguous container (e.g. std::vector<MyStruct>)
/// @tparam Proj       Callable: (const value_type&) -> arithmetic key,
///                    or a member pointer (e.g. &MyStruct::field)
/// @param  c          Container to sort in-place
/// @param  proj       Key extraction function
///
/// Example (lambda):
///   struct Particle { std::string name; double energy; };
///   std::vector<Particle> ps = { {"b", 3.0}, {"a", 1.0}, {"c", 2.0} };
///   boost::algorithm::statsort(ps, [](const Particle& p) { return p.energy; });
///   // ps ordered by ascending energy: "a"(1.0), "c"(2.0), "b"(3.0)
///
/// Example (member pointer):
///   struct Point { int x, y; };
///   std::vector<Point> pts = { {3,0}, {1,0}, {2,0} };
///   boost::algorithm::statsort(pts, &Point::x);
///   // pts ordered by x: {1,0}, {2,0}, {3,0}
template <
    typename Container,
    typename Proj,
    typename T    = typename Container::value_type,
    typename Key  = std::invoke_result_t<Proj, const T&>,
    typename      = std::enable_if_t<std::is_arithmetic_v<Key>>
>
void statsort(Container& c, Proj proj)
{
    const std::size_t n = c.size();
    if (n <= 1) return;

    T* data = c.data();

    auto key_cmp = [&](const T& a, const T& b) {
        return std::invoke(proj, a) < std::invoke(proj, b);
    };
    Key min_key = std::invoke(proj, *std::min_element(data, data + n, key_cmp));
    Key max_key = std::invoke(proj, *std::max_element(data, data + n, key_cmp));

    if (min_key >= max_key) return;  // all keys equal

    const double mind = static_cast<double>(min_key);
    const double maxd = static_cast<double>(max_key)
                      + 0.0001 * (static_cast<double>(max_key) - mind);

    // Wrap in a lambda so std::invoke handles both lambdas and member pointers
    auto proj_fn = [&](const T& elem) -> Key {
        return std::invoke(proj, elem);
    };

    std::vector<T> scratch(n);
    detail::statsort_impl_proj(data, n, mind, maxd, scratch.data(), proj_fn);
}


/// boost::algorithm::statsort — iterator overload (arithmetic types)
///
/// Sorts the range [first, last) of arithmetic values in ascending order.
/// Behaves identically to the container overload.
///
/// @tparam RandomIt  Random-access iterator whose value_type is arithmetic
/// @param  first     Beginning of range
/// @param  last      One past end of range
///
/// Example:
///   std::vector<int> v = {5, 3, 8, 1};
///   boost::algorithm::statsort(v.begin(), v.end());
template <
    typename RandomIt,
    typename T = typename std::iterator_traits<RandomIt>::value_type,
    typename   = std::enable_if_t<std::is_arithmetic_v<T>>
>
void statsort(RandomIt first, RandomIt last)
{
    const std::size_t n = static_cast<std::size_t>(std::distance(first, last));
    if (n <= 1) return;

    std::vector<T> tmp(first, last);
    statsort(tmp);
    std::copy(tmp.begin(), tmp.end(), first);
}


/// boost::algorithm::statsort — iterator overload with projection
///
/// Sorts the range [first, last) in ascending order of the numeric key
/// returned by proj.
///
/// @tparam RandomIt  Random-access iterator
/// @tparam Proj      Callable: (const value_type&) -> arithmetic key
/// @param  first     Beginning of range
/// @param  last      One past end of range
/// @param  proj      Key extraction function
///
/// Example:
///   struct Item { int id; float score; };
///   std::vector<Item> items = { {1, 9.5f}, {2, 3.2f}, {3, 7.1f} };
///   boost::algorithm::statsort(items.begin(), items.end(),
///                              [](const Item& i) { return i.score; });
///   // ordered by score: id=2 (3.2), id=3 (7.1), id=1 (9.5)
template <
    typename RandomIt,
    typename Proj,
    typename T   = typename std::iterator_traits<RandomIt>::value_type,
    typename Key = std::invoke_result_t<Proj, const T&>,
    typename     = std::enable_if_t<std::is_arithmetic_v<Key>>
>
void statsort(RandomIt first, RandomIt last, Proj proj)
{
    const std::size_t n = static_cast<std::size_t>(std::distance(first, last));
    if (n <= 1) return;

    std::vector<T> tmp(first, last);
    statsort(tmp, proj);
    std::copy(tmp.begin(), tmp.end(), first);
}


} // namespace algorithm
} // namespace boost

#endif // BOOST_ALGORITHM_STATSORT_HPP
