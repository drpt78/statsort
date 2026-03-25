rm ./yo_example
rm ./yo_test

g++ -O2 statsort_example.cpp -I./include -o yo_example
g++ -O2 statsort_test.cpp -I./include -o yo_test

./yo_example
./yo_test
