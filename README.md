# Boost Vault Submission: `boost::sort::statsort`

**Author:** Peter Taraba & Claude AI

**Date:** March 2026

**ORCID:** https://orcid.org/0000-0002-8199-3723

**Blog:** https://randommathguy.blogspot.com/

---

## WARNING #1

there is something wrong with Adversarial

## WARNING #2

see all the runs

## Question for Claude #1

why is he compiling with O3 instead of O2 (Recommended Standard Optimization).

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

| Distribution / N                |   std::sort  |  statsort    | spreadsort   |  pdqsort     | flat_stable_sort  | speedup |
| --------------------------------|--------------|--------------|--------------|--------------|--------------|--------------|
| Uniform    n=100000             |     12.36 ms |      5.24 ms |      4.80 ms |      5.40 ms |     13.35 ms |    2.36x     |
| Gaussian   n=100000             |     12.34 ms |      5.95 ms |      4.75 ms |      5.45 ms |     13.35 ms |    2.07x     |
| Exponential n=100000            |     12.48 ms |      6.15 ms |      4.74 ms |      5.39 ms |     13.34 ms |    2.03x     |
| Uniform    n=1000000            |    148.76 ms |     65.60 ms |     63.99 ms |     61.56 ms |    169.09 ms |    2.27x     |
| Gaussian   n=1000000            |    149.12 ms |     59.12 ms |     61.53 ms |     63.20 ms |    170.56 ms |    2.52x     |
| Exponential n=1000000           |    148.60 ms |     66.97 ms |     54.62 ms |     61.85 ms |    170.53 ms |    2.22x     |

Run #2

=== Performance comparison ===

| Distribution / N | std::sort  | statsort  | spreadsort   | pdqsort| flat_stable_sort| speedup | 
| --------------------------------|--------------|--------------|--------------|--------------|--------------|--------------|
| Uniform    n=10000 | 0.50 ms |0.25 ms |0.17 ms |0.23 ms |0.64 ms |    1.99x |
| Gaussian   n=10000 | 0.53 ms |0.24 ms |0.18 ms |0.25 ms |0.53 ms |    2.22x |
| Exponential n=10000 | 0.51 ms |0.24 ms |0.17 ms |0.30 ms |0.52 ms |    2.17x |
| Uniform    n=100000 | 6.58 ms |3.40 ms |2.42 ms |2.78 ms |6.79 ms |    1.94x |
| Gaussian   n=100000 | 6.25 ms |3.32 ms |2.38 ms |2.75 ms |6.78 ms |    1.88x |
| Exponential n=100000 | 6.34 ms |3.26 ms |2.39 ms |2.73 ms |6.78 ms |    1.94x |
| Uniform    n=1000000 | 74.93 ms |57.40 ms |28.28 ms |31.10 ms |86.22 ms |    1.31x |
| Gaussian   n=1000000 | 74.75 ms |38.94 ms |31.48 ms |31.14 ms |86.27 ms |    1.92x |
| Exponential n=1000000 | 75.22 ms |36.24 ms |28.33 ms |31.45 ms |86.14 ms |    2.08x |
| Uniform    n=10000000 | 877.81 ms |286.90 ms |251.74 ms |353.42 ms |1042.73 ms |    3.06x |
| Gaussian   n=10000000 | 873.53 ms |303.89 ms |333.76 ms |351.04 ms |1047.55 ms |    2.87x |
| Exponential n=10000000 | 878.38 ms |338.22 ms |249.86 ms |361.43 ms |1056.43 ms |    2.60x |
| Uniform    n=100000000 | 10016.03 ms |3212.13 ms |3333.95 ms |3915.51 ms |12116.38 ms |    3.12x |
| Gaussian   n=100000000 | 10011.34 ms |3482.98 ms |3241.77 ms |3872.79 ms |12085.62 ms |    2.87x |
| Exponential n=100000000 | 10029.70 ms |3796.02 ms |3047.21 ms |3897.04 ms |12079.65 ms |    2.64x |


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
