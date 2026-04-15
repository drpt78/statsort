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

Run #1 (Manjaro KDE)

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

Run #2 (Alpine Linux)

| Distribution / N | std::sort  | statsort  | spreadsort   | pdqsort| flat_stable_sort| speedup | 
|---|---|---|---|---|---|---|
| Uniform    n=10000 | 0.50 ms |0.21 ms |**0.20** ms |0.24 ms |0.53 ms |    2.39x |
| Gaussian   n=10000 | 0.51 ms |0.24 ms |**0.18** ms |0.24 ms |0.53 ms |    2.14x |
| Exponential n=10000 | 0.51 ms |0.25 ms |**0.18** ms |0.24 ms |0.53 ms |    1.99x |
|---|---|---|---|---|---|---|
| Uniform    n=100000 | 6.28 ms |2.72 ms |**2.50** ms |2.83 ms |6.92 ms |    2.31x |
| Gaussian   n=100000 | 6.28 ms |2.92 ms |**2.46** ms |2.82 ms |6.94 ms |    2.15x |
| Exponential n=100000 | 6.33 ms |2.73 ms |**2.41** ms |2.80 ms |6.95 ms |    2.31x |
|---|---|---|---|---|---|---|
| Uniform    n=1000000 | 74.95 ms |31.56 ms |**29.56** ms |31.81 ms |88.24 ms |    2.37x |
| Gaussian   n=1000000 | 75.35 ms |31.97 ms |**31.79** ms |31.92 ms |88.59 ms |    2.36x |
| Exponential n=1000000 | 75.29 ms |30.89 ms |**28.94** ms |31.86 ms |87.38 ms |    2.44x |
|---|---|---|---|---|---|---|
| Uniform    n=10000000 | 876.23 ms |327.53 ms |**254.56** ms |359.97 ms |1058.44 ms |    2.68x |
| Gaussian   n=10000000 | 874.03 ms |**300.72** ms |336.19 ms |355.76 ms |1061.82 ms |    2.91x |
| Exponential n=10000000 | 872.06 ms |311.08 ms |**243.10** ms |359.19 ms |1076.37 ms |    2.80x |
|---|---|---|---|---|---|---|
| Uniform    n=100000000 | 10067.15 ms |**3356.90** ms |3423.24 ms |4052.16 ms |12354.46 ms |    3.00x |
| Gaussian   n=100000000 | 10029.56 ms |3589.33 ms |**3302.30** ms |3961.61 ms |12421.55 ms |    2.79x |
| Exponential n=100000000 | 9996.04 ms |3808.52 ms |**3151.68** ms |3976.97 ms |12324.82 ms |    2.62x |
|---|---|---|---|---|---|---|

Run #3 (Manjaro XFCE)

| Distribution / N | std::sort  | statsort  | spreadsort   | pdqsort| flat_stable_sort| speedup | 
|---|---|---|---|---|---|---|
| Uniform    n=10000 | 0.50 ms |0.17 ms |**0.17** ms |0.24 ms |0.53 ms |    2.86x |
| Gaussian   n=10000 | 0.49 ms |0.17 ms |**0.16** ms |0.25 ms |0.52 ms |    2.85x |
| Exponential n=10000 | 0.50 ms |0.18 ms |**0.18** ms |0.24 ms |0.52 ms |    2.74x |
|---|---|---|---|---|---|---|
| Uniform    n=100000 | 6.16 ms |**2.22** ms |2.47 ms |2.81 ms |6.87 ms |    2.78x |
| Gaussian   n=100000 | 6.14 ms |**2.26** ms |2.44 ms |2.85 ms |6.86 ms |    2.72x |
| Exponential n=100000 | 6.21 ms |2.58 ms |**2.40** ms |2.78 ms |6.86 ms |    2.41x |
|---|---|---|---|---|---|---|
| Uniform    n=1000000 | 73.31 ms |**27.28** ms |30.08 ms |32.02 ms |87.85 ms |    2.69x |
| Gaussian   n=1000000 | 73.57 ms |**26.98** ms |31.24 ms |31.99 ms |87.53 ms |    2.73x |
| Exponential n=1000000 | 74.15 ms |**27.76** ms |28.46 ms |31.91 ms |87.58 ms |    2.67x |
|---|---|---|---|---|---|---|
| Uniform    n=10000000 | 864.53 ms |300.58 ms |**252.66** ms |362.01 ms |1050.54 ms |    2.88x |
| Gaussian   n=10000000 | 868.45 ms |**310.24** ms |352.57 ms |365.32 ms |1087.64 ms |    2.80x |
| Exponential n=10000000 | 868.83 ms |293.95 ms |**244.58** ms |362.73 ms |1090.34 ms |    2.96x |
|---|---|---|---|---|---|---|
| Uniform    n=100000000 | 10024.20 ms |3591.31 ms |**3415.93** ms |4001.33 ms |12218.84 ms |    2.79x |
| Gaussian   n=100000000 | 9863.21 ms |3460.79 ms |**3324.23** ms |3979.62 ms |12239.44 ms |    2.85x |
| Exponential n=100000000 | 9873.33 ms |3452.54 ms |**3095.25** ms |3954.24 ms |12334.34 ms |    2.86x |
|---|---|---|---|---|---|---|


## Implementation Notes

- **Header-only**, C++17
- Supports any `std::is_arithmetic<T>` type (`int`, `float`, `double`, etc.)
- No external dependencies beyond the C++ standard library
- Compatible with the Boost.Sort library structure


# To Do

## Consider feedback:

### High Priority
I'm curious to know if sqrt(n) buckets is somehow optimal, or if the performance has been evaluated for other powers such as 0.4 or 0.6?  One reason I'm curious is if 5 billion items works fine for 16-bit bucket index, to spare the memory expense of 32-bit bucket index.

### Lower Priority
One thing I think is great about boost::sort is the multi-threading.  For some workloads sorting can definitely be a bottleneck and throwing more cores at that is beneficial.  In rendering for example, knowing the order of things towards or away from the current viewpoint, can be useful.  It does seem like this algorithm is amenable to multi-threading, and think would be a requirement for production use.

### Unknown Priority
If I had a precomputed bucket per item, that would likely fit into a 16-bit unsigned integer.  And if evaluating that is costly, or not all the items are changing, would be an additional performance advantage.  So consider a variant of statsort accepting a seperate array of buckets, rather than always "lazily" evaluated.  One other advantage of decoupling the bucket evaluation is that non-uniform distributions can be dealt with as a seperate concern.  (Such as percentile via CDF by some good enough method)

### Unknown Priority
One code suggestion is to nest statsort namespace inside detail, rather than prefixing the function names there.
