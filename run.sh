rm statsort_example
rm statsort_test

g++ -O3 statsort_example.cpp -I./include -o statsort_example
g++ -O3 statsort_test.cpp -I./include -o statsort_test

echo "------ example -----"
./statsort_example

echo "----- test -----"
./statsort_test
