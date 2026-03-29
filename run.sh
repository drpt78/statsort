rm statsort_bench
rm statsort_test

g++ -O3 statsort_bench.cpp -I./include -o statsort_bench
g++ -O3 statsort_test.cpp -I./include -o statsort_test

echo "------ benchmark -----"
./statsort_bench

echo "----- test -----"
./statsort_test
