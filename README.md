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
| Uniform    n=10000 | 0.50 ms |**0.16** ms |0.18 ms |0.24 ms |0.53 ms |    3.21x |
| Gaussian   n=10000 | 0.50 ms |0.17 ms |**0.16** ms |0.24 ms |0.52 ms |    2.93x |
| Exponential n=10000 | 0.50 ms |0.17 ms |**0.17** ms |0.24 ms |0.52 ms |    2.91x |
|---|---|---|---|---|---|---|
| Uniform    n=100000 | 6.25 ms |**2.42** ms |2.45 ms |2.80 ms |6.87 ms |    2.59x |
| Gaussian   n=100000 | 6.24 ms |**2.36** ms |2.43 ms |2.81 ms |6.88 ms |    2.65x |
| Exponential n=100000 | 6.29 ms |2.42 ms |**2.36** ms |2.77 ms |6.87 ms |    2.60x |
|---|---|---|---|---|---|---|
| Uniform    n=1000000 | 74.26 ms |**27.25** ms |28.62 ms |31.54 ms |87.87 ms |    2.72x |
| Gaussian   n=1000000 | 74.52 ms |**26.86** ms |31.08 ms |31.61 ms |87.81 ms |    2.78x |
| Exponential n=1000000 | 74.87 ms |**26.69** ms |28.16 ms |31.56 ms |87.79 ms |    2.81x |
|---|---|---|---|---|---|---|
| Uniform    n=10000000 | 868.74 ms |283.55 ms |**249.33** ms |355.28 ms |1067.62 ms |    3.06x |
| Gaussian   n=10000000 | 867.05 ms |**279.41** ms |332.58 ms |352.55 ms |1067.87 ms |    3.10x |
| Exponential n=10000000 | 869.17 ms |288.64 ms |**237.57** ms |354.54 ms |1068.23 ms |    3.01x |
|---|---|---|---|---|---|---|
| Uniform    n=100000000 | 9909.40 ms |**3080.69** ms |3342.19 ms |3937.08 ms |12482.25 ms |    3.22x |
| Gaussian   n=100000000 | 9954.85 ms |**3193.78** ms |3314.00 ms |3890.58 ms |12843.27 ms |    3.12x |
| Exponential n=100000000 | 9969.68 ms |3313.66 ms |**3095.22** ms |3915.90 ms |12546.81 ms |    3.01x |
|---|---|---|---|---|---|---|



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
