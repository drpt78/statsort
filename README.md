# Boost Vault Submission: `boost::sort::statsort`

**Author:** Peter Taraba & Claude AI

**Date:** March 2026

**ORCID:** https://orcid.org/0000-0002-8199-3723

**Blog:** https://randommathguy.blogspot.com/

---

## WARNING #1

the test keeps running even when it should be done. Have to ask Claude why. Do not see the problem yet.

## WARNING #2

see three different runs

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
