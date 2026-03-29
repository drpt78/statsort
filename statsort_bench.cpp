// Copyright (c) Peter Taraba 2025
// Distributed under the Boost Software License, Version 1.0.

#include <boost/algorithm/statsort.hpp>
#include <boost/sort/spreadsort/spreadsort.hpp>
#include <boost/sort/sort.hpp>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

void basic_examples()
{
    std::cout << "=== Basic usage ===\n\n";

    std::vector<double> prices = {9.99, 1.49, 5.00, 3.75, 8.20, 0.50};
    boost::algorithm::statsort(prices);
    std::cout << "Sorted prices: ";
    for (double p : prices) std::cout << p << " ";
    std::cout << "\n";

    std::vector<int> scores = {88, 42, 95, 17, 63, 71, 55};
    boost::algorithm::statsort(scores);
    std::cout << "Sorted scores: ";
    for (int s : scores) std::cout << s << " ";
    std::cout << "\n";

    std::vector<float> temps = {36.6f, 38.1f, 37.0f, 35.9f, 39.2f};
    boost::algorithm::statsort(temps.begin(), temps.end());
    std::cout << "Sorted temps:  ";
    for (float t : temps) std::cout << t << " ";
    std::cout << "\n\n";
}

using Clock = std::chrono::high_resolution_clock;
using Ms    = std::chrono::duration<double, std::milli>;

template <typename SortFn>
double time_ms(SortFn fn, const std::vector<double>& base, int runs = 5)
{
    double total = 0;
    for (int r = 0; r < runs; ++r) {
        auto v = base;
        auto t0 = Clock::now();
        fn(v);
        total += Ms(Clock::now() - t0).count();
    }
    return total / runs;
}

void benchmark()
{
    std::cout << "=== Performance comparison ===\n\n";
    std::cout << std::left  << std::setw(3) << "| Distribution / N"
              << std::right << std::setw(12) << "| std::sort"
              << std::setw(12) << "| statsort"
              << std::setw(14) << "| spreadsort"
              << std::setw(12) << "| pdqsort"
              << std::setw(18) << "| flat_stable_sort"
              << std::setw(10) << "| speedup | \n"
              << std::string(90, '-') << "\n";

    auto print_row = [&](const std::string& label,
                         const std::vector<double>& base) {
        double t_std             = time_ms([](auto& v){ std::sort(v.begin(), v.end()); }, base);
        double t_stat            = time_ms([](auto& v){ boost::algorithm::statsort(v); }, base);
        double t_spreadsort      = time_ms([](auto& v){ boost::sort::spreadsort::spreadsort(v.begin(), v.end()); }, base);
        double t_pdqsort         = time_ms([](auto& v){ boost::sort::pdqsort(v.begin(), v.end()); }, base);
        double t_flat_stable_sort = time_ms([](auto& v){ boost::sort::flat_stable_sort(v.begin(), v.end()); }, base);

        double t_min = std::min(t_std, std::min(t_stat, std::min(t_spreadsort, std::min(t_pdqsort, t_flat_stable_sort))));

        std::cout << std::left  << std::setw(2) << "| " << label << " | "
                  << std::right << std::fixed << std::setprecision(2)
                  << ((t_std == t_min) ? "**" : "") << std::setw(3) << t_std << ((t_std == t_min) ? "**" : "") << " ms |"
                  << ((t_stat == t_min) ? "**" : "") << std::setw(3) << t_stat << ((t_stat == t_min) ? "**" : "") << " ms |"
                  << ((t_spreadsort == t_min) ? "**" : "") << std::setw(3) << t_spreadsort << ((t_spreadsort == t_min) ? "**" : "") << " ms |"
                  << ((t_pdqsort == t_min) ? "**" : "") << std::setw(3) << t_pdqsort << ((t_pdqsort == t_min) ? "**" : "") << " ms |"
                  << ((t_flat_stable_sort == t_min) ? "**" : "") << std::setw(3) << t_flat_stable_sort << ((t_flat_stable_sort == t_min) ? "**" : "") << " ms |"
                  << std::setw(8)  << std::setprecision(2) << t_std/t_stat << "x |\n";
    };

    for (std::size_t n : {10000UL, 100000UL}//, 1000000UL, 10000000UL, 100000000UL}
    ) {
        std::mt19937 rng(42);
        std::string ns = " n=" + std::to_string(n);

        { std::uniform_real_distribution<double> d(0,1e6);
          std::vector<double> v(n); std::generate(v.begin(),v.end(),[&]{return d(rng);});
          print_row("Uniform   " + ns, v); }

        { std::normal_distribution<double> d(500000,100000);
          std::vector<double> v(n); std::generate(v.begin(),v.end(),[&]{return d(rng);});
          print_row("Gaussian  " + ns, v); }

        { std::exponential_distribution<double> d(0.00001);
          std::vector<double> v(n); std::generate(v.begin(),v.end(),[&]{return d(rng);});
          print_row("Exponential" + ns, v); }


        {
          std::vector<double> v(n);
          std::iota(v.begin(), v.end(), 0.0);
          std::uniform_int_distribution<std::size_t> swap_idx(0, n - 1);
          for (std::size_t k = 0; k < n / 100; ++k)  // 1 % random swaps
            std::swap(v[swap_idx(rng)], v[swap_idx(rng)]);
          print_row("Nearly sorted" + ns, v); }

        {
           std::vector<double> v(n, 1.0);
           v.back() = 10000000.0;   // one extreme outlier
           print_row("Spike (fallback)" + ns, v); }

        std::cout << "|---|---|---|---|---|---|---|\n";
    }
}

int main()
{
    basic_examples();
    benchmark();
    return 0;
}

