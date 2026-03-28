# Boost Vault Submission: `boost::sort::statsort`

**Author:** Peter Taraba & Claude AI

**Date:** March 2026

**ORCID:** https://orcid.org/0000-0002-8199-3723

**Blog:** https://randommathguy.blogspot.com/

---

## WARNING #1

there is something wrong with Adversarial

## Question for Boost Leaders #1

do you want O2 (Recommended Standard Optimization) compilation or O3 compilation, and if O3 why?

## Overview & Motivation

Standard sorting algorithms such as `std::sort` operate by comparing elements, achieving O(n log n) average complexity regardless of the data distribution. However, in many real-world applications — scientific computing, financial data processing, signal processing, and simulation pipelines — data is drawn from smooth, well-behaved distributions such as uniform, Gaussian, or exponential.

For these cases, comparison-based sorting discards valuable information: we often know *approximately* where an element belongs before we even look at its neighbors. `statsort` exploits this by using element values directly to guide placement, achieving **O(n log log n) average complexity** on smooth distributions — a meaningful asymptotic and practical improvement over `std::sort`.

**Typical use cases:**
- Sorting large floating-point datasets from physics or fluid simulations
- Pre-processing numeric data before statistical analysis
- Financial time-series sorting where values cluster around known ranges
- Any pipeline handling large arithmetic datasets with approximately smooth distributions

`statsort` is not intended for integer keys with many duplicates, adversarial inputs, or nearly-sorted data, where `std::sort` or a radix-based approach remains preferable.

---

## Algorithm Description

The core idea is straightforward: instead of comparing elements, use their values to place them into one of **√n buckets** via linear interpolation, then recurse within each bucket.

### Key insight: variable bucket count

Most distribution-based sorting algorithms (e.g. bucket sort) use a fixed number of buckets, giving O(log n) recursion depth. By using **√n buckets** — scaling with input size — `statsort` reduces recursion depth to **O(log log n)**, which is the source of its asymptotic advantage.

### Step-by-step

1. Compute the minimum and maximum of the input range.
2. Allocate √n buckets and distribute elements using linear interpolation:
   ```
   bucket_index = floor((value - min) / (max - min) * sqrt(n))
   ```
3. Recurse into each non-trivial bucket.
4. Concatenate buckets to produce the sorted output.

### Fallback behavior

On adversarial or highly skewed inputs, bucket imbalance can degrade performance toward O(n²). To prevent this, `statsort` monitors bucket sizes during distribution and falls back to `std::sort` when imbalance is detected, guaranteeing safe worst-case behavior.

### Complexity summary

| Case | Time Complexity | Space Complexity |
|------|----------------|-----------------|
| Average (smooth distributions) | O(n log log n) | O(√n) |
| Worst case (adversarial input) | O(n log n) (fallback) | O(log n) |

---

## Performance Benchmarks

Benchmarks were run on GCC -O2!!!, x86-64 Linux, comparing `statsort` against `std::sort`.

Run #1

| Distribution / N | std::sort  | statsort  | spreadsort   | pdqsort| flat_stable_sort| speedup | 
|---|---|---|---|---|---|---|
| Uniform    n=10000 | 0.50 ms |0.16 ms |0.17 ms |0.24 ms |0.53 ms |    3.20x |
| Gaussian   n=10000 | 0.51 ms |0.17 ms |0.16 ms |0.24 ms |0.52 ms |    2.97x |
| Exponential n=10000 | 0.50 ms |0.17 ms |0.17 ms |0.24 ms |0.52 ms |    2.89x |

| Uniform    n=100000 | 6.30 ms |2.35 ms |2.45 ms |2.80 ms |6.92 ms |    2.68x |
| Gaussian   n=100000 | 6.27 ms |2.22 ms |2.46 ms |2.81 ms |6.96 ms |    2.83x |
| Exponential n=100000 | 6.36 ms |2.23 ms |2.41 ms |2.80 ms |6.98 ms |    2.85x |

| Uniform    n=1000000 | 75.12 ms |26.70 ms |29.39 ms |31.83 ms |88.78 ms |    2.81x |
| Gaussian   n=1000000 | 75.28 ms |27.19 ms |31.65 ms |31.89 ms |89.17 ms |    2.77x |
| Exponential n=1000000 | 75.61 ms |27.30 ms |28.47 ms |31.71 ms |88.23 ms |    2.77x |

| Uniform    n=10000000 | 878.60 ms |291.82 ms |251.94 ms |357.22 ms |1070.66 ms |    3.01x |
| Gaussian   n=10000000 | 873.57 ms |299.39 ms |334.61 ms |354.16 ms |1070.36 ms |    2.92x |
| Exponential n=10000000 | 874.78 ms |304.07 ms |241.28 ms |357.09 ms |1071.17 ms |    2.88x |

| Uniform    n=100000000 | 9956.79 ms |2973.86 ms |3366.50 ms |3944.46 ms |12439.08 ms |    3.35x |
| Gaussian   n=100000000 | 9943.47 ms |3153.26 ms |3252.73 ms |3901.40 ms |12462.21 ms |    3.15x |
| Exponential n=100000000 | 9960.99 ms |3199.78 ms |3114.21 ms |3948.33 ms |12509.96 ms |    3.11x |

----- test -----
Running 23 test cases...

*** No errors detected



Running 23 test cases...

Full methodology and additional benchmarks are available in the accompanying publication:

> Peter Taraba, *"Why would you sort when you know where things approximately belong?"*, March 2025.
> https://www.authorea.com/users/495213/articles/1240078-why-would-you-sort-when-you-know-where-things-approximately-belong?commit=06b7d9e465d985698104a8fbfe2535fcf35c1940

---

## Implementation Notes

- **Header-only**, C++17
- Supports any `std::is_arithmetic<T>` type (`int`, `float`, `double`, etc.)
- No external dependencies beyond the C++ standard library
- Compatible with the Boost.Sort library structure

---

*Preliminary submission — feedback welcome. Happy to extend this with an API reference and further documentation based on community input.*
