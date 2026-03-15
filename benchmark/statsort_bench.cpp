// benchmark/statsort_bench.cpp
//
// Copyright (c) Peter Taraba 2025
// Distributed under the Boost Software License, Version 1.0.
//
// Reproducible benchmark comparing boost::algorithm::statsort against
// std::sort across several input distributions and sizes.
//
// Build (standalone):
//   g++ -O3 -std=c++17 -I../include -o statsort_bench statsort_bench.cpp
//
// Build (CMake):
//   cmake -DSTATSORT_BUILD_BENCHMARKS=ON ..
//   make statsort_bench
//
// Usage:
//   ./statsort_bench            # default: n=1,000,000 and n=10,000,000
//   ./statsort_bench 500000     # custom n

#include "statsort.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

// ── timing helpers ─────────────────────────────────────────────────────────

using Clock = std::chrono::high_resolution_clock;
using Ms    = std::chrono::duration<double, std::milli>;

/// Run `fn` on a fresh copy of `base` for `runs` repetitions; return
/// the median elapsed time in milliseconds.
template <typename SortFn>
double time_median_ms(SortFn fn, const std::vector<double>& base, int runs = 7)
{
    std::vector<double> times;
    times.reserve(runs);
    for (int r = 0; r < runs; ++r) {
        auto v  = base;            // fresh copy each run
        auto t0 = Clock::now();
        fn(v);
        times.push_back(Ms(Clock::now() - t0).count());
    }
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];   // median
}

// ── report ─────────────────────────────────────────────────────────────────

void print_header()
{
    std::cout
        << "\nbuild: " << __DATE__ << " " << __TIME__
        << "  |  C++" << __cplusplus / 100 - 2000 << "\n\n"
        << std::left  << std::setw(34) << "Distribution / n"
        << std::right << std::setw(13) << "std::sort"
        << std::setw(13) << "statsort"
        << std::setw(10) << "speedup\n"
        << std::string(70, '-') << "\n";
}

void bench_distribution(const std::string& label,
                        const std::vector<double>& data)
{
    double t_std  = time_median_ms(
        [](auto& v){ std::sort(v.begin(), v.end()); }, data);
    double t_stat = time_median_ms(
        [](auto& v){ boost::algorithm::statsort(v); }, data);

    std::cout
        << std::left  << std::setw(34) << label
        << std::right << std::fixed << std::setprecision(2)
        << std::setw(11) << t_std  << " ms"
        << std::setw(11) << t_stat << " ms"
        << std::setw(8)  << std::setprecision(2) << (t_std / t_stat) << "x\n";
}

// ── main ───────────────────────────────────────────────────────────────────

int main(int argc, char* argv[])
{
    const std::size_t default_sizes[] = { 100'000, 1'000'000 };

    std::vector<std::size_t> sizes;
    for (int i = 1; i < argc; ++i) {
        sizes.push_back(static_cast<std::size_t>(std::atoll(argv[i])));
        // it's not printing out -> very weird
        std::cout
            << sizes[i-1] << "\n";
    }

    if (sizes.empty())
        sizes.assign(std::begin(default_sizes), std::end(default_sizes));

    std::cout << "boost::algorithm::statsort benchmark\n"
              << "Timing method: median of 7 runs on a fresh vector copy\n";
    print_header();

    for (std::size_t n : sizes) {
        std::mt19937_64 rng(42);
        const std::string ns = "  n=" + std::to_string(n);

        // Uniform
        {
            std::uniform_real_distribution<double> d(0.0, 1e6);
            std::vector<double> v(n);
            std::generate(v.begin(), v.end(), [&]{ return d(rng); });
            bench_distribution("Uniform" + ns, v);
        }

        // Gaussian
        {
            std::normal_distribution<double> d(500'000.0, 100'000.0);
            std::vector<double> v(n);
            std::generate(v.begin(), v.end(), [&]{ return d(rng); });
            bench_distribution("Gaussian" + ns, v);
        }

        // Exponential
        {
            std::exponential_distribution<double> d(1e-5);
            std::vector<double> v(n);
            std::generate(v.begin(), v.end(), [&]{ return d(rng); });
            bench_distribution("Exponential" + ns, v);
        }

        // Nearly sorted (std::sort's sweet spot)
        {
            std::vector<double> v(n);
            std::iota(v.begin(), v.end(), 0.0);
            std::uniform_int_distribution<std::size_t> swap_idx(0, n - 1);
            for (std::size_t k = 0; k < n / 100; ++k)  // 1 % random swaps
                std::swap(v[swap_idx(rng)], v[swap_idx(rng)]);
            bench_distribution("Nearly sorted" + ns, v);
        }

        // Adversarial: spike distribution (triggers imbalance fallback)
        {
            std::vector<double> v(n, 1.0);
            v.back() = 1e12;   // one extreme outlier
            bench_distribution("Spike (fallback)" + ns, v);
        }

        std::cout << "\n";
    }

    return 0;
}
