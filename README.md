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

Benchmarks were run on GCC -O3, x86-64 Linux, comparing `statsort` against `std::sort`.

Run #1:

| Input Size  Distribution | `std::sort` | `statsort` | Speedup |
|------------------------|------------|-----------|---------|
| Uniform  n=1000000                 |      148.77 ms |       62.70 ms |     2.37x |
| Gaussian  n=1000000                |      148.34 ms |       63.34 ms |     2.34x |
| Exponential  n=1000000             |      148.52 ms |       64.78 ms |     2.29x |
| Nearly sorted  n=1000000           |       41.39 ms |       41.98 ms |     0.99x |

Run #2:

| Input Size  Distribution | `std::sort` | `statsort` | Speedup |
|------------------------|------------|-----------|---------|
| Uniform  n=1000000                 |      150.12 ms |       63.19 ms |     2.38x |
| Gaussian  n=1000000                |      149.45 ms |       63.72 ms |     2.35x |
| Exponential  n=1000000             |      149.95 ms |       64.12 ms |     2.34x |
| Nearly sorted  n=1000000           |       41.58 ms |       41.76 ms |     1.00x |

Run #3 - another weird thing !!!:

| Input Size  Distribution | `std::sort` | `statsort` | Speedup |
|------------------------|------------|-----------|---------|
| Uniform  n=1000000                 |      235.18 ms |       91.60 ms |     2.57x |
| Gaussian  n=1000000                |      236.00 ms |       92.23 ms |     2.56x |
| Exponential  n=1000000             |      166.17 ms |       65.39 ms |     2.54x |
| Nearly sorted  n=1000000           |       41.27 ms |       41.67 ms |     0.99x |

Run #4

| Input Size  Distribution | `std::sort` | `statsort` | Speedup |
|------------------------|------------|-----------|---------|
| Uniform  n=1000000                 |      236.93 ms |       92.27 ms |     2.57x |
| Gaussian  n=1000000                |      234.95 ms |       92.39 ms |     2.54x |
| Exponential  n=1000000             |      175.18 ms |       65.23 ms |     2.69x |
| Nearly sorted  n=1000000           |       41.36 ms |       41.51 ms |     1.00x |

| Input Size  Distribution | `std::sort` | `statsort` | Speedup |
|------------------------|------------|-----------|---------|
| Uniform  n=10000000                |     1733.84 ms |      593.49 ms |     2.92x |
| Gaussian  n=10000000               |     1736.99 ms |      582.94 ms |     2.98x |
| Exponential  n=10000000            |     1743.59 ms |      595.40 ms |     2.93x |
| Nearly sorted  n=10000000          |      489.72 ms |      396.52 ms |     1.24x |

| Distribution / n                  | std::sort   | statsort | spreadsort    | pdqsort   | spinsort| flat_stable_sort| speedup
|------------------------|------------|-----------|---------|---------|---------|---------|---------|
| Uniform  n=1000000                 |      233.36 ms |       90.86 ms |       85.20 ms |        - ms |        - ms |        - ms |     2.57x |
| Gaussian  n=1000000                |      154.30 ms |       64.83 ms |       55.40 ms |        - ms |        - ms |        - ms |     2.38x |
| Exponential  n=1000000             |      148.17 ms |       57.30 ms |       54.17 ms |        - ms |        - ms |        - ms |     2.59x |
| Nearly sorted  n=1000000           |       41.46 ms |       40.32 ms |       35.03 ms |        - ms |        - ms |        - ms |     1.03x |
| Uniform  n=10000000                |     1715.21 ms |      584.74 ms |      482.90 ms |        - ms |        - ms |        - ms |     2.93x |
| Gaussian  n=10000000               |     1712.58 ms |      581.03 ms |      657.46 ms |        - ms |        - ms |        - ms |     2.95x |
| Exponential  n=10000000            |     1724.43 ms |      597.52 ms |      455.59 ms |        - ms |        - ms |        - ms |     2.89x |
| Nearly sorted  n=10000000          |      493.04 ms |      405.42 ms |      450.32 ms |        - ms |        - ms |        - ms |     1.22x |

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
