# statsort (statistical [sorting](https://faculty.kfupm.edu.sa/phys/aanaqvi/Numerical%20Recipes-The%20Art%20of%20Scientific%20Computing%203rd%20Edition%20(Press%20et%20al).pdf))

## Overview & Motivation

> Peter Taraba, *"Why would you sort when you know where things approximately belong?"*, March 2025.
> https://www.authorea.com/users/495213/articles/1240078-why-would-you-sort-when-you-know-where-things-approximately-belong?commit=06b7d9e465d985698104a8fbfe2535fcf35c1940

### Complexity summary

| Case                           | Time Complexity       |
|--------------------------------|-----------------------|
| Average (smooth distributions) | O(n log log n)        |


## Performance Benchmarks

Run #1 (Manjaro Linux x86_64 with Kernel: 7.0.3-1-MANJARO on AMD Ryzen 5 7520U with Radeon Graphics (8) @ 4.386GHz)

| Distribution / N | std::sort  | statsort  | spreadsort   | pdqsort| flat_stable_sort| speedup | 
|---|---|---|---|---|---|---|
| Uniform    n=10000 | 0.49 ms |**0.16** ms |0.17 ms |0.24 ms |0.53 ms |    3.02x |
| Gaussian   n=10000 | 0.50 ms |0.17 ms |**0.16** ms |0.24 ms |0.52 ms |    2.91x |
| Exponential n=10000 | 0.50 ms |0.18 ms |**0.17** ms |0.25 ms |0.52 ms |    2.76x |
|---|---|---|---|---|---|---|
| Uniform    n=100000 | 6.19 ms |**2.22** ms |2.46 ms |2.79 ms |6.87 ms |    2.79x |
| Gaussian   n=100000 | 6.23 ms |**2.17** ms |2.44 ms |2.81 ms |6.87 ms |    2.87x |
| Exponential n=100000 | 6.23 ms |**2.19** ms |2.39 ms |2.79 ms |6.85 ms |    2.84x |
|---|---|---|---|---|---|---|
| Uniform    n=1000000 | 73.64 ms |**26.77** ms |28.75 ms |31.82 ms |87.32 ms |    2.75x |
| Gaussian   n=1000000 | 73.92 ms |**26.91** ms |31.17 ms |31.90 ms |87.50 ms |    2.75x |
| Exponential n=1000000 | 74.17 ms |**27.67** ms |28.64 ms |31.91 ms |87.31 ms |    2.68x |
|---|---|---|---|---|---|---|
| Uniform    n=10000000 | 862.80 ms |283.60 ms |**251.18** ms |358.40 ms |1036.61 ms |    3.04x |
| Gaussian   n=10000000 | 864.42 ms |**282.76** ms |341.01 ms |355.59 ms |1045.37 ms |    3.06x |
| Exponential n=10000000 | 857.47 ms |288.46 ms |**241.98** ms |357.71 ms |1046.92 ms |    2.97x |
|---|---|---|---|---|---|---|
| Uniform    n=100000000 | 9780.80 ms |**2917.15** ms |3369.98 ms |3973.03 ms |12157.66 ms |    3.35x |
| Gaussian   n=100000000 | 9760.81 ms |**3069.10** ms |3254.75 ms |3919.40 ms |12173.78 ms |    3.18x |
| Exponential n=100000000 | 9923.21 ms |3289.10 ms |**3118.75** ms |3930.38 ms |12357.87 ms |    3.02x |
|---|---|---|---|---|---|---|
