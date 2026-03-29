rm statsort_bench

g++ -O3 statsort_bench.cpp -I./include -o statsort_bench

echo "------ benchmark -----"
./statsort_bench

rm statsort_bench
