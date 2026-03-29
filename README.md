# Boost Vault Submission: `boost::sort::statsort`

**Author:** Peter Taraba & Claude AI (Claude AI is a messy boy)

**Date:** March 2026

**ORCID:** https://orcid.org/0000-0002-8199-3723

## Overview & Motivation

> Peter Taraba, *"Why would you sort when you know where things approximately belong?"*, March 2025.
> https://www.authorea.com/users/495213/articles/1240078-why-would-you-sort-when-you-know-where-things-approximately-belong?commit=06b7d9e465d985698104a8fbfe2535fcf35c1940

### Complexity summary

| Case                           | Time Complexity       | Space Complexity |
|--------------------------------|-----------------------|------------------|
| Average (smooth distributions) | O(n log log n)        | O(√n)            |
| Worst case (adversarial input) | O(n log n) (fallback) | O(log n)         |


## Performance Benchmarks

Benchmarks were run on GCC -O2!!!, x86-64 Linux, comparing `statsort` against `std::sort`.

Run #1

| Distribution / N | std::sort  | statsort  | spreadsort   | pdqsort| flat_stable_sort| speedup | 
|---|---|---|---|---|---|---|
| Uniform    n=10000 | 0.50 ms |**0.16** ms |0.17 ms |0.24 ms |0.53 ms |    3.15x |
| Gaussian   n=10000 | 0.50 ms |0.20 ms |**0.16** ms |0.24 ms |0.52 ms |    2.54x |
| Exponential n=10000 | 0.51 ms |0.18 ms |**0.17** ms |0.24 ms |0.52 ms |    2.88x |
|---|---|---|---|---|---|---|
| Uniform    n=100000 | 6.75 ms |**2.24** ms |2.47 ms |2.80 ms |6.85 ms |    3.01x |
| Gaussian   n=100000 | 6.30 ms |**2.33** ms |2.46 ms |2.80 ms |6.89 ms |    2.71x |
| Exponential n=100000 | 6.41 ms |**2.19** ms |2.42 ms |2.78 ms |6.86 ms |    2.93x |
|---|---|---|---|---|---|---|
| Uniform    n=1000000 | 74.82 ms |36.43 ms |47.84 ms |**32.42** ms |88.06 ms |    2.05x |
| Gaussian   n=1000000 | 74.77 ms |**28.83** ms |31.15 ms |31.80 ms |87.86 ms |    2.59x |
| Exponential n=1000000 | 75.16 ms |**27.48** ms |28.17 ms |31.68 ms |86.47 ms |    2.74x |
|---|---|---|---|---|---|---|
| Uniform    n=10000000 | 873.71 ms |308.04 ms |**254.33** ms |357.65 ms |1059.79 ms |    2.84x |
| Gaussian   n=10000000 | 876.60 ms |**292.89** ms |338.64 ms |357.70 ms |1050.76 ms |    2.99x |
| Exponential n=10000000 | 874.52 ms |311.47 ms |**239.44** ms |356.03 ms |1061.97 ms |    2.81x |
|---|---|---|---|---|---|---|
| Uniform    n=100000000 | 9951.55 ms |**3308.78** ms |3348.43 ms |3934.31 ms |12570.35 ms |    3.01x |
| Gaussian   n=100000000 | 9868.01 ms |3329.67 ms |**3250.56** ms |3908.51 ms |12153.82 ms |    2.96x |
| Exponential n=100000000 | 9948.71 ms |3382.96 ms |**3059.96** ms |3909.26 ms |12226.33 ms |    2.94x |
|---|---|---|---|---|---|---|




## Implementation Notes

- **Header-only**, C++17
- Supports any `std::is_arithmetic<T>` type (`int`, `float`, `double`, etc.)
- No external dependencies beyond the C++ standard library
- Compatible with the Boost.Sort library structure
