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
| Uniform    n=10000 | 0.50 ms |**0.16** ms |0.17 ms |0.24 ms |0.53 ms |    3.14x |
| Gaussian   n=10000 | 0.50 ms |0.19 ms |**0.17** ms |0.24 ms |0.52 ms |    2.66x |
| Exponential n=10000 | 0.50 ms |0.18 ms |**0.17** ms |0.24 ms |0.52 ms |    2.87x |
| Nearly sorted n=10000 | 0.14 ms |0.09 ms |0.15 ms |**0.08** ms |0.10 ms |    1.55x |
| Spike (fallback) n=10000 | 0.09 ms |0.17 ms |0.01 ms |0.01 ms |**0.01** ms |    0.50x |
|---|---|---|---|---|---|---|
| Uniform    n=100000 | 6.31 ms |**2.41** ms |2.47 ms |2.79 ms |6.92 ms |    2.62x |
| Gaussian   n=100000 | 6.30 ms |2.53 ms |**2.45** ms |2.79 ms |6.89 ms |    2.50x |
| Exponential n=100000 | 6.32 ms |2.52 ms |**2.37** ms |2.76 ms |6.85 ms |    2.50x |
| Nearly sorted n=100000 | 1.74 ms |1.96 ms |1.27 ms |**1.08** ms |1.19 ms |    0.89x |
| Spike (fallback) n=100000 | 1.00 ms |2.19 ms |0.08 ms |0.14 ms |**0.07** ms |    0.45x |
|---|---|---|---|---|---|---|
| Uniform    n=1000000 | 74.87 ms |**27.35** ms |29.29 ms |31.72 ms |87.96 ms |    2.74x |
| Gaussian   n=1000000 | 75.40 ms |**28.00** ms |31.19 ms |31.80 ms |87.97 ms |    2.69x |
| Exponential n=1000000 | 75.28 ms |**28.27** ms |28.51 ms |31.78 ms |88.09 ms |    2.66x |
| Nearly sorted n=1000000 | 21.18 ms |18.89 ms |18.22 ms |**13.86** ms |16.40 ms |    1.12x |
| Spike (fallback) n=1000000 | 12.70 ms |25.75 ms |0.92 ms |1.54 ms |**0.81** ms |    0.49x |
|---|---|---|---|---|---|---|
| Uniform    n=10000000 | 871.07 ms |288.51 ms |**253.22** ms |360.88 ms |1054.89 ms |    3.02x |
| Gaussian   n=10000000 | 868.68 ms |**284.47** ms |334.99 ms |355.93 ms |1058.72 ms |    3.05x |
| Exponential n=10000000 | 870.41 ms |296.49 ms |**241.76** ms |357.24 ms |1057.03 ms |    2.94x |
| Nearly sorted n=10000000 | 257.78 ms |178.92 ms |246.53 ms |**165.94** ms |212.49 ms |    1.44x |
| Spike (fallback) n=10000000 | 165.31 ms |280.10 ms |8.88 ms |15.29 ms |**7.74** ms |    0.59x |
|---|---|---|---|---|---|---|
| Uniform    n=100000000 | 9930.92 ms |3546.04 ms |**3519.24** ms |4015.07 ms |12294.79 ms |    2.80x |
| Gaussian   n=100000000 | 9931.71 ms |3281.21 ms |**3266.95** ms |3986.43 ms |12375.05 ms |    3.03x |
| Exponential n=100000000 | 9997.86 ms |3320.13 ms |**3083.56** ms |3940.47 ms |12317.14 ms |    3.01x |
| Nearly sorted n=100000000 | 2952.48 ms |**1753.14** ms |2144.12 ms |1946.54 ms |2533.34 ms |    1.68x |
| Spike (fallback) n=100000000 | 1900.25 ms |3037.91 ms |90.73 ms |155.06 ms |**78.60** ms |    0.63x |
|---|---|---|---|---|---|---|


----- test -----
Running 23

test cases...

*** No errors detected

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
